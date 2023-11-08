#ifndef EXTRA_MACROS_H_GUARD_
#define EXTRA_MACROS_H_GUARD_

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>
#include "stdint.h"
#include "htmxlintrin.h"

# define INACTIVE         0
# define ACTIVE           1
# define NON_DURABLE      2
# define first_2bits_zero 0x3ffffffffffffffff
# define CACHE_LINE_SIZE  128

#define LOGSIZE_mask ((1l<<LOGSIZE_bits)-1l)
#define LOGSIZE_bits 19l
#define LOGSIZE (1<<LOGSIZE_bits)
#define MIN_SPACE_LOG 200000

#define bit63one              0x8000000000000000lu
#define bit62one              (1l<<62)
#define isbit63one(ts)        ((ts & bit63one)==bit63one)
#define isbit62one(ts)        ((ts & bit62one)==bit62one)
#define zeroBit63(ts)         (ts & ~bit63one)
#define store_fence()         __atomic_thread_fence(__ATOMIC_RELEASE)
#define load_fence()          __atomic_thread_fence(__ATOMIC_ACQUIRE)
#define atomic_STORE(ptr,val) __atomic_store_n(&(ptr), val, __ATOMIC_RELEASE)
#define atomic_LOAD(ptr)      __atomic_load_n(&(ptr), __ATOMIC_ACQUIRE)
#define __dcbst(base, index)  \
  __asm__ ("dcbst %0, %1" : /*no result*/ : "b%" (index), "r" (base) : "memory")

# define IS_LOCKED(lock)    (atomic_LOAD(lock) != 0 && atomic_LOAD(lock) != (loc_var.tid + 1))
# define TRY_LOCK(lock)     __sync_bool_compare_and_swap((volatile int*)(&lock), 0, loc_var.tid + 1)
# define UNLOCK(lock)       atomic_STORE(lock, 0)

#ifndef READ_TIMESTAMP
# define READ_TIMESTAMP(dest) __asm__ volatile("0:                  \n\tmfspr   %0,268           \n": "=r"(dest));
#endif 

extern int place_abort_marker;

typedef struct spinlock {
    int lock;
    char suffixPadding[CACHE_LINE_SIZE];
} __attribute__((aligned(CACHE_LINE_SIZE))) spinlock_t;

typedef struct padded_scalar {
    char prefixPadding[CACHE_LINE_SIZE];
    volatile long value;
//    char suffixPadding[CACHE_LINE_SIZE];
} __attribute__((aligned(CACHE_LINE_SIZE))) padded_scalar_t;

typedef struct quiscence_call_args {
    char prefixPadding[CACHE_LINE_SIZE];
    volatile int num_threads;
    volatile int tid;
    volatile int index;
    volatile int state;
    volatile long temp;
    volatile uint64_t *logptr;
    volatile long start_wait_time; 
    volatile long end_wait_time; 
    volatile long ts1; 
    volatile long ts2; 
    char suffixPadding[CACHE_LINE_SIZE];
} __attribute__((aligned(CACHE_LINE_SIZE))) QUIESCENCE_CALL_ARGS_t;

typedef struct tx_local_vars {
  char prefixPadding[CACHE_LINE_SIZE];
  volatile int rot_budget;
  volatile unsigned char tx_status;
  volatile unsigned char rot_status;
  TM_buff_type TM_buff;
  volatile uint64_t *mylogpointer;
  volatile uint64_t *mylogpointer_snapshot;
  volatile uint64_t *mylogend;
  volatile uint64_t *mylogstart;
  volatile int tid;
  volatile int exec_mode;
  volatile long ts1;
  volatile long ts2;
  char suffixPadding[CACHE_LINE_SIZE];
} __attribute__((aligned(CACHE_LINE_SIZE))) tx_local_vars_t;

typedef struct padded_pointer {
    long* addr;
    char suffixPadding[CACHE_LINE_SIZE];
} __attribute__((aligned(CACHE_LINE_SIZE))) padded_pointer_t;

typedef struct padded_statistics {
    unsigned long total_time;
    unsigned long wait_time;
    unsigned long read_commits;
    unsigned long htm_commits;
    unsigned long htm_conflict_aborts;
    unsigned long htm_self_conflicts;
    unsigned long htm_trans_conflicts;
    unsigned long htm_nontrans_conflicts;
    unsigned long htm_user_aborts;
    unsigned long htm_capacity_aborts;
    unsigned long htm_persistent_aborts;
    unsigned long htm_other_aborts;
    unsigned long rot_commits;
    unsigned long rot_conflict_aborts;
    unsigned long rot_self_conflicts;
    unsigned long rot_trans_conflicts;
    unsigned long rot_nontrans_conflicts;
    unsigned long rot_other_conflicts;
    unsigned long rot_user_aborts;
    unsigned long rot_persistent_aborts;
    unsigned long rot_capacity_aborts;
    unsigned long rot_other_aborts;
    unsigned long gl_commits;
    unsigned long commit_time;
    unsigned long abort_time;
    unsigned long sus_time;
    unsigned long flush_time;
    unsigned long wait2_time;
    char suffixPadding[CACHE_LINE_SIZE];
} __attribute__((aligned(CACHE_LINE_SIZE))) padded_statistics_t;

typedef struct readset_item {
  long* addr;
	long addr_p;
	int type;
  struct readset_item *next;
} readset_item_t;

typedef struct readset {
  readset_item_t *head;
} readset_t;


extern __attribute__((aligned(CACHE_LINE_SIZE))) padded_scalar_t counters[];
extern __attribute__((aligned(CACHE_LINE_SIZE))) padded_scalar_t rot_counters[];
extern __attribute__((aligned(CACHE_LINE_SIZE))) padded_scalar_t triggers[];
extern __attribute__((aligned(CACHE_LINE_SIZE))) padded_statistics_t stats_array[];
extern __attribute__((aligned(CACHE_LINE_SIZE))) int single_global_lock;
extern __thread unsigned long rs_counter;

extern __attribute__((aligned(CACHE_LINE_SIZE))) padded_scalar_t ts_state[];
extern __attribute__((aligned(CACHE_LINE_SIZE))) padded_scalar_t order_ts[];

extern __attribute__((aligned(CACHE_LINE_SIZE))) __thread long ts_snapshot[80];
extern __attribute__((aligned(CACHE_LINE_SIZE))) __thread long state_snapshot[80];
extern __attribute__((aligned(CACHE_LINE_SIZE))) __thread tx_local_vars_t loc_var;
extern __attribute__((aligned(CACHE_LINE_SIZE))) __thread QUIESCENCE_CALL_ARGS_t q_args;

extern __attribute__((aligned(CACHE_LINE_SIZE))) int global_order_ts;
extern uint64_t **log_per_thread;
extern uint64_t **log_pointer;
// extern __thread volatile uint64_t* mylogpointer;
// extern __thread volatile uint64_t* mylogpointer_snapshot;
// extern __thread volatile uint64_t* mylogend;
// extern __thread volatile uint64_t* mylogstart;
extern uint64_t  **log_replayer_start_ptr;
extern uint64_t  **log_replayer_end_ptr;
extern __thread volatile long start_tx;
extern __thread volatile long end_tx;
extern __thread volatile long start_ts;
extern __thread volatile long end_ts;
extern __thread volatile long start_sus;
extern __thread volatile long end_sus;
extern __thread volatile long start_flush;
extern __thread volatile long end_flush;
extern __thread volatile long start_wait2;
extern __thread volatile long end_wait2;

void
my_tm_startup(int numThread);

void
my_tm_thread_enter();

# ifndef MIN_BACKOFF
#  define MIN_BACKOFF                   (1UL << 2)
# endif /* MIN_BACKOFF */
# ifndef MAX_BACKOFF
#  define MAX_BACKOFF                   (1UL << 31)
# endif /* MAX_BACKOFF */


# define UPDATE_TS_STATE(state){\
  register long temp; \
  READ_TIMESTAMP(temp);\
  temp = temp & first_2bits_zero;\
  temp = (state<<62)|temp;\
  ts_state[loc_var.tid].value=temp;\
}\

# define UPDATE_STATE(state) \
{ \
  long temp = state;\
  temp = temp<<2;\
  temp = temp>>2;\
  temp = (state << 62) | temp;\
  ts_state[loc_var.tid].value = temp;\
}\
// UPDATE_STATE

# define check_state(temp)({\
  (temp & (3l<<62))>>62;\
})\

#define flush_log_commit_marker(ptr,ts,start,end)\
  *(ptr)=bit63one | ts; \
  __dcbst((ptr),0); \
  ptr = start + ((((ptr) - (start)) + 1) & LOGSIZE_mask); \
  atomic_STORE(log_replayer_end_ptr[loc_var.tid],ptr); \
// end flush_log_commit_marker

#define place_abort_marker_in_log(ptr, ts, start, end) \
  *(ptr) = bit62one | ts; \
  __dcbst((ptr), 0); /* flush the marker to the log */ \
  (ptr) = start + ((((ptr) - (start)) + 1) & LOGSIZE_mask); \
  atomic_STORE(log_replayer_end_ptr[loc_var.tid], ptr); \
  loc_var.mylogpointer_snapshot = ptr; \
// end place_abort_marker_in_log

#define abortMarker() \
{ \
  if ( order_ts[loc_var.tid].value != -1 ) \
  { \
    place_abort_marker_in_log( \
      loc_var.mylogpointer, \
      order_ts[loc_var.tid].value, \
      loc_var.mylogstart, \
      loc_var.mylogend \
    );\
  } \
  UPDATE_STATE(INACTIVE); \
}; \
// end abortMarker

# define atomicInc()   __atomic_add_fetch(&global_order_ts, 1, __ATOMIC_RELEASE) 

#define write_in_log(ptr,addr,val,start,end) \
  *(ptr) = (uint64_t)addr; \
  ptr = (start) + ((((ptr)-(start))+1)&LOGSIZE_mask); \
  *(ptr) = val; \
  ptr = (start) + ((((ptr)-(start))+1)&LOGSIZE_mask); \
  /* assert(ptr!=0 && "ptr is null"); */ \
  /* assert(ptr-end<=0 && "ptr is larger than end of the log"); */ \


# ifndef delay_for_pm
// # define delay_for_pm 25 //number that gives a latency between 0.18 usec and 0.5 usec
# define delay_for_pm 25 //number that gives a latency between 0.18 usec and 0.5 usec
# define delay_per_cache_line 25
# endif

# define emulate_pm_slowdown() \
{ \
  volatile int i; \
  for (i = 0; i < delay_for_pm; ++i) \
    __asm__ volatile ("nop" ::: "memory"); \
}

# define emulate_pm_slowdown_foreach_line(n_cache) \
{\
    int j;\
    for( j = 0 ; j < n_cache; j++) \
    {\
        emulate_pm_slowdown();\
    }\
}\

#define flush_log_entries(ptr, ptr_snapshot, start, end)\
if (ptr_snapshot < ptr ) \
{\
  while (ptr_snapshot <= ptr ) \
  { \
    __dcbst(ptr_snapshot, 0); /* TODO: sometimes it breaks the ROTs */ \
    emulate_pm_slowdown(); \
    /*advance one cacheline */ \
   ptr_snapshot += 16; \
  } \
} \
else \
{ \
  while (ptr_snapshot != ptr ) \
  { \
    __dcbst(ptr_snapshot, 0); \
    emulate_pm_slowdown(); \
    /*advance one cacheline */ \
    int _i;\
    for( _i = 0; _i < 16; _i++ ) \
    { \
      if (ptr_snapshot == ptr ) \
        break;\
      ptr_snapshot++; \
      if ( end - ptr_snapshot <= 0 ) \
      { \
        ptr_snapshot = start; \
      } \
    } \
  } \
} \

#define flush_log_entries_no_wait(ptr, ptr_snapshot, start, end) \
if (ptr_snapshot < ptr) /* handles normal case */ \
{ \
    while (ptr_snapshot <= ptr) \
    { \
        __dcbst(ptr_snapshot,0); \
        max_cache_line[local_thread_id].value++; \
        /*emulate_pm_slowdown();\
        /*advance one cacheline */ \
        ptr_snapshot += 16; \
    } \
} \
else /* handles warp around case */ \
{ \
    while (ptr_snapshot != ptr) \
    { \
        __dcbst(ptr_snapshot,0); \
        max_cache_line[local_thread_id].value++;\
        /*emulate_pm_slowdown();\
        /*advance one cacheline */\
        int _i;\
        for ( _i=0; _i<16; _i++) \
        { \
            if ( ptr_snapshot == ptr ) \
                break;\
            ptr_snapshot++;\
            if ( end-ptr_snapshot <= 0 ) \
                ptr_snapshot = start;\
        } \
    } \
}\


# define WAIT(_cond) \
  while ( _cond ) \
  { \
    cpu_relax(); \
  } \
// end WAIT

# define CONTINUE_LOOP_IF(_cond, ...) \
  if ( _cond ) \
  { \
    __VA_ARGS__; \
    continue; \
  } \
// end CONTINUE_LOOP_IF

# define BREAK_LOOP_IF(_cond, ...) \
  if ( _cond ) \
  { \
    __VA_ARGS__; \
    break; \
  } \
// end BREAK_LOOP_IF

//-------------------------------TM_BEGIN------------------------------

# define QUIESCENCE_CALL_GL() \
{ \
	int index;\
	int num_threads = global_numThread; \
  for ( index=0; index < num_threads; index++ ) \
  { \
   /*wait for active threads*/  \
    WAIT( (check_state(ts_state[index].value)) != INACTIVE ); \
  } \
};\
// end QUIESCENCE_CALL_GL

# define ACQUIRE_GLOBAL_LOCK() \
{ \
	UPDATE_STATE(INACTIVE); \
  rmb(); \
	WAIT( TRY_LOCK(single_global_lock) ); \
	QUIESCENCE_CALL_GL(); \
};\
// end ACQUIRE_GLOBAL_LOCK

//Begin ROT
# define USE_ROT() \
{ \
	loc_var.rot_budget = ROT_RETRIES; \
	WAIT( IS_LOCKED(single_global_lock) ); \
	while ( loc_var.rot_budget > 0 ) \
  { \
		loc_var.rot_status = 1; \
    READ_TIMESTAMP(loc_var.ts1); \
    UPDATE_TS_STATE(ACTIVE);\
		rmb(); \
		CONTINUE_LOOP_IF( IS_LOCKED(single_global_lock), \
    { \
      READ_TIMESTAMP(loc_var.ts1); \
			UPDATE_TS_STATE(INACTIVE); /* inactive rot*/ \
			rmb(); \
			WAIT ( IS_LOCKED(single_global_lock) ); \
		}); \
		/*BEGIN_ROT ------------------------------------------------*/ \
    READ_TIMESTAMP(start_tx); \
    loc_var.tx_status = __TM_begin_rot(&(loc_var.TM_buff)); \
		BREAK_LOOP_IF ( loc_var.tx_status == _HTM_TBEGIN_STARTED ); \
    /* ABORT HAPPENS !!!! */ \
    if ( place_abort_marker ) \
    { abortMarker(); } \
    loc_var.rot_status = 0; \
    loc_var.rot_budget--; \
    READ_TIMESTAMP(end_tx); \
    stats_array[loc_var.tid].abort_time += end_tx - start_tx;\
		if ( __TM_conflict(&(loc_var.TM_buff)) ) \
    { \
      stats_array[loc_var.tid].rot_conflict_aborts ++; \
			if ( __TM_is_self_conflict(&(loc_var.TM_buff)) ) \
        stats_array[loc_var.tid].rot_self_conflicts++; \
			else if ( __TM_is_trans_conflict(&(loc_var.TM_buff)) ) \
        stats_array[loc_var.tid].rot_trans_conflicts++; \
			else if ( __TM_is_nontrans_conflict(&(loc_var.TM_buff)) ) \
        stats_array[loc_var.tid].rot_nontrans_conflicts++; \
			else \
        stats_array[loc_var.tid].rot_other_conflicts++; \
			int state = check_state(ts_state[loc_var.tid].value); \
      if ( state == ACTIVE ) { \
        UPDATE_STATE(INACTIVE);\
        rmb(); \
      } \
    } \
    else if (__TM_user_abort(&(loc_var.TM_buff))) \
    { stats_array[loc_var.tid].rot_user_aborts ++; } \
    else if ( __TM_capacity_abort(&(loc_var.TM_buff)) ) \
    { \
			stats_array[loc_var.tid].rot_capacity_aborts ++; \
			if ( __TM_is_persistent_abort(&(loc_var.TM_buff)) ) \
        stats_array[loc_var.tid].rot_persistent_aborts ++; \
      break; \
		} \
    else \
    { stats_array[loc_var.tid].rot_other_aborts ++; } \
	} \
};\

//Begin WRITE
# define ACQUIRE_WRITE_LOCK() \
{ \
	loc_var.exec_mode = 1; \
	loc_var.rot_status = 0; \
	USE_ROT(); \
	if ( !loc_var.rot_status ) \
  { \
		loc_var.exec_mode = 2; \
		ACQUIRE_GLOBAL_LOCK(); \
	} \
};\
// end ACQUIRE_WRITE_LOCK

//Begin READ
# define ACQUIRE_READ_LOCK() \
{ \
	while ( 1 ) \
  { \
    READ_TIMESTAMP(loc_var.ts1); \
		UPDATE_TS_STATE(ACTIVE); \
		rmb(); \
		CONTINUE_LOOP_IF ( IS_LOCKED(single_global_lock), \
    { \
			UPDATE_STATE(INACTIVE); \
			rmb(); \
			WAIT( IS_LOCKED(single_global_lock) ); \
		}); \
		break; \
	} \
}; \
// end ACQUIRE_READ_LOCK

# define TM_BEGIN_EXT(b,ro) \
{  \
	loc_var.exec_mode = 0; \
	rs_counter = 0; \
	local_thread_id = SPECIAL_THREAD_ID();\
  order_ts[local_thread_id].value = -1;\
  loc_var.mylogpointer_snapshot = loc_var.mylogpointer;\
  loc_var.tid = local_thread_id;\
  q_args.tid = local_thread_id;\
  q_args.num_threads = global_numThread;\
	if ( ro ) \
  { \
		ACQUIRE_READ_LOCK(); \
	} \
	else \
  { \
		ACQUIRE_WRITE_LOCK(); \
	} \
} \
// end TM_BEGIN_EXT

# define TM_END() \
	if ( ro ) \
  { \
		RELEASE_READ_LOCK(); \
	} \
	else \
  { \
		RELEASE_WRITE_LOCK(); \
	} \
// end TM_END

//-------------------------------TM_END------------------------------

#define FINAL_PRINT(_start_ts, _end_ts) \
  READ_TIMESTAMP(_end_ts); \
  stats_array[0].total_time = _end_ts - _start_ts;\
  unsigned long wait_time = 0; \
  unsigned long total_time = 0; \
  unsigned long read_commits = 0; \
  unsigned long htm_commits = 0; \
  unsigned long htm_conflict_aborts = 0; \
  unsigned long htm_user_aborts = 0; \
  unsigned long htm_self_conflicts = 0; \
  unsigned long htm_trans_conflicts = 0; \
  unsigned long htm_nontrans_conflicts = 0; \
  unsigned long htm_persistent_aborts = 0; \
  unsigned long htm_capacity_aborts = 0; \
  unsigned long htm_other_aborts = 0; \
  unsigned long rot_commits = 0; \
  unsigned long rot_conflict_aborts = 0; \
  unsigned long rot_user_aborts = 0; \
  unsigned long rot_self_conflicts = 0; \
  unsigned long rot_trans_conflicts = 0; \
  unsigned long rot_nontrans_conflicts = 0; \
  unsigned long rot_other_conflicts = 0; \
  unsigned long rot_persistent_aborts = 0; \
  unsigned long rot_capacity_aborts = 0; \
  unsigned long rot_other_aborts = 0; \
  unsigned long gl_commits = 0; \
  unsigned long commit_time = 0; \
  unsigned long abort_time = 0; \
  unsigned long sus_time = 0; \
  unsigned long flush_time = 0; \
  unsigned long wait2_time = 0; \
  int i = 0; \
  for (; i < 80; i++) \
  { \
    wait_time += stats_array[i].wait_time; \
    total_time += stats_array[i].total_time; \
    read_commits += stats_array[i].read_commits; \
    htm_commits += stats_array[i].htm_commits; \
    htm_conflict_aborts += stats_array[i].htm_conflict_aborts; \
    htm_user_aborts += stats_array[i].htm_user_aborts; \
    htm_self_conflicts += stats_array[i].htm_self_conflicts; \
    htm_trans_conflicts += stats_array[i].htm_trans_conflicts; \
    htm_nontrans_conflicts += stats_array[i].htm_nontrans_conflicts; \
    htm_persistent_aborts += stats_array[i].htm_persistent_aborts; \
    htm_capacity_aborts += stats_array[i].htm_capacity_aborts; \
    htm_other_aborts += stats_array[i].htm_other_aborts; \
    rot_commits += stats_array[i].rot_commits; \
    rot_conflict_aborts += stats_array[i].rot_conflict_aborts; \
    rot_user_aborts += stats_array[i].rot_user_aborts; \
    rot_self_conflicts += stats_array[i].rot_self_conflicts; \
    rot_trans_conflicts += stats_array[i].rot_trans_conflicts; \
    rot_nontrans_conflicts += stats_array[i].rot_nontrans_conflicts; \
    rot_other_conflicts += stats_array[i].rot_other_conflicts; \
    rot_persistent_aborts += stats_array[i].rot_persistent_aborts; \
    rot_capacity_aborts += stats_array[i].rot_capacity_aborts; \
    rot_other_aborts += stats_array[i].rot_other_aborts; \
    gl_commits += stats_array[i].gl_commits; \
    commit_time += stats_array[i].commit_time; \
    sus_time += stats_array[i].sus_time; \
    flush_time += stats_array[i].flush_time; \
    wait2_time += stats_array[i].wait2_time; \
    abort_time += stats_array[i].abort_time; \
  } \
  printf(\
"Total sum time: %lu\n" \
"Total commit time: %lu\n" \
"Total abort time: %lu\n" \
"Total wait time: %lu\n" \
"Total sus time: %lu\n" \
"Total flush time: %lu\n" \
"Total wait2 time: %lu\n" \
"Total commits: %lu\n" \
  "\tRead commits: %lu\n" \
  "\tHTM commits:  %lu\n" \
  "\tROT commits:  %lu\n" \
  "\tGL commits: %lu\n" \
"Total aborts: %lu\n" \
  "\tHTM conflict aborts:  %lu\n" \
    "\t\tHTM self aborts:  %lu\n" \
    "\t\tHTM trans aborts:  %lu\n" \
    "\t\tHTM non-trans aborts:  %lu\n" \
  "\tHTM user aborts :  %lu\n" \
  "\tHTM capacity aborts:  %lu\n" \
    "\t\tHTM persistent aborts:  %lu\n" \
  "\tHTM other aborts:  %lu\n" \
  "\tROT conflict aborts:  %lu\n" \
    "\t\tROT self aborts:  %lu\n" \
    "\t\tROT trans aborts:  %lu\n" \
    "\t\tROT non-trans aborts:  %lu\n" \
    "\t\tROT other conflict aborts:  %lu\n" \
  "\tROT user aborts:  %lu\n" \
  "\tROT capacity aborts:  %lu\n" \
    "\t\tROT persistent aborts:  %lu\n" \
  "\tROT other aborts:  %lu\n", \
  total_time, \
  commit_time, \
  abort_time, \
  wait_time, \
  sus_time, \
  flush_time, \
  wait2_time, \
  read_commits+htm_commits+rot_commits+gl_commits, \
  read_commits, \
  htm_commits, \
  rot_commits, \
  gl_commits, \
  htm_conflict_aborts+htm_user_aborts+htm_capacity_aborts+htm_other_aborts+rot_conflict_aborts+rot_user_aborts+rot_capacity_aborts+rot_other_aborts, \
  htm_conflict_aborts, \
  htm_self_conflicts, \
  htm_trans_conflicts, \
  htm_nontrans_conflicts, \
  htm_user_aborts, \
  htm_capacity_aborts, \
  htm_persistent_aborts, \
  htm_other_aborts, \
  rot_conflict_aborts, \
  rot_self_conflicts, \
  rot_trans_conflicts, \
  rot_nontrans_conflicts, \
  rot_other_conflicts, \
  rot_user_aborts, \
  rot_capacity_aborts, \
  rot_persistent_aborts, \
  rot_other_aborts); \
// end FINAL_PRINT

#ifdef __cplusplus
}
#endif


#endif // EXTRA_MACROS_H_GUARD_