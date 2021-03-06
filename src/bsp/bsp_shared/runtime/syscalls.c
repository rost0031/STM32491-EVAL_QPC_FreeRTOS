/**
 * @file   syscalls.c
 * @brief  This file contains low level system calls and stubs.  See:
 * 		   http://embdev.net/topic/177530
 * 		   http://www.utasker.com/forum/index.php?topic=748.0
 * 		   Also see documentation in
 * 		   share/doc/arm-arm-none-eabi/html/libc/Syscalls.html
 *
 * @date   09/27/2012
 * @author Harry Rostovtsev
 * @email  rost0031@gmail.com
 * Copyright (C) 2012 Harry Rostovtsev. All rights reserved.
 *
 * @addtogroup groupSTM32runtime
 * @{
 * These are a minimal set of POSIX function remaps to allow the use of printf
 * type functions.  See original function documentation online.
 */

/* Includes ------------------------------------------------------------------*/
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include "stm32f4xx.h"                                     /* For __get_MSP() */
#include "stm32f4xx_usart.h"                             /* For USART support */
#include "Shared.h"                                     /* For MEMCPY/SMEMCPY */

/* Compile-time called macros ------------------------------------------------*/
/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/

/**
 * @brief Standard out redirect to UART
 */
#ifndef STDOUT_USART
#define STDOUT_USART 1
#endif

/**
 * @brief Standard error redirect to UART
 */
#ifndef STDERR_USART
#define STDERR_USART 1
#endif

/**
 * @brief Standard in redirect to UART
 */
#ifndef STDIN_USART
#define STDIN_USART 1
#endif

/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/
/**
 * @brief External error that can be set by some POSIX functions.
 */
#undef errno
extern int errno;

/**
 * @brief External end of stack.
 */
extern int  _end;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
caddr_t _sbrk(int incr)
{
   static unsigned char *heap = NULL;
   unsigned char *prev_heap;
   if (heap == NULL) {
      heap = (unsigned char *)&_end;
   }

   prev_heap = heap;
   heap += incr;
   return ((caddr_t) prev_heap);
}

/******************************************************************************/
void _exit(int status)
{
   while(1) {;} // Prevent the processor from faulting
}

/******************************************************************************/
int _getpid(void)
{
   return(1);
}

/******************************************************************************/
int _kill(int pid, int sig)
{
   //pid = pid; sig = sig; /* avoid warnings */
   errno = EINVAL;
   return(-1);
}

/******************************************************************************/
int _close(int file)
{
   return(-1);
}

/******************************************************************************/
int _fstat(int file, struct stat *st)
{
   st->st_mode = S_IFCHR;
   return(0);
}

/******************************************************************************/
int _isatty(int file)
{
   return(1);
}

/******************************************************************************/
int _lseek(int file, int ptr, int dir)
{
   return(0);
}

/******************************************************************************/
int _read(int file, char *ptr, int len)
{
   int n;
   int num = 0;
   switch (file) {
      case STDIN_FILENO:
         for (n = 0; n < len; n++) {
#if   STDIN_USART == 1
            while ((USART1->SR & USART_FLAG_RXNE) == (uint16_t)RESET) {}
            char c = (char)(USART1->DR & (uint16_t)0x01FF);
#elif STDIN_USART == 2
            while ((USART2->SR & USART_FLAG_RXNE) == (uint16_t) RESET) {}
            char c = (char) (USART2->DR & (uint16_t) 0x01FF);
#elif STDIN_USART == 3
            while ((USART3->SR & USART_FLAG_RXNE) == (uint16_t)RESET) {}
            char c = (char)(USART3->DR & (uint16_t)0x01FF);
#elif STDIN_USART == 4
            while ((UART4->SR & USART_FLAG_RXNE) == (uint16_t)RESET) {}
            char c = (char)(UART4->DR & (uint16_t)0x01FF);
#endif
            *ptr++ = c;
            num++;
         }
         break;
      default:
         errno = EBADF;
         return(-1);
   }
   return(num);
}

/******************************************************************************/
int _write(int file, char *ptr, int len)
{
   int n;
   switch (file) {
      case STDOUT_FILENO: /*stdout*/
         for (n = 0; n < len; n++) {
#if STDOUT_USART == 1
            while ((USART1->SR & USART_FLAG_TC) == (uint16_t)RESET) {}
            USART1->DR = (*ptr++ & (uint16_t)0x01FF);
#elif  STDOUT_USART == 2
            while ((USART2->SR & USART_FLAG_TC) == (uint16_t) RESET) {}
            USART2->DR = (*ptr++ & (uint16_t) 0x01FF);
#elif  STDOUT_USART == 3
            while ((USART3->SR & USART_FLAG_TC) == (uint16_t)RESET) {}
            USART3->DR = (*ptr++ & (uint16_t)0x01FF);
#elif  STDOUT_USART == 4
            while ((UART4->SR & USART_FLAG_TC) == (uint16_t)RESET) {}
            UART4->DR = (*ptr++ & (uint16_t)0x01FF);
#endif
         }
         break;
      case STDERR_FILENO: /* stderr */
         for (n = 0; n < len; n++) {
#if STDERR_USART == 1
            while ((USART1->SR & USART_FLAG_TC) == (uint16_t)RESET) {}
            USART1->DR = (*ptr++ & (uint16_t)0x01FF);
#elif  STDERR_USART == 2
            while ((USART2->SR & USART_FLAG_TC) == (uint16_t) RESET) {}
            USART2->DR = (*ptr++ & (uint16_t) 0x01FF);
#elif  STDERR_USART == 3
            while ((USART3->SR & USART_FLAG_TC) == (uint16_t)RESET) {}
            USART3->DR = (*ptr++ & (uint16_t)0x01FF);
#elif  STDOUT_USART == 4
            while ((UART4->SR & USART_FLAG_TC) == (uint16_t)RESET) {}
            UART4->DR = (*ptr++ & (uint16_t)0x01FF);
#endif
         }
         break;
      default:
         errno = EBADF;
         return(-1);
   }
   return(len);
}


static char *heap_end;

/******************************************************************************/
char* get_heap_end(void)
{
   return ((char*) heap_end);
}

/******************************************************************************/
char* get_stack_top(void)
{
   return ((char*) __get_MSP());
}

/**
 * @}
 * end addtogroup groupSTM32runtime
 */

/******** Copyright (C) 2012 Harry Rostovtsev. All rights reserved *****END OF FILE****/
