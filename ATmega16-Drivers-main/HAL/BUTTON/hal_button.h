/*
 * hal_button.h
 *
 *  Created on: Nov 18, 2023
 *      Author: Adnan
 */

#ifndef HAL_BUTTON_HAL_BUTTON_H_
#define HAL_BUTTON_HAL_BUTTON_H_

/* ------------------------------- Includes ------------------------------- */
#include "../../MCAL/GPIO/mcal_gpio.h"

/* ------------------------------- Macros Declarations ------------------------------- */
#define BUTTON_STATE_ACTIVE_LOW				0
#define BUTTON_STATE_ACTIVE_HIGH			1

#define BUTTON_NOT_PRESSED					0
#define BUTTON_PRESSED						1

/* ------------------------------- data types declarations ------------------------------- */


/* ----------------------- Software Interfaces Declarations ----------------------- */
/**
 * @brief Initialize button pin as Input
 * @param _btn_port Port index for btn
 * @param _btn_pin  pin number for btn
 * @return none
 */
void button_init(gpio_port_index_t _btn_port, gpio_pin_index_t _btn_pin);

/**
 * @brief get state of button (pressed or not)
 * @param _btn_port Port index for led
 * @param _btn_pin  pin number for led
 * @param _btn_active_state State of button(Active High or Active Low).
 * @param _btn_state State of button
 * @return none
 */
void button_read_state(gpio_port_index_t _btn_port, gpio_pin_index_t _btn_pin, uint8_t _btn_active_state, uint8_t* _btn_state);

#endif /* HAL_BUTTON_HAL_BUTTON_H_ */
