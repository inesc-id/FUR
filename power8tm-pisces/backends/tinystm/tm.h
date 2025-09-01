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
#  include <stm.h>
#  include <wrappers.h>
#  include <stdint.h>
#  include "thread.h"

#    define TM_ARG                      /* nothing */
#    define TM_ARG_ALONE                /* nothing */
#    define TM_ARGDECL                  /* nothing */
#    define TM_ARGDECL_ALONE            /* nothing */
#    define TM_CALLABLE                 /* nothing */

#    define TM_STARTUP(numThread,nb)    stm_init()
#    define TM_SHUTDOWN()               stm_exit() 

#    define TM_THREAD_ENTER()           stm_init_thread() 
#    define TM_THREAD_EXIT()            stm_exit_thread() 

#      define P_MALLOC(size)            malloc(size)
#      define P_FREE(ptr)               free(ptr) 
#      define TM_MALLOC(size)           malloc(size)
#      define TM_FREE(ptr)              free(ptr)
#      define FAST_PATH_FREE(ptr)        
#      define SLOW_PATH_FREE(ptr)       

// #    define TM_BEGIN_EXT(b,ro)
// #    define TM_BEGIN(ro)
// #    define TM_BEGIN_RO()
// #    define TM_END()
// #    define FAST_PATH_RESTART()
// #    define SLOW_PATH_RESTART()

# define AL_LOCK(idx)					          /* nothing */
#    define TM_BEGIN_EXT(b,ro)		    local_exec_mode = 3; \
										do { /* sigsetjmp(local_tx_longjmp, 0); */ \
											stm_tx_attr_t _a = {{.id = thread_getId(), .read_only = ro}}; \
											sigjmp_buf *buf = stm_start(_a); \
											sigsetjmp(*buf, 0); \
										} while(0); \
										//
#    define TM_BEGIN(ro)                local_exec_mode = 3; TM_BEGIN_EXT(0, ro)
#    define TM_BEGIN_RO()               local_exec_mode = 3; TM_BEGIN_EXT(0, 1)
#    define TM_END()                    stm_commit()
#    define FAST_PATH_RESTART()         stm_abort(1)
#    define SLOW_PATH_RESTART()         stm_abort(1)


#    define TM_EARLY_RELEASE(var)       /* nothing */

# define TM_SHARED_READ(var) SLOW_PATH_SHARED_READ(var) 
# define TM_SHARED_READ_P(var) SLOW_PATH_SHARED_READ_P(var)  
# define TM_SHARED_READ_D(var) SLOW_PATH_SHARED_READ_D(var)
# define TM_SHARED_WRITE(var, val) SLOW_PATH_SHARED_WRITE(var, val) 
# define TM_SHARED_WRITE_P(var, val) SLOW_PATH_SHARED_WRITE_P(var, val) 
# define TM_SHARED_WRITE_D(var, val) SLOW_PATH_SHARED_WRITE_D(var, val)

# define FAST_PATH_SHARED_READ(var)            SLOW_PATH_SHARED_READ(var) 
# define FAST_PATH_SHARED_READ_P(var)          SLOW_PATH_SHARED_READ_P(var)  
# define FAST_PATH_SHARED_READ_D(var)          SLOW_PATH_SHARED_READ_D(var)
# define FAST_PATH_SHARED_WRITE(var, val)      SLOW_PATH_SHARED_WRITE(var, val) 
# define FAST_PATH_SHARED_WRITE_P(var, val)    SLOW_PATH_SHARED_WRITE_P(var, val) 
# define FAST_PATH_SHARED_WRITE_D(var, val)    SLOW_PATH_SHARED_WRITE_D(var, val)


#  define SLOW_PATH_SHARED_READ(var)           stm_load((stm_word_t *)&(var))
#  define SLOW_PATH_SHARED_READ_P(var)         stm_load_ptr((volatile void **)&(var))
#  define SLOW_PATH_SHARED_READ_D(var)         stm_load_double((volatile double *)&(var))
#  define SLOW_PATH_SHARED_WRITE(var, val)     stm_store((stm_word_t *)&(var), (stm_word_t)(val))
#  define SLOW_PATH_SHARED_WRITE_P(var, val)   stm_store_ptr((volatile void **)&(var), (void *)(val))
#  define SLOW_PATH_SHARED_WRITE_D(var, val)   stm_store_double((volatile double *)&(var), (double)(val))

#  define TM_LOCAL_WRITE(var, val)             var = val
#  define TM_LOCAL_WRITE_P(var, val)           var = val
#  define TM_LOCAL_WRITE_D(var, val)           var = val

#endif
