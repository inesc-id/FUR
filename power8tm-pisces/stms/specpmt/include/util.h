#pragma once

#include <immintrin.h>
#include <iostream>
#include <atomic>
using namespace std;

extern atomic<uint64_t> fence_cnt;

#define XBEGIN(label) asm volatile goto(".byte 0xc7,0xf8 ; .long %l0-1f\n1:" ::: "eax","memory" : label)
#define XEND() asm volatile(".byte 0x0f,0x01,0xd5" ::: "memory")
#define XFAIL(label) label: do { asm volatile(".byte 0x48, 0x87, 0xC9" ::: "memory"); asm volatile("" ::: "eax", "memory"); } while(0)
#define XFAIL_STATUS(label, status) label: asm volatile("" : "=a" (status) :: "memory")
#define XABORT(status) do { asm volatile (".byte 0xc6, 0xf8, " #status :::"eax"); } while(0)
#define XTEST() ({ char o = 0 ;                     \
           asm volatile(".byte 0x0f,0x01,0xd6 ; setnz %0" : "+r" (o)::"memory"); \
           o; })

/* Status bits */
#define XABORT_EXPLICIT_ABORT   (1 << 0)
#define XABORT_RETRY        (1 << 1)
#define XABORT_CONFLICT     (1 << 2)
#define XABORT_CAPACITY     (1 << 3)
#define XABORT_DEBUG        (1 << 4)
#define XABORT_STATUS(x)    (((x) >> 24) & 0xff)
#define XABORT_LOG_ALLOC    (1<<25)
#define XABORT_SPINLOCK     (1<<26)

typedef struct {
  volatile unsigned long flag;
  unsigned long padding[7]; 
} __attribute__ ((aligned(64))) spinlock_t;

inline void spinlock_init(volatile spinlock_t *lock) { lock->flag = 0; }
inline void spinlock_acquire(volatile spinlock_t *lock) { 
  while(lock->flag == 1UL || __sync_lock_test_and_set(&lock->flag, 1UL) == 1UL) _mm_pause(); 
}
inline void spinlock_release(volatile spinlock_t *lock) {
  __sync_lock_release(&lock->flag);
}
inline int spinlock_isfree(volatile spinlock_t *lock) {
  return lock->flag == 0;
}

inline int htm_start(spinlock_t& global_rtm_mutex, int &tries){
  const int max_tries = 16;
  tries = max_tries;
  int status = 0;
  {
    __label__ failure;
    XFAIL_STATUS(failure,status);
    tries--;
    if(tries <= 0) { spinlock_acquire(&global_rtm_mutex); }
    else { status = 0; XBEGIN(failure); if(!spinlock_isfree(&global_rtm_mutex)) XABORT(XABORT_SPINLOCK); }
  }
  return status;
}

inline void htm_end(spinlock_t& global_rtm_mutex, int &tries){
  if(tries > 0) XEND();
  else spinlock_release(&global_rtm_mutex);
}

inline void sfence(){
  _mm_sfence();
  //fence_cnt++;
}

//#define CLWB
inline void clwb(void* s){
#ifdef CLWB
  _mm_clwb(s);
#elif defined(CLFLUSH)
  _mm_clflush(s);
#elif defined(DRAM_FLUSH)
  #pragma message("use DRAM flush")
#else
  cerr << "no clwb behavior defined" << endl;
  #error clwb behavior needs to be specified. 
#endif
}

inline void clwb(void* s, size_t len){
  char* addr_begin = (char*)((uint64_t)s >> 6 << 6);
  char* addr_end = (char*)s + len;
  while(addr_begin < addr_end){
    clwb((void*)addr_begin);
    addr_begin += 64;
  }
}

//for scalar type, avoid use clwb(void*, size_t) as its branches are slow
template<typename T, typename = enable_if_t<is_scalar<T>::value, bool>>
inline void _clwb(const T* x, int){
  clwb((void*)&x);
}

template<typename T>
inline void _clwb(const T* x, ...){
  clwb((void*)&x, sizeof(x));
}

template<typename T>
inline void clwb(T* x){
  _clwb(x, 0);
}

// uint64_t rdtscp() {
//     unsigned int aux;
//     uint32_t low, high;
//     __asm__ volatile (
//         "rdtscp\n"
//         : "=a" (low), "=d" (high), "=c" (aux)
//         :
//         : 
//     );
//     return ((uint64_t)high << 32) | low;
// }



#include <cstring>

// include <memkind.h>

// inline void log_memkind_init(memkind_t* kind){
//   char* pmp = getenv("PMEMHEAP");
//   if(pmp == nullptr){ 
//     pmp = (char*)malloc(256);
//     strcpy(pmp, getenv("HOME"));
//     strcat(pmp, "/pmsys/ccye/");
//   }
//   // cout << "create pmem pool at : " << pmp << endl;
//   // if(memkind_create_pmem(pmp, 0, kind) != MEMKIND_SUCCESS){
//   //   perror("creating memkind error");
//   // }
// }

// inline void log_memkind_destroy(memkind_t kind){
//   // memkind_destroy_kind(kind);
// }


#include <chrono>

class Timer{
public:
  typedef std::chrono::high_resolution_clock Clock;
  std::chrono::time_point<Clock> tp1, tp2;
  Timer(){
    tp2 = Clock::now();
  }
  void lap(){
    tp1 = tp2;
    tp2 = Clock::now();
  }
  uint64_t elapse(){
    lap();
    return std::chrono::duration_cast<std::chrono::milliseconds>(tp2 - tp1).count();
  }
};

extern Timer timer;
