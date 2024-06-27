#ifndef __MOCK_TX_API_H
#define __MOCK_TX_API_H

#include <stddef.h>

/* Define the TX_THREAD control block extensions for this port. The main reason
   for the multiple macros is so that backward compatibility can be maintained with
   existing ThreadX kernel awareness modules.  */

#define TX_THREAD_EXTENSION_0
#define TX_THREAD_EXTENSION_1
#ifdef  TX_ENABLE_IAR_LIBRARY_SUPPORT
#define TX_THREAD_EXTENSION_2           VOID    *tx_thread_iar_tls_pointer;
#elif defined(__ghs__)
#define TX_THREAD_EXTENSION_2           VOID *  tx_thread_eh_globals;                           \
                                        int     Errno;             /* errno.  */                \
                                        char *  strtok_saved_pos;  /* strtok() position.  */
#else
#define TX_THREAD_EXTENSION_2
#endif


#define TX_THREAD_EXTENSION_3

#ifndef TX_THREAD_USER_EXTENSION
#define TX_THREAD_USER_EXTENSION
#endif

/* Define the port extensions of the remaining ThreadX objects.  */

#define TX_BLOCK_POOL_EXTENSION
#define TX_BYTE_POOL_EXTENSION
#define TX_EVENT_FLAGS_GROUP_EXTENSION
#define TX_MUTEX_EXTENSION
#define TX_QUEUE_EXTENSION
#define TX_SEMAPHORE_EXTENSION
#define TX_TIMER_EXTENSION

#define VOID                                    void
typedef char                                    CHAR;
typedef unsigned char                           UCHAR;
typedef int                                     INT;
typedef unsigned int                            UINT;
typedef long                                    LONG;
typedef unsigned long                           ULONG;
typedef unsigned long long                      ULONG64;
typedef short                                   SHORT;
typedef unsigned short                          USHORT;
#define ULONG64_DEFINED

#endif /*__MOCK_TX_API_H */