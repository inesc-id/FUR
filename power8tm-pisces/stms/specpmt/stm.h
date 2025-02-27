/* =============================================================================
 *
 * stm.h
 *
 * User program interface for STM. For an STM to interface with STAMP, it needs
 * to have its own stm.h for which it redefines the macros appropriately.
 *
 * =============================================================================
 *
 * Author: Chi Cao Minh
 *
 * =============================================================================
 */


#ifndef STM_H
#define STM_H 1


//include "include/sptx_naive.h"
#include "util.h"
#include "stdint.h"

#define STM_THREAD_T                    Thread
#define STM_SELF                        Self
#define STM_RO_FLAG                     ROFlag

#  include <setjmp.h>
#  define STM_JMPBUF_T                  sigjmp_buf
#  define STM_JMPBUF                    buf


#define STM_VALID()                     (1)
#define STM_RESTART()                   
//TxAbort(STM_SELF)

#define STM_STARTUP()                   
//TxOnce()
#define STM_SHUTDOWN()                  
//TxShutdown()

#define STM_NEW_THREAD()                TxNewThread()
#define STM_INIT_THREAD(t, id)          pmt.tm_thread_init()
#define STM_INIT_THREAD(t, id)          TxInitThread(t, id)
#define STM_FREE_THREAD(t)              TxFreeThread(t)


#define STM_VALID()                     (1)
#define STM_RESTART()                   TxAbort(STM_SELF)

#define STM_STARTUP()                   TxOnce()
#define STM_SHUTDOWN()                  TxShutdown()







#  define STM_BEGIN(isReadOnly)         do { \
                                            STM_JMPBUF_T STM_JMPBUF; \
                                            sigsetjmp(STM_JMPBUF, 1); \
                                            pmt.tm_begin(); \
                                        } while (0) /* enforce comma */

#define STM_BEGIN_RD()                  STM_BEGIN(1)
#define STM_BEGIN_WR()                  STM_BEGIN(0)
#define TX_END_HYBRID_FIRST_STEP()      TxValidate(STM_SELF)
#define TX_END_HYBRID_LAST_STEP(clock)  TxFinalize(STM_SELF, clock)
#define TX_AFTER_FINALIZE()             TxResetAfterFinalize (STM_SELF)
#define HYBRID_STM_END()                TxCommitSTM(STM_SELF)
#define STM_END()                       pmt.tm_end()

typedef volatile intptr_t               vintp;

#define STM_HYBRID_READ(varPtr)         ({pmt.tm_read(&var); var;})

#define STM_READ(var)                   ({pmt.tm_read(&var); var;})
#define STM_READ_F(var)                 ({pmt.tm_read(&var); var;})
#define STM_READ_P(var)                 ({pmt.tm_read(&var); var;})

#define STM_HYBRID_WRITE(varPtr, val)   ({var = val; pmt.tm_write(var); var;})
#define STM_WRITE(var, val)             ({var = val; pmt.tm_write(var); var;})
#define STM_WRITE_F(var, val)           ({var = val; pmt.tm_write(var); var;})
#define STM_WRITE_P(var, val)           ({var = val; pmt.tm_write(var); var;})

#define STM_LOCAL_WRITE(var, val)       ({var = val; pmt.tm_write(var); var;})
#define STM_LOCAL_WRITE_F(var, val)     ({var = val; pmt.tm_write(var); var;})
#define STM_LOCAL_WRITE_P(var, val)     ({var = val; pmt.tm_write(var); var;})

#define HTM_INC_CLOCK()                 
#define HTM_READ_CLOCK()                



#endif /* STM_H */


/* =============================================================================
 *
 * End of stm.h
 *
 * =============================================================================
 */
