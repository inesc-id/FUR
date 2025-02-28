/* persistent log */
#pragma once

#include <iostream>
#include <string>
#include <list>
#include <thread>
#include <atomic>
#include <cstring>
#include "util.h"

using namespace std;

class SPLog;

extern thread_local SPLog *local_log;
extern thread_local uint64_t thread_id;
const uint64_t end_mark = 1LL << 63;

//thread local speculative persistent log
class SPLog{
public:
  uint64_t tx_id;
  char* log_area;
  char* current_pos;
  char* start_pos;

  size_t capacity;
  size_t roll_capacity;
  // memkind_t mem_kind;

  SPLog(uint64_t _thread_id) { //, memkind_t _mem_kind) : mem_kind(_mem_kind){
    thread_id = _thread_id;
    this->tx_id = -1;
    capacity = 1<<28; //1MB log default, we assume that the write set size of a transaction is less than this capacity
    roll_capacity = capacity; //rolls at 1MB on commit, no matter what's the capacity; capacity should be larger than the roll_capacity;

    // log_area = (char*)memkind_malloc(mem_kind, capacity);
    this->log_area = (char*)malloc(capacity);
    *(uint64_t*)this->log_area = -1;
    this->start_pos = this->log_area;
    this->current_pos = this->log_area;
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


  template<typename T>
  void insertValueSafe(char*& a, const T v){ //handle log area overflow
    if(current_pos - log_area >= capacity - 64){
      capacity <<= 1;
      roll_capacity = capacity>>1;
      char* x;
      // x = (char*)memkind_malloc(mem_kind, capacity);
      x = (char*)malloc(capacity);
      memcpy(x, log_area, current_pos - log_area);
      start_pos = start_pos - log_area + x;
      current_pos = current_pos - log_area + x;
      //memkind_free(mem_kind, log_area);
      log_area = x;
    }
    setValue(a, v);
    a += sizeof(T);
  }

  /*
  * log entry must be cacheline aligned
  * log entry format: checksum, (address, size, value)*, timestamp of tx end
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
    //insertValue(current_pos, data);
    insertValueSafe(current_pos, data);
  }

  template<typename T>
  void log_data(T& data){
    log_data(&data, data);
  }

  void flush_log(){
    clwb(start_pos, current_pos - start_pos);
  }

  void commit_log(uint64_t timestamp){
    timestamp |= end_mark;
    insertValue(current_pos, timestamp);
    setValue(start_pos, 0LL); //TODO: calculate checksum
    current_pos = (char*)((((uint64_t)current_pos >> 6) + (((uint64_t)current_pos & 63) != 0)) << 6); //log records are cache line-aligned
    setValue(current_pos, -1LL); //set the next checksum as -1
    flush_log();
    if(current_pos > log_area + roll_capacity) //start from begining
      current_pos = start_pos = log_area;
  }

  void output(){ //print all log records
    cout << "log_area : " << (void*)log_area << endl;
    void* x = log_area;
    while(1){
      if(*(uint64_t*)x == -1) break;
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
  list<SPLog*> local_log_list;
  // memkind_t mem_kind;
  PLogMetadata() : timestamp(0), thread_cnt(0){
    // log_memkind_init(&mem_kind);
  }
  
  uint64_t getTimestamp(){
    // return rdtscp();
    return timestamp++;
  }

  void createLocalLog(){
    if(local_log == nullptr){
      local_log = new SPLog(thread_cnt++);//, mem_kind);
      local_log_list.push_back(local_log);
    }
  }

  void destroyLocalLog(){
    //TODO: GC needs to determine whether to remove the log area as some data may rely on speculative log to recover after the thread terminate
  }
};

extern PLogMetadata plm;