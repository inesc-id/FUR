#pragma once
#include <cstdlib>
#include <ctime>

#include "util.h"
#include "log.h"

const size_t value_size = 32;
char end_mark = 0xFF;  

Log _log;

void __attribute__ ((constructor)) init_benchmark() {
  srand(7919);
}

template<typename T>
inline void create_log(T& x){
#ifndef NATIVE
  _log.insert(x);
#endif
}

inline void flush_log(){
#ifdef UNDO
  _log.flush();
#endif
}

inline void tx_commit(){
 
#ifdef SPECLOG
  create_log(end_mark);
  _log.flush_zero_fence();
  sfence();
#elif UNDO
  sfence();
  create_log(end_mark);
  flush_log();
  sfence();
#endif
}