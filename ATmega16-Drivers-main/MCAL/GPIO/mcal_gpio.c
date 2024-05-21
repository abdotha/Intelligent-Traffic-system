/*
 * mcal_gpio.c
 *
 *  Created on: Nov 17, 2023
 *      Author: Adnan
 */

/* ------------------------------- Includes ------------------------------- */
#include "mcal_gpio.h"

/* ----------------------- Software Interfaces Definitions ----------------------- */

/**
 * @brief Initialize the direction of a pin in a port.
 *
 * @param _port: 		Port index (@ref gpio_port_index_t)
 * @param _pin:  		Pin number (@ref gpio_pin_index_t)
 * @param _direction: 	Input or Output
 *
 * @return: None
 */
void gpio_pin_set_direction(gpio_port_index_t _port, gpio_pin_index_t _pin, gpio_direction_t _direction)
{
	if(_direction == INPUT)
	{
		switch(_port)
		{
			case PORTA_INDEX: CLEAR_BIT(GPIO_DDRA,_pin); break;
			case PORTB_INDEX: CLEAR_BIT(GPIO_DDRB,_pin); break;
			case PORTC_INDEX: CLEAR_BIT(GPIO_DDRC,_pin); break;
			case PORTD_INDEX: CLEAR_BIT(GPIO_DDRD,_pin); break;
		}
	}
	else if(_direction == OUTPUT)
	{
		switch(_port)
		{
			case PORTA_INDEX: SET_BIT(GPIO_DDRA,_pin); break;
			case PORTB_INDEX: SET_BIT(GPIO_DDRB,_pin); break;
			case PORTC_INDEX: SET_BIT(GPIO_DDRC,_pin); break;
			case PORTD_INDEX: SET_BIT(GPIO_DDRD,_pin); break;
		}
	}
}

/**
 * @brief outputs a value on a specific pin.
 *
 * @param _port: 		Port index (@ref gpio_port_index_t)
 * @param _pin:  		Pin number (@ref gpio_pin_index_t)
 * @param _value: 		High or Low
 *
 * @return: None
 */
void gpio_pin_set_value(gpio_port_index_t _port, gpio_pin_index_t _pin, gpio_value_t _value)
{
	if(_value == LOW)
	{
		switch(_port)
		{
			case PORTA_INDEX: CLEAR_BIT(GPIO_PORTA,_pin); break;
			case PORTB_INDEX: CLEAR_BIT(GPIO_PORTB,_pin); break;
			case PORTC_INDEX: CLEAR_BIT(GPIO_PORTC,_pin); break;
			case PORTD_INDEX: CLEAR_BIT(GPIO_PORTD,_pin); break;
		}
	}
	else if(_value == HIGH)
	{
		switch(_port)
		{
			case PORTA_INDEX: SET_BIT(GPIO_PORTA,_pin); break;
			case PORTB_INDEX: SET_BIT(GPIO_PORTB,_pin); break;
			case PORTC_INDEX: SET_BIT(GPIO_PORTC,_pin); break;
			case PORTD_INDEX: SET_BIT(GPIO_PORTD,_pin); break;
		}
	}
}

/**
 * @brief toggles value on a specific pin.
 *
 * @param _port: 		Port index (@ref gpio_port_index_t)
 * @param _pin:  		Pin number (@ref gpio_pin_index_t)
 *
 * @return: None
 */
void gpio_pin_toggle(gpio_port_index_t _port, gpio_pin_index_t _pin)
{
	switch(_port)
	{
		case PORTA_INDEX: TOG_BIT(GPIO_PORTA,_pin); break;
		case PORTB_INDEX: TOG_BIT(GPIO_PORTB,_pin); break;
		case PORTC_INDEX: TOG_BIT(GPIO_PORTC,_pin); break;
		case PORTD_INDEX: TOG_BIT(GPIO_PORTD,_pin); break;
	}
}

/**
 * @brief Reads value on a specific pin and stores it in variable pointed to by pointer value.
 *
 * @param _port: 		Port index (@ref gpio_port_index_t)
 * @param _pin:  		Pin number (@ref gpio_pin_index_t)
 * @param value: 		pointer to an uint8_t variable which will store pin's value
 *
 * @return: None
 */
void gpio_pin_get_value(gpio_port_index_t _port, gpio_pin_index_t _pin, uint8_t* value)
{
	switch(_port)
	{
		case PORTA_INDEX:
			*value= GET_BIT(GPIO_PINA,_pin);
			break;
		case PORTB_INDEX:
			*value= GET_BIT(GPIO_PINB,_pin);
			break;
		case PORTC_INDEX:
			*value= GET_BIT(GPIO_PINC,_pin);
			break;
		case PORTD_INDEX:
			*value= GET_BIT(GPIO_PIND,_pin);
			break;
	}
}

/**
 * @brief Connects internal pullup resistor to a pin.
 *
 * @param _port: 		Port index (@ref gpio_port_index_t)
 * @param _pin:  		Pin number (@ref gpio_pin_index_t)
 *
 * @return: None
 */
void gpio_pin_connect_pullup(gpio_port_index_t _port, gpio_pin_index_t _pin)
{
	CLEAR_BIT(SFIOR,PUD);
	switch(_port)
	{
		case PORTA_INDEX:
			CLEAR_BIT(GPIO_DDRA,_pin);
			SET_BIT(GPIO_PORTA,_pin);
			break;
		case PORTB_INDEX:
			CLEAR_BIT(GPIO_DDRB,_pin);
			SET_BIT(GPIO_PORTB,_pin);
			break;
		case PORTC_INDEX:
			CLEAR_BIT(GPIO_DDRC,_pin);
			SET_BIT(GPIO_PORTC,_pin);
			break;
		case PORTD_INDEX:
			CLEAR_BIT(GPIO_DDRD,_pin);
			SET_BIT(GPIO_PORTD,_pin);
			break;
	}
}


/**
 * @brief Initialize the direction of a port.
 *
 * @param _port: 		Port index (@ref gpio_port_index_t)
 * @param _direction: 	an uint8_t value between 0x00 and 0xFF
 * 						0x00 -> all pins on port will be input.
 * 						0xFF -> all pins on port will be output.
 *
 * @return: None
 */
void gpio_port_set_direction(gpio_port_index_t _port, uint8_t _direction)
{
	switch(_port)
	{
		case PORTA_INDEX:
			GPIO_DDRA= _direction;
			break;
		case PORTB_INDEX:
			GPIO_DDRB= _direction;
			break;
		case PORTC_INDEX:
			GPIO_DDRC= _direction;
			break;
		case PORTD_INDEX:
			GPIO_DDRD= _direction;
			break;
	}
}

/**
 * @brief outputs a value on pins of a port.
 *
 * @param _port: 		Port index (@ref gpio_port_index_t)
 * @param _value: 		an uint8_t value between 0x00 and 0xFF
 * 						0x00 -> all pins on port will be Low.
 * 						0xFF -> all pins on port will be High.
 *
 * @return: None
 */
void gpio_port_set_value(gpio_port_index_t _port, uint8_t _value)
{
	switch(_port)
	{
		case PORTA_INDEX:
			GPIO_PORTA= _value;
			break;
		case PORTB_INDEX:
			GPIO_PORTB= _value;
			break;
		case PORTC_INDEX:
			GPIO_PORTC= _value;
			break;
		case PORTD_INDEX:
			GPIO_PORTD= _value;
			break;
	}
}

/**
 * @brief toggles value on every pin on a port.
 *
 * @param _port: 		Port index (@ref gpio_port_index_t)
 *
 * @return: None
 */
void gpio_port_toggle(gpio_port_index_t _port)
{
	switch(_port)
	{
		case PORTA_INDEX:
			GPIO_PORTA ^= 0xFF;
			break;
		case PORTB_INDEX:
			GPIO_PORTB ^= 0xFF;
			break;
		case PORTC_INDEX:
			GPIO_PORTC ^= 0xFF;
			break;
		case PORTD_INDEX:
			GPIO_PORTD ^= 0xFF;
			break;
	}
}

/**
 * @brief Reads value on a specific port and stores it in variable pointed to by pointer value.
 *
 * @param _port: 		Port index (@ref gpio_port_index_t)
 * @param value: 		pointer to an uint8_t variable which will store pin's value
 *
 * @return: None
 */
void gpio_port_get_value(gpio_port_index_t _port, uint8_t* value)
{
	switch(_port)
	{
		case PORTA_INDEX:
			*value= GPIO_PINA;
			break;
		case PORTB_INDEX:
			*value= GPIO_PINB;
			break;
		case PORTC_INDEX:
			*value= GPIO_PINC;
			break;
		case PORTD_INDEX:
			*value= GPIO_PIND;
			break;
	}
}
