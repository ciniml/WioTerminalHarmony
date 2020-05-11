/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This file contains the "main" function for a project.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include <stdint.h>
#include "definitions.h"                // SYS function prototypes
#include "FreeRTOS.h"

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

/* Initialize segments */
extern uint32_t _sfixed;
extern uint32_t _efixed;
extern uint32_t _etext;
extern uint32_t _srelocate;
extern uint32_t _erelocate;
extern uint32_t _szero;
extern uint32_t _ezero;
extern uint32_t _sstack;
extern uint32_t _estack;
/**
 * \brief This is the code that gets called on processor reset.
 * To initialize the device, and call the main() routine.
 */
void Reset_Handler(void)
{
        uint32_t *pSrc, *pDest;

        /* Initialize the relocate segment */
        pSrc = &_etext;
        pDest = &_srelocate;

        if (pSrc != pDest) {
                for (; pDest < &_erelocate;) {
                        *pDest++ = *pSrc++;
                }
        }

        /* Clear the zero segment */
        for (pDest = &_szero; pDest < &_ezero;) {
                *pDest++ = 0;
        }

        /* Set the vector table base address */
        pSrc = (uint32_t *) & _sfixed;
        SCB->VTOR = ((uint32_t) pSrc & SCB_VTOR_TBLOFF_Msk);

#if __FPU_USED
        /* Enable FPU */
        SCB->CPACR |=  (0xFu << 20);
        __DSB();
        __ISB();
#endif

        /* Initialize the C library */
        __libc_init_array();

        /* Branch to main function */
        main();

        /* Infinite loop */
        while (1);
}

// dummy syscalls

#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
extern int errno;
void* _sbrk(int incr) { 
    extern uint32_t _sheap;
    static char* heap_end;
    char* prev_heap_end;
    if( heap_end == NULL ) {
        heap_end = (char*)&_sheap;
    } 
    prev_heap_end = heap_end;
    heap_end += incr;
    return (void*) prev_heap_end;
}

int _kill(int pid, int sig) { errno = ENOSYS; return -1;}
int _getpid(void) { errno = ENOSYS; return -1; }
int _write(int fd, char* ptr, int len) { errno = ENOSYS; return -1; }
int _close(int fd) { errno = ENOSYS; return -1; }
int _fstat(int fd, struct stat *st) { errno = ENOSYS; return -1; }
int _isatty(int fd) { errno = ENOSYS; return 0; }
int _lseek(int fd, int ptr, int dir) { errno = ENOSYS; return -1; }
int _read(int fd, char* ptr, int len) { errno = ENOSYS; return -1; }

// IDLE task definitions

static StaticTask_t idle_task;
static StackType_t idle_task_stack[256];
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
    *ppxIdleTaskTCBBuffer = &idle_task;
    *ppxIdleTaskStackBuffer = idle_task_stack;
    *pulIdleTaskStackSize = sizeof(idle_task_stack) / sizeof(StackType_t);
}

int main ( void )
{
    USER_LED_OutputEnable();
    USER_LED_Set();
    
    /* Initialize all modules */
    SYS_Initialize ( NULL );

    

    while(1) {
        SYS_Tasks();
    }

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/


