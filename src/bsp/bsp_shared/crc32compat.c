/**
 * @file   crc32compat.c
 * @brief  This file contains an implementation of CRC32 algorithm that leverages
 * STM32 hardware for CRC calculations but also produces an output compatible
 * with Boost C++ CRC implementation. These functions were shamelessly ripped
 * off from the STMicro forums and were under no license that I could see.
 *
 * @date   01/22/2013
 * @author Harry Rostovtsev
 * @email  harry_rostovhtsev@datacard.com
 * Copyright (C) 2013 Datacard. All rights reserved.
 *
 * @addtogroup groupSharedBSP
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "crc32compat.h"

/* Compile-time called macros ------------------------------------------------*/
/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/******************************************************************************/
/**
 * @brief This function bit reverses and is written in ARM assembly.
 *
 * @param  data:  a 32 bit word of data to be reversed
 * @return uint32_t: reversed 32 bit word of data.
 */
static uint32_t revbit(uint32_t data)
{
   asm("rbit r0,r0");
   return (data);
};

/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
uint32_t CRC32_Calc(char *buffer, int size)
{
   uint32_t i, j;
   uint32_t ui32;

   CRC->CR=1;

   /* delay for STM32 CRC hardware to be ready */
   asm("NOP");asm("NOP");asm("NOP");

   i = size >> 2;

   while(i--) {

      ui32=*((uint32_t *)buffer);

      buffer += 4;

      /* reverse the bit order of input data */
      ui32=revbit(ui32);

      CRC->DR=ui32;
   }

   ui32=CRC->DR;

   /* reverse the bit order of output data */
   ui32=revbit(ui32);

   i = size & 3;

   while(i--) {
      ui32 ^= (uint32_t)*buffer++;

      for(j=0; j<8; j++) {
         if (ui32 & 1) {
            ui32 = (ui32 >> 1) ^ 0xEDB88320;
         } else {
            ui32 >>= 1;
         }
      }
   }

   /*xor with 0xffffffff */
   ui32^=0xffffffff;

   /* now the output is compatible with windows/winzip/winrar/Boost */
   return (ui32);
}

/**
 * @} end
 * addtogroup groupSharedBSP
 */

/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
