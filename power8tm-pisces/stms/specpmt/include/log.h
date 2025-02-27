#pragma once
#include <iostream>
#include <string>
#include "util.h"

using namespace std;

class Log{
public:
  char* content;
  char* head;
  char* last_flush;
  uint64_t nbyte_written;
  uint64_t ncacheline_persisted;
  const size_t max_log = 1<<12;

  Log(){
    content = new char[max_log<<1];
    memset(content, 0, max_log<<1);
    head = content;
    last_flush = head;
    nbyte_written = 0;
    ncacheline_persisted = 0;
  }

  void flush_zero_fence(){
    while(last_flush < head){
      clwb((void*)last_flush);
      last_flush += 64;
      //ncacheline_persisted++;
    }
    head = last_flush;
  }
  
  void flush(){
    flush_zero_fence();
    sfence();
  }

  //log := (address, data)*
  template<typename T>
  void insert(T &val){    
    if(head >= content + max_log){
      flush_zero_fence();
      head = content;
      last_flush = head;
    }

    //nbyte_written += sizeof(&val) + sizeof(val);
    *((uint64_t*)head) = (uint64_t)&val;
    head += sizeof(&val);
    *((T*)head) = val;
    head += sizeof(val);
  }
  ~Log(){
    cout << nbyte_written << " bytes log written" << endl;
    cout << ncacheline_persisted << " cache line persisted" << endl;
  }
};