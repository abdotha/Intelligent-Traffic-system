/*
 * lcd.h
 *
 * Created: 5/3/2023 5:25:14 PM
 *  Author: Adnan
 */ 


#ifndef LCD_H_
#define LCD_H_
/* ------------------------------- Includes ------------------------------- */
#include "../../MCAL/GPIO/mcal_gpio.h"

/* ------------------------------- Macros Declarations ------------------------------- */
#define _LCD_CLEAR                      0X01
#define _LCD_RETURN_HOME                0X02
#define _LCD_ENTRY_MODE                 0X06
#define _LCD_CURSOR_OFF_DISPLAY_ON      0X0C
#define _LCD_CURSOR_OFF_DISPLAY_OFF     0X08
#define _LCD_CURSOR_ON_BLINK_ON         0X0F
#define _LCD_CURSOR_ON_BLINK_OFF        0X0E
#define _LCD_DISPLAY_SHIFT_RIGHT        0X1C
#define _LCD_DISPLAY_SHIFT_LEFT         0X18
#define _LCD_8BIT_MODE_2_LINE           0X38
#define _LCD_4BIT_MODE_2_LINE           0X28
#define _LCD_CGRAM_START                0X40
#define _LCD_DDRAM_START                0X80

#define ROW1    1
#define ROW2    2

/* ------------------------------- data types declarations ------------------------------- */
typedef struct
{
	gpio_port_index_t lcd_rs_port;
	gpio_pin_index_t lcd_rs_pin;

	gpio_port_index_t lcd_en_port;
	gpio_pin_index_t lcd_en_pin;

	gpio_port_index_t lcd_data_port[4];
	gpio_pin_index_t lcd_data_pin[4];
}chr_lcd_4bit_t;

typedef struct
{
	gpio_port_index_t lcd_rs_port;
	gpio_pin_index_t lcd_rs_pin;

	gpio_port_index_t lcd_en_port;
	gpio_pin_index_t lcd_en_pin;

	gpio_port_index_t lcd_data_port[8];
	gpio_pin_index_t lcd_data_pin[8];
}chr_lcd_8bit_t;

/* ------------------------------- Functions Declarations ------------------------------- */
void lcd_8bit_init(const chr_lcd_8bit_t* _lcd_obj);
void lcd_8bit_send_command(const chr_lcd_8bit_t* _lcd_obj, uint8_t _command);
void lcd_8bit_send_data(const chr_lcd_8bit_t* _lcd_obj, uint8_t _data);
void lcd_8bit_send_data_pos(const chr_lcd_8bit_t* _lcd_obj, uint8_t _data, uint8_t row, uint8_t column);
void lcd_8bit_send_string(const chr_lcd_8bit_t* _lcd_obj, char* str);
void lcd_8bit_send_string_pos(const chr_lcd_8bit_t* _lcd_obj, char* str, uint8_t row, uint8_t column);
void lcd_8bit_send_custom_char(const chr_lcd_8bit_t* _lcd_obj, uint8_t* custom_chr, uint8_t mem_pos, uint8_t row, uint8_t column);
void lcd_8bit_clear_screen(const chr_lcd_8bit_t* _lcd_obj);

void lcd_4bit_init(const chr_lcd_4bit_t* _lcd_obj);
void lcd_4bit_send_command(const chr_lcd_4bit_t* _lcd_obj, uint8_t _command);
void lcd_4bit_send_data(const chr_lcd_4bit_t* _lcd_obj, uint8_t _data);
void lcd_4bit_send_data_pos(const chr_lcd_4bit_t* _lcd_obj, uint8_t _data, uint8_t row, uint8_t column);
void lcd_4bit_send_string(const chr_lcd_4bit_t* _lcd_obj, char* str);
void lcd_4bit_send_string_pos(const chr_lcd_4bit_t* _lcd_obj, char* str, uint8_t row, uint8_t column);
void lcd_4bit_send_custom_char(const chr_lcd_4bit_t* _lcd_obj, uint8_t* custom_chr, uint8_t mem_pos, uint8_t row, uint8_t column);
void lcd_4bit_clear_screen(const chr_lcd_4bit_t* _lcd_obj);

#endif /* LCD_H_ */