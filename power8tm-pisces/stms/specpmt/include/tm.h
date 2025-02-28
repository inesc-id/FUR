#pragma once

#ifndef SPTM_H
#define SPTM_H 1

//must include the following file to find SP_TM_SHARED_WRITE
#define STRINGIFY(x) #x
#define INCLUDFILE(x) STRINGIFY(x.h)
#include INCLUDFILE(TXLIB)

#  include <assert.h>

extern PMT pmt;

#  define TM_ARG                        /* nothing */
#  define TM_ARG_ALONE                  /* nothing */
#  define TM_ARGDECL                    /* nothing */
#  define TM_ARGDECL_ALONE              /* nothing */
#  define TM_CALLABLE                   /* nothing */

#  define TM_STARTUP(numThread)         /* nothing */
#  define TM_SHUTDOWN()                 /* nothing */

#  define TM_THREAD_ENTER()             pmt.tm_thread_init();
#  define TM_THREAD_EXIT()              /* nothing */

#  define P_MALLOC(size)              malloc(size)
#  define P_FREE(ptr)                 free(ptr)
#  define TM_MALLOC(size)             malloc(size)
#  define TM_FREE(ptr)                free(ptr)

#  define TM_BEGIN()                    pmt.tm_begin(thread_getId());
#  define TM_BEGIN_RO()                 pmt.tm_begin(thread_getId());
#  define TM_END()                      pmt.tm_end();
#  define TM_RESTART()                  assert(0)

#  define TM_EARLY_RELEASE(var)         /* nothing */

#  define TM_SHARED_READ(var)           ({pmt.tm_read(&var); var;})
#  define TM_SHARED_READ_P(var)         ({pmt.tm_read(&var); var;})
#  define TM_SHARED_READ_F(var)         ({pmt.tm_read(&var); var;})

#ifdef SP_TM_SHARED_WRITE
//defined for speculative logging
#  define TM_SHARED_WRITE(var, val)     ({var = val; pmt.tm_write(var); var;})
#  define TM_SHARED_WRITE_P(var, val)   ({var = val; pmt.tm_write(var); var;})
#  define TM_SHARED_WRITE_F(var, val)   ({var = val; pmt.tm_write(var); var;})

#  define TM_LOCAL_WRITE(var, val)      ({var = val; pmt.tm_write(var); var;})
#  define TM_LOCAL_WRITE_P(var, val)    ({var = val; pmt.tm_write(var); var;})
#  define TM_LOCAL_WRITE_F(var, val)    ({var = val; pmt.tm_write(var); var;})

#else
//default for undo log
#  define TM_SHARED_WRITE(var, val)     ({pmt.tm_write(var); var = val; var;})
#  define TM_SHARED_WRITE_P(var, val)   ({pmt.tm_write(var); var = val; var;})
#  define TM_SHARED_WRITE_F(var, val)   ({pmt.tm_write(var); var = val; var;})

#  define TM_LOCAL_WRITE(var, val)      ({pmt.tm_write(var); var = val; var;})
#  define TM_LOCAL_WRITE_P(var, val)    ({pmt.tm_write(var); var = val; var;})
#  define TM_LOCAL_WRITE_F(var, val)    ({pmt.tm_write(var); var = val; var;})
#endif

#endif //SPTM_H