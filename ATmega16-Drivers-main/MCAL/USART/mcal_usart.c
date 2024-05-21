/*
 * mcal_usart.c
 *
 *  Created on: Nov 20, 2023
 *      Author: Adnan
 */


/* ----------------------------------- Includes ----------------------------------- */
#include "mcal_usart.h"


uint8_t ucsrc_val=0x00;		// variable temporarily holds the UCSRC-Reg Value

/* ------------------------ Helper Functions Declarations ------------------------- */
static void USART_Mode_Select(const usart_cfg_t* _usart_obj);
static void USART_DataBits_Select(const usart_cfg_t* _usart_obj);
static void USART_Parity_Select(const usart_cfg_t* _usart_obj);
static void USART_StopBits_Select(const usart_cfg_t* _usart_obj);
static void USART_Set_BaudRate(const usart_cfg_t* _usart_obj);


/* ----------------------- Software Interfaces Definitions ----------------------- */
void USART_Init(const usart_cfg_t* _usart_obj)
{
	if(_usart_obj != NULL)
	{
		UCSRB = 0; // disable while init
		UCSRA = 0;
		UCSRC = 0;
		UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0); // async, 8 bit, 1 stop, no parity
		UCSRA = 0;
		UCSRB = (1 << RXEN) | (1 << TXEN); // enable Rx and Tx
		
		/* Configure I/O Pins */
		gpio_pin_set_direction(PORTD_INDEX, PIN0, INPUT);
		gpio_pin_set_direction(PORTD_INDEX, PIN1, OUTPUT);
		
		// The URSEL must be one when writing the UCSRC.
		/*ucsrc_val |= (1 << URSEL);
		UCSRC = (1 << URSEL) | ucsrc_val;*/
		
		/* Configure Baud-Rate */
		USART_Set_BaudRate(_usart_obj);
		
		/* Configure USART Mode */
		USART_Mode_Select(_usart_obj);

		/* Configure Number of Data Bits */
		USART_DataBits_Select(_usart_obj);

		/* Configure Number of Parity Bits */
		USART_Parity_Select(_usart_obj);


		/* Configure Number of Stop Bits */
		USART_StopBits_Select(_usart_obj);
		
		

		/* Enable USART */
		if(_usart_obj->_usart_operation_mode == USART_OP_MODE_TX)
		{
			SET_BIT(UCSRB,TXEN);
			CLEAR_BIT(UCSRB,RXEN);
		}
		else if(_usart_obj->_usart_operation_mode == USART_OP_MODE_RX)
		{
			CLEAR_BIT(UCSRB,TXEN);
			SET_BIT(UCSRB,RXEN);
		}
		else if(_usart_obj->_usart_operation_mode == USART_OP_MODE_TX_RX)
		{
			SET_BIT(UCSRB,TXEN);
			SET_BIT(UCSRB,RXEN);
		}
	}
}

void USART_Blocking_SendByte(const usart_cfg_t* _usart_obj, uint8_t _byte)
{
	while (!GET_BIT(UCSRA,UDRE));
	UDR= _byte;
}

void USART_Blocking_ReceiveByte(const usart_cfg_t* _usart_obj, uint8_t* _rec_byte)
{
	while(!(GET_BIT(UCSRA,RXC)));
	*_rec_byte= UDR;
}

uint8_t USART_ReadUCSRC(void)
{
	uint8_t ucsrc;
	/* Read UCSRC */
	ucsrc = UBRRH;
	ucsrc = UCSRC;
	return ucsrc;
}

/* ------------------------ Helper Functions Definitions ------------------------ */
static void USART_Mode_Select(const usart_cfg_t* _usart_obj)
{
	if(_usart_obj->_usart_mode == USART_MODE_ASYNCH)
	{
		CLEAR_BIT(ucsrc_val,UMSEL);
	}
	else if(_usart_obj->_usart_mode == USART_MODE_SYNCH)
	{
		SET_BIT(ucsrc_val,UMSEL);
	}
}

static void USART_DataBits_Select(const usart_cfg_t* _usart_obj)
{
	switch(_usart_obj->_usart_data_bits)
	{
	case USART_DATA_BITS_5:
		CLEAR_BIT(ucsrc_val,UCSZ0);
		CLEAR_BIT(ucsrc_val,UCSZ1);
		CLEAR_BIT(UCSRB,UCSZ2);
		break;

	case USART_DATA_BITS_6:
		SET_BIT(ucsrc_val,UCSZ0);
		CLEAR_BIT(ucsrc_val,UCSZ1);
		CLEAR_BIT(UCSRB,UCSZ2);
		break;

	case USART_DATA_BITS_7:
		CLEAR_BIT(ucsrc_val,UCSZ0);
		SET_BIT(ucsrc_val,UCSZ1);
		CLEAR_BIT(UCSRB,UCSZ2);
		break;

	case USART_DATA_BITS_8:
		SET_BIT(ucsrc_val,UCSZ0);
		SET_BIT(ucsrc_val,UCSZ1);
		CLEAR_BIT(UCSRB,UCSZ2);
		break;

	case USART_DATA_BITS_9:
		SET_BIT(ucsrc_val,UCSZ0);
		SET_BIT(ucsrc_val,UCSZ1);
		SET_BIT(UCSRB,UCSZ2);
		break;

	}
}

static void USART_Parity_Select(const usart_cfg_t* _usart_obj)
{
	switch(_usart_obj->_usart_parity)
	{
	case USART_PARITY_DISABLED:
		CLEAR_BIT(ucsrc_val,UPM0);
		CLEAR_BIT(ucsrc_val,UPM1);
		break;

	case USART_ODD_PARITY:
		SET_BIT(ucsrc_val,UPM0);
		SET_BIT(ucsrc_val,UPM1);
		break;

	case USART_EVEN_PARITY:
		CLEAR_BIT(ucsrc_val,UPM0);
		SET_BIT(ucsrc_val,UPM1);
		break;
	}
}

static void USART_StopBits_Select(const usart_cfg_t* _usart_obj)
{
	if(_usart_obj->_usart_stop_bit == USART_STOP_BITS_1)
	{
		CLEAR_BIT(ucsrc_val,USBS);
	}
	else if(_usart_obj->_usart_stop_bit == USART_STOP_BITS_2)
	{
		SET_BIT(ucsrc_val,USBS);
	}
}

static void USART_Set_BaudRate(const usart_cfg_t* _usart_obj)
{
	float ubrr_val=0;

	if(_usart_obj->_usart_speed == USART_NORMAL_SPEED)
	{
		ubrr_val= (F_CPU/(16*(float)(_usart_obj->_usart_baudrate))) - 1;
	}
	else if(_usart_obj->_usart_speed == USART_DOUBLE_SPEED)
	{
		USART_DOUBLE_TRANSMISSION_SPEED();
		ubrr_val= (F_CPU/(8*(float)(_usart_obj->_usart_baudrate))) - 1;
	}

	UBRRL= (uint8_t)ubrr_val;
	UBRRH= (uint8_t)((uint8_t)ubrr_val >> 8);
}

/* ------------------------------------ ISRs ------------------------------------ */
