#ifndef TM_H
#define TM_H 1

#  include <stdio.h>


#ifndef REDUCED_TM_API

#  define MAIN(argc, argv)              int main (int argc, char** argv)
#  define MAIN_RETURN(val)              return val

#  define GOTO_SIM()                    /* nothing */
#  define GOTO_REAL()                   /* nothing */
#  define IS_IN_SIM()                   (0)

#  define SIM_GET_NUM_CPU(var)          /* nothing */

#  define TM_PRINTF                     printf
#  define TM_PRINT0                     printf
#  define TM_PRINT1                     printf
#  define TM_PRINT2                     printf
#  define TM_PRINT3                     printf

#  define P_MEMORY_STARTUP(numThread)   /* nothing */
#  define P_MEMORY_SHUTDOWN()           /* nothing */

#  include <assert.h>
#  include "memory.h"
#  include "thread.h"
#  include "types.h"
#  include <math.h>

#  define TM_ARG                        /* nothing */
#  define TM_ARG_ALONE                  /* nothing */
#  define TM_ARGDECL                    /* nothing */
#  define TM_ARGDECL_ALONE              /* nothing */
#  define TM_CALLABLE                   /* nothing */

#  define TM_BEGIN_WAIVER()
#  define TM_END_WAIVER()

#  define P_MALLOC(size)                malloc(size)
#  define P_FREE(ptr)                   free(ptr)
#  define TM_MALLOC(size)               malloc(size)
#  define FAST_PATH_FREE(ptr)           free(ptr)
#  define SLOW_PATH_FREE(ptr)           free(ptr)

# define SETUP_NUMBER_TASKS(n)
# define SETUP_NUMBER_THREADS(n)
# define PRINT_STATS()
# define AL_LOCK(idx)

#endif

#ifdef REDUCED_TM_API
#    define SPECIAL_THREAD_ID()         get_tid()
#else
#    define SPECIAL_THREAD_ID()         thread_getId()
#endif

#include "POWER_common.h"
#include "extra_MACROS.h"

#  define TM_STARTUP(numThread, bId) \
  place_abort_marker = 1; \
  my_tm_startup(numThread); \
  READ_TIMESTAMP(start_ts); \
// end TM_STARTUP
#  define TM_SHUTDOWN() \
{ \
  FINAL_PRINT(start_ts, end_ts); \
/*printf("first time: %d, second time: %d\n",total_first_time,total_second_time);*/ \
} \

#  define TM_THREAD_ENTER() my_tm_thread_enter()
#  define TM_THREAD_EXIT()

# define IS_LOCKED(lock)        *((volatile int*)(&lock)) != 0

# define IS_GLOBAL_LOCKED(lock)        *((volatile int*)(&lock)) == 2

# define TM_BEGIN(ro) TM_BEGIN_EXT(0,ro)

# define READ_TIMESTAMP(dest) __asm__ volatile("0:                  \n\tmfspr   %0,268           \n": "=r"(dest));
//-------------------------------------------------------------------------------
# define INACTIVE    0
# define ACTIVE      1
# define NON_DURABLE 2
# define first_2bits_zero 0x3ffffffffffffffff

# define UPDATE_TS_STATE(state){\
  static __thread long _temp;\
  READ_TIMESTAMP(_temp);\
  _temp=_temp & first_2bits_zero;\
  _temp = (state<<62)|_temp;\
  ts_state[local_thread_id].value=_temp;\
}\

# define UPDATE_STATE(state){\
  static __thread long _temp=state;\
  _temp=_temp<<2;\
  _temp=_temp>>2;\
  _temp = (state<<62)|_temp;\
  ts_state[local_thread_id].value=_temp;\
}\

# define check_state(temp)({\
  (temp & (3l<<62))>>62;\
})\

# define atomicInc()   __atomic_add_fetch(&global_order_ts, 1, __ATOMIC_RELEASE) 


//-------------------------------TM_BEGIN------------------------------

# define QUIESCENCE_CALL_GL() { \
	int index; \
	int num_threads = global_numThread; \
  for ( index = 0; index < num_threads; index++ ) \
  { \
    while( (check_state(ts_state[index].value)) != INACTIVE) /*wait for active threads*/\
    { cpu_relax(); } \
  } \
};\

# define ACQUIRE_GLOBAL_LOCK(){ \
	UPDATE_STATE(INACTIVE); \
  rmb(); \
	while (pthread_spin_trylock(&single_global_lock) != 0) \
  { cpu_relax(); } \
	QUIESCENCE_CALL_GL(); \
};\

//-------------------------------TM_END------------------------------

//todo retirar slowdowns do cmmit log (emulate_pm_slowdown)
//cache line é calculada com um ++ em vez do emulate
# define QUIESCENCE_CALL_ROT(){ \
  __attribute__((aligned(CACHE_LINE_SIZE))) static __thread volatile QUIESCENCE_CALL_ARGS_t q_args;\
	q_args.num_threads = (long)global_numThread; \
	READ_TIMESTAMP(q_args.start_wait_time); \
	for(q_args.index=0; q_args.index < 80; q_args.index++) \
  { \
    if (q_args.index == q_args.num_threads) \
      break; \
		if(q_args.index == local_thread_id) \
      continue; \
		q_args.temp = ts_state[q_args.index].value; \
		q_args.state = (q_args.temp & (3l<<62))>>62; \
		switch(q_args.state) { \
			case ACTIVE:\
				state_snapshot[q_args.index] = q_args.temp; \
				break;\
			case INACTIVE:\
			case NON_DURABLE:\
			default:\
				state_snapshot[q_args.index] = 0; \
				break;\
		} \
  } \
	for(q_args.index=0; q_args.index < q_args.num_threads; q_args.index++) \
  { \
		if(q_args.index == local_thread_id) \
      continue; \
		if(state_snapshot[q_args.index] != 0) \
    { \
			while(ts_state[q_args.index].value==state_snapshot[q_args.index] || ts_state[q_args.index].value > state_snapshot[q_args.index]) \
      { cpu_relax(); } \
		} \
	} \
  READ_TIMESTAMP(q_args.end_wait_time); \
  stats_array[local_thread_id].wait_time += q_args.end_wait_time - q_args.start_wait_time; \
  max_cache_line[local_thread_id].value = 0; \
  commit_log_no_wait(mylogpointer,order_ts[local_thread_id].value,mylogstart,mylogend);\
  if (((q_args.end_wait_time - q_args.start_wait_time)/delay_per_cache_line) < max_cache_line[local_thread_id].value) \
  {\
    emulate_pm_slowdown_foreach_line(  /* 0); */ \
    max_cache_line[local_thread_id].value /* computed number of cache-lines to flush*/ \
    - ((q_args.end_wait_time - q_args.start_wait_time)/delay_per_cache_line) /* discount of the wait phase */ );\
  }\
  READ_TIMESTAMP(q_args.start_wait_time); \
  stats_array[local_thread_id].flush_time += q_args.start_wait_time - q_args.end_wait_time; \
};


//
//
# define RELEASE_WRITE_LOCK(){ \
	if(local_exec_mode == 1) \
  { \
	  __TM_suspend(); \
      READ_TIMESTAMP(start_sus);\
	    UPDATE_TS_STATE(NON_DURABLE); /* committing rot*/ \
      order_ts[local_thread_id].value=atomicInc();\
		  QUIESCENCE_CALL_ROT(); \
      rmb(); \
      READ_TIMESTAMP(end_sus);\
      stats_array[local_thread_id].sus_time+=end_sus-start_sus;\
	  __TM_resume(); \
		__TM_end(); \
    READ_TIMESTAMP(end_tx); \
    stats_array[local_thread_id].commit_time += end_tx - start_tx;\
    READ_TIMESTAMP(start_flush);\
    /*commit_log(mylogpointer,order_ts[local_thread_id].value,mylogstart,mylogend);*/\
    commit_log_marker(mylogpointer,order_ts[local_thread_id].value,mylogstart,mylogend); \
    READ_TIMESTAMP(end_flush);\
    stats_array[local_thread_id].flush_time+=end_flush-start_flush;\
    long num_threads = global_numThread; \
    long index;\
    long state;\
    READ_TIMESTAMP(start_wait2);\
    for(index=0; index < num_threads; index++) \
    { \
      if(index == local_thread_id) \
        continue; \
      state = (ts_state[index].value & (3l<<62))>>62;\
      while(state == NON_DURABLE && order_ts[index].value <= order_ts[local_thread_id].value)\
      { cpu_relax(); } \
    } \
    READ_TIMESTAMP(end_wait2);\
    stats_array[local_thread_id].wait2_time+=end_wait2-start_wait2;\
		UPDATE_STATE(INACTIVE); /* inactive rot*/ \
		stats_array[local_thread_id].rot_commits++; \
	} \
	else \
  { \
    order_ts[local_thread_id].value = global_order_ts++; \
    rmb(); \
		pthread_spin_unlock(&single_global_lock); \
		stats_array[local_thread_id].gl_commits++; \
	} \
};

# define RELEASE_READ_LOCK(){\
  rwmb();\
  UPDATE_STATE(INACTIVE);\
  stats_array[local_thread_id].read_commits++;\
  \ 
  long num_threads = global_numThread; \
  long index;\
  volatile long ts_snapshot = ts_state[local_thread_id].value; \
  long state;\
  READ_TIMESTAMP(start_wait2);\
  for(index=0; index < num_threads; index++) \
  { \
    if(index == local_thread_id) \
      continue; \
    state = (ts_state[index].value & (3l<<62))>>62;\
    while(state == NON_DURABLE && ts_state[index].value < ts_state[local_thread_id].value) \
    { cpu_relax(); } \
	} \
  READ_TIMESTAMP(end_wait2);\
  stats_array[local_thread_id].wait2_time+=end_wait2-start_wait2;\
} \

//-------------------------------------------------------------------------------
#    define TM_BEGIN_RO()                 TM_BEGIN(1)
#    define TM_RESTART()                  __TM_abort();
#    define TM_EARLY_RELEASE(var)

# define FAST_PATH_RESTART() __TM_abort();

#define SLOW_PATH_SHARED_READ(var)             var;
#define SLOW_PATH_SHARED_READ_P(var)           var;
#define SLOW_PATH_SHARED_READ_D(var)           var;
#define SLOW_PATH_SHARED_READ_F(var)           var;

#define FAST_PATH_SHARED_READ(var)                 var
#define FAST_PATH_SHARED_READ_P(var)               var
#define FAST_PATH_SHARED_READ_D(var)               var
#define FAST_PATH_SHARED_READ_F(var)               var

# define FAST_PATH_SHARED_WRITE(var, val)   ({var = val;write_in_log(mylogpointer,&(var),val,mylogstart,mylogend); var;})
# define FAST_PATH_SHARED_WRITE_P(var, val) ({var = val;write_in_log(mylogpointer,&(var),val,mylogstart,mylogend); var;})
# define FAST_PATH_SHARED_WRITE_D(var, val) ({var = val;write_in_log(mylogpointer,&(var),val,mylogstart,mylogend); var;})
# define FAST_PATH_SHARED_WRITE_F(var, val) ({var = val;write_in_log(mylogpointer,&(var),val,mylogstart,mylogend); var;})

# define SLOW_PATH_RESTART() FAST_PATH_RESTART()
# define SLOW_PATH_SHARED_WRITE(var, val)     FAST_PATH_SHARED_WRITE(var, val)
# define SLOW_PATH_SHARED_WRITE_P(var, val)   FAST_PATH_SHARED_WRITE_P(var, val)
# define SLOW_PATH_SHARED_WRITE_D(var, val)   FAST_PATH_SHARED_WRITE_D(var, val)
# define SLOW_PATH_SHARED_WRITE_F(var, val)   FAST_PATH_SHARED_WRITE_F(var, val)


#  define TM_LOCAL_WRITE(var, val)      ({var = val; var;})
#  define TM_LOCAL_WRITE_P(var, val)    ({var = val; var;})
#  define TM_LOCAL_WRITE_D(var, val)    ({var = val; var;})
#  define TM_LOCAL_WRITE_F(var, val)    ({var = val; var;})


#endif
