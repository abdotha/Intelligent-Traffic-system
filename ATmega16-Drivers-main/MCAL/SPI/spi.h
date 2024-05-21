/*
 * spi.h
 *
 * Created: 5/10/2024 12:04:46 PM
 *  Author: Adnan
 */ 


#ifndef SPI_H_
#define SPI_H_

/* ----------------------------------- Includes ----------------------------------- */
#include "../GPIO/mcal_gpio.h"

/* ----------------------------------- Macros ----------------------------------- */
// SPI Mode
#define SPI_MODE_SLAVE					0
#define SPI_MODE_MASTER					1

// SPI Data Order
#define SPI_DATAORDER_MSB_FIRST			0
#define SPI_DATAORDER_LSB_FIRST			1

// SPI Clock Polarity
#define SPI_CPOL_IDLE_LOW				0
#define SPI_CPOL_IDLE_HIGH				1	

// SPI Clock Phase
#define SPI_CPHA_SAMPLE_LEADING			0
#define SPI_CPHA_SAMPLE_TRAILING		1	

// SPI Clock Rate
#define SPI_CLOCKRATE_DIV_4				0
#define SPI_CLOCKRATE_DIV_16			1
#define SPI_CLOCKRATE_DIV_64			2
#define SPI_CLOCKRATE_DIV_128			3
#define SPI_CLOCKRATE_DIV_2				4
#define SPI_CLOCKRATE_DIV_8				5
#define SPI_CLOCKRATE_DIV_32			6

// SPI Pins
#define SS		4
#define MOSI	5
#define MISO	6
#define SCK		7		

/* ------------------------ Functions Like Macro ------------------------- */
#define SPI_ENABLE()					SET_BIT(SPCR,SPE)
#define SPI_DISABLE()					CLEAR_BIT(SPCR,SPE)

#define SPI_INTERRUPT_ENABLE()			SET_BIT(SPCR,SPIE)
#define SPI_INTERRUPT_DISABLE()			CLEAR_BIT(SPCR,SPIE)

#define SPI_TRANSMIT_LSB_FIRST()		SET_BIT(SPCR,DORD)
#define SPI_TRANSMIT_MSB_FIRST()		CLEAR_BIT(SPCR,DORD)

#define SPI_SET_MODE_MASTER()			SET_BIT(SPCR,MSTR)
#define SPI_SET_MODE_SLAVE()			CLEAR_BIT(SPCR,MSTR)

#define SPI_SCK_IDLEHIGH()				SET_BIT(SPCR,CPOL)
#define SPI_SCK_IDLELOW()				CLEAR_BIT(SPCR,CPOL)

#define SPI_TRAILING_DATASAMPLED()		SET_BIT(SPCR,CPHA)
#define SPI_LEADING_DATASAMPLED()		CLEAR_BIT(SPCR,CPHA)

/* ----------------------- Data Types ----------------------- */
typedef struct  
{
	uint8_t spi_mode;
	uint8_t spi_data_order;
	uint8_t spi_clock_polarity;
	uint8_t spi_clock_phase;
	uint8_t spi_clock_rate;
}Spicfg_t;

/* ----------------------- Software Interfaces Declarations ----------------------- */
void SPI_Init(const Spicfg_t* spi_obj);

void SPI_Transmit(const Spicfg_t* spi_obj, uint8_t data);

uint8_t SPI_Receive(const Spicfg_t* spi_obj);

uint8_t SPI_Transceive(const Spicfg_t* spi_obj, uint8_t data);


#endif /* SPI_H_ */