#ifndef __REGISTERS_H__
#define __REGISTERS_H__

#include "std_types.h"
#include "Bit_Masking.h"

// Special Function I/O Register – SFIOR
#define SFIOR				(*(volatile uint8_t*)0x50)
#define PSR10				0
#define PSR2				1
#define PUD					2		// The Pull-up disable bit in MCUCR-Reg
#define ACME				3
#define ADTS0				5
#define ADTS1				6
#define ADTS2				7

/* ------------------------------------------- GPIO Registers ------------------------------------------- */
// PORTA
#define GPIO_PORTA			(*(volatile uint8_t*)0x3B)			   		// The Port A Data Register
#define GPIO_DDRA			(*(volatile uint8_t*)0x3A)			  	 	// The Port A Direction Register
#define GPIO_PINA			(*(volatile uint8_t*)0x39)			   		// The Port A Input Pins Address
// PORTB
#define GPIO_PORTB			(*(volatile uint8_t*)0x38)			   		// The Port B Data Register
#define GPIO_DDRB			(*(volatile uint8_t*)0x37)			  	 	// The Port B Direction Register
#define GPIO_PINB			(*(volatile uint8_t*)0x36)			   		// The Port B Input Pins Address
// PORTC
#define GPIO_PORTC			(*(volatile uint8_t*)0x35)			   		// The Port C Data Register
#define GPIO_DDRC			(*(volatile uint8_t*)0x34)			   		// The Port C Direction Register
#define GPIO_PINC			(*(volatile uint8_t*)0x33)			   		// The Port C Input Pins Address
// PORTD
#define GPIO_PORTD			(*(volatile uint8_t*)0x32)			   		// The Port D Data Register
#define GPIO_DDRD			(*(volatile uint8_t*)0x31)			   		// The Port D Direction Register
#define GPIO_PIND			(*(volatile uint8_t*)0x30)			   		// The Port D Input Pins Address


/* ------------------------------------------- Interrupt Registers ------------------------------------------- */
// The AVR Status Register - SREG
#define SREG				(*(volatile uint8_t*)0x5F)
#define I					7

// MCU Control Register – MCUCR
#define MCUCR				(*(volatile uint8_t*)0x55)
#define ISC00				0
#define ISC01				1
#define ISC10				2
#define ISC11				3
#define SM0					4
#define SM1					5
#define SE					6
#define SM2					7

// MCU Control and Status Register – MCUCSR
#define MCUCSR				(*(volatile uint8_t*)0x54)
#define PORF				0
#define EXTRF				1
#define BORF				2
#define WDRF				3
#define JTRF				4
#define ISC2				6
#define JTD					7

// General Interrupt Control Register – GICR
#define GICR				(*(volatile uint8_t*)0x5B)
#define IVCE				0
#define IVSEL				1
#define INT2				5
#define INT0				6
#define INT1				7

// General Interrupt Flag Register – GIFR
#define GIFR				(*(volatile uint8_t*)0x5A)
#define INTF2				5
#define INTF0				6
#define INTF1				7

/* ------------------------------------------- USART Registers ------------------------------------------- */
// USART I/O Data Register – UDR
#define UDR					(*(volatile uint8_t*)0x2C)

// USART Control and Status Register A – UCSRA
#define UCSRA				(*(volatile uint8_t*)0x2B)
#define MPCM				0
#define U2X					1
#define PE					2
#define DOR					3
#define FE					4
#define UDRE				5
#define TXC					6
#define RXC					7

// USART Control and Status Register B – UCSRB
#define UCSRB				(*(volatile uint8_t*)0x2A)
#define TXB8				0
#define RXB8				1
#define UCSZ2				2
#define TXEN				3
#define RXEN				4
#define UDRIE				5
#define TXCIE				6
#define RXCIE				7

// USART Control and Status Register C – UCSRC
#define UCSRC				(*(volatile uint8_t*)0x40)
#define UCPOL				0
#define UCSZ0				1
#define UCSZ1				2
#define USBS				3
#define UPM0				4
#define UPM1				5
#define UMSEL				6
#define URSEL				7

// USART Baud Rate High Register - UBRRH
#define UBRRH				(*(volatile uint8_t*)0x40)

// USART Baud Rate Low Register - UBRRL
#define UBRRL				(*(volatile uint8_t*)0x29)

/* -------------------------------------------------- TIMERS -------------------------------------------------- */
// Timer/CounterInterrupt Mask Register – TIMSK
#define TIMSK				(*(volatile uint8_t*)0x59)
#define TOIE0				0
#define OCIE0				1
#define TOIE1				2
#define OCIE1B				3
#define OCIE1A				4
#define TICIE1				5
#define TOIE2				6
#define OCIE2				7

// Timer/CounterInterrupt Flag Register – TIFR
#define TIFR				(*(volatile uint8_t*)0x58)
#define TOV0				0
#define OCF0				1
#define TOV1				2
#define OCF1B				3
#define OCF1A				4
#define ICF1				5
#define TOV2				6
#define OCF2				7

/* ------------------------------------------- Timer0 Registers ------------------------------------------- */
// Timer/Counter Control Register – TCCR0
#define TCCR0				(*(volatile uint8_t*)0x53)
#define CS00				0
#define CS01				1
#define CS02				2
#define WGM01				3
#define COM00				4
#define COM01				5
#define WGM00				6
#define FOC0				7

// Timer/Counter Register – TCNT0
#define TCNT0				(*(volatile uint8_t*)0x52)

// Output CompareRegister – OCR0
#define OCR0				(*(volatile uint8_t*)0x5C)




#endif
