/*
 * mcal_timer0.c
 *
 * Created: 11/24/2023 2:29:57 PM
 *  Author: Adnan
 */ 
/* ----------------------------------- Includes ----------------------------------- */
#include "mcal_timer0.h"


/* ------------------------ Helper Functions Declarations ------------------------- */
static void TIMER0_Select_Mode(const Timer0_cfg_t* _timer_obj);
static void TIMER0_Select_PreScaler(const Timer0_cfg_t* _timer_obj);

/* ----------------------- Software Interfaces Definitions ----------------------- */
/**
 * @brief Initialize Timer0
 * @param Timer0_Ticks: Number of ticks between two interrupts
 * @return	None
 */
void Timer0_Init(const Timer0_cfg_t* _timer_obj)
{
	/* Configure Timer0 Operation Mode */
	TIMER0_Select_Mode(_timer_obj);

	/* Configure Pre-Scaler */
	TIMER0_Select_PreScaler(_timer_obj);
	
	/* Set Preloaded Value */
	TCNT0= _timer_obj->Timer0_Preloaded_Value;
	
}

/**
 * @brief Timer0 Waits(Blocking) till reaching required tick.
 * @param Timer0_Ticks: Number of ticks between two interrupts
 * @return	0: None
 */
void Timer0_WaitBlocking(const Timer0_cfg_t* _timer_obj)
{
	uint32_t l_cov_counter= 0;
	while(l_cov_counter != _timer_obj->COV)
	{
		while(!(GET_BIT(TIFR,TOV0)));
		TIMER0_OVERFLOW_INTERRUPT_FLAG_CLEAR();
		l_cov_counter++;
	}
	
	
}

/* ------------------------ Helper Functions Definitions ------------------------ */
static void TIMER0_Select_Mode(const Timer0_cfg_t* _timer_obj)
{
	switch(_timer_obj->Timer0_mode)
	{
		case TIMER0_MODE_NORMAL:
			CLEAR_BIT(TCCR0,WGM00);
			CLEAR_BIT(TCCR0,WGM01);
		break;
			
		case TIMER0_MODE_PWM_PHASE_CORRECT:
			SET_BIT(TCCR0,WGM00);
			CLEAR_BIT(TCCR0,WGM01);
		break;
		
		case TIMER0_MODE_CTC:
			CLEAR_BIT(TCCR0,WGM00);
			SET_BIT(TCCR0,WGM01);
		break;
		
		case TIMER0_MODE_FAST_PWM:
			SET_BIT(TCCR0,WGM00);
			SET_BIT(TCCR0,WGM01);
		break;
	}
}

static void TIMER0_Select_PreScaler(const Timer0_cfg_t* _timer_obj)
{
	switch(_timer_obj->Timer0_PreScaler)
	{
		case TIMER0_CLOCK_DISABLED:
			CLEAR_BIT(TCCR0,CS00);
			CLEAR_BIT(TCCR0,CS01);
			CLEAR_BIT(TCCR0,CS02);
		break;
		
		case TIMER0_CLOCK_PRESCALER_1:
			SET_BIT(TCCR0,CS00);
			CLEAR_BIT(TCCR0,CS01);
			CLEAR_BIT(TCCR0,CS02);
		break;
		
		case TIMER0_CLOCK_PRESCALER_8:
			CLEAR_BIT(TCCR0,CS00);
			SET_BIT(TCCR0,CS01);
			CLEAR_BIT(TCCR0,CS02);
		break;
		
		case TIMER0_CLOCK_PRESCALER_64:
			SET_BIT(TCCR0,CS00);
			SET_BIT(TCCR0,CS01);
			CLEAR_BIT(TCCR0,CS02);
		break;
		
		case TIMER0_CLOCK_PRESCALER_256:
			CLEAR_BIT(TCCR0,CS00);
			CLEAR_BIT(TCCR0,CS01);
			SET_BIT(TCCR0,CS02);
		break;
		
		case TIMER0_CLOCK_PRESCALER_1024:
			SET_BIT(TCCR0,CS00);
			CLEAR_BIT(TCCR0,CS01);
			SET_BIT(TCCR0,CS02);
		break;
		
		case TIMER0_CLOCK_EXTERNAL_FALLING_EDGE:
			CLEAR_BIT(TCCR0,CS00);
			SET_BIT(TCCR0,CS01);
			SET_BIT(TCCR0,CS02);
		break;
		
		case TIMER0_CLOCK_EXTERNAL_RISING_EDGE:
			SET_BIT(TCCR0,CS00);
			SET_BIT(TCCR0,CS01);
			SET_BIT(TCCR0,CS02);
		break;
	}
}

/* ------------------------------------ ISRs ------------------------------------ */