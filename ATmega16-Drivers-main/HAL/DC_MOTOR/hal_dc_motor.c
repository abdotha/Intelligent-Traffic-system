/*
 * hal_dc_motor.c
 *
 * Created: 11/30/2023 10:22:51 PM
 *  Author: Adnan
 */ 
/* ------------------------------- Includes ------------------------------- */
#include "hal_dc_motor.h"

/* ----------------------- Software Interfaces Definitions ----------------------- */
void dc_motor_init(const dc_motor_cfg_t* _dc_motor_obj)
{
	gpio_pin_set_direction(_dc_motor_obj->dc_motor_in1_port, _dc_motor_obj->dc_motor_in1_pin, OUTPUT);

	gpio_pin_set_direction(_dc_motor_obj->dc_motor_in2_port, _dc_motor_obj->dc_motor_in2_pin, OUTPUT);

	gpio_pin_set_direction(_dc_motor_obj->dc_motor_en_port, _dc_motor_obj->dc_motor_en_pin, OUTPUT);

	dc_motor_stop(_dc_motor_obj);
}

void dc_motor_move_clock_wise(const dc_motor_cfg_t* _dc_motor_obj)
{
	gpio_pin_set_value(_dc_motor_obj->dc_motor_en_port, _dc_motor_obj->dc_motor_en_pin, HIGH);
	
	gpio_pin_set_value(_dc_motor_obj->dc_motor_in1_port, _dc_motor_obj->dc_motor_in1_pin, HIGH);
	gpio_pin_set_value(_dc_motor_obj->dc_motor_in2_port, _dc_motor_obj->dc_motor_in2_pin, LOW);
}

void dc_motor_move_counter_clock_wise(const dc_motor_cfg_t* _dc_motor_obj)
{
	gpio_pin_set_value(_dc_motor_obj->dc_motor_en_port, _dc_motor_obj->dc_motor_en_pin, HIGH);
	
	gpio_pin_set_value(_dc_motor_obj->dc_motor_in1_port, _dc_motor_obj->dc_motor_in1_pin, LOW);
	gpio_pin_set_value(_dc_motor_obj->dc_motor_in2_port, _dc_motor_obj->dc_motor_in2_pin, HIGH);
}

void dc_motor_stop(const dc_motor_cfg_t* _dc_motor_obj)
{
	gpio_pin_set_value(_dc_motor_obj->dc_motor_en_port, _dc_motor_obj->dc_motor_en_pin, LOW);
	
	gpio_pin_set_value(_dc_motor_obj->dc_motor_in1_port, _dc_motor_obj->dc_motor_in1_pin, LOW);
	gpio_pin_set_value(_dc_motor_obj->dc_motor_in2_port, _dc_motor_obj->dc_motor_in2_pin, LOW);
}