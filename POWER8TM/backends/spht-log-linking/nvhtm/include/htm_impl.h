#ifndef HTM_IMPL_H_GUARD
#define HTM_IMPL_H_GUARD

// before include
//#define HTM_SGL_INIT_BUDGET /* default */20

#define HTM_SGL_INIT_BUDGET 10
#include "htm_retry_template.h"
#include "impl_pcwm2.h"

extern __thread uint64_t PCWM2_timeAbortedTX;
extern __thread uint64_t PCWM2_timeAbortedTX_TS1;
extern __thread uint64_t PCWM2_timeAbortedTX_TS2;

#undef AFTER_ABORT
#define AFTER_ABORT(tid, budget, status) \
  if (!isCraftySet || (isCraftySet && !crafty_isValidate)) {\
    MEASURE_TS(PCWM2_timeAbortedTX_TS2); \
    INC_PERFORMANCE_COUNTER(PCWM2_timeAbortedTX_TS1, PCWM2_timeAbortedTX_TS2, PCWM2_timeAbortedTX); \
    PCWM2_timeAbortedTX_TS1 = PCWM2_timeAbortedTX_TS2; \
  } \
  MACRO_PCWM2_on_htm_abort_pcwm2(tid)

#undef BEFORE_SGL_BEGIN
#define BEFORE_SGL_BEGIN(HTM_SGL_tid) MEASURE_TS(timeSGL_TS1);

#undef AFTER_SGL_COMMIT
#define AFTER_SGL_COMMIT(HTM_SGL_tid) \
  MEASURE_TS(timeSGL_TS2); \
  INC_PERFORMANCE_COUNTER(timeSGL_TS1, timeSGL_TS2, timeSGL);

#undef BEFORE_CHECK_BUDGET
#define BEFORE_CHECK_BUDGET(HTM_SGL_budget) MEASURE_TS(PCWM2_timeAbortedTX_TS1);

#undef BEFORE_HTM_BEGIN
#define BEFORE_HTM_BEGIN(_tid, _budget) \
	MACRO_PCWM2_on_before_htm_begin_pcwm2(_tid, PCWM2_readonly_tx) \
//

#undef BEFORE_HTM_COMMIT
#define BEFORE_HTM_COMMIT(_tid, _budget) \
	MACRO_PCWM2_on_before_htm_commit_pcwm2(_tid) /* onBeforeHtmCommit(HTM_SGL_tid); */ \
//

#undef AFTER_HTM_COMMIT
#define AFTER_HTM_COMMIT(_tid, _budget) \
	on_after_htm_commit_pcwm2(_tid) /* on_after_htm_commit(HTM_SGL_tid); */ \
//

#undef BEFORE_SGL_COMMIT
#define BEFORE_SGL_COMMIT(_tid) \
	MACRO_PCWM2_on_before_sgl_commit_pcwm2(_tid) \
//

#endif /* HTM_IMPL_H_GUARD */
