/*
 * hal_ir.c
 *
 * Created: 11/30/2023 11:25:37 PM
 *  Author: Adnan
 */ 
/* ------------------------------- Includes ------------------------------- */
#include "hal_ir.h"

/* ----------------------- Software Interfaces Definitions ----------------------- */
void Infrared_Init(gpio_port_index_t _IR_port, gpio_pin_index_t _IR_pin)
{
	gpio_pin_set_direction(_IR_port, _IR_pin, INPUT);
}

void Infrared_read_state(gpio_port_index_t _IR_port, gpio_pin_index_t _IR_pin, uint8_t* _ir_state)
{
	gpio_pin_get_value(_IR_port, _IR_pin, _ir_state);
}

