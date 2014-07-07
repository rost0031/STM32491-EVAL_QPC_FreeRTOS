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
#include "stm32f2xx_it.h"
#include "project_includes.h"

/* Compile-time called macros ------------------------------------------------*/
Q_DEFINE_THIS_FILE                  /* For QSPY to know the name of this file */

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
#define EE_START_ADDR      0x00 /**< EEProms internal memory starting address */
#define I2C_BUS_SPEED      100000   /**<I2C clock speed configuration (in Hz) */
#define EE_PAGESIZE        32    /**< EEProm's internal page size (in bytes). */

/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/
/**
 * @brief Buffer for I2C device
 */
static uint8_t          i2c_buffer[MAX_MSG_LEN];

/**
 * @brief An internal structure that holds almost all the settings for the U
 * SART ports.
 */
static I2C_Settings_t s_I2C_Dev[MAX_I2C] =
{
      {
            EEPROM,                    /**< i2c_dev */

            /* I2C device settings */
            I2C1,                      /**< i2c_bus */
            RCC_APB1Periph_I2C1,       /**< i2c_clk */
            0xA0,                      /**< i2c_dev_addr */
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
            &i2c_buffer[0],            /**< *buffer */
            0,                         /**< index */
      }
};

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
void I2CD_Init( I2C_Device_t device )
{
   /* Periph clock enable.  The GPIO clocks should already be enabled by BSP_Init() */
   RCC_APB1PeriphClockCmd(s_I2C_Dev[device].i2c_clk, ENABLE);

   /* Connect PXx to I2C SCL alt function. */
   GPIO_PinAFConfig(
         s_I2C_Dev[device].scl_port,
         s_I2C_Dev[device].scl_af_pin_source,
         s_I2C_Dev[device].scl_af
   );

   /* Connect PXx to I2C SDA alt function. */
   GPIO_PinAFConfig(
         s_I2C_Dev[device].sda_port,
         s_I2C_Dev[device].sda_af_pin_source,
         s_I2C_Dev[device].sda_af
   );

   /* Configure I2C pins: SCL */
   GPIO_InitTypeDef  GPIO_InitStructure;
   GPIO_InitStructure.GPIO_Pin = s_I2C_Dev[device].scl_pin;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
   GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
   GPIO_Init(s_I2C_Dev[device].scl_port, &GPIO_InitStructure);

   /* Configure I2C pins: SDA */
   GPIO_InitStructure.GPIO_Pin = s_I2C_Dev[device].sda_pin;
   GPIO_Init(s_I2C_Dev[device].sda_port, &GPIO_InitStructure);

   /* Reset I2C IP */
   RCC_APB1PeriphResetCmd(s_I2C_Dev[device].i2c_clk, ENABLE);

   /* Release reset signal of EE_I2C IP */
   RCC_APB1PeriphResetCmd(s_I2C_Dev[device].i2c_clk, DISABLE);

   /* I2C configuration */
   I2C_DeInit( s_I2C_Dev[device].i2c_bus );

   I2C_InitTypeDef  I2C_InitStructure;
   I2C_InitStructure.I2C_Mode = I2C_Mode_SMBusHost;
   I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
   I2C_InitStructure.I2C_OwnAddress1 = s_I2C_Dev[device].i2c_dev_addr;
   I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
   I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
   I2C_InitStructure.I2C_ClockSpeed = I2C_BUS_SPEED;

   /* Set up Interrupt controller to handle I2C Event interrupts */
   NVIC_Config(
         s_I2C_Dev[device].i2c_ev_irq_num,
         s_I2C_Dev[device].i2c_ev_irq_prio
   );

   /* Set up Interrupt controller to handle I2C Error interrupts */
//   NVIC_Config(
//         s_I2C_Dev[device].i2c_er_irq_num,
//         s_I2C_Dev[device].i2c_er_irq_prio
//   );

   /* Enable All I2C Interrupts */
   I2C_ITConfig(
         s_I2C_Dev[device].i2c_bus,
         I2C_IT_EVT | I2C_IT_ERR ,
         ENABLE
   );

   /* Apply I2C configuration */
   I2C_Init( s_I2C_Dev[device].i2c_bus, &I2C_InitStructure );
   I2C_AcknowledgeConfig(s_I2C_Dev[device].i2c_bus, ENABLE );

   /* I2C Peripheral Enable */
   I2C_Cmd( s_I2C_Dev[device].i2c_bus, ENABLE );
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
   switch (I2CEvent) {
      case 0x00000001:
         /* This is the I2C "Event" that is always triggering so we explicitly
          * ignore it here */
         break;

      case I2C_EVENT_MASTER_MODE_SELECT:
         dbg_slow_printf("I2C_EVENT_MASTER_MODE_SELECT event\n");
         /* Send slave Address for write */
//         I2C_Send7bitAddress(I2C1, (uint8_t)SLAVE_ADDRESS, I2C_Direction_Transmitter);
         break;


         /* Check on EV6 */
      case I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED:
         dbg_slow_printf("I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED event\n");
         /* Transmit the First Data  */
//         I2C_SendData(I2C1, TxBuffer[Tx_Idx++]);
         break;

         /* Check on EV8 */
      case I2C_EVENT_MASTER_BYTE_TRANSMITTING:
         dbg_slow_printf("I2C_EVENT_MASTER_BYTE_TRANSMITTING event\n");
         break;

      case I2C_EVENT_MASTER_BYTE_TRANSMITTED:
         dbg_slow_printf("I2C_EVENT_MASTER_BYTE_TRANSMITTED event\n");
//         if (Tx_Idx == (uint8_t)NumberOfByteToTransmit) {
//            /* Send STOP condition */
//            I2C_GenerateSTOP(I2C1, ENABLE);
//            I2C_ITConfig(I2C1, I2C_IT_EVT | I2C_IT_BUF, DISABLE);
//         } else {
//            /* Transmit Data TxBuffer */
//            I2C_SendData(I2C1, TxBuffer[Tx_Idx++]);
//         }
         break;

      default:
         dbg_slow_printf("default case: %08x\n", I2CEvent);
         break;
   }

// dbg_slow_printf("I2C Event\n");
   QK_ISR_EXIT();                           /* inform QK about exiting an ISR */
}

/******************************************************************************/
void I2C1_ER_IRQHandler( void )
{
   QK_ISR_ENTRY();                         /* inform QK about entering an ISR */
   dbg_slow_printf("I2C Error\n");
   /* Read SR1 register to get I2C error */
   if ((I2C_ReadRegister(I2C1, I2C_Register_SR1) & 0xFF00) != 0x00) {
      /* Clears error flags */
      I2C1->SR1 &= 0x00FF;
   }
   QK_ISR_EXIT();                           /* inform QK about exiting an ISR */
}

/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
