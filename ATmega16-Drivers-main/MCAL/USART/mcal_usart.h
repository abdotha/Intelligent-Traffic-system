/*
 * mcal_usart.h
 *
 *  Created on: Nov 20, 2023
 *      Author: Adnan
 */

#ifndef MCAL_USART_MCAL_USART_H_
#define MCAL_USART_MCAL_USART_H_

/* ----------------------------------------- Includes ----------------------------------------- */
#include "../GPIO/mcal_gpio.h"
#include "mcal_usart_cfg.h"

/* ----------------------------------- Macros Declarations ------------------------------------ */
#define USART_DATA_BITS_5						0
#define USART_DATA_BITS_6						1
#define USART_DATA_BITS_7						2
#define USART_DATA_BITS_8						3
#define USART_DATA_BITS_9						4

#define USART_PARITY_DISABLED					0
#define USART_ODD_PARITY						1
#define USART_EVEN_PARITY						2

#define FRAME_ERROR								0
#define DATA_OVERRUN_ERROR						1
#define PARITY_ERROR							1

#define USART_MODE_ASYNCH						0
#define USART_MODE_SYNCH						1

#define USART_OP_MODE_TX						0
#define USART_OP_MODE_RX						1
#define USART_OP_MODE_TX_RX						2

#define USART_STOP_BITS_1						0
#define USART_STOP_BITS_2						1

#define USART_NORMAL_SPEED						0
#define USART_DOUBLE_SPEED						1

/* ------------------------------- Macro Functions declarations ------------------------------- */
#define USART_RX_ENABLE()						(SET_BIT(UCSRB,RXEN))
#define USART_RX_DISABLE()						(CLEAR_BIT(UCSRB,RXEN))

#define USART_TX_ENABLE()						(SET_BIT(UCSRB,TXEN))
#define USART_TX_DISABLE()						(CLEAR_BIT(UCSRB,TXEN))

#define USART_RX_INTERRUPT_ENABLE()				(SET_BIT(UCSRB,RXCEI))
#define USART_RX_INTERRUPT_DISABLE()			(CLEAR_BIT(UCSRB,RXCEI))

#define USART_TX_INTERRUPT_ENABLE()				(SET_BIT(UCSRB,TXCEI))
#define USART_TX_INTERRUPT_DISABLE()			(CLEAR_BIT(UCSRB,TXCEI))

#define USART_UDR_EMPTY_INTERRUPT_ENABLE()		(SET_BIT(UCSRB,UDRIE))
#define USART_UDR_EMPTY_INTERRUPT_DISABLE()		(CLEAR_BIT(UCSRB,UDRIE))

#define USART_DOUBLE_TRANSMISSION_SPEED()		(SET_BIT(UCSRA,U2X))

/* --------------------------------- Data Types declarations ---------------------------------- */
typedef struct
{
	uint8_t _usart_mode;
	uint8_t _usart_operation_mode;
	uint16 _usart_baudrate;
	uint8_t _usart_speed;
	uint8_t _usart_data_bits;
	uint8_t _usart_parity;
	uint8_t _usart_stop_bit;
}usart_cfg_t;

/* ----------------------------- Software Interfaces Declarations ----------------------------- */
void USART_Init(const usart_cfg_t* _usart_obj);

void USART_Blocking_SendByte(const usart_cfg_t* _usart_obj, uint8_t _byte);

void USART_Blocking_ReceiveByte(const usart_cfg_t* _usart_obj, uint8_t* _rec_byte);

void USART_Interrupt_SendByte(const usart_cfg_t* _usart_obj, uint8_t _byte);

void USART_Interrupt_ReceiveByte(const usart_cfg_t* _usart_obj, uint8_t* _rec_byte);

uint8_t USART_ReadUCSRC(void);

#endif /* MCAL_USART_MCAL_USART_H_ */
