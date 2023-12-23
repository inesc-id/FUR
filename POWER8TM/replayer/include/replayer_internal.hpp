#pragma once
#include "replayer.hpp"
#include <cstdint>
#include <stdio.h>

#ifndef EASY_MALLOC
#define ARCH_CACHE_LINE_SIZE 128
#define MALLOC_FN(size) aligned_alloc(ARCH_CACHE_LINE_SIZE, (size))
#define EASY_MALLOC(var, nb) \
  if ((var = (__typeof__(var))MALLOC_FN(sizeof(__typeof__(*(var)))*(nb))) == NULL) \
    perror("malloc");
#endif /* EASY_MALLOC */

#define bit63one              0x8000000000000000lu
#define bit62one              0x4000000000000000lu
#define isbit63one(ts)        ((ts & bit63one)==bit63one)
#define isbit62one(ts)        ((ts & bit62one)==bit62one)
#define flipBit63(ts)         ( bit63one ^ (ts))
#define onesBit63(ts)         ( bit63one | (ts))
#define zeroBit63(ts)         (~bit63one & (ts))
#define store_fence()         __atomic_thread_fence(__ATOMIC_RELEASE)
#define load_fence()          __atomic_thread_fence(__ATOMIC_ACQUIRE)
#define atomic_STORE(ptr,val) __atomic_store_n(&(ptr), val, __ATOMIC_RELEASE)
#define atomic_LOAD(ptr)      __atomic_load_n(&(ptr), __ATOMIC_ACQUIRE)
#define __dcbst(base, index)  \
  __asm__ ("dcbst %0, %1" : /*no result*/ : "b%" (index), "r" (base) : "memory")

namespace rep
{

struct log_entry_t
{
  union {
    struct {
      uint64_t addr;
      uint64_t val;
    };
    struct {
      uint64_t is_commit; // bit in position 63 to one
      uint64_t ts; // TODO: not used
    };
  };
} __attribute__((packed));

namespace naive
{

int init(args_t&);
int generate_log();
int replay();
int destroy();

}

namespace seq_log
{

int init(args_t&);
int generate_log();
int replay();
int destroy();

}

}
