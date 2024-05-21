#include "REG52.H"
#include "inc/LCD.h"

#define EXT0_VECTOR 0  /* 0x03 external interrupt 0 */
#define TIM0_VECTOR 1  /* 0x0b timer 0 */
#define EXT1_VECTOR 2  /* 0x13 external interrupt 1 */
#define TIM1_VECTOR 3  /* 0x1b timer 1 */
#define UART0_VECTOR 4 /* 0x23 serial port 0 */

// LED pin define
sfr LCD_PORT = 0x90;    //P1 = KeyPadPort_data pins

sbit RS = P2  ^ 1;
sbit E = P2 ^ 2;
sbit Servo  = P3 ^ 6;


uint8_t high_set=0xFA;
uint8_t low_set=0x9A;

uint8_t high_reset=0xBD;
uint8_t low_reset=0x65;
 

void initial(void);
void sero_delay(uint8_t high ,uint8_t low);

void uart_interrupt_handler() interrupt UART0_VECTOR
{
    if(RI==1)
    {
        LCD_CLEAR();
        if ( SBUF == 'g')
        {
            // set the intial value of Timers
            // create pulse with duty cycle 7.5% and period of 20ms
            high_set=0xFC;
            low_set=0x67;

            high_reset=0xBB;
            low_reset=0x98;
            
            
            LCD_STRING("GO ->>");
        }
        else if(SBUF == 'r')
        {
            // set the intial value of Timers
            // create pulse with duty cycle 5% and period of 20ms
            high_set=0xFA;
            low_set=0x9A;

            high_reset=0xBD;
            low_reset=0x65;

            LCD_STRING("STOP");
        }
        else if(SBUF == 'y')
        {
           // set the intial value of Timers
            // create pulse with duty cycle 5% and period of 20ms
            high_set=0xFA;
            low_set=0x9A;

            high_reset=0xBD;
            low_reset=0x65;

            LCD_STRING("WAIT..");  
        }
        else
        {
             // set the intial value of Timers
            // create pulse with duty cycle 5% and period of 20ms
            high_set=0xFA;
            low_set=0x9A;

            high_reset=0xBD;
            low_reset=0x65;
            
            LCD_STRING("Press the Button..");
        }
        RI=0;
    }
    
}

void main()
{
	initial();
    LCD_INIT();   
    LCD_STRING("Press the Button..");
    while (1)
    {
        Servo=1;
        // Delay of ON time 
        sero_delay(high_set,low_set);
        Servo = 0;
        // Delay of OFF time
        sero_delay(high_reset,low_reset);        
    }
}


void initial(void)
{
    LCD_PORT=0;      // set lcd_port as output 
    RS=0;             
    E=0;            // set RS & E as output pins 

    TMOD = 0x21;  // Timer 1, mode 2, 8-bit reload & Timer 0 ,mode 1,16-bit reload
    TH1 = 0xFD;   // Load TH1 to generate baud rate 9600
    SCON = 0x50; // Mode 1, Receive enable
    TR1 = 1;     // Start Timer 1
    IE=0x90;      //uart interrupt enable
    
}
void sero_delay(uint8_t high ,uint8_t low)
{
    TH0=high;
    TL0=low;

    TR0=1;
    while (TF0 == 0);
        
    TF0=0;
    TR0=0;

}


