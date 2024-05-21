/*
 * seg.c
 *
 * Created: 5/8/2024 11:46:54 PM
 *  Author: Adnan
 */ 
#include "seg.h"

uint8_t com_cathode_arr[10]= {0b00111111 , 0b00000110 , 0b01011011 , 0b01001111 , 0b01100110 , 0b01101101 , 0b01111101 , 0b00000111 , 0b01111111 , 0b01101111};
uint8_t com_anode_arr[10]=   {0b11000000 , 0b11111001 , 0b10100100 , 0b10110000 , 0b10011001 , 0b10010010 , 0b10000010 , 0b11111000 , 0b10000000 , 0b10010000};

std_return_type_t seg_init(const seg_t* _seg_obj)
{
	std_return_type_t ret_val= RET_NOK;
	if(NULL == _seg_obj)
	{
		return NULL_PTR;
	}
	else
	{
		uint8_t index_i= 0;
		for(index_i= 0; index_i < 10; index_i++)
		{
			gpio_pin_set_direction(_seg_obj->seg_pins[index_i]._port, _seg_obj->seg_pins[index_i]._pin, OUTPUT);
		}

	}
	return ret_val;
}

std_return_type_t seg_write_nnum(const seg_t* _seg_obj, uint8_t num)
{
	std_return_type_t ret_val= RET_NOK;
	if(NULL == _seg_obj)
	{
		return NULL_PTR;
	}
	else
	{
		uint8_t index_i= 0;
		if(_seg_obj->seg_mode == SEG_COM_ANODE)
		{
			for(index_i= 0; index_i < 8; index_i++)
			{
				gpio_pin_set_value(_seg_obj->seg_pins[index_i]._port, _seg_obj->seg_pins[index_i]._pin, (com_anode_arr[num] >> index_i)&0x01);
			}
		}
		else if(_seg_obj->seg_mode == SEG_COM_CATHODE)
		{
			for(index_i= 0; index_i < 8; index_i++)
			{
				gpio_pin_set_value(_seg_obj->seg_pins[index_i]._port, _seg_obj->seg_pins[index_i]._pin, (com_anode_arr[num] >> index_i)&0x01);
			}
		}
	}
	return ret_val;
}
