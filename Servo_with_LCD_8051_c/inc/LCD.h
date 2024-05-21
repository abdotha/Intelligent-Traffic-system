#ifndef LCD_H_
#define LCD_H_

#include "REG52.H"
typedef unsigned char uint8_t;
typedef unsigned int uint16_t;
typedef unsigned long uint32_t;

typedef signed char int8_t;
typedef signed int int16_t;
typedef signed long int32_t;

void DELAY(uint16_t time);
void LCD_INIT(void);
void LCD_CLEAR();
void LCD_DATA(uint8_t disp_data);
int8_t UART_RxChar();
void LCD_STRING(int8_t *str);
void LCD_COMM(uint8_t command);


#endif /* LCD_H_ */