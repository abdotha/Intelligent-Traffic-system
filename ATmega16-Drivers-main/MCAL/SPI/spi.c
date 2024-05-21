/*
 * spi.c
 *
 * Created: 5/10/2024 12:04:36 PM
 *  Author: Adnan
 */ 

/* ----------------------------------- Includes ----------------------------------- */
#include "spi.h"

/* ------------------------ Helper Functions Declarations ------------------------- */
static void SPI_Set_ClockRate(uint8_t SPI_clockRate);
static void SPI_Config_Pins(const Spicfg_t* spi_obj);

/* ----------------------- Software Interfaces Definitions ----------------------- */
void SPI_Init(const Spicfg_t* spi_obj)
{
	if(spi_obj != NULL)
	{
		// Configure Pins of SPI
		SPI_Config_Pins(spi_obj);
		
		// Configure Mode
		if(spi_obj->spi_mode == SPI_MODE_SLAVE)
		{
			SPI_SET_MODE_SLAVE();
		}
		else if(spi_obj->spi_mode == SPI_MODE_MASTER)
		{
			SPI_SET_MODE_MASTER();
		}
		
		// Configure Data Order
		if(spi_obj->spi_data_order == SPI_DATAORDER_MSB_FIRST)
		{
			SPI_TRANSMIT_MSB_FIRST();
		}
		else if(spi_obj->spi_data_order == SPI_DATAORDER_LSB_FIRST)
		{
			SPI_TRANSMIT_LSB_FIRST();
		}
		
		// Configure Clock Rate
		SPI_Set_ClockRate(spi_obj->spi_clock_rate);
		
		// Configure Clock Polarity
		if(spi_obj->spi_clock_polarity == SPI_CPOL_IDLE_LOW)
		{
			SPI_SCK_IDLELOW();
		}
		else if(spi_obj->spi_clock_polarity == SPI_CPOL_IDLE_HIGH)
		{
			SPI_SCK_IDLEHIGH();
		}
		
		// Configure Clock Phase
		if(spi_obj->spi_clock_phase == SPI_CPHA_SAMPLE_LEADING)
		{
			SPI_LEADING_DATASAMPLED();
		}
		else if(spi_obj->spi_clock_phase == SPI_CPHA_SAMPLE_TRAILING)
		{
			SPI_TRAILING_DATASAMPLED();
		}
		
		// Enable SPI
		SPI_ENABLE();
	}
}

void SPI_Transmit(const Spicfg_t* spi_obj, uint8_t data)
{
	/* Make Low on SS pin */
	gpio_pin_set_value(PORTB_INDEX, SS, LOW);
	
	/* Put data in SPI Data Register */
	SPDR= data;
	
	/* Wait for transmission complete */
	while((GET_BIT(SPSR,SPIF) == 0));
}

uint8_t SPI_Receive(const Spicfg_t* spi_obj)
{
	/* Wait for reception complete */
	while((GET_BIT(SPSR,SPIF) == 0));
	
	/* Return data register */
	return SPDR;
}

uint8_t SPI_Transceive(const Spicfg_t* spi_obj, uint8_t data)
{
	/* Make Low on SS pin */
	gpio_pin_set_value(PORTB_INDEX, SS, LOW);
	
	/* Put data in SPI Data Register */
	SPDR= data;
	
	/* Wait for transmission complete */
	while((GET_BIT(SPSR,SPIF) == 0));
	
	/* Return data register */
	return SPDR;
}

/* ------------------------ Helper Functions Definitions ------------------------ */
static void SPI_Set_ClockRate(uint8_t SPI_clockRate)
{
	switch(SPI_clockRate)
	{
		case SPI_CLOCKRATE_DIV_4:
			CLEAR_BIT(SPCR, SPR0);
			CLEAR_BIT(SPCR, SPR1);
			CLEAR_BIT(SPSR, SPI2X);
		break;
		case SPI_CLOCKRATE_DIV_16:
			SET_BIT(SPCR, SPR0);
			CLEAR_BIT(SPCR, SPR1);
			CLEAR_BIT(SPSR, SPI2X);
		break;
		case SPI_CLOCKRATE_DIV_64:
			CLEAR_BIT(SPCR, SPR0);
			SET_BIT(SPCR, SPR1);
			CLEAR_BIT(SPSR, SPI2X);
		break;
		case SPI_CLOCKRATE_DIV_128:
			SET_BIT(SPCR, SPR0);
			SET_BIT(SPCR, SPR1);
			CLEAR_BIT(SPSR, SPI2X);
		break;
		case SPI_CLOCKRATE_DIV_2:
			CLEAR_BIT(SPCR, SPR0);
			CLEAR_BIT(SPCR, SPR1);
			SET_BIT(SPSR, SPI2X);
		break;
		case SPI_CLOCKRATE_DIV_8:
			SET_BIT(SPCR, SPR0);
			CLEAR_BIT(SPCR, SPR1);
			SET_BIT(SPSR, SPI2X);
		break;
		case SPI_CLOCKRATE_DIV_32:
			CLEAR_BIT(SPCR, SPR0);
			SET_BIT(SPCR, SPR1);
			SET_BIT(SPSR, SPI2X);
		break;
	}
}

static void SPI_Config_Pins(const Spicfg_t* spi_obj)
{
	/* Make MOSI as Output pin */
	gpio_pin_set_direction(PORTB_INDEX, MOSI, OUTPUT);
	
	/* Make MISO pin as input pin */
	gpio_pin_set_direction(PORTB_INDEX, MISO, INPUT);
	
	if(spi_obj->spi_mode == SPI_MODE_MASTER)
	{
		gpio_pin_set_direction(PORTB_INDEX, SS, OUTPUT);
		gpio_pin_set_direction(PORTB_INDEX, SCK, OUTPUT);
	}
	else if(spi_obj->spi_mode == SPI_MODE_SLAVE)
	{
		gpio_pin_set_direction(PORTB_INDEX, SS, INPUT);
		gpio_pin_set_direction(PORTB_INDEX, SCK, INPUT);
	}	
	
	/* Make high on SS pin */
	gpio_pin_set_value(PORTB_INDEX, SS, HIGH);
}
