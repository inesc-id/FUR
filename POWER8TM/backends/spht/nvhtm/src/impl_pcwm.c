#include "impl.h"
#include "spins.h"
#include "rdtsc.h"

#include <stdio.h>
#include <limits.h>
#include <stdint.h>
#include <errno.h>

#include "htm_impl.h"

// use power2: 0 --> does on every sample: 1 --> does on none
// #define PCWM_NB_SAMPLES 0

#define LARGER_THAN(_TSi, _TSj, _i, _j) ((_TSi > _TSj) || (_TSi == _TSj && _i > _j))

typedef uintptr_t bit_array_t;

volatile __thread uint64_t PCWM_readClockVal;

volatile __thread
  int PCWM_writeLogStart, PCWM_writeLogEnd;

volatile __thread int PCWM_canJumpToWait = 0;

// volatile __thread uint64_t nbSamples;
// volatile __thread uint64_t nbSamplesDone;

volatile __thread uint64_t PCWM_timeFlushTS1 = 0;
volatile __thread uint64_t PCWM_timeFlushTS2 = 0;

volatile __thread uint64_t PCWM_timeWaitingTS1 = 0;
volatile __thread uint64_t PCWM_timeWaitingTS2 = 0;
volatile __thread uint64_t PCWM_timeWaiting = 0;
volatile __thread uint64_t PCWM_timeFlushing = 0;
volatile __thread uint64_t PCWM_timeTX_upd = 0;
volatile __thread uint64_t PCWM_timeTX_ro = 0;
volatile __thread uint64_t PCWM_ro_durability_wait_time = 0;
volatile __thread uint64_t PCWM_dur_commit_time = 0;
volatile __thread uint64_t PCWM_ro_durability_wait_spins = 0;

__thread int PCWM_readonly_tx;

volatile __thread uint64_t PCWM_countUpdCommitPhases = 0;
volatile __thread uint64_t PCWM_countROCommitPhases = 0;

//volatile uint64_t PCWM_incNbSamples = 0;
volatile uint64_t PCWM_incCommitsPhases = 0;
volatile uint64_t PCWM_incROCommitsPhases = 0;
volatile uint64_t PCWM_incTimeTotal = 0;
volatile uint64_t PCWM_incAfterTx = 0;
volatile uint64_t PCWM_incWaiting = 0;
volatile uint64_t PCWM_incFlushing = 0;
volatile uint64_t PCWM_incTXTime_upd = 0;
volatile uint64_t PCWM_incTXTime_ro = 0;
volatile uint64_t PCWM_inc_ro_durability_wait_time = 0;
volatile uint64_t PCWM_inc_dur_commit_time = 0;




#ifdef DETAILED_BREAKDOWN_PROFILING
/* Breakdown of the main stages (JOAO) */
#define MAX_PROFILE_COUNT 10000
static uint64_t **ro_durability_wait_duration;
static uint64_t *ro_durability_wait_count;
static uint64_t **upd_after_commit_duration;
static uint64_t **upd_log_flush_duration;
static uint64_t *upd_after_commit_count;
#endif




void init_stats_pcwm() {
  #ifdef DETAILED_BREAKDOWN_PROFILING
  ro_durability_wait_duration = (uint64_t**)malloc(gs_appInfo->info.nbThreads*sizeof(uint64_t*));
  ro_durability_wait_count = (uint64_t*)malloc(gs_appInfo->info.nbThreads*sizeof(uint64_t));
  upd_after_commit_duration = (uint64_t**)malloc(gs_appInfo->info.nbThreads*sizeof(uint64_t*));
  upd_log_flush_duration = (uint64_t**)malloc(gs_appInfo->info.nbThreads*sizeof(uint64_t*));
  upd_after_commit_count = (uint64_t*)malloc(gs_appInfo->info.nbThreads*sizeof(uint64_t));
  for (int i=0; i<gs_appInfo->info.nbThreads; i++) {
    ro_durability_wait_duration[i] = (uint64_t*)malloc(MAX_PROFILE_COUNT*sizeof(uint64_t));
    ro_durability_wait_count[i] = 0;
    upd_after_commit_duration[i] = (uint64_t*)malloc(MAX_PROFILE_COUNT*sizeof(uint64_t));
    upd_log_flush_duration[i] = (uint64_t*)malloc(MAX_PROFILE_COUNT*sizeof(uint64_t));
    upd_after_commit_count[i] = 0;
  }
#endif
}

void state_gather_profiling_info_pcwm(int threadId)
{
  __sync_fetch_and_add(&PCWM_incCommitsPhases, PCWM_countUpdCommitPhases);
  __sync_fetch_and_add(&PCWM_incROCommitsPhases, PCWM_countROCommitPhases);
  // __sync_fetch_and_add(&PCWM_incNbSamples, nbSamplesDone);
  __sync_fetch_and_add(&PCWM_incTimeTotal, timeTotal);
  __sync_fetch_and_add(&PCWM_incAfterTx, timeAfterTXSuc);
  __sync_fetch_and_add(&PCWM_incWaiting, PCWM_timeWaiting);
  __sync_fetch_and_add(&PCWM_incFlushing, PCWM_timeFlushing);
  __sync_fetch_and_add(&PCWM_incTXTime_upd, PCWM_timeTX_upd);
  __sync_fetch_and_add(&PCWM_incTXTime_ro, PCWM_timeTX_ro);
  __sync_fetch_and_add(&PCWM_inc_dur_commit_time, PCWM_dur_commit_time);
  __sync_fetch_and_add(&PCWM_inc_ro_durability_wait_time, PCWM_ro_durability_wait_time);
  __sync_fetch_and_add(&timeSGL_global, timeSGL);
  __sync_fetch_and_add(&timeAbortedTX_global, timeAbortedUpdTX);
  __sync_fetch_and_add(&timeAbortedTX_global, timeAbortedROTX);

  stats_array[threadId].htm_commits = PCWM_countUpdCommitPhases + PCWM_countROCommitPhases;
  stats_array[threadId].flush_time = PCWM_timeFlushing;
  stats_array[threadId].tx_time_upd_txs = PCWM_timeTX_upd;
  stats_array[threadId].tx_time_ro_txs = PCWM_timeTX_ro;
  stats_array[threadId].dur_commit_time = PCWM_dur_commit_time;
  stats_array[threadId].readonly_durability_wait_time = PCWM_ro_durability_wait_time;
  stats_array[threadId].time_aborted_upd_txs = timeAbortedUpdTX;
  stats_array[threadId].time_aborted_ro_txs = timeAbortedROTX;
  stats_array[threadId].ro_durability_wait_spins = PCWM_ro_durability_wait_spins;

  timeSGL = 0;
  timeAbortedUpdTX = 0;
  timeAbortedROTX = 0;
  PCWM_timeTX_upd = 0;
  PCWM_timeTX_ro = 0;
  PCWM_dur_commit_time = 0;
  PCWM_ro_durability_wait_time = 0;
  timeAfterTXSuc = 0;
  PCWM_timeWaiting = 0;
  timeTotal = 0;
  PCWM_countUpdCommitPhases = 0;
  PCWM_countROCommitPhases = 0;
}

void state_fprintf_profiling_info_pcwm(char *filename)
{
  FILE *fp = fopen(filename, "a+");
  if (fp == NULL) {
    printf("Cannot open %s! Try to remove it.\n", filename);
    return;
  }
  fseek(fp, 0L, SEEK_END);
  if ( ftell(fp) < 8 ) {
      fprintf(fp, "#%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n",
              "NB_THREADS",
              "NB_COMMIT_PHASES",
              "TIME_TOTAL",
              "TIME_AFTER_TX",
              "TIME_TX",
              "TIME_WAIT",
              "TIME_SGL",
              "TIME_ABORTED_TX",
              "TIME_AFTER_TX_FAIL",
              "TIME_FLUSH_LOG");
  }
  fprintf(fp, "%i\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\n", gs_appInfo->info.nbThreads,
    PCWM_incCommitsPhases, PCWM_incTimeTotal, PCWM_incAfterTx, PCWM_incTXTime_upd, PCWM_incWaiting, timeSGL_global, timeAbortedTX_global, 0L, PCWM_incFlushing);

  fclose(fp);

  #ifdef DETAILED_BREAKDOWN_PROFILING
  char fname_aux[200];
  snprintf(fname_aux, 200, "detailed.%s", filename);
  fp = fopen(fname_aux, "w"); 
  if (fp == NULL) {
    printf("Cannot open %s! Try to remove it.\n", filename);
    return;
  }

  fprintf(fp, "STAGE: RO durability wait\n");  
  for (int i=0; i<gs_appInfo->info.nbThreads; i++) {
    int c = ro_durability_wait_count[i];
    for (int k = 0; k<c; k++) {
      fprintf(fp, "%lu\n", ro_durability_wait_duration[i][k]);
    }
  }
  fprintf(fp, "STAGE: Update post-commit until return\tSTAGE: Update flush log entries\n");
  for (int i=0; i<gs_appInfo->info.nbThreads; i++) {
    int c = upd_after_commit_count[i];
    for (int k = 0; k<c; k++) {
      fprintf(fp, "%lu\t%lu\n", upd_after_commit_duration[i][k], upd_log_flush_duration[i][k]);
    }
  }
  fclose(fp);
#endif
}

static inline void fetch_log(int threadId)
{
  write_log_thread[PCWM_writeLogEnd] = 0;
  write_log_thread[(PCWM_writeLogEnd + 8) & (gs_appInfo->info.allocLogSize - 1)] = 0;
}

void on_before_htm_begin_pcwm(int threadId, int ro)
{
  onBeforeWrite = on_before_htm_write;
  onBeforeHtmCommit = on_before_htm_commit;
  write_log_thread = &(P_write_log[threadId][0]);

  // nbSamples++;
  // if ((nbSamples & (PCWM_NB_SAMPLES - 1)) == (PCWM_NB_SAMPLES - 1)) {
  //   nbSamplesDone++;
  // }
  
  PCWM_writeLogEnd = PCWM_writeLogStart = G_next[threadId].log_ptrs.write_log_next;
  fetch_log(threadId);

  if (log_replay_flags & LOG_REPLAY_CONCURRENT) {
    // check space in the log
    const int MIN_SPACE_IN_LOG = 128;
    long totSize = gs_appInfo->info.allocLogSize;
    volatile long start = G_next[threadId].log_ptrs.write_log_start;
    long next = G_next[threadId].log_ptrs.write_log_next;
    long nextExtra = (next + MIN_SPACE_IN_LOG) & (totSize - 1);
    volatile long size = next >= start ? next - start : totSize - (start - next);
    long extraSize = nextExtra > next ? nextExtra - next : totSize - (next - nextExtra);
    while (size + extraSize > totSize) {
      // wait the background replayer to gather some transactions
      start = __atomic_load_n(&G_next[threadId].log_ptrs.write_log_start, __ATOMIC_ACQUIRE);
      size = next >= start ? next - start : totSize - (start - next);
      extraSize = nextExtra > next ? nextExtra - next : totSize - (next - nextExtra);
    }
  }

  if (gs_ts_array[threadId].pcwm.isUpdate != 0)
    __atomic_store_n(&gs_ts_array[threadId].pcwm.isUpdate, 0, __ATOMIC_RELEASE);
  PCWM_readonly_tx = ro;
  if (!PCWM_readonly_tx)
    __atomic_store_n(&gs_ts_array[threadId].pcwm.ts, rdtsc(), __ATOMIC_RELEASE);
  else {
    //For RO txs (which run non-transactionally), we set their ts to infinity
    if (gs_ts_array[threadId].pcwm.ts != onesBit63(0))
    __atomic_store_n(&gs_ts_array[threadId].pcwm.ts, onesBit63(0), __ATOMIC_RELEASE);
  }

}

void on_htm_abort_pcwm(int threadId)
{
  if (!PCWM_readonly_tx)
    __atomic_store_n(&gs_ts_array[threadId].pcwm.ts, onesBit63(0), __ATOMIC_RELEASE);
    // __atomic_store_n(&gs_ts_array[threadId].pcwm.ts, rdtsc(), __ATOMIC_RELEASE);
}

void on_before_htm_write_8B_pcwm(int threadId, void *addr, uint64_t val)
{
  write_log_thread[PCWM_writeLogEnd] = (uint64_t)addr;
  PCWM_writeLogEnd = (PCWM_writeLogEnd + 1) & (gs_appInfo->info.allocLogSize - 1);
  write_log_thread[PCWM_writeLogEnd] = (uint64_t)val;
  PCWM_writeLogEnd = (PCWM_writeLogEnd + 1) & (gs_appInfo->info.allocLogSize - 1);
}

void on_before_htm_commit_pcwm(int threadId)
{
  PCWM_readClockVal = rdtscp();
}

void on_before_sgl_commit_pcwm(int threadId) {
  // printf("called on_before_sgl_commit (%d)\n", loc_var.exec_mode);
  smart_close_log_pcwm(
  /* commit value */ onesBit63(PCWM_readClockVal),
  /* marker position */ (uint64_t*)&(write_log_thread[PCWM_writeLogEnd])
  );
  FENCE_PREV_FLUSHES();
  //emulating durmarker flush
  FLUSH_CL(&P_last_safe_ts->ts);
  FENCE_PREV_FLUSHES();

  //just to be safe
  __atomic_store_n(&gs_ts_array[threadId].pcwm.ts, onesBit63(0), __ATOMIC_RELEASE);

  return ;
}

void wait_commit_pcwm(int threadId)
{
  // if ((nbSamples & (PCWM_NB_SAMPLES - 1)) == (PCWM_NB_SAMPLES - 1)) {
    MEASURE_TS(PCWM_timeWaitingTS1);
  // }
  volatile uintptr_t myTS = gs_ts_array[threadId].pcwm.ts;
  volatile uint64_t snapshotTS[gs_appInfo->info.nbThreads];
  volatile uint64_t snapshotDiscard[gs_appInfo->info.nbThreads];
int i;
  for (i = 0; i < gs_appInfo->info.nbThreads; ++i) {
    snapshotDiscard[i] = 0;
  }

  for (i = 0; i < gs_appInfo->info.nbThreads; ++i) {
    if (i == threadId) continue;
    snapshotTS[i] = gs_ts_array[i].pcwm.ts; // puts too much presure on this cache line
    if (LARGER_THAN(zeroBit63(snapshotTS[i]), myTS, i, threadId) || isBit63One(snapshotTS[i])) {
      snapshotDiscard[i] = 1;
    }
#ifndef DISABLE_PCWM_OPT
    if (LARGER_THAN(zeroBit63(snapshotTS[i]), PCWM_readClockVal, i, threadId)
        && __atomic_load_n(&gs_ts_array[i].pcwm.isUpdate, __ATOMIC_ACQUIRE) == 1) {
      PCWM_canJumpToWait = 1;
    }
#endif
  }

  for (i = 0; i < gs_appInfo->info.nbThreads; ++i) {
    if (i == threadId || snapshotDiscard[i] == 1) continue;
    do {
      snapshotTS[i] = gs_ts_array[i].pcwm.ts;
      // _mm_pause();
    } while (!(LARGER_THAN(zeroBit63(snapshotTS[i]), myTS, i, threadId) || isBit63One(snapshotTS[i]))
      /* && !gs_appInfo->info.isExit */);
  }

  // if ((nbSamples & (PCWM_NB_SAMPLES - 1)) == (PCWM_NB_SAMPLES - 1)) {
    MEASURE_TS(PCWM_timeWaitingTS2);
    INC_PERFORMANCE_COUNTER(PCWM_timeWaitingTS1, PCWM_timeWaitingTS2, PCWM_timeWaiting);
  // }
}

void install_bindings_pcwm()
{
  // on_before_htm_begin  = on_before_htm_begin_pcwm;
  // on_htm_abort         = on_htm_abort_pcwm;
  // on_before_htm_write  = on_before_htm_write_8B_pcwm;
  // on_before_htm_commit = on_before_htm_commit_pcwm;
  // on_after_htm_commit  = on_after_htm_commit_pcwm;
  // on_before_sgl_commit = on_before_sgl_commit_pcwm;

  wait_commit_fn = wait_commit_pcwm;
}