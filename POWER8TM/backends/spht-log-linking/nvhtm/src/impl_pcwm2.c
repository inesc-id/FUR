#include "impl.h"
#include "spins.h"
#include "rdtsc.h"

#include <stdio.h>
#include <limits.h>
#include <stdint.h>
#include <assert.h>

#include "htm_impl.h"

#define LARGER_THAN(_TSi, _TSj, _i, _j) ((_TSi > _TSj) || (_TSi == _TSj && _i > _j))

typedef uintptr_t bit_array_t;

__thread int PCWM2_readonly_tx;

volatile __thread uint64_t PCWM2_readClockVal;
volatile __thread uint64_t PCWM2_myPreviousClock;

volatile __thread int PCWM2_nbPrevThreads;
volatile __thread int *PCWM2_prevThreadsArray;
volatile __thread int *PCWM2_logPosArray;
// volatile __thread int *PCWM2_prevLogPosArray;
volatile __thread uint64_t *PCWM2_TSArray;
// volatile __thread uint64_t *PCWM2_prevTSArray;

volatile __thread int PCWM2_prevThread = -1;
volatile __thread int PCWM2_prevLogPos = -1;
volatile __thread uint64_t PCWM2_prevTS = 0;

volatile __thread
  int PCWM2_writeLogStart, PCWM2_writeLogEnd;

volatile __thread int PCWM2_canJumpToWait = 0;

volatile __thread uint64_t PCWM2_timeFlushTS1 = 0;
volatile __thread uint64_t PCWM2_timeFlushTS2 = 0;

volatile __thread uint64_t PCWM2_timeScanTS1 = 0;
volatile __thread uint64_t PCWM2_timeScanTS2 = 0;

volatile __thread uint64_t PCWM2_timeWaitingTS1 = 0;
volatile __thread uint64_t PCWM2_timeWaitingTS2 = 0;
volatile __thread uint64_t PCWM2_timeWaiting = 0;
volatile __thread uint64_t PCWM2_timeFlushing = 0;
volatile __thread uint64_t PCWM2_timeScanning = 0;
volatile __thread uint64_t PCWM2_timeTX_upd = 0;

volatile __thread uint64_t PCWM2_countCommitPhases = 0;

volatile uint64_t PCWM2_incCommitsPhases = 0;
volatile uint64_t PCWM2_incTimeTotal = 0;
volatile uint64_t PCWM2_incAfterTx = 0;
volatile uint64_t PCWM2_incWaiting = 0;
volatile uint64_t PCWM2_incFlushing = 0;
volatile uint64_t PCWM2_incScanning = 0;
volatile uint64_t PCWM2_incTXTime_upd = 0;

volatile __thread uint64_t PCWM2_durability_RO_spins = 0;

__thread uint64_t PCWM2_timeAbortedTX;
__thread uint64_t PCWM2_timeAbortedTX_TS1;
__thread uint64_t PCWM2_timeAbortedTX_TS2;

// typedef struct {
//   uint64_t a[4];
// } __m256i;
// #define _mm256_load_si256(_m256i_addr) \
//   *(_m256i_addr)
// #define _mm256_store_si256(_m256i_addr, _m256i_val) \
//   *(_m256i_addr) = _m256i_val

void install_bindings_pcwm2()
{
  // on_before_htm_begin  = on_before_htm_begin_pcwm2;
  // on_htm_abort         = on_htm_abort_pcwm2;
  // on_before_htm_write  = on_before_htm_write_8B_pcwm2;
  // on_before_htm_commit = on_before_htm_commit_pcwm2;
  // on_after_htm_commit  = on_after_htm_commit_pcwm2;

  wait_commit_fn = wait_commit_pcwm2;
}

void state_gather_profiling_info_pcwm2(int threadId)
{
  __sync_fetch_and_add(&PCWM2_incCommitsPhases, PCWM2_countCommitPhases);
  __sync_fetch_and_add(&PCWM2_incTimeTotal, timeTotal);
  __sync_fetch_and_add(&PCWM2_incAfterTx, timeAfterTXSuc);
  __sync_fetch_and_add(&PCWM2_incWaiting, PCWM2_timeWaiting);
  __sync_fetch_and_add(&PCWM2_incFlushing, PCWM2_timeFlushing);
  __sync_fetch_and_add(&PCWM2_incScanning, PCWM2_timeScanning);
  __sync_fetch_and_add(&PCWM2_incTXTime_upd, PCWM2_timeTX_upd);
  __sync_fetch_and_add(&timeAbortedTX_global, PCWM2_timeAbortedTX);

	// printf("durability_RO_spins: %d\n", PCWM2_durability_RO_spins);

  timeSGL = 0;
  PCWM2_timeAbortedTX = 0;
  PCWM2_timeTX_upd = 0;
  timeAfterTXSuc = 0;
  PCWM2_timeWaiting = 0;
  timeTotal = 0;
  PCWM2_countCommitPhases = 0;
}

void state_fprintf_profiling_info_pcwm2(char *filename)
{
  FILE *fp = fopen(filename, "a+");
  if (fp == NULL) {
    printf("Cannot open %s! Try to remove it.\n", filename);
    return;
  }
  fseek(fp, 0L, SEEK_END);
  if ( ftell(fp) < 8 ) {
      fprintf(fp, "#%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n",
              "NB_THREADS",
              "NB_COMMIT_PHASES",
              "TIME_TOTAL",
              "TIME_AFTER_TX",
              "TIME_TX",
              "TIME_WAIT",
              "TIME_SGL",
              "TIME_ABORTED_TX",
              "TIME_AFTER_TX_FAIL",
              "TIME_FLUSHING",
              "TIME_SCANNING");
  }
  fprintf(fp, "%i\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\n", gs_appInfo->info.nbThreads,
    PCWM2_incCommitsPhases, PCWM2_incTimeTotal, PCWM2_incAfterTx, PCWM2_incTXTime_upd, PCWM2_incWaiting, timeSGL_global, timeAbortedTX_global, 0L,
    PCWM2_incFlushing, PCWM2_incScanning);
}

void wait_commit_pcwm2(int threadId)
{
  MEASURE_TS(PCWM2_timeWaitingTS1);
  volatile uintptr_t myTS = gs_ts_array[threadId].pcwm.ts;
  volatile uint64_t snapshotTS[gs_appInfo->info.nbThreads];

  uint64_t ts;
  for (int j = 0; j < PCWM2_nbPrevThreads; ++j) {
    int i = PCWM2_prevThreadsArray[j];
    // either the unstable TS become stable OR is still unstable but larger than myTS
    // in the second case check the PCWM2_prevTS
    do {
      snapshotTS[i] = gs_ts_array[i].pcwm.ts;
      // _mm_pause();
    } while (!(LARGER_THAN(zeroBit63(snapshotTS[i]), myTS, i, threadId) || isBit63One(snapshotTS[i])));

#ifndef DISABLE_PCWM_OPT
    if (LARGER_THAN(zeroBit63(snapshotTS[i]), PCWM2_readClockVal, i, threadId)
        && __atomic_load_n(&gs_ts_array[i].pcwm.isUpdate, __ATOMIC_ACQUIRE) == 1) {
      PCWM2_canJumpToWait = 1;
    }
#endif

    ts = P_write_log[i][PCWM2_logPosArray[j]];

    // read-only transactions break this

    if (isBit63One(ts)) {
      continue;
    }

    if (LARGER_THAN(PCWM2_readClockVal, ts, threadId, i) && LARGER_THAN(ts, PCWM2_prevTS, i, PCWM2_prevThread)) {
      PCWM2_prevTS = ts;
      PCWM2_prevThread = i;
      PCWM2_prevLogPos = PCWM2_logPosArray[j];
    }
  }

  // if (PCWM2_nbPrevThreads > 0) {
  //   printf("[%i] TS %lx > %lx (t%i): %i unstable t%i pTS=%lx TS=%lx logPos = %lx/%lx\n",
  //     threadId, PCWM2_readClockVal & 0xFFFFFF, PCWM2_prevTS & 0xFFFFFF, PCWM2_prevThread, PCWM2_nbPrevThreads,
  //     PCWM2_prevThreadsArray[0], PCWM2_prevTSArray[0] & 0xFFFFFF, PCWM2_TSArray[0] & 0xFFFFFF,
  //     ts , P_write_log[PCWM2_prevThreadsArray[0]][PCWM2_logPosArray[0]]  & 0xFFFFFF);
  // } else {
  //   printf("[%i] TS %lx > %lx (t%i) \n",
  //     threadId, PCWM2_readClockVal & 0xFFFFFF, PCWM2_prevTS & 0xFFFFFF, PCWM2_prevThread);
  // }

  MEASURE_TS(PCWM2_timeWaitingTS2);
  INC_PERFORMANCE_COUNTER(PCWM2_timeWaitingTS1, PCWM2_timeWaitingTS2, PCWM2_timeWaiting);
}

