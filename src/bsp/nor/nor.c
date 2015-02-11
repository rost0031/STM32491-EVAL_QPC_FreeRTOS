/**
 * @file 	nor.c
 * @brief   Basic driver for M29WV128G NOR Flash memory on the FMC bus on the
 * STM324x9I-EVAL board.
 *
 * @date   	09/23/2014
 * @author 	Harry Rostovtsev
 * @email  	harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 *
 * @addtogroup groupNOR
 * @{
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "nor.h"
#include "qp_port.h"                                        /* for QP support */
#include "stm32f4xx_it.h"
#include "project_includes.h"
#include "Shared.h"
#include "stm32f4xx_fmc.h"                         /* For STM32F4 FMC support */
#include "sdram.h"

/* Compile-time called macros ------------------------------------------------*/
Q_DEFINE_THIS_FILE                  /* For QSPY to know the name of this file */
DBG_DEFINE_THIS_MODULE( DBG_MODL_NOR ); /* For debug system to ID this module */

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/

/**
  * @brief  FMC NOR Bank Address
  */
#define NOR_BANK_ADDR        ((uint32_t)0x60000000)

/**
  * @brief  FMC NOR block erase timeout
  */
#define BLOCKERASE_TIMEOUT   ((uint32_t)0x00A00000)

/**
  * @brief  FMC NOR chip erase timeout
  */
#define CHIPERASE_TIMEOUT    ((uint32_t)0x30000000)

/**
  * @brief  FMC NOR program timeout
  */
#define PROGRAM_TIMEOUT      ((uint32_t)0x00004400)

/**
 *  @brief NOR Ready/Busy signal GPIO definitions
 */
#define NOR_READY_BUSY_PIN    GPIO_Pin_6
#define NOR_READY_BUSY_GPIO   GPIOD
#define NOR_READY_STATE       SET
#define NOR_BUSY_STATE        RESET

/* Private macros ------------------------------------------------------------*/

/**
  * @brief  FMC NOR address shift
  */
#define ADDR_SHIFT(Address) (NOR_BANK_ADDR + (2 * (Address)))

/**
  * @brief  FMC NOR write
  */
#define NOR_WRITE(Address, Data)  (*(__IO uint16_t *)(Address) = (Data))

/* Private variables and Local objects ---------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
void NOR_Init( void )
{
   /* GPIO configuration for FMC NOR bank */
   NOR_GPIOInit();

   /* Enable FMC Clock */
   RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FMC, ENABLE);

   /* FMC Configuration ------------------------------------------------------*/
   FMC_NORSRAMInitTypeDef        FMC_NORSRAMInitStructure;
   FMC_NORSRAMTimingInitTypeDef  FMC_NORSRAMTimingStructure;

   /* NOR memory timing configuration */
   FMC_NORSRAMTimingStructure.FMC_AddressSetupTime       = 4;
   FMC_NORSRAMTimingStructure.FMC_AddressHoldTime        = 3;
   FMC_NORSRAMTimingStructure.FMC_DataSetupTime          = 7;
   FMC_NORSRAMTimingStructure.FMC_BusTurnAroundDuration  = 1;
   FMC_NORSRAMTimingStructure.FMC_CLKDivision            = 1;
   FMC_NORSRAMTimingStructure.FMC_DataLatency            = 0;
   FMC_NORSRAMTimingStructure.FMC_AccessMode             = FMC_AccessMode_A;

   /* NOR memory control configuration */
   FMC_NORSRAMInitStructure.FMC_Bank                  = FMC_Bank1_NORSRAM1;
   FMC_NORSRAMInitStructure.FMC_DataAddressMux        = FMC_DataAddressMux_Disable;
   FMC_NORSRAMInitStructure.FMC_MemoryType            = FMC_MemoryType_NOR;
   FMC_NORSRAMInitStructure.FMC_MemoryDataWidth       = FMC_NORSRAM_MemoryDataWidth_16b;
   FMC_NORSRAMInitStructure.FMC_BurstAccessMode       = FMC_BurstAccessMode_Disable;
   FMC_NORSRAMInitStructure.FMC_WaitSignalPolarity    = FMC_WaitSignalPolarity_Low;
   FMC_NORSRAMInitStructure.FMC_WrapMode              = FMC_WrapMode_Disable;
   FMC_NORSRAMInitStructure.FMC_WaitSignalActive      = FMC_WaitSignalActive_BeforeWaitState;
   FMC_NORSRAMInitStructure.FMC_WriteOperation        = FMC_WriteOperation_Enable;
   FMC_NORSRAMInitStructure.FMC_WaitSignal            = FMC_WaitSignal_Disable;
   FMC_NORSRAMInitStructure.FMC_ExtendedMode          = FMC_ExtendedMode_Disable;
   FMC_NORSRAMInitStructure.FMC_AsynchronousWait      = FMC_AsynchronousWait_Disable;
   FMC_NORSRAMInitStructure.FMC_WriteBurst            = FMC_WriteBurst_Disable;
   FMC_NORSRAMInitStructure.FMC_ContinousClock        = FMC_CClock_SyncOnly;
   FMC_NORSRAMInitStructure.FMC_ReadWriteTimingStruct = &FMC_NORSRAMTimingStructure;
   FMC_NORSRAMInitStructure.FMC_WriteTimingStruct     = &FMC_NORSRAMTimingStructure;

   /* FMC NOR memory de-initializtion */
   FMC_NORSRAMDeInit( FMC_Bank1_NORSRAM1 );

   /* FMC NOR bank initialization */
   FMC_NORSRAMInit( &FMC_NORSRAMInitStructure );

   /* Enable the NOR memory bank */
   FMC_NORSRAMCmd( FMC_Bank1_NORSRAM1, ENABLE );
}

/******************************************************************************/
void NOR_GPIOInit( void )
{
   /* GPIOs Configuration ----------------------------------------------------*/
   /*
    +-------------------+------------------+-----------------+-----------------+
    +                       NOR flash pins assignment        +                 +
    +-------------------+------------------+-----------------+-----------------+
    | PD0  <-> FMC_D2   | PE2  <-> FMC_A23 | PF0  <-> FMC_A0 | PG0 <-> FMC_A10 |
    | PD1  <-> FMC_D3   | PE3  <-> FMC_A19 | PF1  <-> FMC_A1 | PG1 <-> FMC_A11 |
    | PD3  <-> FMC_CLK  | PE4  <-> FMC_A20 | PF2  <-> FMC_A2 | PG2 <-> FMC_A12 |
    | PD4  <-> FMC_NOE  | PE5  <-> FMC_A21 | PF3  <-> FMC_A3 | PG3 <-> FMC_A13 |
    | PD5  <-> FMC_NWE  | PE6  <-> FMC_A22 | PF4  <-> FMC_A4 | PG4 <-> FMC_A14 |
    | PD6  <-> FMC_NWAIT| PE7  <-> FMC_D4  | PF5  <-> FMC_A5 | PG5 <-> FMC_A15 |
    | PD7  <-> FMC_NE1  | PE8  <-> FMC_D5  | PF12 <-> FMC_A6 |-----------------+
    | PD8  <-> FMC_D13  | PE9  <-> FMC_D6  | PF13 <-> FMC_A7 |
    | PD9  <-> FMC_D14  | PE10 <-> FMC_D7  | PF14 <-> FMC_A8 |
    | PD10 <-> FMC_D15  | PE11 <-> FMC_D8  | PF15 <-> FMC_A9 |
    | PD11 <-> FMC_A16  | PE12 <-> FMC_D9  | ----------------+
    | PD12 <-> FMC_A17  | PE13 <-> FMC_D10 |
    | PD13 <-> FMC_A18  | PE14 <-> FMC_D11 |
    | PD14 <-> FMC_D0   | PE15 <-> FMC_D12 |
    | PD15 <-> FMC_D1   |------------------+
    +-------------------+
    */
   GPIO_InitTypeDef GPIO_InitStructure;

   /* Enable GPIO Clocks */
   RCC_AHB1PeriphClockCmd(
         RCC_AHB1Periph_GPIOD |
         RCC_AHB1Periph_GPIOG |
         RCC_AHB1Periph_GPIOE |
         RCC_AHB1Periph_GPIOF,
         ENABLE
   );

   /* Common GPIO configuration */
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

   /* GPIOD configuration */
   GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOD, GPIO_PinSource3, GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOD, GPIO_PinSource7, GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FMC);

   GPIO_InitStructure.GPIO_Pin =
         GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_3  | GPIO_Pin_4  | GPIO_Pin_5  |
         GPIO_Pin_6  | GPIO_Pin_7  | GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10 |
         GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;

   GPIO_Init(GPIOD, &GPIO_InitStructure);

   /* GPIOE configuration */
   GPIO_PinAFConfig(GPIOE, GPIO_PinSource2, GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOE, GPIO_PinSource3, GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOE, GPIO_PinSource4, GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOE, GPIO_PinSource5, GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOE, GPIO_PinSource6, GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOE, GPIO_PinSource7, GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOE, GPIO_PinSource8, GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOE, GPIO_PinSource10, GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOE, GPIO_PinSource12, GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOE, GPIO_PinSource14, GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOE, GPIO_PinSource15, GPIO_AF_FMC);

   GPIO_InitStructure.GPIO_Pin =
         GPIO_Pin_2  | GPIO_Pin_3  | GPIO_Pin_4  | GPIO_Pin_5  | GPIO_Pin_6  |
         GPIO_Pin_7  | GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10 | GPIO_Pin_11 |
         GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;

   GPIO_Init(GPIOE, &GPIO_InitStructure);

   /* GPIOF configuration */
   GPIO_PinAFConfig(GPIOF,GPIO_PinSource0, GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOF,GPIO_PinSource1, GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOF,GPIO_PinSource2, GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOF,GPIO_PinSource3, GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOF,GPIO_PinSource4, GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOF,GPIO_PinSource5, GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOF,GPIO_PinSource12, GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOF,GPIO_PinSource13, GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOF,GPIO_PinSource14, GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOF,GPIO_PinSource15, GPIO_AF_FMC);

   GPIO_InitStructure.GPIO_Pin =
         GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_2  | GPIO_Pin_3  | GPIO_Pin_4  |
         GPIO_Pin_5  | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;

   GPIO_Init(GPIOF, &GPIO_InitStructure);

   /* GPIOG configuration */
   GPIO_PinAFConfig(GPIOG,GPIO_PinSource0, GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOG,GPIO_PinSource1, GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOG,GPIO_PinSource2, GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOG,GPIO_PinSource3, GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOG,GPIO_PinSource4, GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOG,GPIO_PinSource5, GPIO_AF_FMC);

   GPIO_InitStructure.GPIO_Pin =
         GPIO_Pin_0 |GPIO_Pin_1 | GPIO_Pin_2 |
         GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;

   GPIO_Init(GPIOG, &GPIO_InitStructure);
}

/******************************************************************************/
void NOR_ReadID( NOR_IDTypeDef* pNOR_ID )
{
   NOR_WRITE( ADDR_SHIFT( 0x0555 ), 0x00AA );
   NOR_WRITE( ADDR_SHIFT( 0x02AA ), 0x0055 );
   NOR_WRITE( ADDR_SHIFT( 0x0555 ), 0x0090 );

   pNOR_ID->Manufacturer_Code  = *(__IO uint16_t *) ADDR_SHIFT( 0x0000 );
   pNOR_ID->Device_Code1       = *(__IO uint16_t *) ADDR_SHIFT( 0x0001 );
   pNOR_ID->Device_Code2       = *(__IO uint16_t *) ADDR_SHIFT( 0x000E );
   pNOR_ID->Device_Code3       = *(__IO uint16_t *) ADDR_SHIFT( 0x000F );
}

/******************************************************************************/
CBErrorCode NOR_EraseBlock(uint32_t uwBlockAddress)
{
   NOR_WRITE( ADDR_SHIFT( 0x0555), 0x00AA );
   NOR_WRITE( ADDR_SHIFT( 0x02AA), 0x0055 );
   NOR_WRITE( ADDR_SHIFT( 0x0555), 0x0080 );
   NOR_WRITE( ADDR_SHIFT( 0x0555), 0x00AA );
   NOR_WRITE( ADDR_SHIFT( 0x02AA), 0x0055 );
   NOR_WRITE( ( NOR_BANK_ADDR + uwBlockAddress ), 0x30);

   return ( NOR_GetStatus( BLOCKERASE_TIMEOUT ) );
}

/******************************************************************************/
CBErrorCode NOR_EraseChip(void)
{
   NOR_WRITE( ADDR_SHIFT( 0x0555 ), 0x00AA );
   NOR_WRITE( ADDR_SHIFT( 0x02AA ), 0x0055 );
   NOR_WRITE( ADDR_SHIFT( 0x0555 ), 0x0080 );
   NOR_WRITE( ADDR_SHIFT( 0x0555 ), 0x00AA );
   NOR_WRITE( ADDR_SHIFT( 0x02AA ), 0x0055 );
   NOR_WRITE( ADDR_SHIFT( 0x0555 ), 0x0010 );

   return ( NOR_GetStatus( CHIPERASE_TIMEOUT ) );
}

/******************************************************************************/
CBErrorCode NOR_WriteHalfWord( uint32_t uwWriteAddress, uint16_t uhData )
{
   NOR_WRITE( ADDR_SHIFT( 0x0555 ), 0x00AA );
   NOR_WRITE( ADDR_SHIFT( 0x02AA ), 0x0055 );
   NOR_WRITE( ADDR_SHIFT( 0x0555 ), 0x00A0 );
   NOR_WRITE( (NOR_BANK_ADDR + uwWriteAddress ), uhData );

   return ( NOR_GetStatus( PROGRAM_TIMEOUT ) );
}

/******************************************************************************/
CBErrorCode NOR_WriteBuffer(
      uint16_t* pBuffer,
      uint32_t uwWriteAddress,
      uint32_t uwBufferSize
)
{
   CBErrorCode status = ERR_NOR_BUSY;

   do {
      /* Transfer data to the memory */
      status = NOR_WriteHalfWord( uwWriteAddress, *pBuffer++ );
      uwWriteAddress = uwWriteAddress + 2;
      uwBufferSize--;
   }
   while((status == ERR_NONE) && (uwBufferSize != 0));

   return (status);

}

/******************************************************************************/
uint16_t NOR_ReadHalfWord( uint32_t uwReadAddress )
{
   NOR_WRITE( ADDR_SHIFT( 0x00555 ), 0x00AA );
   NOR_WRITE( ADDR_SHIFT( 0x002AA ), 0x0055 );

   /* Send read data command */
   NOR_WRITE( ( NOR_BANK_ADDR + uwReadAddress ), 0x00F0 );

   /* Return the data read */
   return (*(__IO uint16_t *)( ( NOR_BANK_ADDR + uwReadAddress ) ) );
}

/******************************************************************************/
void NOR_ReadBuffer(
      uint16_t* pBuffer,
      uint32_t uwReadAddress,
      uint32_t uwBufferSize
)
{
   NOR_WRITE( ADDR_SHIFT( 0x0555 ), 0x00AA );
   NOR_WRITE( ADDR_SHIFT( 0x02AA ), 0x0055 );
   NOR_WRITE( ( NOR_BANK_ADDR + uwReadAddress ), 0x00F0 );

   for(; uwBufferSize != 0x00; uwBufferSize--) {/* while there is data to read */
      /* Read a Halfword from the NOR */
      *pBuffer++ = *(__IO uint16_t *)((NOR_BANK_ADDR + uwReadAddress));
      uwReadAddress = uwReadAddress + 2;
   }
}

/******************************************************************************/
void NOR_ReturnToReadMode( void )
{
  NOR_WRITE( NOR_BANK_ADDR, 0x00F0 );
}

/******************************************************************************/
void NOR_Reset( void )
{
   NOR_WRITE( ADDR_SHIFT( 0x00555 ), 0x00AA );
   NOR_WRITE( ADDR_SHIFT( 0x002AA ), 0x0055 );

   /* Send read mode command */
   NOR_WRITE(NOR_BANK_ADDR, 0x00F0);
}

/******************************************************************************/
CBErrorCode NOR_GetStatus( uint32_t Timeout )
{
   uint16_t val1 = 0, val2 = 0;
   CBErrorCode status = ERR_NOR_BUSY;
   uint32_t timeout = Timeout;

   /* Poll on NOR memory Ready/Busy signal ------------------------------------*/
   while((GPIO_ReadInputDataBit(NOR_READY_BUSY_GPIO, NOR_READY_BUSY_PIN) != NOR_BUSY_STATE) && (timeout > 0))
   {
      timeout--;
   }

   timeout = Timeout;

   while((GPIO_ReadInputDataBit(NOR_READY_BUSY_GPIO, NOR_READY_BUSY_PIN) == NOR_BUSY_STATE) && (timeout > 0))
   {
      timeout--;
   }

   /* Get the NOR memory operation status -------------------------------------*/
   while((Timeout != 0x00) && (status != ERR_NONE)) {
      Timeout--;

      /*!< Read DQ6 and DQ5 */
      val1 = *(__IO uint16_t *)(NOR_BANK_ADDR);
      val2 = *(__IO uint16_t *)(NOR_BANK_ADDR);

      /* If DQ6 did not toggle between the two reads then return NOR_Success */
      if((val1 & 0x0040) == (val2 & 0x0040)) {
         return ERR_NONE;
      }

      if((val1 & 0x0020) != 0x0020) {
         status = ERR_NOR_BUSY;
      }

      val1 = *(__IO uint16_t *)(NOR_BANK_ADDR);
      val2 = *(__IO uint16_t *)(NOR_BANK_ADDR);

      if((val1 & 0x0040) == (val2 & 0x0040)) {
         return ERR_NONE;
      } else if((val1 & 0x0020) == 0x0020) {
         return ERR_NOR_ERROR;
      }
   }

   if(Timeout == 0) {
      status = ERR_NOR_TIMEOUT;
   }

   /* Return the operation status */
   return (status);
}

/******************************************************************************/
void NOR_TestDestructive( void )
{
#define BUFFER_SIZE        ((uint32_t)0x0400)
#define WRITE_READ_ADDR    ((uint16_t)0x8000)

   uint16_t aTxBuffer[BUFFER_SIZE];
   uint16_t aRxBuffer[BUFFER_SIZE];

   uint16_t tmpIndex = 0;
   uint16_t uwOffset = 0x2A50;
   CBErrorCode status = ERR_NONE;

   /* 1. Fill the buffer to write */
   DBG_printf("Filling buffer...\n");
   for (tmpIndex = 0; tmpIndex < BUFFER_SIZE; tmpIndex++ ) {
      aTxBuffer[tmpIndex] = tmpIndex + uwOffset;
   }

   /* Return to read mode */
   NOR_ReturnToReadMode();

   /* Erase the NOR memory block to write on */
   NOR_EraseBlock(WRITE_READ_ADDR);

   /* 2. Write buffer to NOR Flash */
   DBG_printf("Writing buffer...\n");
   status = NOR_WriteBuffer(aTxBuffer, WRITE_READ_ADDR, BUFFER_SIZE);
   DBG_printf("Write finished with status: 0x%08x\n", status);

   /* 3. Read data from the NOR memory */
   DBG_printf("Reading buffer...\n");
   NOR_ReadBuffer(aRxBuffer, WRITE_READ_ADDR, BUFFER_SIZE);
   DBG_printf("Read finished with status: 0x%08x\n", status);

   /* 4. Check buffers against each other */
   DBG_printf("Checking buffers...\n");
   uint32_t uwIndex = 0;
   __IO uint32_t uwWriteReadStatus = 0;
   for (uwIndex = 0; (uwIndex < BUFFER_SIZE) && (uwWriteReadStatus == 0); uwIndex++) {
      if (aRxBuffer[uwIndex] != aTxBuffer[uwIndex]) {
         ERR_printf("Written and read back values don't match: wrote 0x%04x and read back 0x%04x\n", aTxBuffer[uwIndex], aRxBuffer[uwIndex] );
         uwWriteReadStatus = uwIndex + 1;
      }
   }
   DBG_printf("Result of buffer check: %d\n", uwWriteReadStatus );
}

/******************************************************************************/
void NOR_SDRAMTestInteraction( void )
{
#define BUFFER_SIZE_SD_NOR     ((uint32_t)0x0400)
#define WRITE_READ_ADDR_NOR    ((uint16_t)0x8000)
#define WRITE_READ_ADDR_SDRAM  ((uint32_t)0x0800)

   uint8_t  aTxBuffer[BUFFER_SIZE_SD_NOR*2];
   uint16_t aRxNorBuffer[BUFFER_SIZE_SD_NOR];
   uint32_t aRxSdramBuffer[BUFFER_SIZE_SD_NOR/2];

   uint16_t tmpIndex = 0;
   uint16_t uwOffset = 0x3CA5;
   CBErrorCode status = ERR_NONE;

   /* 1. Fill the buffer to write */
   DBG_printf("Filling buffer...\n");
   for (tmpIndex = 0; tmpIndex < BUFFER_SIZE_SD_NOR * 2; tmpIndex++ ) {
      aTxBuffer[tmpIndex] = tmpIndex + uwOffset;
   }

   /* 2. Write buffer to SDRAM */
   DBG_printf("Writing buffer to SDRAM...\n");
   SDRAM_WriteBuffer((uint32_t *)&aTxBuffer[0], WRITE_READ_ADDR_SDRAM, BUFFER_SIZE_SD_NOR/2);
   DBG_printf("Write to SDRAM finished\n");

   /* 3. Read data from the SDRAM */
   DBG_printf("Reading buffer back from SDRAM...\n");
   SDRAM_ReadBuffer(aRxSdramBuffer, WRITE_READ_ADDR_SDRAM, BUFFER_SIZE_SD_NOR/2);
   DBG_printf("Read from SDRAM finished\n");

   /* 4. Write buffer buffer from SDRAM to NOR Flash */
   /* Return to read mode */
   NOR_ReturnToReadMode();

   /* Erase the NOR memory block to write on */
   NOR_EraseBlock(WRITE_READ_ADDR_NOR);

   DBG_printf("Writing buffer to NOR...\n");
   status = NOR_WriteBuffer((uint16_t *)&aRxSdramBuffer[0], WRITE_READ_ADDR_NOR, BUFFER_SIZE_SD_NOR);
   DBG_printf("Write finished with status: 0x%08x\n", status);

   /* 5. Read back the buffer from NOR */
   DBG_printf("Reading buffer back from NOR...\n");
   NOR_ReadBuffer(aRxNorBuffer, WRITE_READ_ADDR_NOR, BUFFER_SIZE_SD_NOR);
   DBG_printf("Read finished\n");

   /* 6. Read back data from SDRAM to make sure refresh lines are working ok */
   memset(aRxSdramBuffer, 0, sizeof(aRxSdramBuffer));
   DBG_printf("Reading buffer back from SDRAM to make sure the refresh is working...\n");
   SDRAM_ReadBuffer(aRxSdramBuffer, WRITE_READ_ADDR_SDRAM, BUFFER_SIZE_SD_NOR/2);
   DBG_printf("Read finished\n");

   /* Compare the buffers read back from NOR and SDRAM and make sure they are
    * same. */
   DBG_printf("Checking buffers...\n");
   uint32_t uwIndex = 0;
   uint8_t *sdRxPtr = &aRxSdramBuffer[0];
   uint8_t *norRxPtr = &aRxNorBuffer[0];
   __IO uint32_t uwWriteReadStatus = 0;
   for (uwIndex = 0; (uwIndex < BUFFER_SIZE_SD_NOR*2) && (uwWriteReadStatus == 0); uwIndex++) {
      if (sdRxPtr[uwIndex] != norRxPtr[uwIndex]) {
         ERR_printf(
               "Written and read back values don't match: wrote 0x%04x and read back 0x%04x\n",
               sdRxPtr[uwIndex], norRxPtr[uwIndex] );
         uwWriteReadStatus = uwIndex + 1;
      }
   }
   DBG_printf("Result of buffer check: %d\n", uwWriteReadStatus );
}

/******************************************************************************/
inline void NOR_CallbackExample( void )
{
   QF_CRIT_STAT_TYPE intStat;
   BaseType_t lHigherPriorityTaskWoken = pdFALSE;

   QF_ISR_ENTRY(intStat);                        /* inform QF about ISR entry */
   /* Code goes below this comment */


   /* Code goes above this comment */
   QF_ISR_EXIT(intStat, lHigherPriorityTaskWoken);/* inform QF about ISR exit */

   /* yield only when needed... */
   if (lHigherPriorityTaskWoken != pdFALSE) {
      vTaskMissedYield();
   }
}

/**
 * @}
 * end addtogroup groupNOR
 */

/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
