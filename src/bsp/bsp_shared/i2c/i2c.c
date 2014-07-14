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
#include "qp_port.h"                                        /* for QP support */
#include "stm32f2xx_it.h"
#include "project_includes.h"
#include "Shared.h"
#include "I2CMgr.h"                                    /* For I2C event types */

/* Compile-time called macros ------------------------------------------------*/
Q_DEFINE_THIS_FILE                  /* For QSPY to know the name of this file */

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
#define EE_START_ADDR      0x00 /**< EEProms internal memory starting address */
#define EE_PAGESIZE        32    /**< EEProm's internal page size (in bytes). */

/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/
/**
 * @brief Buffer for I2C device
 */
static uint8_t          i2cRxBuffer[MAX_MSG_LEN];

/**
 * @brief An internal structure that holds settings for I2C devices on all I2C
 * busses.
 */
static I2C_BusDevice_t s_I2C_Dev[MAX_I2C_DEV] =
{
      {
            EEPROM,                    /**< i2c_dev */
            I2C1,                      /**< i2c_bus */

            0xA0,                      /**< i2c_address */
      }
};

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
            100000,                    /**< i2c_bus_speed (Hz)*/
            RCC_APB1Periph_I2C1,       /**< i2c_clk */

            I2C1_EV_IRQn,              /**< i2c_ev_irq_num */
            I2C1_EV_PRIO,              /**< i2c_ev_irq_prio */
            I2C1_ER_IRQn,              /**< i2c_er_irq_num */
            I2C1_ER_PRIO,              /**< i2c_er_irq_prio */

            GPIOB,                     /**< scl_port */
            GPIO_Pin_8,                /**< scl_pin */
            GPIO_PinSource8,           /**< scl_af_pin_source */
            GPIO_AF_I2C1,              /**< scl_af */

            GPIOB,                     /**< sda_port */
            GPIO_Pin_9,                /**< sda_pin */
            GPIO_PinSource9,           /**< sda_af_pin_source */
            GPIO_AF_I2C1,              /**< sda_af */

            /* Buffer management */
            &i2cRxBuffer[0],           /**< *pRXBuffer */
            0,                         /**< nRXindex */

            /* Device management */
            EEPROM,                    /**< i2c_cur_dev */
            0xA0,                      /**< i2c_cur_dev_addr */
            I2C_Direction_Transmitter, /**< bTransDirection */
            0,                         /**< nBytesExpected */
            0,                         /**< nBytesCurrent */
      }
};

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
void I2C_BusInit( I2C_Bus_t iBus )
{
   /* Periph clock enable.  The GPIO clocks should already be enabled by BSP_Init() */
   RCC_APB1PeriphClockCmd(s_I2C_Bus[iBus].i2c_clk, ENABLE);

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

   /* Configure I2C pins: SCL */
   GPIO_InitTypeDef  GPIO_InitStructure;
   GPIO_InitStructure.GPIO_Pin      = s_I2C_Bus[iBus].scl_pin;
   GPIO_InitStructure.GPIO_Mode     = GPIO_Mode_AF;
   GPIO_InitStructure.GPIO_Speed    = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_OType    = GPIO_OType_OD;
   GPIO_InitStructure.GPIO_PuPd     = GPIO_PuPd_NOPULL;
   GPIO_Init(s_I2C_Bus[iBus].scl_port, &GPIO_InitStructure);

   /* Configure I2C pins: SDA */
   GPIO_InitStructure.GPIO_Pin = s_I2C_Bus[iBus].sda_pin;
   GPIO_Init(s_I2C_Bus[iBus].sda_port, &GPIO_InitStructure);

   /* Reset I2C IP */
   I2C_DeInit( s_I2C_Bus[iBus].i2c_bus );

   /* I2C configuration */
   I2C_InitTypeDef  I2C_InitStructure;
   I2C_InitStructure.I2C_Mode                = I2C_Mode_SMBusHost;
   I2C_InitStructure.I2C_DutyCycle           = I2C_DutyCycle_2;
   I2C_InitStructure.I2C_OwnAddress1         = s_I2C_Bus[iBus].i2c_cur_dev_addr;
   I2C_InitStructure.I2C_Ack                 = I2C_Ack_Enable;
   I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
   I2C_InitStructure.I2C_ClockSpeed          = s_I2C_Bus[iBus].i2c_bus_speed;

   /* Set up Interrupt controller to handle I2C Event interrupts */
//   NVIC_Config(
//         s_I2C_Bus[iBus].i2c_ev_irq_num,
//         s_I2C_Bus[iBus].i2c_ev_irq_prio
//   );

   /* Set up Interrupt controller to handle I2C Error interrupts */
//   NVIC_Config(
//         s_I2C_Bus[iBus].i2c_er_irq_num,
//         s_I2C_Bus[iBus].i2c_er_irq_prio
//   );

   /* Enable All I2C Interrupts */
//   I2C_ITConfig(
//         s_I2C_Bus[iBus].i2c_bus,
//         I2C_IT_EVT, // | I2C_IT_BUF, // | I2C_IT_ERR,
//         ENABLE
//   );

   /* Apply I2C configuration */
   I2C_Init( s_I2C_Bus[iBus].i2c_bus, &I2C_InitStructure );
   I2C_AcknowledgeConfig(s_I2C_Bus[iBus].i2c_bus, ENABLE );

   /* I2C Peripheral Enable */
   I2C_Cmd( s_I2C_Bus[iBus].i2c_bus, ENABLE );

}

/******************************************************************************/
void I2C_SetDirection( I2C_Bus_t iBus,  uint8_t I2C_Direction)
{
   /* Check inputs */
   assert_param( IS_I2C_DIRECTION( I2C_Direction ) );
   assert_param( IS_I2C_BUS( iBus ) );

   s_I2C_Bus[iBus].bTransDirection = I2C_Direction;
}

/**
 * @}
 * end addtogroup groupI2C
 */

/******************************************************************************/
void I2C1_EV_IRQHandler( void )
{
   QK_ISR_ENTRY();                         /* inform QK about entering an ISR */

   /* Get Last I2C Event */
   __IO uint32_t I2CEvent = I2C_GetLastEvent( I2C1 );

   /* Check which I2C device on this bus is being used */
//   if ( EEPROM == s_I2C_Bus[I2CBus1].i2c_cur_dev ) {
      switch (I2CEvent) {
         //      case 0x00000000:
         //      case 0x00000001:
         //      case 0x00004001:
         //      case 0x00034044:
         //         /* This is the I2C "Event" that is always triggering so we explicitly
         //          * ignore it here */
         //         break;

         case I2C_EVENT_MASTER_MODE_SELECT:

            /* Send slave Address for write */
            I2C_Send7bitAddress(
                  I2C1,                                  // This is always the bus used in this ISR
                  s_I2C_Bus[I2CBus1].i2c_cur_dev_addr,   // Look up the current device address for this bus
                  s_I2C_Bus[I2CBus1].bTransDirection     // Direction of data on this bus
            );
//            I2C_Send7bitAddress(
//                  I2C1,                                  // This is always the bus used in this ISR
//                  0xA0,   // Look up the current device address for this bus
//                  I2C_Direction_Receiver     // Direction of data on this bus
//            );
//            isr_dbg_slow_printf("I2C_EVENT_MASTER_MODE_SELECT\n");
            break;


         /* Check on EV6 */
         case I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED:

            /* Send the EEPROM's internal address to read from:
             * MSB of the address first */
//            I2C_SendData(I2C1, (uint8_t)((0x00 & 0xFF00) >> 8));
            isr_dbg_slow_printf("I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED\n");


//            /*!< Send the EEPROM's internal address to read from: MSB of the address first */
//            I2C_SendData(I2C1, (uint8_t)((0x0000 & 0xFF00) >> 8));
//
//            /*!< Test on EV8 and clear it */
//            uint16_t sEETimeout = 10000;
//            while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
//            {
//               if((sEETimeout--) == 0) {
//                  isr_dbg_slow_printf("Timed out sending addr msb\n");
//               }
//            }
//
//            /*!< Send the EEPROM's internal address to read from: LSB of the address */
//            I2C_SendData(I2C1, (uint8_t)(0x0000 & 0x00FF));
//            sEETimeout = 10000;
//            while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF) == RESET)
//            {
//               if((sEETimeout--) == 0) {
//                  isr_dbg_slow_printf("Timed out sending addr lsb\n");
//               }
//            }
//            isr_dbg_slow_printf("Sent addr\n");
            /* Create and publish event for I2CMgr */
            QEvt *qEvt = Q_NEW( QEvt, I2C_MSTR_MODE_SELECTED_SIG );
            QF_PUBLISH( (QEvt *)qEvt, AO_I2CMgr );
            break;

         /* Check on EV8 */
         case I2C_EVENT_MASTER_BYTE_TRANSMITTING:
            isr_dbg_slow_printf("I2C_EVENT_MASTER_BYTE_TRANSMITTING\n");
            break;

         case I2C_EVENT_MASTER_BYTE_TRANSMITTED:
            isr_dbg_slow_printf("I2C_EVENT_MASTER_BYTE_TRANSMITTED\n");
            /* Create and publish event for I2CMgr */
            QEvt *qEvt1 = Q_NEW( QEvt, I2C_MSTR_BYTE_TRANSMITTED_SIG );
            QF_PUBLISH( (QEvt *)qEvt1, AO_I2CMgr );

            //         if (Tx_Idx == (uint8_t)NumberOfByteToTransmit) {
            //            /* Send STOP condition */
            //            I2C_GenerateSTOP(I2C1, ENABLE);
            //            I2C_ITConfig(I2C1, I2C_IT_EVT | I2C_IT_BUF, DISABLE);
            //         } else {
            //            /* Transmit Data TxBuffer */
            //            I2C_SendData(I2C1, TxBuffer[Tx_Idx++]);
            //         }

            break;

//         case I2C_EVENT_MASTER_BYTE_RECEIVED:
//            isr_dbg_slow_printf("I2C_EVENT_MASTER_BYTE_RECEIVED\n");
//
//            /* Read the byte received */
//            s_I2C_Bus[I2CBus1].pRxBuffer[ s_I2C_Bus[I2CBus1].nRxindex++ ] = I2C_ReceiveData( I2C1 );
//               ++s_I2C_Bus[I2CBus1].nBytesCurrent;
//
//               /* Start
//                * This is a workaround for the STM32 I2C hardware bug.  You have
//                * to send the STOP bit before receiving the last byte.  See the
//                * STM32 errata for more details. */
//               if ( 1 == s_I2C_Bus[I2CBus1].nBytesExpected - s_I2C_Bus[I2CBus1].nBytesCurrent) {
//                  /* Disable Acknowledgment */
//                  I2C_AcknowledgeConfig( I2C1, DISABLE );
//
//                  /* Send STOP Condition */
//                  I2C_GenerateSTOP( I2C1, ENABLE );
//               }/* End */
//
//               /* Check if all the expected bytes have been read in from the bus */
//               if ( s_I2C_Bus[I2CBus1].nBytesExpected == s_I2C_Bus[I2CBus1].nBytesCurrent ) {
//                  /* Create an event and fill it with the data from the RX buffer of the bus.*/
//                  I2CDataEvt *qEvtI2CReadDone = Q_NEW( I2CDataEvt, I2C_READ_DONE_SIG );
//                  qEvtI2CReadDone->i2cDevice = EEPROM;
//                  qEvtI2CReadDone->wBufferLen = s_I2C_Bus[I2CBus1].nBytesCurrent;
//                  MEMCPY(
//                        qEvtI2CReadDone->buffer,
//                        s_I2C_Bus[I2CBus1].pRxBuffer,
//                        qEvtI2CReadDone->wBufferLen
//                  );
//                  QF_PUBLISH( (QEvt *)qEvtI2CReadDone, AO_I2CMgr );
//               }
//
//               /* Re-Enable Acknowledgment to be ready for another reception */
//               I2C_AcknowledgeConfig(I2C1, ENABLE);
//
//               break;
            default:
               isr_dbg_slow_printf("dc: %08x\n", (unsigned int)I2CEvent);
               break;

      }
//   }else {
//      isr_dbg_slow_printf("Invalid I2C device selected: %d\n", s_I2C_Bus[I2CBus1].i2c_cur_dev );
//   }

 isr_dbg_slow_printf("I2C Event\n");
   QK_ISR_EXIT();                           /* inform QK about exiting an ISR */
}

/******************************************************************************/
void I2C1_ER_IRQHandler( void )
{
   QK_ISR_ENTRY();                         /* inform QK about entering an ISR */

   /* Read SR1 register to get I2C error */
   __IO uint16_t regVal = I2C_ReadRegister(I2C1, I2C_Register_SR1) & 0xFF00;
   if (regVal != 0x0000) {
      /* Clears error flags */
      I2C1->SR1 &= 0x00FF;

      isr_dbg_slow_printf("I2C Error: 0x%04x\n", regVal);
   }
   QK_ISR_EXIT();                           /* inform QK about exiting an ISR */
}

/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
