#pragma once

//include files to find class PMT;
#define STRINGIFY(x) #x
#define INCLUDFILE(x) STRINGIFY(x.h)
#include INCLUDFILE(TXLIB)

#  include <assert.h>

extern PMT pmt;



#  define TX_START(type, thread_id)      pmt.tm_begin(thread_id);
#  define TX_END                         pmt.tm_end();
#define TM_SHUTDOWN()                   /*nothing*/

#  define FREE(addr, size)               free(addr)
#  define MALLOC(size)                   malloc(size)

#  define TM_CALLABLE                    /* nothing */
#  define TM_ARGDECL_ALONE               /* nothing */
#  define TM_ARGDECL                     /* nothing */
#  define TM_ARG                         /* nothing */
#  define TM_ARG_LAST                    /* nothing */
#  define TM_ARG_ALONE                   /* nothing */
#  define TM_STARTUP()                   /* nothing */

#  define TM_THREAD_ENTER()             pmt.tm_thread_init();
#  define TM_THREAD_EXIT()              /* nothing */



#  define TX_LOAD(addr)           ({pmt.tm_read(addr);*addr;})

#ifdef SP_TM_SHARED_WRITE
//defined for speculative logging
#  define TX_STORE(addr, val)  ({*addr=val; pmt.tm_write(addr);})

#else
//default for undo log
#  define TX_STORE(addr, val)  ({pmt.tm_write(addr); *addr=val;})
#endif