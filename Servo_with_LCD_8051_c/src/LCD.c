#include "inc/LCD.h"

sfr LCD_PORT = 0x90;    //P1 = KeyPadPort_data pins

sbit RS = P2  ^ 1;
sbit E = P2 ^ 2;

void DELAY(uint16_t time)
{
    uint16_t I,J;
    for(I=0;I<time;I++)
    for(J=0;J<1275;J++);
}
void LCD_COMM(uint8_t command)
{
    LCD_PORT=command;
    RS=0;
    E=1;
    DELAY(1);
    E=0;
}
void LCD_INIT(void)
{
    LCD_COMM(0X38);        //for using 2 lines and 5x7 matrix of lcd
    DELAY(10);
    LCD_COMM(0x0E);         //to turn display on, cursor blinking 
    DELAY(10);
    LCD_COMM(0x01);         //clear screen
    DELAY(10);
}
void LCD_CLEAR()
{
    LCD_COMM(0X01);  // Clear the lcd screan
    DELAY(10);
}
void LCD_DATA(uint8_t disp_data)
{
    LCD_PORT=disp_data;
    RS=1;
    E=1;
    DELAY(4);
    E=0;
}
void LCD_STRING(int8_t *str)
{         //display string on screen
    int i;
    for(i=0;str[i]!=0;i++){   //send each character of the string till the null
        LCD_DATA(str[i]);
        DELAY(1);
    }
}

// Function to receive a character over UART
int8_t UART_RxChar() 
{
    while(RI == 0);  // Wait until receive is complete
    RI = 0;          // Clear receive interrupt flag
    return SBUF;     // Return received character
}
