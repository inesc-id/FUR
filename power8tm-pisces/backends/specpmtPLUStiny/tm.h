#ifndef TM_H
#define TM_H 1

#ifdef HAVE_CONFIG_H
# include "STAMP_config.h"
#endif

#  include <stdio.h>

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

# define SETUP_NUMBER_TASKS(n)
# define SETUP_NUMBER_THREADS(n)
# define PRINT_STATS()


#  include <string.h>
#  include <tinystm/include/stm.h>
#  include <tinystm/include/wrappers.h>
#  include <specpmt/stm.h>
#  include <stdint.h>
#  include "thread.h"
#  include "specpmt/specpmt.h"
#  include "specpmt/include/sptx_naive.h"

extern PMT pmt;

#    define TM_ARG                      STM_SELF,
#    define TM_ARG_ALONE                STM_SELF
#    define TM_ARGDECL                  STM_THREAD_T* TM_ARG
#    define TM_ARGDECL_ALONE            STM_THREAD_T* TM_ARG_ALONE
#    define TM_CALLABLE                 /* nothing */

#    define TM_STARTUP(numThread,nb)    stm_init(); STM_STARTUP()
#    define TM_SHUTDOWN()               stm_exit(); STM_SHUTDOWN()

#    define TM_THREAD_ENTER()           stm_init_thread(); TM_ARGDECL_ALONE = STM_NEW_THREAD(); \
                                        STM_INIT_THREAD(TM_ARG_ALONE, thread_getId())
#    define TM_THREAD_EXIT()            stm_exit_thread(); STM_FREE_THREAD(TM_ARG_ALONE)

#      define P_MALLOC(size)            malloc(size)
#      define P_FREE(ptr)               free(ptr) 
#      define TM_MALLOC(size)           malloc(size)
#      define FAST_PATH_FREE(ptr)        
#      define SLOW_PATH_FREE(ptr)       

# define AL_LOCK(idx)					          /* nothing */
#    define TM_BEGIN_EXT(b,ro)		      local_exec_mode = 3; \
																				do { /* sigsetjmp(local_tx_longjmp, 0); */ \
																					stm_tx_attr_t _a = {{.id = thread_getId(), .read_only = ro}}; \
																					pmt.tm_begin(thread_getId()); /* TODO: maybe before stm_start(_a)? maybe after the jump? */ \
																					sigjmp_buf *buf = stm_start(_a); \
																					sigsetjmp(*buf, 0); /* On abort the STM rolls back and jumps here */ \
																				} while(0); \
																				//
#    define TM_BEGIN(ro)                local_exec_mode = 3; TM_BEGIN_EXT(0, ro)
#    define TM_BEGIN_RO()               local_exec_mode = 3; TM_BEGIN_EXT(0, 1)
#    define TM_END()                    stm_commit(); /* TODO: SpecPMT commits after tiny? */ STM_END()
#    define FAST_PATH_RESTART()         stm_abort(1); STM_RESTART()
#    define SLOW_PATH_RESTART()         stm_abort(1); STM_RESTART()


#    define TM_EARLY_RELEASE(var)       /* nothing */


# define FAST_PATH_SHARED_READ(var)         SLOW_PATH_SHARED_READ(var) 
# define FAST_PATH_SHARED_READ_P(var)       SLOW_PATH_SHARED_READ_P(var)  
# define FAST_PATH_SHARED_READ_D(var)       SLOW_PATH_SHARED_READ_D(var)
# define FAST_PATH_SHARED_WRITE(var, val)   SLOW_PATH_SHARED_WRITE(var, val) 
# define FAST_PATH_SHARED_WRITE_P(var, val) SLOW_PATH_SHARED_WRITE_P(var, val) 
# define FAST_PATH_SHARED_WRITE_D(var, val) SLOW_PATH_SHARED_WRITE_D(var, val)

// TODO: had to change the macro --> this is a GNU extension, also note that there are 2 instanciations of "var" and "val"
#  define SLOW_PATH_SHARED_READ(var)           ({ __typeof__(&var) _var = &(var); \
																									stm_load((stm_word_t *)_var); /* TODO: tiny load before SpecPMT */ pmt.tm_read(_var); var; })
#  define SLOW_PATH_SHARED_READ_P(var)         ({ __typeof__(&var) _var = &(var); \
																									stm_load_ptr((volatile void **)_var); pmt.tm_read(_var); var; })
#  define SLOW_PATH_SHARED_READ_D(var)         ({ __typeof__(&var) _var = &(var); \
																									stm_load_double((volatile double *)_var); pmt.tm_read(_var); var; })
#  define SLOW_PATH_SHARED_WRITE(var, val)     ({ __typeof__(&var) _var = &(var); \
																									__typeof__(val) _val = val; \
																									stm_store((stm_word_t *)_var, (stm_word_t)(_val)); pmt.tm_write(var); var; })
#  define SLOW_PATH_SHARED_WRITE_P(var, val)   ({ __typeof__(&var) _var = &(var); \
																									__typeof__(val) _val = val; \
																									stm_store_ptr((volatile void **)_var, (void *)(_val)); pmt.tm_write(var); var; })
#  define SLOW_PATH_SHARED_WRITE_D(var, val)   ({ __typeof__(&var) _var = &(var); \
																									__typeof__(val) _val = val; \
																									stm_store_double((volatile double *)_var, (double)(_val)); pmt.tm_write(var); var; })

#  define TM_LOCAL_WRITE(var, val)      /* Tiny does nothing on local writes */ STM_LOCAL_WRITE(var, val)
#  define TM_LOCAL_WRITE_P(var, val)    STM_LOCAL_WRITE_P(var, val)
#  define TM_LOCAL_WRITE_D(var, val)    STM_LOCAL_WRITE_F(var, val)

#endif
