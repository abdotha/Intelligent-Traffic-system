/*
 * lcd.c
 *
 * Created: 5/3/2023 5:25:06 PM
 *  Author: Adnan
 */ 

#include "hal_lcd.h"

/* ------------------------------- Helper functions declarations ------------------------------- */
static void lcd_8bit_send_enable_signal(const chr_lcd_8bit_t* _lcd_obj);
static void lcd_4bit_send_enable_signal(const chr_lcd_4bit_t* _lcd_obj);
static void lcd_8bit_set_cursor(const chr_lcd_8bit_t* _lcd_obj, uint8_t row, uint8_t coloumn);
static void lcd_4bit_set_cursor(const chr_lcd_4bit_t* _lcd_obj, uint8_t row, uint8_t coloumn);
static void lcd_send_4bit(const chr_lcd_4bit_t *_lcd_obj, uint8_t _data_command);

/* ------------------------------- Character LCD 8bit mode functions definitions ------------------------------- */
void lcd_8bit_init(const chr_lcd_8bit_t* _lcd_obj)
{
	uint8_t index_i= 0;

	// Wait more than 30ms until VDD rises to 4.5v
	_delay_ms(50);


	// Initialize LCD RS and EN Pins as Outputs
	gpio_pin_set_direction(_lcd_obj->lcd_rs_port, _lcd_obj->lcd_rs_pin, OUTPUT);
	gpio_pin_set_direction(_lcd_obj->lcd_en_port, _lcd_obj->lcd_en_pin, OUTPUT);

	// Initialize LCD Data Pins as Outputs
	for(index_i= 0; index_i < 8; index_i++)
	{
		gpio_pin_set_direction(_lcd_obj->lcd_data_port[index_i], _lcd_obj->lcd_data_pin[index_i], OUTPUT);
	}

	// Return cursor to the first position on the first line
	lcd_8bit_send_command(_lcd_obj, _LCD_RETURN_HOME);
	_delay_ms(1);

	// Send Command : 8bit mode, 2lines
	lcd_8bit_send_command(_lcd_obj, _LCD_8BIT_MODE_2_LINE);
	_delay_ms(1); // wait more than 39 Ms

	// DISPLAY & Cursor (ON / OFF) Control
	lcd_8bit_send_command(_lcd_obj, _LCD_CURSOR_OFF_DISPLAY_ON);
	_delay_ms(1);

	// DISPLAY CLEAR
	lcd_8bit_clear_screen(_lcd_obj);

	// ENTRY MODE  SET
	lcd_8bit_send_command(_lcd_obj, _LCD_ENTRY_MODE);
	_delay_ms(1);
}

void lcd_8bit_send_command(const chr_lcd_8bit_t* _lcd_obj, uint8_t _command)
{
	uint8_t index_i= 0;
	// Make RS pin LOW for Command
	gpio_pin_set_value(_lcd_obj->lcd_rs_port, _lcd_obj->lcd_rs_pin, LOW);
	// Sending 8bit command data
	for(index_i= 0; index_i < 8; index_i++)
	{
		gpio_pin_set_value(_lcd_obj->lcd_data_port[index_i], _lcd_obj->lcd_data_pin[index_i], (_command>>index_i) & 0x01);
	}

	// Sending High to Low transition on Enable Signal
	lcd_8bit_send_enable_signal(_lcd_obj);
}

void lcd_8bit_send_data(const chr_lcd_8bit_t* _lcd_obj, uint8_t _data)
{
	uint8_t index_i= 0;
	// Make RS pin LOW for data
	gpio_pin_set_value(_lcd_obj->lcd_rs_port, _lcd_obj->lcd_rs_pin, HIGH);
	// Sending 8bit data
	for(index_i= 0; index_i < 8; index_i++)
	{
		gpio_pin_set_value(_lcd_obj->lcd_data_port[index_i], _lcd_obj->lcd_data_pin[index_i], (_data>>index_i) & 0x01);
	}

	// Sending High to Low transition on Enable Signal
	lcd_8bit_send_enable_signal(_lcd_obj);
}

void lcd_8bit_send_data_pos(const chr_lcd_8bit_t* _lcd_obj, uint8_t _data, uint8_t row, uint8_t column)
{

	lcd_8bit_set_cursor(_lcd_obj, row, column);
	lcd_8bit_send_data(_lcd_obj, _data);
}

void lcd_8bit_send_string(const chr_lcd_8bit_t* _lcd_obj, char* str)
{
	uint8_t index_i= 0;
	while(str[index_i] != '\0')
	{
		lcd_8bit_send_data(_lcd_obj, str[index_i]);
		index_i++;
	}
}

void lcd_8bit_send_string_pos(const chr_lcd_8bit_t* _lcd_obj, char* str, uint8_t row, uint8_t column)
{
	lcd_8bit_set_cursor(_lcd_obj, row, column);
	lcd_8bit_send_string(_lcd_obj, str);
}

void lcd_8bit_send_custom_char(const chr_lcd_8bit_t* _lcd_obj, uint8_t* custom_chr, uint8_t mem_pos, uint8_t row, uint8_t column)
{
	uint8_t index_i= 0;
	lcd_8bit_send_command(_lcd_obj, (_LCD_CGRAM_START+ 8*mem_pos));
	for(index_i= 0; index_i<8; index_i++)
	{
		lcd_8bit_send_data(_lcd_obj, custom_chr[index_i]);
	}
	lcd_8bit_send_data_pos(_lcd_obj, mem_pos, row, column);
}

void lcd_8bit_clear_screen(const chr_lcd_8bit_t* _lcd_obj)
{
	lcd_8bit_send_command(_lcd_obj, _LCD_CURSOR_OFF_DISPLAY_ON);
	_delay_ms(1);
}

/* ------------------------------- Character LCD 4bit mode functions definitions ------------------------------- */
void lcd_4bit_init(const chr_lcd_4bit_t* _lcd_obj)
{
	uint8_t index_i= 0;

	// Wait more than 30ms until VDD rises to 4.5v
	_delay_ms(50);


	// Initialize LCD RS and EN Pins as Outputs
	gpio_pin_set_direction(_lcd_obj->lcd_rs_port, _lcd_obj->lcd_rs_pin, OUTPUT);
	gpio_pin_set_direction(_lcd_obj->lcd_en_port, _lcd_obj->lcd_en_pin, OUTPUT);

	// Initialize LCD Data Pins as Outputs
	for(index_i= 0; index_i < 4; index_i++)
	{
		gpio_pin_set_direction(_lcd_obj->lcd_data_port[index_i], _lcd_obj->lcd_data_pin[index_i], OUTPUT);
	}

	// Return cursor to the first position on the first line
	lcd_4bit_send_command(_lcd_obj, _LCD_RETURN_HOME);
	_delay_ms(1);

	// Send Command : 8bit mode, 2lines
	lcd_4bit_send_command(_lcd_obj, _LCD_4BIT_MODE_2_LINE);
	_delay_ms(1); // wait more than 39 Ms

	// DISPLAY & Cursor (ON / OFF) Control
	lcd_4bit_send_command(_lcd_obj, _LCD_CURSOR_OFF_DISPLAY_ON);
	_delay_ms(1);

	// DISPLAY CLEAR
	lcd_4bit_clear_screen(_lcd_obj);

	// ENTRY MODE  SET
	lcd_4bit_send_command(_lcd_obj, _LCD_ENTRY_MODE);
	_delay_ms(1);
}

void lcd_4bit_send_command(const chr_lcd_4bit_t* _lcd_obj, uint8_t _command)
{

	// Make RS pin LOW for Command
	gpio_pin_set_value(_lcd_obj->lcd_rs_port, _lcd_obj->lcd_rs_pin, LOW);
	// Sending Higher 4bit command data
	lcd_send_4bit(_lcd_obj, _command >> 4);

	// Sending High to Low transition on Enable Signal
	lcd_4bit_send_enable_signal(_lcd_obj);

	// Sending Lower 4bit command data
	lcd_send_4bit(_lcd_obj, _command);

	// Sending High to Low transition on Enable Signal
	lcd_4bit_send_enable_signal(_lcd_obj);
}

void lcd_4bit_send_data(const chr_lcd_4bit_t* _lcd_obj, uint8_t _data)
{
	// Make RS pin High for data
	gpio_pin_set_value(_lcd_obj->lcd_rs_port, _lcd_obj->lcd_rs_pin, HIGH);
	// Sending Higher 4bit command data
	lcd_send_4bit(_lcd_obj, _data >> 4);

	// Sending High to Low transition on Enable Signal
	lcd_4bit_send_enable_signal(_lcd_obj);

	// Sending Lower 4bit command data
	lcd_send_4bit(_lcd_obj, _data);

	// Sending High to Low transition on Enable Signal
	lcd_4bit_send_enable_signal(_lcd_obj);
}

void lcd_4bit_send_data_pos(const chr_lcd_4bit_t* _lcd_obj, uint8_t _data, uint8_t row, uint8_t column)
{
	lcd_4bit_set_cursor(_lcd_obj, row, column);
	lcd_4bit_send_data(_lcd_obj, _data);
}

void lcd_4bit_send_string(const chr_lcd_4bit_t* _lcd_obj, char* str)
{

	uint8_t index_i= 0;
	while(str[index_i] != '\0')
	{
		lcd_4bit_send_data(_lcd_obj, str[index_i]);
		index_i++;
	}
}

void lcd_4bit_send_string_pos(const chr_lcd_4bit_t* _lcd_obj, char* str, uint8_t row, uint8_t column)
{
	lcd_4bit_set_cursor(_lcd_obj, row, column);
	lcd_4bit_send_string(_lcd_obj, str);
}

void lcd_4bit_send_custom_char(const chr_lcd_4bit_t* _lcd_obj, uint8_t* custom_chr, uint8_t mem_pos, uint8_t row, uint8_t column)
{
	uint8_t index_i= 0;
	lcd_4bit_send_command(_lcd_obj, (_LCD_CGRAM_START+ 8*mem_pos));
	for(index_i= 0; index_i<8; index_i++)
	{
		lcd_4bit_send_data(_lcd_obj, custom_chr[index_i]);
	}
	lcd_4bit_send_data_pos(_lcd_obj, mem_pos, row, column);
}

void lcd_4bit_clear_screen(const chr_lcd_4bit_t* _lcd_obj)
{
	lcd_4bit_send_command(_lcd_obj, _LCD_CURSOR_OFF_DISPLAY_ON);
	_delay_ms(1);
}



/* ------------------------------- Helper functions definitions ------------------------------- */
static void lcd_8bit_send_enable_signal(const chr_lcd_8bit_t* _lcd_obj)
{
	gpio_pin_set_value(_lcd_obj->lcd_en_port, _lcd_obj->lcd_en_pin, HIGH);
	_delay_ms(5);
	gpio_pin_set_value(_lcd_obj->lcd_en_port, _lcd_obj->lcd_en_pin, LOW);
	_delay_ms(5);
}

static void lcd_4bit_send_enable_signal(const chr_lcd_4bit_t* _lcd_obj)
{
	gpio_pin_set_value(_lcd_obj->lcd_en_port, _lcd_obj->lcd_en_pin, HIGH);
	_delay_ms(5);
	gpio_pin_set_value(_lcd_obj->lcd_en_port, _lcd_obj->lcd_en_pin, LOW);
	_delay_ms(5);
}

static void lcd_8bit_set_cursor(const chr_lcd_8bit_t* _lcd_obj, uint8_t row, uint8_t coloumn)
{
	coloumn--;
	switch(row)
	{
		case ROW1:
		lcd_8bit_send_command(_lcd_obj, 0x80+coloumn);
		break;
		case ROW2:
		lcd_8bit_send_command(_lcd_obj, 0xc0+coloumn);
		break;
	}
}

static void lcd_4bit_set_cursor(const chr_lcd_4bit_t* _lcd_obj, uint8_t row, uint8_t coloumn)
{
	coloumn--;
	switch(row)
	{
		case ROW1:
		lcd_4bit_send_command(_lcd_obj, 0x80+coloumn);
		break;
		case ROW2:
		lcd_4bit_send_command(_lcd_obj, 0xc0+coloumn);
		break;
	}
}


static void lcd_send_4bit(const chr_lcd_4bit_t *_lcd_obj, uint8_t _data_command)
{
	uint8_t index_i= 0;
	for(index_i= 0; index_i < 4; index_i++)
	{
		gpio_pin_set_value(_lcd_obj->lcd_data_port[index_i], _lcd_obj->lcd_data_pin[index_i], (_data_command>>index_i) & 0x01);
	}
}