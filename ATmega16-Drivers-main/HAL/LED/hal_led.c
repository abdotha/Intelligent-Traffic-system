/*
 * hal_led.c
 *
 *  Created on: Nov 18, 2023
 *      Author: Adnan
 */

/* ------------------------------- Includes ------------------------------- */
#include "hal_led.h"

/* ----------------------- Software Interfaces Definitions ----------------------- */
void led_init(gpio_port_index_t _led_port, gpio_pin_index_t _led_pin)
{
	gpio_pin_set_direction(_led_port, _led_pin, OUTPUT);
}

void led_turn_on(gpio_port_index_t _led_port, gpio_pin_index_t _led_pin)
{
	gpio_pin_set_value(_led_port, _led_pin, HIGH);
}

void led_turn_off(gpio_port_index_t _led_port, gpio_pin_index_t _led_pin)
{
	gpio_pin_set_value(_led_port, _led_pin, LOW);
}

void led_toggle(gpio_port_index_t _led_port, gpio_pin_index_t _led_pin)
{
	gpio_pin_toggle(_led_port, _led_pin);
}
