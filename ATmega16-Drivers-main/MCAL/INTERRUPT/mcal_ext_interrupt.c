/*
 * mcal_ext_interrupt.c
 *
 *  Created on: Nov 19, 2023
 *      Author: Adnan
 */

/* ------------------------------- Includes ------------------------------- */
#include "mcal_ext_interrupt.h"

static void (*EXTI_INT0_InterruptHandler)(void)= NULL;
static void (*EXTI_INT1_InterruptHandler)(void)= NULL;
static void (*EXTI_INT2_InterruptHandler)(void)= NULL;

/* ----------------------- Helper Functions Declarations ----------------------- */
static void Interrupt_INTx_Pin_Init(uint8_t _intx_src);
static void Interrupt_INTx_Sense_Init(uint8_t _intx_src, uint8_t _intx_sense);
static void Interrupt_INTx_Src_req_enable(uint8_t _intx_src);
static void Interrupt_INTx_Set_Handler(uint8_t _intx_src, void (*EXTI_INTx_Handler)(void));


/* ----------------------- Software Interfaces Definitions ----------------------- */
void Interrupr_INTx_Init(uint8_t _intx_src, uint8_t _intx_sense, void (*EXTI_INTx_InterruptHandler)(void))
{
	/* Configure External Interrupt Pin as Input */
	Interrupt_INTx_Pin_Init(_intx_src);

	/* Configure External Interrupt Sense */
	Interrupt_INTx_Sense_Init(_intx_src, _intx_sense);

	/* Configure Call-Back */
	Interrupt_INTx_Set_Handler(_intx_src, EXTI_INTx_InterruptHandler);

	/* Enable Global Interrupts */
	INTERRUPT_GlobalInterrupt_ENABLE();

	/* Enable Interrupt Source Request */
	Interrupt_INTx_Src_req_enable(_intx_src);
}



/* ----------------------- Helper Functions Definitions ----------------------- */
static void Interrupt_INTx_Pin_Init(uint8_t _intx_src)
{
	switch(_intx_src)
	{
		case EXT_INT0:
			gpio_pin_set_direction(PORTD_INDEX, PIN2, INPUT);
			break;

		case EXT_INT1:
			gpio_pin_set_direction(PORTD_INDEX, PIN3, INPUT);
			break;

		case EXT_INT2:
			gpio_pin_set_direction(PORTB_INDEX, PIN2, INPUT);
			break;
	}
}


static void Interrupt_INTx_Sense_Init(uint8_t _intx_src, uint8_t _intx_sense)
{
	if(_intx_src == EXT_INT0)
	{
		switch(_intx_sense)
		{
			case INTx_SENSE_LOW_LEVEL:
				CLEAR_BIT(MCUCR,ISC00);
				CLEAR_BIT(MCUCR,ISC01);
				break;

			case INTx_SENSE_LOGICAL_CHANGE:
				SET_BIT(MCUCR,ISC00);
				CLEAR_BIT(MCUCR,ISC01);
				break;

			case INTx_SENSE_FALLING_EDGE:
				CLEAR_BIT(MCUCR,ISC00);
				SET_BIT(MCUCR,ISC01);
				break;

			case INTx_SENSE_RISING_EDGE:
				SET_BIT(MCUCR,ISC00);
				SET_BIT(MCUCR,ISC01);
				break;
		}
	}
	else if(_intx_src == EXT_INT1)
	{
		switch(_intx_sense)
		{
			case INTx_SENSE_LOW_LEVEL:
				CLEAR_BIT(MCUCR,ISC10);
				CLEAR_BIT(MCUCR,ISC11);
				break;

			case INTx_SENSE_LOGICAL_CHANGE:
				SET_BIT(MCUCR,ISC10);
				CLEAR_BIT(MCUCR,ISC11);
				break;

			case INTx_SENSE_FALLING_EDGE:
				CLEAR_BIT(MCUCR,ISC10);
				SET_BIT(MCUCR,ISC11);
				break;

			case INTx_SENSE_RISING_EDGE:
				SET_BIT(MCUCR,ISC10);
				SET_BIT(MCUCR,ISC11);
				break;
		}
	}
	else if(_intx_src == EXT_INT2)	// When changing the ISC2 bit, an interrupt can occur.
	{
		// First disable INT2 by clearing its Interrupt Enable bit in the GICR Register.
		INTERRUPT_EXTI2_REQUEST_DISABLE();

		// Then, the ISC2 bit can be changed.
		switch(_intx_sense)
		{
			case INTx_SENSE_FALLING_EDGE:
				CLEAR_BIT(MCUCSR,ISC2);
				break;

			case INTx_SENSE_RISING_EDGE:
				SET_BIT(MCUCSR,ISC2);
				break;
		}

		// Finally, the INT2 Interrupt Flag should be cleared by writing a logical one to its Interrupt Flag bit (INTF2) in the GIFR Register
		INTERRUPT_EXTI2_FLAG_CLEAR();
	}
}

static void Interrupt_INTx_Src_req_enable(uint8_t _intx_src)
{
	if(_intx_src == EXT_INT0)
	{
		INTERRUPT_EXTI0_REQUEST_ENABLE();
	}
	else if(_intx_src == EXT_INT1)
	{
		INTERRUPT_EXTI1_REQUEST_ENABLE();
	}
	else if(_intx_src == EXT_INT2)
	{
		INTERRUPT_EXTI2_REQUEST_ENABLE();
	}
}

static void Interrupt_INTx_Set_Handler(uint8_t _intx_src, void (*EXTI_INTx_Handler)(void))
{
	switch(_intx_src)
	{
		case EXT_INT0:
			EXTI_INT0_InterruptHandler= EXTI_INTx_Handler;
			break;

		case EXT_INT1:
			EXTI_INT1_InterruptHandler= EXTI_INTx_Handler;
			break;

		case EXT_INT2:
			EXTI_INT2_InterruptHandler= EXTI_INTx_Handler;
			break;
	}
}


/* ----------------------- ISRs ----------------------- */
void __vector_1(void)   __attribute__((signal));
void __vector_1(void)
{
	if(EXTI_INT0_InterruptHandler != NULL)
	{
		/* Clear Flag */
		INTERRUPT_EXTI0_FLAG_CLEAR();

		/* Call Function */
		EXTI_INT0_InterruptHandler();

	}
}

void __vector_2(void)   __attribute__((signal));
void __vector_2(void)
{
	if(EXTI_INT1_InterruptHandler != NULL)
	{
		/* Clear Flag */
		INTERRUPT_EXTI1_FLAG_CLEAR();

		/* Call Function */
		EXTI_INT1_InterruptHandler();

	}
}

void __vector_18(void)   __attribute__((signal));
void __vector_18(void)
{
	if(EXTI_INT2_InterruptHandler != NULL)
	{
		/* Clear Flag */
		INTERRUPT_EXTI2_FLAG_CLEAR();

		/* Call Function */
		EXTI_INT2_InterruptHandler();

	}
}
