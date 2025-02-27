#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <signal.h>
#include "platform.h"
#include "util.h"

# define delay_for_pm 70
# define emulate_pm_slowdown() \
{ \
  volatile int i; \
  for (i = 0; i < delay_for_pm; ++i) \
    __asm__ volatile ("nop" ::: "memory"); \
}

static unsigned long start_time, end_time;

#ifndef READ_TIMESTAMP
#	if defined(__i386__)
#		error "READ_TIMESTAMP not refined for __i386__"
#	elif defined(__x86_64__)
static __inline__ unsigned long long
rdtsc_x86_64( void )
{ 
	unsigned long long res; 
	__asm__ __volatile__ ( 
	"xor %%rax,%%rax \n\t" 
	"rdtsc           \n\t" 
	"shl $32,%%rdx   \n\t" 
	"or  %%rax,%%rdx \n\t" 
	"mov %%rdx,%0    \n\t" 
	: "=r"(res) 
	: 
	: "rax", "rdx"); 
	return res; 
}

static __inline__ unsigned long long
rdtscp_x86_64( void ) 
{
	unsigned long long res;
	__asm__ __volatile__ (
	"xor %%rax,%%rax \n\t"
	"rdtscp          \n\t"
	"shl $32,%%rdx   \n\t"
	"or  %%rax,%%rdx \n\t"
	"mov %%rdx,%0    \n\t"
	: "=r"(res)
	:
	: "rax", "rdx", "ecx");
	return res;
}

# 	define READ_TIMESTAMP(dest) dest = rdtsc_x86_64()

#	elif defined(__powerpc__)
# 	define READ_TIMESTAMP(dest) __asm__ volatile("0: \n\tmfspr   %0,268 \n": "=r"(dest));
#	endif /* ARCH options */
#endif /* READ_TIMESTAMP */


struct _Thread {
    long UniqID;

    volatile long Retries;
    long Starts;
    long Aborts; /* Tally of # of aborts */
    long flush_time;
    long upd_dur_commit_time;
    long ro_wait_time;
    long isolation_wait_time;
    long ro_tx_time;
    long upd_tx_time;
    long abort_time;
    long upd_commits;
    long ro_commits;
    
    long tsBegin_for_stats;

    int numStores;
    int numLoads;

    sigjmp_buf* envPtr;
};


#define MAX_THREADS 64
static volatile Thread* threads[MAX_THREADS];

typedef struct
{
    long value;
    char suffixPadding[128];
} aligned_type_t ;

__attribute__((aligned(128))) volatile aligned_type_t* LOCK;


static pthread_key_t    global_key_self;


__INLINE__ intptr_t
AtomicAdd (volatile intptr_t* addr, intptr_t dx)
{
    intptr_t v;
    for (v = *addr; CAS(addr, v, v+dx) != v; v = *addr) {}
    return (v+dx);
}

volatile long StartTally         = 0;
volatile long AbortTally         = 0;

volatile long flush_timeTally = 0;
volatile long upd_dur_commit_timeTally  = 0;
volatile long ro_wait_timeTally = 0;
volatile long isolation_wait_timeTally = 0;
volatile long ro_tx_timeTally = 0;
volatile long upd_tx_timeTally = 0;
volatile long abort_timeTally = 0;
volatile long stm_commitsTally = 0;

volatile long stm_ro_commitsTally = 0;
volatile long stm_upd_commitsTally = 0;


volatile long ReadOverflowTally  = 0;
volatile long WriteOverflowTally = 0;
volatile long LocalOverflowTally = 0;
#define pisces_TALLY_MAX          (((unsigned long)(-1)) >> 1)


void
TxOnce ()
{
    // LOCK = (aligned_type_t*) malloc(sizeof(aligned_type_t));
    // LOCK->value = 0;

    pthread_key_create(&global_key_self, NULL); /* CCM: do before we register handler */

READ_TIMESTAMP(start_time);
}


void
TxShutdown ()
{
    printf("specpmt system shutdown:\n"
           "  Starts=%li Aborts=%li\n"
           "  Overflows: R=%li W=%li L=%li\n"
           , StartTally, AbortTally,
           ReadOverflowTally, WriteOverflowTally, LocalOverflowTally);

READ_TIMESTAMP(end_time);

    // printf("pisces system shutdown:\n"
    //        "  Starts=%li Aborts=%li\n"
    //        "  Overflows: R=%li W=%li L=%li\n"
    //        , StartTally, AbortTally,
    //        ReadOverflowTally, WriteOverflowTally, LocalOverflowTally);

  unsigned long total_time = end_time - start_time; \

  

// printf(\
// "Total sum time: %lu\n" \
// "Total commit time: %lu\n" \
// "Total abort time: %lu\n" \
// "Total wait time: %lu\n" \
// "Total sus time: %lu\n" \
// "Total flush time: %lu\n" \
// "Total dur_commit time: %lu\n" \
// "Total RO_dur_wait time: %lu\n" \
// "Total upd tx time: %lu\n" \
// "Total RO tx time: %lu\n" \
// "Total commits: %lu\n" \
//   "\tRead commits: %lu\n" \
//   "\tHTM commits:  %lu\n" \
//   "\tROT commits:  %lu\n" \
//   "\tGL commits: %lu\n" \
// "Total aborts: %lu\n" \
//   "\tHTM conflict aborts:  %lu\n" \
//     "\t\tHTM self aborts:  %lu\n" \
//     "\t\tHTM trans aborts:  %lu\n" \
//     "\t\tHTM non-trans aborts:  %lu\n" \
//   "\tHTM user aborts :  %lu\n" \
//   "\tHTM capacity aborts:  %lu\n" \
//     "\t\tHTM persistent aborts:  %lu\n" \
//   "\tHTM other aborts:  %lu\n" \
//   "\tROT conflict aborts:  %lu\n" \
//     "\t\tROT self aborts:  %lu\n" \
//     "\t\tROT trans aborts:  %lu\n" \
//     "\t\tROT non-trans aborts:  %lu\n" \
//     "\t\tROT other conflict aborts:  %lu\n" \
//   "\tROT user aborts:  %lu\n" \
//   "\tROT capacity aborts:  %lu\n" \
//     "\t\tROT persistent aborts:  %lu\n" \
//   "\tROT other aborts:  %lu\n", \


  printf(\
"Total sum time: %lu\n" \
"Total commit time: %lu\n" \
"Total abort-upd time: %lu\n" \
"Total abort-ro time: %lu\n" \
"Total wait time: %lu\n" \
"Total sus time: %lu\n" \
"Total flush time: %lu\n" \
"Total dur_commit time: %lu\n" \
"Total RO_dur_wait time: %lu\n" \
"Total upd tx time: %lu\n" \
"Total RO tx time: %lu\n" \
"Total commits: %lu\n" \
  "\tNon-tx commits: %lu\n" \
  "\tHTM commits:  %lu\n" \
  "\tROT commits:  %lu\n" \
  "\tGL commits: %lu\n" \
  "\tSTM commits:  %lu\n" \
  "\t\tSTM RO commits:  %lu\n" \
  "\t\tSTM upd commits:  %lu\n" \
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
  0L,
  0L,
  abort_timeTally,
  0L, //in pisces, RO never abort
  isolation_wait_timeTally,
  0L,
  flush_timeTally,
  upd_dur_commit_timeTally,
  ro_wait_timeTally,
  upd_tx_timeTally,
  ro_tx_timeTally,
  stm_commitsTally, 
  0L,
  0L, 
  0L,
  0L,
  stm_commitsTally,
  stm_ro_commitsTally,
  stm_upd_commitsTally,
  AbortTally, 
  AbortTally,
  0L,
  AbortTally,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L,
  0L);

  printf("Emulated PM latency: %d\n", delay_for_pm);

    pthread_key_delete(global_key_self);
    for (int i = 0; i < MAX_THREADS; i++) {
        if (threads[i])
            threads[i] = NULL;
    }

    MEMBARSTLD();
}


Thread*
TxNewThread ()
{
    Thread* t = (Thread*)calloc(sizeof(Thread),1);
    assert(t);
    return t;
}


void
TxFreeThread (Thread* t)
{
    // AtomicAdd((volatile intptr_t*)((void*)(&ReadOverflowTally)),  t->rdSet.ovf);

    // long wrSetOvf = 0;
    // Log* wr;
    // wr = &t->wrSet;
    // {
    //     wrSetOvf += wr->ovf;
    // }
    // AtomicAdd((volatile intptr_t*)((void*)(&WriteOverflowTally)), wrSetOvf);

    AtomicAdd((volatile intptr_t*)((void*)(&StartTally)),         t->Starts);
    AtomicAdd((volatile intptr_t*)((void*)(&AbortTally)),         t->Aborts);


    AtomicAdd((volatile intptr_t*)((void*)(&flush_timeTally)),         t->flush_time);
    AtomicAdd((volatile intptr_t*)((void*)(&upd_dur_commit_timeTally)),         t->upd_dur_commit_time);
    AtomicAdd((volatile intptr_t*)((void*)(&ro_wait_timeTally)),         t->ro_wait_time);
    AtomicAdd((volatile intptr_t*)((void*)(&isolation_wait_timeTally)),         t->isolation_wait_time);
    AtomicAdd((volatile intptr_t*)((void*)(&ro_tx_timeTally)),         t->ro_tx_time);
    AtomicAdd((volatile intptr_t*)((void*)(&upd_tx_timeTally)),         t->upd_tx_time);
    AtomicAdd((volatile intptr_t*)((void*)(&abort_timeTally)),         t->abort_time);

    AtomicAdd((volatile intptr_t*)((void*)(&stm_commitsTally)),         t->upd_commits);
    AtomicAdd((volatile intptr_t*)((void*)(&stm_upd_commitsTally)),         t->upd_commits);
    AtomicAdd((volatile intptr_t*)((void*)(&stm_commitsTally)),         t->ro_commits);
    AtomicAdd((volatile intptr_t*)((void*)(&stm_ro_commitsTally)),         t->ro_commits);

    AtomicAdd((volatile intptr_t*)((void*)(&AbortTally)),         t->Aborts);



    // FreeList(&(t->rdSet),     pisces_INIT_RDSET_NUM_ENTRY);
    // FreeList(&(t->wrSet),     pisces_INIT_WRSET_NUM_ENTRY);

    // free(t);
}

void
TxInitThread (Thread* t, long id)
{
    /* CCM: so we can access pisces's thread metadata in signal handlers */
    pthread_setspecific(global_key_self, (void*)t);

    memset(t, 0, sizeof(*t));     /* Default value for most members */

    t->UniqID = id;
}

__INLINE__ void
txReset (Thread* Self)
{
    READ_TIMESTAMP(Self->tsBegin_for_stats);
}

__INLINE__ void
txCommitReset (Thread* Self)
{
    txReset(Self);
    Self->Retries = 0;
}



static __inline__ unsigned long long rdtsc(void)
{
  unsigned long long int result=0;
  unsigned long int upper, lower,tmp;
  __asm__ volatile(
                "0:                  \n"
                "\tmftbu   %0           \n"
                "\tmftb    %1           \n"
                "\tmftbu   %2           \n"
                "\tcmpw    %2,%0        \n"
                "\tbne     0b         \n"
                : "=r"(upper),"=r"(lower),"=r"(tmp)
                );
  result = upper;
  result = result<<32;
  result = result|lower;

  return(result);
}


void
TxAbort (Thread* Self)
{
  
    txReset(Self);
    Self->Retries++;
    Self->Aborts++;

    SIGLONGJMP(*Self->envPtr, 1);
    ASSERT(0);
}
