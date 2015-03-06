/**
 * @file 	i2c.c
 * @brief   Basic driver for I2C bus.
 *
 * @date   	06/30/2014
 * @author 	Harry Rostovtsev
 * @email  	harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 *
 * @addtogroup groupI2C
 * @{
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "i2c.h"
#include "i2c_defs.h"
#include "i2c_dev.h"
#include "qp_port.h"                                        /* for QP support */
#include "stm32f4xx_it.h"
#include "project_includes.h"
#include "Shared.h"
#include "I2CBusMgr.h"                                 /* For I2C event types */
#include "stm32f4xx.h"
#include "stm32f4xx_dma.h"                           /* For STM32 DMA support */
#include "stm32f4xx_i2c.h"                           /* For STM32 DMA support */

/* Compile-time called macros ------------------------------------------------*/
Q_DEFINE_THIS_FILE                  /* For QSPY to know the name of this file */
DBG_DEFINE_THIS_MODULE( DBG_MODL_I2C ); /* For debug system to ID this module */

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/

/* Maximum Timeout values for flags and events waiting loops. These timeouts are
   not based on accurate values, they just guarantee that the application will
   not remain stuck if the I2C communication is corrupted.
   These are only used for blocking I2C operations which are only called before
   any threads/objects are running and after (if they have crashed). */
#define I2C_SHORT_TIMEOUT         ((uint32_t)0x1000)
#define I2C_LONG_TIMEOUT         ((uint32_t)(30 * I2C_SHORT_TIMEOUT))

/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/

/**
 * @brief RX Buffer for I2C1 bus
 */
uint8_t          i2c1RxBuffer[MAX_I2C_READ_LEN];

/**
 * @brief TX Buffer for I2C1 bus
 */
uint8_t          i2c1TxBuffer[MAX_I2C_WRITE_LEN];

/**
 * @brief An internal structure that holds almost all the settings for the I2C
 * Busses
 */
I2C_BusSettings_t s_I2C_Bus[MAX_I2C_BUS] =
{
      {
            I2CBus1,                   /**< i2c_sys_bus */

            /* I2C bus settings */
            I2C1,                      /**< i2c_bus */
            I2C_SPEED,                 /**< i2c_bus_speed (Hz)*/
            RCC_APB1Periph_I2C1,       /**< i2c_clk */

            /* I2C interrupt settings */
            I2C1_EV_IRQn,              /**< i2c_ev_irq_num */
            I2C1_EV_PRIO,              /**< i2c_ev_irq_prio */
            I2C1_ER_IRQn,              /**< i2c_er_irq_num */
            I2C1_ER_PRIO,              /**< i2c_er_irq_prio */

            /* I2C GPIO settings for SCL */
            GPIOB,                     /**< scl_port */
            GPIO_Pin_6,                /**< scl_pin */
            GPIO_PinSource6,           /**< scl_af_pin_source */
            GPIO_AF_I2C1,              /**< scl_af */
            RCC_AHB1Periph_GPIOB,      /**< scl_clk */

            /* I2C GPIO settings for SDA */
            GPIOB,                     /**< sda_port */
            GPIO_Pin_9,                /**< sda_pin */
            GPIO_PinSource9,           /**< sda_af_pin_source */
            GPIO_AF_I2C1,              /**< sda_af */
            RCC_AHB1Periph_GPIOB,      /**< sda_clk */

            /* Common I2C DMA settings */
            DMA1,                      /**< i2c_dma */
            DMA_Channel_1,             /**< i2c_dma_channel */
            (uint32_t)&(I2C1->DR),     /**< i2c_dma_dr_addr */
            RCC_AHB1Periph_DMA1,       /**< i2c_dma_clk */

            /* TX I2C DMA settings */
            DMA1_Stream6,              /**< i2c_dma_tx_stream */
            DMA1_Stream6_IRQn,         /**< i2c_dma_tx_irq_num */
            DMA1_Stream6_PRIO,         /**< i2c_dma_tx_irq_prio */
            DMA_FLAG_TCIF6 |
            DMA_FLAG_FEIF6 |
            DMA_FLAG_DMEIF6 |
            DMA_FLAG_TEIF6 |
            DMA_FLAG_HTIF6,            /**< i2c_dma_tx_flags */

            /* RX I2C DMA settings */
            DMA1_Stream0,              /**< i2c_dma_rx_stream */
            DMA1_Stream0_IRQn,         /**< i2c_dma_rx_irq_num */
            DMA1_Stream0_PRIO,         /**< i2c_dma_rx_irq_prio */
            DMA_FLAG_TCIF0 |
            DMA_FLAG_FEIF0 |
            DMA_FLAG_DMEIF0 |
            DMA_FLAG_TEIF0 |
            DMA_FLAG_HTIF0,            /**< i2c_dma_rx_flags */

            /* Buffer management */
            &i2c1RxBuffer[0],          /**< *pRxBuffer */
            0,                         /**< nRxindex */
            &i2c1TxBuffer[0],          /**< *pTxBuffer */
            0,                         /**< nTxindex */

            /* Device management */
            I2C_Direction_Transmitter, /**< bTransDirection */
            0,                         /**< nBytesExpected */
            0,                         /**< nBytesCurrent */
      }
};

/* Private function prototypes -----------------------------------------------*/
/**
 * @brief  I2C function to setup up both read and write operations.
 *
 * This function sets up the I2C bus for blocking reads and writes to a memory
 * device on the I2C bus.  Since both operations share this code, it has been
 * broken out into a separate static function.
 *
 * @param [in] iBus: I2C_Bus_t type specifying the I2C bus where the error occurred.
 *    @arg I2CBus1
 * @param [in] i2cDevAddr: address of the device on the I2C bus.
 * @param [in] i2cMemAddr: internal memory address of the device on the I2C bus.
 * @param [in] i2cMemAddrSize: size of the memory address i2cMemAddr.
 *    @arg 1: a 1 byte address.
 *    @arg 2: a 2 byte address.
 *    No other sizes will be handled.
 * @return error: CBErrorCode that occurred during the I2C bus calls.
 */
static CBErrorCode I2C_setupMemRW(
      I2C_Bus_t iBus,
      uint8_t i2cDevAddr,
      uint16_t i2cMemAddr,
      uint8_t i2cMemAddrSize
);

/**
 * @brief  Writes a block of data from a memory device on any I2C bus.
 *
 * @note:  This is a slow function that should not be called by any threads or
 * objects. It's for use in case of crashes and before the entire system has
 * come up.
 *
 * @param [in] iBus: I2C_Bus_t type specifying the I2C bus where device lives.
 *    @arg I2CBus1
 * @param [in] i2cDevAddr: address of the device on the I2C bus.
 * @param [in] i2cMemAddr: internal memory address of the device on the I2C bus.
 * @param [in] i2cMemAddrSize: size of the memory address i2cMemAddr.
 *    @arg 1: a 1 byte address.
 *    @arg 2: a 2 byte address.
 *    No other sizes will be handled.
 * @param [in|out] *pBuffer: uint8_t pointer to buffer from where to write the
 * data from to the I2C device.
 * @param [in] bytesToWrite : uint8_t variable specifying how many bytes to
 * write. Note that this should always be less than or equal to the page size of
 * the target memory device since most memory devices will loop around to the
 * front of the page if you go over the page boundary.
 *
 * @return CBErrorCode: status of the read operation
 *    @arg ERR_NONE: if no errors occurred
 */
static CBErrorCode I2C_writePageBLK(
      I2C_Bus_t iBus,
      uint8_t i2cDevAddr,
      uint16_t i2cMemAddr,
      uint8_t i2cMemAddrSize,
      uint8_t* pBuffer,
      uint16_t bytesToWrite
);

/**
 * @brief  I2C error handling callback for slow blocking I2C bus access
 * functions.
 * @param [in] iBus: I2C_Bus_t type specifying the I2C bus where the error occurred.
 *    @arg I2CBus1
 * @param [in] error: CBErrorCode type specifying the error that occurred.
 * @return error: CBErrorCode that occurred to cause this callback to be called.
 */
static CBErrorCode I2C_TIMEOUT_UserCallbackRaw(
      I2C_Bus_t iBus,
      CBErrorCode error,
      const char *func,
      int line
);

/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
void I2C_BusInit( I2C_Bus_t iBus )
{
   /* Deinit the bus just in case. */
   I2C_BusDeInit( iBus );

   /* 1. Enable clocks ------------------------------------------------------ */
   /* Enable I2C clock */
   RCC_APB1PeriphClockCmd( s_I2C_Bus[iBus].i2c_clk, ENABLE );

   /* Enable I2C GPIO clocks */
   RCC_AHB1PeriphClockCmd(
         s_I2C_Bus[iBus].scl_clk | s_I2C_Bus[iBus].sda_clk,
         ENABLE
   );

   /* Enable the SYSCFG clock */
   RCC_APB2PeriphClockCmd( RCC_APB2Periph_SYSCFG, ENABLE );

   /* Enable I2C DMA clock */
   RCC_AHB1PeriphClockCmd( s_I2C_Bus[iBus].i2c_dma_clk, ENABLE );

   /* 2. Set up GPIO -------------------------------------------------------- */
   /* Connect PXx to I2C SCL alt function. */
   GPIO_PinAFConfig(
         s_I2C_Bus[iBus].scl_port,
         s_I2C_Bus[iBus].scl_af_pin_source,
         s_I2C_Bus[iBus].scl_af
   );

   /* Connect PXx to I2C SDA alt function. */
   GPIO_PinAFConfig(
         s_I2C_Bus[iBus].sda_port,
         s_I2C_Bus[iBus].sda_af_pin_source,
         s_I2C_Bus[iBus].sda_af
   );

   /* Configure common elements of the SDA and SCL pins */
   GPIO_InitTypeDef  GPIO_InitStructure;
   GPIO_InitStructure.GPIO_Mode     = GPIO_Mode_AF;
   GPIO_InitStructure.GPIO_Speed    = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_OType    = GPIO_OType_OD;
   GPIO_InitStructure.GPIO_PuPd     = GPIO_PuPd_NOPULL;

   /* Configure SCL specific settings for I2C */
   GPIO_InitStructure.GPIO_Pin      = s_I2C_Bus[iBus].scl_pin;
   GPIO_Init(s_I2C_Bus[iBus].scl_port, &GPIO_InitStructure);

   /* Configure SDA specific settings for I2C */
   GPIO_InitStructure.GPIO_Pin = s_I2C_Bus[iBus].sda_pin;
   GPIO_Init(s_I2C_Bus[iBus].sda_port, &GPIO_InitStructure);

   /* 3. Set up DMA I2C settings. The R/W function will set the structure --- */
   DMA_Cmd( s_I2C_Bus[iBus].i2c_dma_rx_stream, DISABLE );
   DMA_DeInit( s_I2C_Bus[iBus].i2c_dma_rx_stream );

   DMA_Cmd( s_I2C_Bus[iBus].i2c_dma_tx_stream, DISABLE );
   DMA_DeInit( s_I2C_Bus[iBus].i2c_dma_tx_stream );

   /* 4. Set up interrupts -------------------------------------------------- */
   /* Set up Interrupt controller to handle I2C Error interrupts */
   NVIC_Config(
         s_I2C_Bus[iBus].i2c_er_irq_num,
         s_I2C_Bus[iBus].i2c_er_irq_prio
   );

   /* Enable I2C Error Interrupts */
   I2C_ITConfig(
         s_I2C_Bus[iBus].i2c_bus,
         I2C_IT_ERR,
         ENABLE
   );

   /* Initialize the IRQ and priorities for I2C DMA */
   NVIC_Config(
         s_I2C_Bus[iBus].i2c_dma_rx_irq_num,
         s_I2C_Bus[iBus].i2c_dma_rx_irq_prio
   );

   NVIC_Config(
         s_I2C_Bus[iBus].i2c_dma_tx_irq_num,
         s_I2C_Bus[iBus].i2c_dma_tx_irq_prio
   );

   /* I2C configuration */
   I2C_InitTypeDef  I2C_InitStructure;
   I2C_InitStructure.I2C_Mode                = I2C_Mode_I2C; /* Mode for the M24CXX EEPROM used by the STM324x9I-EVAL2 dev kit */
   I2C_InitStructure.I2C_DutyCycle           = I2C_DutyCycle_2;
   I2C_InitStructure.I2C_OwnAddress1         = 0x00;
   I2C_InitStructure.I2C_Ack                 = I2C_Ack_Enable;
   I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
   I2C_InitStructure.I2C_ClockSpeed          = s_I2C_Bus[iBus].i2c_bus_speed;

   /* Apply I2C configuration */
   I2C_Init( s_I2C_Bus[iBus].i2c_bus, &I2C_InitStructure );
   I2C_AcknowledgeConfig( s_I2C_Bus[iBus].i2c_bus, ENABLE );

   /* Configure I2C Filters */
   I2C_AnalogFilterCmd( s_I2C_Bus[iBus].i2c_bus, ENABLE );
   I2C_DigitalFilterConfig( s_I2C_Bus[iBus].i2c_bus, 0x0F );

   /* I2C Peripheral Enable */
   //   I2C_Cmd( s_I2C_Bus[iBus].i2c_bus, ENABLE );

}

/******************************************************************************/
void I2C_BusDeInit( I2C_Bus_t iBus )
{
   /* DeInit the I2C IP ----------------------------------------------------- */
   I2C_DeInit( s_I2C_Bus[iBus].i2c_bus );

   /* Disable I2C GPIO clocks */
   RCC_AHB1PeriphClockCmd(
         s_I2C_Bus[iBus].scl_clk | s_I2C_Bus[iBus].sda_clk,
         DISABLE
   );

   /* Disable I2C DMA clock */
   RCC_AHB1PeriphClockCmd( s_I2C_Bus[iBus].i2c_dma_clk, DISABLE );

   /* 2. Set up GPIO for input ---------------------------------------------- */

   /* Configure common elements of the SDA and SCL pins to not use I2C periph */
   GPIO_InitTypeDef  GPIO_InitStructure;
   GPIO_InitStructure.GPIO_Mode     = GPIO_Mode_IN;
   GPIO_InitStructure.GPIO_Speed    = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_OType    = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_PuPd     = GPIO_PuPd_NOPULL;

   /* Configure SCL specific settings for I2C */
   GPIO_InitStructure.GPIO_Pin      = s_I2C_Bus[iBus].scl_pin;
   GPIO_Init(s_I2C_Bus[iBus].scl_port, &GPIO_InitStructure);

   /* Configure SDA specific settings for I2C */
   GPIO_InitStructure.GPIO_Pin = s_I2C_Bus[iBus].sda_pin;
   GPIO_Init(s_I2C_Bus[iBus].sda_port, &GPIO_InitStructure);

   /* 3. DeInit DMA I2C ----------------------------------------------------- */
   DMA_Cmd( s_I2C_Bus[iBus].i2c_dma_rx_stream, DISABLE );
   DMA_DeInit( s_I2C_Bus[iBus].i2c_dma_rx_stream );

   DMA_Cmd( s_I2C_Bus[iBus].i2c_dma_tx_stream, DISABLE );
   DMA_DeInit( s_I2C_Bus[iBus].i2c_dma_tx_stream );
}

/******************************************************************************/
void I2C_BusInitForRecovery( I2C_Bus_t iBus )
{
   /* 1. DeInit the I2C bus */
   I2C_BusDeInit( iBus );

   /* 2. Set up GPIO for output --------------------------------------------- */

   /* Enable I2C GPIO clocks */
   RCC_AHB1PeriphClockCmd(
         s_I2C_Bus[iBus].scl_clk | s_I2C_Bus[iBus].sda_clk,
         ENABLE
   );

   /* Configure common elements of the SDA and SCL pins to not use I2C periph */
   GPIO_InitTypeDef  GPIO_InitStructure;
   GPIO_InitStructure.GPIO_Speed    = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_OType    = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_PuPd     = GPIO_PuPd_NOPULL;

   /* Configure SCL specific settings for I2C - Output for this pin since SCL
    * is the pin that will be toggled */
   GPIO_InitStructure.GPIO_Mode     = GPIO_Mode_OUT;
   GPIO_InitStructure.GPIO_Pin      = s_I2C_Bus[iBus].scl_pin;
   GPIO_Init(s_I2C_Bus[iBus].scl_port, &GPIO_InitStructure);

   /* Configure SDA specific settings for I2C - Input for this pin since SDA
    * needs to be checked to see if it's cleared after toggling the SCL line */
   GPIO_InitStructure.GPIO_Mode     = GPIO_Mode_IN;
   GPIO_InitStructure.GPIO_Pin = s_I2C_Bus[iBus].sda_pin;
   GPIO_Init(s_I2C_Bus[iBus].sda_port, &GPIO_InitStructure);

}

/******************************************************************************/
void I2C_SetDirection( I2C_Bus_t iBus,  uint8_t I2C_Direction )
{
   /* Check inputs */
   assert_param( IS_I2C_DIRECTION( I2C_Direction ) );
   assert_param( IS_I2C_BUS( iBus ) );

   s_I2C_Bus[iBus].bTransDirection = I2C_Direction;
}

/******************************************************************************/
uint8_t I2C_getDirection( I2C_Bus_t iBus )
{
   /* Check inputs */
   assert_param( IS_I2C_BUS( iBus ) );

   return( s_I2C_Bus[iBus].bTransDirection );
}

/******************************************************************************/
void I2C_StartDMARead( I2C_Bus_t iBus, uint16_t wReadLen )
{
   /* Check inputs */
   assert_param( IS_I2C_BUS( iBus ) );

   /* Set the structure buffer management for how many bytes to expect and how
    * many are currently there (none). */
   s_I2C_Bus[iBus].nBytesExpected = wReadLen;
   s_I2C_Bus[iBus].nBytesCurrent  = 0;
   s_I2C_Bus[iBus].nRxIndex       = 0;

   /* Clear out the DMA settings */
   DMA_Cmd( s_I2C_Bus[iBus].i2c_dma_rx_stream, DISABLE );
   DMA_DeInit( s_I2C_Bus[iBus].i2c_dma_rx_stream );

   /* Set up a new DMA structure for reading */
   DMA_InitTypeDef    DMA_InitStructure;
   DMA_InitStructure.DMA_Channel             = s_I2C_Bus[iBus].i2c_dma_channel;
   DMA_InitStructure.DMA_PeripheralBaseAddr  = s_I2C_Bus[iBus].i2c_dma_dr_addr;
   DMA_InitStructure.DMA_DIR                 = DMA_DIR_PeripheralToMemory;
   DMA_InitStructure.DMA_PeripheralInc       = DMA_PeripheralInc_Disable;
   DMA_InitStructure.DMA_MemoryInc           = DMA_MemoryInc_Enable;
   DMA_InitStructure.DMA_PeripheralDataSize  = DMA_PeripheralDataSize_Byte;
   DMA_InitStructure.DMA_MemoryDataSize      = DMA_MemoryDataSize_Byte;
   DMA_InitStructure.DMA_Mode                = DMA_Mode_Normal;
   DMA_InitStructure.DMA_Priority            = DMA_Priority_VeryHigh;
   DMA_InitStructure.DMA_FIFOMode            = DMA_FIFOMode_Enable;
   DMA_InitStructure.DMA_FIFOThreshold       = DMA_FIFOThreshold_Full;
   DMA_InitStructure.DMA_MemoryBurst         = DMA_MemoryBurst_Single;
   DMA_InitStructure.DMA_PeripheralBurst     = DMA_PeripheralBurst_Single;
   DMA_InitStructure.DMA_Memory0BaseAddr     = (uint32_t)s_I2C_Bus[iBus].pRxBuffer;
   DMA_InitStructure.DMA_BufferSize          = s_I2C_Bus[iBus].nBytesExpected;

   DBG_printf("Attempting to read via DMA %d bytes\n", DMA_InitStructure.DMA_BufferSize);

   /* Initialize the DMA with the filled in structure */
   DMA_Init( s_I2C_Bus[iBus].i2c_dma_rx_stream, &DMA_InitStructure );

   /* Enable the TransferComplete interrupt in the DMA */
   DMA_ITConfig( s_I2C_Bus[iBus].i2c_dma_rx_stream, DMA_IT_TC, ENABLE );

   /* Clear any pending flags on RX Stream */
   DMA_ClearFlag(
         s_I2C_Bus[iBus].i2c_dma_rx_stream,
         s_I2C_Bus[iBus].i2c_dma_rx_flags
   );

   /* I2Cx DMA Enable */
   I2C_DMACmd( s_I2C_Bus[iBus].i2c_bus, ENABLE );

   /* Finally, activate DMA */
   DMA_Cmd( s_I2C_Bus[iBus].i2c_dma_rx_stream, ENABLE );

}

/******************************************************************************/
void I2C_StartDMAWrite( I2C_Bus_t iBus, uint16_t wWriteLen )
{
   /* Check inputs */
   assert_param( IS_I2C_BUS( iBus ) );

   /* Set the structure buffer management for how many bytes to expect and how
    * many are currently there (none). */
   s_I2C_Bus[iBus].nBytesExpected = wWriteLen;
   s_I2C_Bus[iBus].nBytesCurrent  = 0;
   s_I2C_Bus[iBus].nTxIndex       = 0;

   /* Clear out the DMA settings */
   DMA_Cmd( s_I2C_Bus[iBus].i2c_dma_tx_stream, DISABLE );
   DMA_DeInit( s_I2C_Bus[iBus].i2c_dma_tx_stream );

   /* Set up a new DMA structure for reading */
   DMA_InitTypeDef    DMA_InitStructure;
   DMA_InitStructure.DMA_Channel             = s_I2C_Bus[iBus].i2c_dma_channel;
   DMA_InitStructure.DMA_PeripheralBaseAddr  = s_I2C_Bus[iBus].i2c_dma_dr_addr;
   DMA_InitStructure.DMA_DIR                 = DMA_DIR_MemoryToPeripheral;
   DMA_InitStructure.DMA_PeripheralInc       = DMA_PeripheralInc_Disable;
   DMA_InitStructure.DMA_MemoryInc           = DMA_MemoryInc_Enable;
   DMA_InitStructure.DMA_PeripheralDataSize  = DMA_PeripheralDataSize_Byte;
   DMA_InitStructure.DMA_MemoryDataSize      = DMA_MemoryDataSize_Byte;
   DMA_InitStructure.DMA_Mode                = DMA_Mode_Normal;
   DMA_InitStructure.DMA_Priority            = DMA_Priority_VeryHigh;
   DMA_InitStructure.DMA_FIFOMode            = DMA_FIFOMode_Enable;
   DMA_InitStructure.DMA_FIFOThreshold       = DMA_FIFOThreshold_Full;
   DMA_InitStructure.DMA_MemoryBurst         = DMA_MemoryBurst_Single;
   DMA_InitStructure.DMA_PeripheralBurst     = DMA_PeripheralBurst_Single;
   DMA_InitStructure.DMA_Memory0BaseAddr     = (uint32_t)s_I2C_Bus[iBus].pTxBuffer;
   DMA_InitStructure.DMA_BufferSize          = s_I2C_Bus[iBus].nBytesExpected;

   DBG_printf("Attempting to write via DMA %d bytes\n", DMA_InitStructure.DMA_BufferSize);

   /* Initialize the DMA with the filled in structure */
   DMA_Init( s_I2C_Bus[iBus].i2c_dma_tx_stream, &DMA_InitStructure );

   /* Enable the TransferComplete interrupt in the DMA */
   DMA_ITConfig( s_I2C_Bus[iBus].i2c_dma_tx_stream, DMA_IT_TC, ENABLE );

   /* Clear any pending flags on RX Stream */
   DMA_ClearFlag(
         s_I2C_Bus[iBus].i2c_dma_tx_stream,
         s_I2C_Bus[iBus].i2c_dma_tx_flags
   );

   /* I2Cx DMA Enable */
   I2C_DMACmd( s_I2C_Bus[iBus].i2c_bus, ENABLE );

   /* Finally, activate DMA */
   DMA_Cmd( s_I2C_Bus[iBus].i2c_dma_tx_stream, ENABLE );
}

/******************************************************************************/
CBErrorCode I2C_readBufferBLK(
      I2C_Bus_t iBus,
      uint8_t i2cDevAddr,
      uint16_t i2cMemAddr,
      uint8_t i2cMemAddrSize,
      uint8_t* pBuffer,
      uint16_t bytesToRead
)
{
   /* Call the common setup for reads and writes for memory devices on I2C */
   CBErrorCode status = I2C_setupMemRW(
         iBus,
         i2cDevAddr,
         i2cMemAddr,
         i2cMemAddrSize
   );
   if ( ERR_NONE != status ) {
      return I2C_TIMEOUT_UserCallback(iBus, status);
   }

   /*!< Send STRAT condition a second time */
   I2C_GenerateSTART(s_I2C_Bus[iBus].i2c_bus, ENABLE);

   /*!< Test on EV5 and clear it (cleared by reading SR1 then writing to DR) */
   uint32_t nI2CBusTimeout = I2C_LONG_TIMEOUT;
   status = ERR_I2CBUS_EV5_TIMEOUT;
   while(!I2C_CheckEvent(s_I2C_Bus[iBus].i2c_bus, I2C_EVENT_MASTER_MODE_SELECT)) {
      if((nI2CBusTimeout--) == 0) return I2C_TIMEOUT_UserCallback(iBus, status);
   }

   /*!< Send EEPROM address for read */
   I2C_Send7bitAddress(s_I2C_Bus[iBus].i2c_bus, (uint8_t)(i2cDevAddr & 0x00FF),  I2C_Direction_Receiver);

   /* Wait on ADDR flag to be set (ADDR is still not cleared at this level */
   nI2CBusTimeout = I2C_LONG_TIMEOUT;
   status = ERR_I2CBUS_EV6_TIMEOUT;
   while( !I2C_CheckEvent(s_I2C_Bus[iBus].i2c_bus, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) ) {
      if((nI2CBusTimeout--) == 0) return I2C_TIMEOUT_UserCallback(iBus, status);
   }

   while ( bytesToRead > 1 ) {
      nI2CBusTimeout = I2C_LONG_TIMEOUT;
      status = ERR_I2CBUS_RXNE_FLAG_TIMEOUT;
      while(I2C_GetFlagStatus(s_I2C_Bus[iBus].i2c_bus, I2C_FLAG_RXNE) == RESET) {
         if((nI2CBusTimeout--) == 0) return I2C_TIMEOUT_UserCallback(iBus, status);
      }

      /* Read the byte received from the EEPROM */
      *pBuffer = I2C_ReceiveData(s_I2C_Bus[iBus].i2c_bus);
      pBuffer++;

      /* Decrement the read bytes counter */
      bytesToRead--;
   }

   /*!< Disable Acknowledgement */
   I2C_AcknowledgeConfig(s_I2C_Bus[iBus].i2c_bus, DISABLE);

   /* Clear ADDR register by reading SR1 then SR2 register (SR1 has already been read) */
   (void)s_I2C_Bus[iBus].i2c_bus->SR2;

   /*!< Send STOP Condition */
   I2C_GenerateSTOP(s_I2C_Bus[iBus].i2c_bus, ENABLE);

   /* Wait for the byte to be received */
   nI2CBusTimeout = I2C_LONG_TIMEOUT;
   status = ERR_I2CBUS_RXNE_FLAG_TIMEOUT;
   while(I2C_GetFlagStatus(s_I2C_Bus[iBus].i2c_bus, I2C_FLAG_RXNE) == RESET) {
      if((nI2CBusTimeout--) == 0) return I2C_TIMEOUT_UserCallback(iBus, status);
   }

   /*!< Read the byte received from the EEPROM */
   *pBuffer = I2C_ReceiveData(s_I2C_Bus[iBus].i2c_bus);

   /*!< Decrement the read bytes counter */
   bytesToRead--;

   /* Wait to make sure that STOP control bit has been cleared */
   nI2CBusTimeout = I2C_LONG_TIMEOUT;
   status = ERR_I2CBUS_STOP_BIT_TIMEOUT;
   while(s_I2C_Bus[iBus].i2c_bus->CR1 & I2C_CR1_STOP) {
      if((nI2CBusTimeout--) == 0) return I2C_TIMEOUT_UserCallback(iBus, status);
   }

   /*!< Re-Enable Acknowledgement to be ready for another reception */
   I2C_AcknowledgeConfig(s_I2C_Bus[iBus].i2c_bus, ENABLE);

   status = ERR_NONE;  /* Just for consistency, set the status and return it. */
   return( status );
}

/******************************************************************************/
CBErrorCode I2C_writeBufferBLK(
      I2C_Bus_t iBus,
      uint8_t i2cDevAddr,
      uint16_t i2cMemAddr,
      uint8_t i2cMemAddrSize,
      uint8_t* pBuffer,
      uint16_t bytesToWrite
)
{
   /* Figure out how to lay out the data over the page boundaries */
   uint8_t pageSize = 0;
   uint8_t writeSizeFirstPage = 0;
   uint8_t writeSizeLastPage = 0;
   uint8_t writeTotalPages = 0;
   CBErrorCode status = I2C_calcEepromPageWriteSizes(
         &pageSize,
         &writeSizeFirstPage,
         &writeSizeLastPage,
         &writeTotalPages,
         i2cMemAddr,
         bytesToWrite
   );

   if ( ERR_NONE != status ) {
      return I2C_TIMEOUT_UserCallback(iBus, status);
   }

   uint16_t writeSizeCurr = 0;            /* To keep track of how many bytes to
                                             write each iteration */
   uint16_t i2cMemAddrCurr = i2cMemAddr;  /* For updating the write address for
                                             each iteration */

   uint8_t bufferIndex = 0;               /* To keep track of where in the
                                             buffer we are for each iteration */

   for ( uint8_t page = 0; page < writeTotalPages; page++ ) {
      /* Figure out if we are writing first, last, or any of the middle pages
       * and set the current write size accordingly. */
      if ( page == 0 ) {                                        /* First page */
         writeSizeCurr = writeSizeFirstPage;
      } else if (page == (writeTotalPages-1)) {                  /* Last page */
         writeSizeCurr = writeSizeLastPage;
      } else {                                            /* Some middle page */
         writeSizeCurr = pageSize;
      }

      /* Do the write for this loop iteration */
      status = I2C_writePageBLK(
            iBus,
            i2cDevAddr,
            i2cMemAddrCurr,
            i2cMemAddrSize,
            &pBuffer[bufferIndex],
            writeSizeCurr
      );

      if ( ERR_NONE != status ) {
         return I2C_TIMEOUT_UserCallback(iBus, status);
      }

      /* Update the address and the pointer in the buffer for next iteration */
      i2cMemAddrCurr += writeSizeCurr;
      bufferIndex += writeSizeCurr;
   }

   return( status );
}

/******************************************************************************/
static CBErrorCode I2C_writePageBLK(
      I2C_Bus_t iBus,
      uint8_t i2cDevAddr,
      uint16_t i2cMemAddr,
      uint8_t i2cMemAddrSize,
      uint8_t* pBuffer,
      uint16_t bytesToWrite
)
{
   /* Call the common setup for reads and writes for memory devices on I2C */
   CBErrorCode status = I2C_setupMemRW(
         iBus,
         i2cDevAddr,
         i2cMemAddr,
         i2cMemAddrSize
   );
   if ( ERR_NONE != status ) {
      return I2C_TIMEOUT_UserCallback(iBus, status);
   }

   while( bytesToWrite ) {

      /* Write a byte */
      I2C_SendData(s_I2C_Bus[iBus].i2c_bus, *pBuffer);

      uint32_t nI2CBusTimeout = I2C_LONG_TIMEOUT;
      status = ERR_I2CBUS_WRITE_BYTE_TIMEOUT;
      while(!I2C_CheckEvent(s_I2C_Bus[iBus].i2c_bus, I2C_EVENT_MASTER_BYTE_TRANSMITTING)) {
         if((nI2CBusTimeout--) == 0) return I2C_TIMEOUT_UserCallback(iBus, status);
      }
      ++pBuffer;                 /* Increment after the write was successful. */
      --bytesToWrite;           /* Decrement how many bytes are left to write */
   }

   /* Wait to make sure that STOP control bit has been cleared */
   uint32_t nI2CBusTimeout = I2C_LONG_TIMEOUT;
   status = ERR_I2CBUS_WRITE_BYTE_TIMEOUT;
   while(!I2C_GetFlagStatus(s_I2C_Bus[iBus].i2c_bus, I2C_FLAG_BTF)) {
      if((nI2CBusTimeout--) == 0) return I2C_TIMEOUT_UserCallback(iBus, status);
   }

   /*!< Send STOP Condition */
   I2C_GenerateSTOP(s_I2C_Bus[iBus].i2c_bus, ENABLE);

   /* Wait to make sure that STOP control bit has been cleared */
   nI2CBusTimeout = I2C_LONG_TIMEOUT;
   status = ERR_I2CBUS_STOP_BIT_TIMEOUT;
   while(s_I2C_Bus[iBus].i2c_bus->CR1 & I2C_CR1_STOP) {
      if((nI2CBusTimeout--) == 0) return I2C_TIMEOUT_UserCallback(iBus, status);
   }

   /* Now need to wait 5ms for a write out time (see ATMLH224 documentation) */

   /* This is a blocking delay function and is exactly why this function or any
    * that call it SHOULD NOT be called when threads/AOs are running. */
   BSP_Delay( (SystemCoreClock / BSP_TICKS_PER_SEC) * 5 );

   status = ERR_NONE;  /* Just for consistency, set the status and return it. */
   return( status );
}

/******************************************************************************/
static CBErrorCode I2C_setupMemRW(
      I2C_Bus_t iBus,
      uint8_t i2cDevAddr,
      uint16_t i2cMemAddr,
      uint8_t i2cMemAddrSize
)
{
   CBErrorCode status;

   /*!< While the bus is busy */
   uint32_t nI2CBusTimeout = I2C_LONG_TIMEOUT;
   status = ERR_I2CBUS_BUSY;
   while(RESET != I2C_GetFlagStatus(s_I2C_Bus[iBus].i2c_bus, I2C_FLAG_BUSY)) {
      if((nI2CBusTimeout--) == 0) return I2C_TIMEOUT_UserCallback(iBus, status);
   }

   /*!< Send START condition */
   I2C_GenerateSTART(s_I2C_Bus[iBus].i2c_bus, ENABLE);

   /*!< Test on EV5 and clear it (cleared by reading SR1 then writing to DR) */
   nI2CBusTimeout = I2C_LONG_TIMEOUT;
   status = ERR_I2CBUS_EV5_TIMEOUT;
   while(!I2C_CheckEvent(s_I2C_Bus[iBus].i2c_bus, I2C_EVENT_MASTER_MODE_SELECT)) {
      if((nI2CBusTimeout--) == 0) return I2C_TIMEOUT_UserCallback(iBus, status);
   }

   /*!< Send EEPROM address for write */
   I2C_Send7bitAddress(s_I2C_Bus[iBus].i2c_bus, i2cDevAddr, I2C_Direction_Transmitter);

   /*!< Test on EV6 and clear it */
   nI2CBusTimeout = I2C_LONG_TIMEOUT;
   status = ERR_I2CBUS_EV6_TIMEOUT;
   while(!I2C_CheckEvent(s_I2C_Bus[iBus].i2c_bus, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
      if((nI2CBusTimeout--) == 0) return I2C_TIMEOUT_UserCallback(iBus, status);
   }

   /* Some devices have 16 bit addresses so we have to send MSB first and then
    * LSB.  Otherwise, we can skip the MSB and just send the LSB */
   if (2 == i2cMemAddrSize ) {
      /*!< Send the EEPROM's internal address to write to: MSB of the address first */
      I2C_SendData(s_I2C_Bus[iBus].i2c_bus, (uint8_t)((i2cMemAddr & 0xFF00) >> 8));

      /*!< Test on EV8 and clear it */
      nI2CBusTimeout = I2C_LONG_TIMEOUT;
      status = ERR_I2CBUS_EV8_TIMEOUT;
      while(!I2C_CheckEvent(s_I2C_Bus[iBus].i2c_bus, I2C_EVENT_MASTER_BYTE_TRANSMITTING)) {
         if((nI2CBusTimeout--) == 0) return I2C_TIMEOUT_UserCallback(iBus, status);
      }
   }

   /*!< Send the EEPROM's internal address to write to: LSB of the address */
   I2C_SendData(s_I2C_Bus[iBus].i2c_bus, (uint8_t)(i2cMemAddr & 0x00FF));

   /*!< Test on EV8 and clear it */
   nI2CBusTimeout = I2C_LONG_TIMEOUT;
   status = ERR_I2CBUS_EV8_TIMEOUT;
   while(I2C_GetFlagStatus(s_I2C_Bus[iBus].i2c_bus, I2C_FLAG_BTF) == RESET) {
      if((nI2CBusTimeout--) == 0) return I2C_TIMEOUT_UserCallback(iBus, status);
   }

   status = ERR_NONE;  /* Just for consistency, set the status and return it. */
   return( status );
}

/******************************************************************************/
/***                      Callback functions for I2C                        ***/
/******************************************************************************/

/******************************************************************************/
inline void I2C1_DMAReadCallback( void )
{
   /* Test on DMA Stream Transfer Complete interrupt */
   if ( RESET != DMA_GetITStatus(DMA1_Stream0, DMA_IT_TCIF0) ) {
      /* Start of STM32 I2C HW bug workaround:
       * This is a workaround for the STM32 I2C hardware bug.  You have to send
       * the STOP bit before receiving the last byte.  In the case of DMA xfers,
       * this means doing it before you shut off the DMA stream.
       * See the STM32 errata for more details. - HR */

      I2C_AcknowledgeConfig( I2C1, DISABLE);        /* Disable Acknowledgment */

      /* Disable DMA so it doesn't keep outputting the buffer. */
      //      DMA_Cmd( DMA1_Stream0, DISABLE );
      I2C_GenerateSTOP(I2C1, ENABLE);                        /* Generate Stop */

      /* Wait for the byte to be received.
       * Note: There's really no way around waiting for this flag to be reset
       * outside of the ISR since you have to do this BEFORE you shut off DMA
       * which has to be done in this ISR. Thankfully, this flag gets reset
       * fairly quickly (125 times through the loop). - HR. */
      uint16_t nI2CBusTimeout = MAX_I2C_TIMEOUT;
      while(I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) == RESET) {
         if((nI2CBusTimeout--) == 0) {
            ERR_printf("Timeout waiting for I2C Stop bit flag reset!\n");

            /* Error condition.  Make sure to do proper cleanup*/
            goto I2C1_DMAReadCallback_cleanup;
         }
      }

      I2C_AcknowledgeConfig(I2C1, ENABLE);        /* Re-enable Acknowledgment */
      /* End of STM32 I2C HW bug workaround */

      //      /* Disable DMA so it doesn't keep outputting the buffer. */
      DMA_Cmd( DMA1_Stream0, DISABLE );

      /* Don't transport the data with the event.  The appropriate I2CBusMgr AO
       * will handle copying out the data from the buffers.  Nobody else is
       * allowed to touch this bus so there's no contention. */
      static QEvt const qEvt = { I2C_BUS_DMA_DONE_SIG, 0U, 0U };
      QACTIVE_POST(AO_I2CBusMgr[I2CBus1], &qEvt, AO_I2CBusMgr[I2CBus1]);

      //      DBG_printf("%d loop iters\n", MAX_I2C_TIMEOUT - nI2CBusTimeout);

      /* Tag for a common exit from this handler.  DON'T call return.  Instead, use
       * goto I2C1_DMAReadCallback_cleanup;
       * This allows for always correctly clearing the interrupt status bits. Else
       * things will lock up. */
      I2C1_DMAReadCallback_cleanup:

      /* Clear DMA Stream Transfer Complete interrupt pending bit */
      DMA_ClearITPendingBit( DMA1_Stream0, DMA_IT_TCIF0 );
   }
}

/******************************************************************************/
inline void I2C1_DMAWriteCallback( void )
{
   /* Test on DMA Stream Transfer Complete interrupt */
   if ( RESET != DMA_GetITStatus(DMA1_Stream6, DMA_IT_TCIF6) ) {

      /* Wait for the byte to be sent over the I2C bus.
       * Note: There's really no way around waiting for this flag to be reset
       * outside of the ISR since you have to do this BEFORE you shut off DMA
       * which has to be done in this ISR. Thankfully, this flag gets reset
       * fairly quickly (125 times through the loop at 100kHz and 57 at 400kHz).
       * - HR. */
      uint16_t nI2CBusTimeout = MAX_I2C_TIMEOUT;
      while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF) == RESET) {
         if((nI2CBusTimeout--) == 0) {
            ERR_printf("Timeout waiting for I2C Stop bit flag reset!\n");

            /* Error condition.  Make sure to do proper cleanup*/
            goto I2C1_DMAWriteCallback_cleanup;
         }
      }

      I2C_GenerateSTOP(I2C1, ENABLE);                        /* Generate Stop */

      DMA_Cmd( DMA1_Stream6, DISABLE );            /* Disable the DMA stream. */

      /* Directly post event to the correct I2CBusMgr AO instance indicating
       * that the DMA is finished. Let it decide what to do. */
      static QEvt const qEvt = { I2C_BUS_DMA_DONE_SIG, 0U, 0U };
      QACTIVE_POST(AO_I2CBusMgr[I2CBus1], &qEvt, AO_I2CBusMgr[I2CBus1]);

      DBG_printf("%d loop iters\n", MAX_I2C_TIMEOUT - nI2CBusTimeout);

      /* Tag for a common exit from this handler.  DON'T call return.  Instead, use
       * goto I2C1_DMAWriteCallback_cleanup;
       * This allows for always correctly clearing the interrupt status bits. Else
       * things will lock up. */
      I2C1_DMAWriteCallback_cleanup:
      /* Clear DMA Stream Transfer Complete interrupt pending bit */
      DMA_ClearITPendingBit( DMA1_Stream6, DMA_IT_TCIF6 );
   }
}

/******************************************************************************/
inline void I2C1_EventCallback( void )
{
   QF_CRIT_STAT_TYPE intStat;
   BaseType_t lHigherPriorityTaskWoken = pdFALSE;

   QF_ISR_ENTRY(intStat);                        /* inform QF about ISR entry */

   QF_ISR_EXIT(intStat, lHigherPriorityTaskWoken);/* inform QF about ISR exit */

   /* the usual end of FreeRTOS ISR... */
   portEND_SWITCHING_ISR(lHigherPriorityTaskWoken);
}

/******************************************************************************/
inline void I2C1_ErrorEventCallback( void )
{
   /* Read SR1 register to get I2C error */
   __IO uint16_t regVal = I2C_ReadRegister(I2C1, I2C_Register_SR1) & 0xFF00;
   if (regVal != 0x0000) {
      /* Clears error flags */
      I2C1->SR1 &= 0x00FF;

      ERR_printf("I2C Error: 0x%04x.  Resetting error bits\n", regVal);
      I2C_BusInit( I2CBus1 );
   }
}

/******************************************************************************/
static CBErrorCode I2C_TIMEOUT_UserCallbackRaw(
      I2C_Bus_t iBus,
      CBErrorCode error,
      const char *func,
      int line
)
{
   if ( ERR_NONE != error ) {
      /* Use the slow interface to print out the error and where it occured. */
      err_slow_printf("I2C%d bus error 0x%08x at %s():%d\n", iBus+1, error, func, line);
   }
   /* Return the error code so the top level can handle it. */
   return( error );
}

/**
 * @}
 * end addtogroup groupI2C
 */

/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
