/*
 * hal_dc_motor.h
 *
 * Created: 11/30/2023 10:23:00 PM
 *  Author: Adnan
 */ 


#ifndef HAL_DC_MOTOR_H_
#define HAL_DC_MOTOR_H_

/* ------------------------------- Includes ------------------------------- */
#include "../../MCAL/GPIO/mcal_gpio.h"

/* ------------------------------- Macros Declarations ------------------------------- */


/* ------------------------------- data types declarations ------------------------------- */
typedef struct  
{
	gpio_port_index_t dc_motor_in1_port;
	gpio_pin_index_t dc_motor_in1_pin;
	gpio_port_index_t dc_motor_in2_port;
	gpio_pin_index_t dc_motor_in2_pin;
	gpio_port_index_t dc_motor_en_port;
	gpio_pin_index_t dc_motor_en_pin;
}dc_motor_cfg_t;

/* ----------------------- Software Interfaces Declarations ----------------------- */
void dc_motor_init(const dc_motor_cfg_t* _dc_motor_obj);
void dc_motor_move_clock_wise(const dc_motor_cfg_t* _dc_motor_obj);
void dc_motor_move_counter_clock_wise(const dc_motor_cfg_t* _dc_motor_obj);
void dc_motor_stop(const dc_motor_cfg_t* _dc_motor_obj);

#endif /* HAL_DC_MOTOR_H_ */