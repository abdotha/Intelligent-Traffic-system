/*
 * hal_button.c
 *
 *  Created on: Nov 18, 2023
 *      Author: Adnan
 */
 

/* ------------------------------- Includes ------------------------------- */
#include "hal_button.h"

/* ----------------------- Software Interfaces Definitions ----------------------- */
/**
 * @brief Initialize button pin as Input
 * @param _btn_port Port index for btn
 * @param _btn_pin  pin number for btn
 * @return none
 */
void button_init(gpio_port_index_t _btn_port, gpio_pin_index_t _btn_pin)
{
	gpio_pin_set_direction(_btn_port, _btn_pin, INPUT);
}

/**
 * @brief get state of button (pressed or not)
 * @param _btn_port Port index for led
 * @param _btn_pin  pin number for led
 * @param _btn_active_state State of button(Active High or Active Low).
 * @param _btn_state State of button(Pressed or not)
 * @return none
 */
void button_read_state(gpio_port_index_t _btn_port, gpio_pin_index_t _btn_pin, uint8_t _btn_active_state, uint8_t* _btn_state)
{
	uint8_t l_btn_pressed_state= 0;
	gpio_pin_get_value(_btn_port, _btn_pin, &l_btn_pressed_state);

	if(_btn_active_state == BUTTON_STATE_ACTIVE_HIGH)
	{
		if(l_btn_pressed_state == LOW)
		{
			*_btn_state= BUTTON_NOT_PRESSED;
		}
		else if(l_btn_pressed_state == HIGH)
		{
			*_btn_state= BUTTON_PRESSED;
		}
	}
	else if(_btn_active_state == BUTTON_STATE_ACTIVE_LOW)
	{
		if(l_btn_pressed_state == LOW)
		{
			*_btn_state= BUTTON_PRESSED;
		}
		else if(l_btn_pressed_state == HIGH)
		{
			*_btn_state= BUTTON_NOT_PRESSED;
		}
	}
}
