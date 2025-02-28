#pragma once
#include <iostream>

#include "splog_naive.h"
#include "util.h"

#define SP_TM_SHARED_WRITE

/*
- use circular buffer for logs
- no log GC
- no speculative log checking on writes
- no concurrency
*/

class SPPMT{
public:
  SPPMT();
  
  void tm_system_init(); //called on program init
  void tm_system_shutdown(); //called on program exit
  void tm_thread_init(); //called on per-thread tm init
  void tm_thread_exit();
  void tm_begin(long thread_id);
  void tm_end();
  
  template<typename T> void tm_read(T* addr);
  template<typename T> void tm_write(const T& val);
  template<typename T> void _tm_write(const T& val, ...); //for scalar types, these should be aligned, such that they should stay in a single cache line
  template<typename T, enable_if_t<is_scalar<T>::value, bool> = false> void _tm_write(const T& val, int); //the last parameter, int is used for compiler to find the best-match template specialization
};

template<typename T>
void SPPMT::tm_read(T* addr){
}

template<typename T>
void SPPMT::tm_write(const T& val){
  local_log->log_data(val);
  clwb(&val);
}

typedef SPPMT PMT;