/*
 * mcal_timer0.h
 *
 * Created: 11/24/2023 2:30:09 PM
 *  Author: Adnan
 */ 


#ifndef MCAL_TIMER0_H_
#define MCAL_TIMER0_H_
/* ----------------------------------------- Includes ----------------------------------------- */
#include "../../GPIO/mcal_gpio.h"

/* ----------------------------------- Macros Declarations ------------------------------------ */
// Modes of operation
#define TIMER0_MODE_NORMAL									0
#define TIMER0_MODE_PWM_PHASE_CORRECT						1
#define TIMER0_MODE_CTC										2
#define TIMER0_MODE_FAST_PWM								3

// Output Compare pin (OC0) behavior in case of normal or CTC mode (non-PWM).
#define OC0_DISCONNECT										0
#define OC0_TOGGLE_ON_COMPARE_MATCH							1
#define OC0_CLEAR_ON_COMPARE_MATCH							2
#define OC0_SET_ON_COMPARE_MATCH							3

// clock source to be used by the Timer/Counter.
#define TIMER0_CLOCK_DISABLED								0
#define TIMER0_CLOCK_PRESCALER_1							1
#define TIMER0_CLOCK_PRESCALER_8							2
#define TIMER0_CLOCK_PRESCALER_64							3
#define TIMER0_CLOCK_PRESCALER_256							4
#define TIMER0_CLOCK_PRESCALER_1024							5
#define TIMER0_CLOCK_EXTERNAL_FALLING_EDGE					6
#define TIMER0_CLOCK_EXTERNAL_RISING_EDGE					7


/* ------------------------------- Macro Functions declarations ------------------------------- */
#define TIMER0_PRESCALER_RESET()							(SET_BIT(SFIOR,PSR10))

#define TIMER0_COMPARE_MATCH_INTERRUPT_ENABLE()				(SET_BIT(TIMSK,OCIE0))
#define TIMER0_COMPARE_MATCH_INTERRUPT_DISABLE()			(CLEAR_BIT(TIMSK,OCIE0))
#define TIMER0_COMPARE_MATCH_INTERRUPT_FLAG_CLEAR()			(SET_BIT(TIFR,OCF0))

#define TIMER0_OVERFLOW_INTERRUPT_ENABLE()					(SET_BIT(TIMSK,TOIE0))
#define TIMER0_OVERFLOW_INTERRUPT_DISABLE()					(CLEAR_BIT(TIMSK,TOIE0))
#define TIMER0_OVERFLOW_INTERRUPT_FLAG_CLEAR()				(SET_BIT(TIFR,TOV0))




/* --------------------------------- Data Types declarations ---------------------------------- */
typedef struct
{
	uint8_t Timer0_mode;
	uint8_t Timer0_Preloaded_Value;
	uint8_t Timer0_PreScaler;
	uint32_t COV;
	void (*TIMER0_CALLBACK)(void);
}Timer0_cfg_t;

/* ----------------------------- Software Interfaces Declarations ----------------------------- */
/**
 * @brief Initialize Timer0
 * @param Timer0_Ticks: Number of ticks between two interrupts
 * @return	None
 */
void Timer0_Init(const Timer0_cfg_t* _timer_obj);

/**
 * @brief Timer0 Waits(Blocking) till reaching required tick.
 * @param Timer0_Ticks: Number of ticks between two interrupts
 * @return	0: None
 */
void Timer0_WaitBlocking(const Timer0_cfg_t* _timer_obj);




#endif /* MCAL_TIMER0_H_ */