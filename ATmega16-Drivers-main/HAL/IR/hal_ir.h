/*
 * hal_ir.h
 *
 * Created: 11/30/2023 11:25:48 PM
 *  Author: Adnan
 */ 


#ifndef HAL_IR_H_
#define HAL_IR_H_
/* ------------------------------- Includes ------------------------------- */
#include "../../MCAL/GPIO/mcal_gpio.h"

/* ------------------------------- Macros Declarations ------------------------------- */


/* ------------------------------- data types declarations ------------------------------- */

/* ----------------------- Software Interfaces Declarations ----------------------- */

/**
 * @brief Initialize IR pin as Input
 * @param _IR_port Port index for IR
 * @param _IR_pin  pin number for IR
 * @return none
 */
void Infrared_Init(gpio_port_index_t _IR_port, gpio_pin_index_t _IR_pin);

/**
 * @brief get state of IR (HIGH or LOW)
	HIGH --> No Obstacle.
	LOW --> there is an Obstacle.
 * @param _IR_port Port index of IR Port
 * @param _IR_pin  Pin of IR
 * @param _ir_state State of IR
 * @return none
 */
void Infrared_read_state(gpio_port_index_t _IR_port, gpio_pin_index_t _IR_pin, uint8_t* _ir_state);


#endif /* HAL_IR_H_ */