/* persistent log */
#pragma once

#include <iostream>
#include <string>
#include <list>
#include <thread>
#include <atomic>
#include <list>
#include <unordered_map>
#include <vector>
#include "util.h"

using namespace std;

class SPGCLog;
extern thread_local SPGCLog *local_log;
extern thread_local uint64_t thread_id;
const uint64_t end_mark = 1LL << 63;
const uint64_t gc_mark = 1LL << 62;

const uint64_t invalid_checksum = -1LL;
const uint64_t pointer_prefix = 2LL << 62;

//a global background GC compacting SPGCLogs
class SPGCLogGC{
public:
  unordered_map<uint64_t, pair<uint64_t, uint64_t*>> last_update; //timestamps of last update on addresses; global
  vector<uint64_t*> dead_records; //can be thread local to encourage concurrency
  SPGCLogGC(){}
  char* scanLogChunk(char* log_chunk){ //return nullptr or the address of next chunk
    char* x = log_chunk;
    while(1){
      if(*(uint64_t*)x == invalid_checksum) return nullptr;
      else if(*(uint64_t*)x >= pointer_prefix) return (char*)(*(uint64_t*)x - pointer_prefix);
      x = x + 8; //skip checksum
      vector<uint64_t*> addr_vec;
      while(!(*(uint64_t*)x & end_mark)){
        size_t s = 1<<(*(uint64_t*)x & 15);
        addr_vec.push_back((uint64_t*)x); //TODO: currently, we gc log records of exactly the same sizes. For overlapped log records, we consider them as different. So, we preserve the size of log record as part of the key.
        x = (char*)x + 8;
        x += s;
      }
      uint64_t ts = (*(uint64_t*)x ^ end_mark);
      x += 8;
      x = (char*)(((uint64_t)x + 63) >> 6 << 6);
      
      /*check the liveness of the log record*/
      for(auto itr = addr_vec.begin(); itr != addr_vec.end(); itr++){
        auto x = last_update.find(**itr);
        if(x == last_update.end()){
          last_update[**itr] = make_pair(ts, *itr);
        }else{
          if(x->second.first < ts){
            last_update[**itr] = make_pair(ts, *itr);
            dead_records.push_back(x->second.second);
          }
          else{
            dead_records.push_back(*itr);
          }
        }
      }
    }
  }
  void scanLogArea(char* log_area){
    char* x = scanLogChunk(log_area);
    while(x != nullptr)
      x = scanLogChunk(x);
  }

  void output(){
    cout << "dead record" << endl;
    for(auto itr = dead_records.begin(); itr != dead_records.end(); itr++)
      cout << "\t" << *itr << " " << (void*)(**itr) << endl;
    
    cout << "aliave record" << endl;
    for(auto itr = last_update.begin(); itr != last_update.end(); itr++)
      cout << "\t" << (void*)itr->first << " " << itr->second.first << " " << itr->second.second << endl;
  }
};

//thread local speculative persistent log
class SPGCLog{
public:
  uint64_t tx_id;
  char* log_area; //the begin of current log chunk
  char* current_pos; //where current log writes
  char* start_pos; //where the tx starts
  char* first_log_chunk;
  size_t capacity;
  //memkind_t mem_kind;
  
  void runGC(SPGCLogGC& splog_gc){
    splog_gc.scanLogArea(first_log_chunk);
  }

  void allocateLogChunk(){
    // log_area = (char*)memkind_malloc(mem_kind, capacity<<1);
    log_area = (char*)malloc(capacity<<1);
    *(uint64_t*)log_area = invalid_checksum;
  }

  SPGCLog(uint64_t _thread_id) {/*, memkind_t _mem_kind) : mem_kind(_mem_kind){
    thread_id = _thread_id;
    tx_id = -1;
    capacity = 1<<20; //1MB log default, we assume that the write set size of a transaction is less than this capacity
    //capacity = 3 * (1<<6);
    allocateLogChunk();
    start_pos = log_area;
    current_pos = log_area;
    first_log_chunk = log_area;
  }
  
  template<typename T>
  void setValue(char* a, const T v){
    *(T*)a = v;
  }

  template<typename T>
  void insertValue(char*& a, const T v){
    setValue(a, v);
    a += sizeof(T);
  }
  /*
  * log entry must be cacheline aligned
  * checksum := [ "00 checksum", "11 invalidchecksum", "10 pointer"]
  * log entry format := checksum, (address, size, value)*, timestamp of tx end
  * log chunk format := log entry*, [invalid_checksum, pointer to next chunk]
  * timestamp of tx end use is used as the end mark of the log entry; Its first bit is 1. The recovery can tell that this is an invalid address and it is the timestamp
  * unlike undo logging, speculative logging doesn't need the timestamp of tx start as it will not use the log of uncommitted tx
  */
  void start_tx(){
    tx_id++;
    start_pos = current_pos;
    insertValue(current_pos, 0LL); //checksum
  }

  template<typename T>
  void log_data(const void* address, const T& data){ //sizeof(T) should be less than 2^16 bytes
    uint64_t compound_address = (uint64_t)address;
    compound_address <<= 4;
    compound_address |= (63-__builtin_clzll(sizeof(T)))+(__builtin_popcount(sizeof(T)) != 1); //round the data size to 2^n
    insertValue(current_pos, compound_address);
    insertValue(current_pos, data);
  }

  template<typename T>
  void log_data(T& data){
    log_data(&data, data);
  }

  void flush_log(){
    clwb(start_pos, current_pos - start_pos + 64);
  }

  void commit_log(uint64_t timestamp){
    timestamp |= end_mark;
    insertValue(current_pos, timestamp);
    setValue(start_pos, 0LL); //TODO: calculate checksum
    current_pos = (char*)((((uint64_t)current_pos >> 6) + (((uint64_t)current_pos & 63) != 0)) << 6);
    if(current_pos < log_area + capacity)
      setValue(current_pos, invalid_checksum); //set the next checksum to invalid_checksum to indicate the end of log records
    else{//buffer overflow, create a new log chunk and put the pointer at the end of current chunk
      allocateLogChunk(); //TODO: this approach can be optimized as it waste too much persistent memory space, up to twice. Customize memory allocator or use linked listed log records to save memory space.
      setValue(current_pos, (uint64_t)log_area | pointer_prefix);
      current_pos = start_pos = log_area;
    }
    flush_log();
  }

  void output(){ //print all log records
    cout << "log_area : " << (void*)first_log_chunk << endl;
    void* x = first_log_chunk;
    char* current_log_chunk = first_log_chunk;
    while(1){
      if(*(uint64_t*)x == invalid_checksum) break;
      else if(*(uint64_t*)x >= pointer_prefix){
        current_log_chunk = (char*)(*(uint64_t*)x - pointer_prefix);
        x = current_log_chunk;
        cout << "switch to next log chunk: " << x << endl;
        continue;
      }
      
      cout << "checksum : " << hex << *(uint64_t*)x << dec << endl; //checksum
      x = (char*)x + 8;

      while(!(*(uint64_t*)x & end_mark)){ //entry
        size_t s = 1<<(*(uint64_t*)x & 15);
        //cout << (void*)(((uint64_t)x)) << " " << s << " ";
        cout << (void*)((*(uint64_t*)x)>>4) << " " << s << " ";
        x = (char*)x + 8;
        switch(s){
          case(4): cout << *(int*)x; break;
          case(8): cout << *(uint64_t*)x; break;
          default: cout << "NOT SUPPORTED";;
        }
        cout << " ";
        x = (char*)x + s;
      }
      cout << "\ntimestamp : " << hex << (*(uint64_t*)x ^ end_mark) << dec << endl; //timestamp
      x = (char*)x + 8;
      x = (void*)(((uint64_t)x + 63) >> 6 << 6);
    }
  }
};


//global log metadata
class PLogMetadata{
public:
  atomic<uint64_t> timestamp;
  atomic<uint64_t> thread_cnt;
  list<SPGCLog*> local_log_list;
  // memkind_t mem_kind;
  SPGCLogGC splog_gc;
  PLogMetadata() : timestamp(0), thread_cnt(0), splog_gc(){
    // log_memkind_init(&mem_kind);
  }
  
  uint64_t getTimestamp(){
    // return rdtscp();
    return timestamp++;
  }

  void createLocalLog(){
    if(local_log == nullptr){
      local_log = new SPGCLog(thread_cnt++, mem_kind);
      local_log_list.push_back(local_log);
    }
  }

  void destroyLocalLog(){
    //TODO: GC needs to determine whether to remove the log area as some data may rely on speculative log to recover after the thread terminate
  }

  void runGC(){
    for(auto itr = local_log_list.begin(); itr != local_log_list.end(); itr++){
      (*itr)->runGC(splog_gc);
    }
    //splog_gc.output();
    cout << "number of dead records : " << splog_gc.dead_records.size() << endl;
    cout << "number of alive records : " << splog_gc.last_update.size() << endl;
  }

  ~PLogMetadata(){
    //runGC();
    // log_memkind_destroy(mem_kind);
  }
};

extern PLogMetadata plm;

