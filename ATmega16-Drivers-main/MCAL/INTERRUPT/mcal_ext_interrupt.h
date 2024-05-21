/*
 * mcal_ext_interrupt.h
 *
 *  Created on: Nov 19, 2023
 *      Author: Adnan
 */

#ifndef MCAL_INTERRUPT_MCAL_EXT_INTERRUPT_H_
#define MCAL_INTERRUPT_MCAL_EXT_INTERRUPT_H_

/* ------------------------------- Includes ------------------------------- */
#include "../GPIO/mcal_gpio.h"

/* ------------------------------- Macros Declarations ------------------------------- */
// External Interrupts Sources
#define EXT_INT0										0
#define EXT_INT1										1
#define EXT_INT2										2

// External Interrupts Sense
#define INTx_SENSE_LOW_LEVEL							0
#define INTx_SENSE_LOGICAL_CHANGE						1
#define INTx_SENSE_FALLING_EDGE							2
#define INTx_SENSE_RISING_EDGE							3


/* ------------------------------- Macro Functions declarations ------------------------------- */
#define INTERRUPT_GlobalInterrupt_ENABLE()				(SET_BIT(SREG,I))	// The Global Interrupt Enable bit must be set for the interrupts to be enabled.
#define INTERRUPT_GlobalInterrupt_DISABLE()				(CLEAR_BIT(SREG,I))

#define INTERRUPT_EXTI0_REQUEST_ENABLE()				(SET_BIT(GICR,INT0))
#define INTERRUPT_EXTI0_REQUEST_DISABLE()				(CLEAR_BIT(GICR,INT0))
#define INTERRUPT_EXTI0_FLAG_CLEAR()					(SET_BIT(GIFR,INTF0))

#define INTERRUPT_EXTI1_REQUEST_ENABLE()				(SET_BIT(GICR,INT1))
#define INTERRUPT_EXTI1_REQUEST_DISABLE()				(CLEAR_BIT(GICR,INT1))
#define INTERRUPT_EXTI1_FLAG_CLEAR()					(SET_BIT(GIFR,INTF1))

#define INTERRUPT_EXTI2_REQUEST_ENABLE()				(SET_BIT(GICR,INT2))
#define INTERRUPT_EXTI2_REQUEST_DISABLE()				(CLEAR_BIT(GICR,INT2))
#define INTERRUPT_EXTI2_FLAG_CLEAR()					(SET_BIT(GIFR,INTF2))


/* ----------------------- Software Interfaces Declarations ----------------------- */
void Interrupr_INTx_Init(uint8_t _intx_src, uint8_t _intx_sense, void (*EXTI_INTx_InterruptHandler)(void));

#endif /* MCAL_INTERRUPT_MCAL_EXT_INTERRUPT_H_ */
