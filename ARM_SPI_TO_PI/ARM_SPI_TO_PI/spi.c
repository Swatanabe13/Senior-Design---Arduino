//------------------------------------------------------------------------------
//             __             __   ___  __
//     | |\ | /  ` |    |  | |  \ |__  /__`
//     | | \| \__, |___ \__/ |__/ |___ .__/
//
//------------------------------------------------------------------------------

#include "sam.h"
#include "spi.h"
#include "stdbool.h"

//------------------------------------------------------------------------------
//      __   ___  ___         ___  __
//     |  \ |__  |__  | |\ | |__  /__`
//     |__/ |___ |    | | \| |___ .__/
//
//------------------------------------------------------------------------------

#define SPI_MOSI (PORT_PB10)
#define SPI_MOSI_GROUP (1)
#define SPI_MOSI_PIN (PIN_PB10%32)
#define SPI_MOSI_PMUX (SPI_MOSI_PIN/2)

#define SPI_MISO (PORT_PA12)
#define SPI_MISO_GROUP (0)
#define SPI_MISO_PIN (PIN_PA12%32)
#define SPI_MISO_PMUX (SPI_MISO_PIN/2)

#define SPI_SCK (PORT_PB11)
#define SPI_SCK_GROUP (1)
#define SPI_SCK_PIN (PIN_PB11%32)
#define SPI_SCK_PMUX (SPI_SCK_PIN/2)

#define SPI_SS (PORT_PA13)
#define SPI_SS_GROUP (0)
#define SPI_SS_PIN (PIN_PA13%32)
#define SPI_SS_PMUX (SPI_SCK_PIN/2)
//------------------------------------------------------------------------------
//     ___      __   ___  __   ___  ___  __
//      |  \ / |__) |__  |  \ |__  |__  /__`
//      |   |  |    |___ |__/ |___ |    .__/
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                __          __        ___  __
//     \  /  /\  |__) |  /\  |__) |    |__  /__`
//      \/  /~~\ |  \ | /~~\ |__) |___ |___ .__/
//
//------------------------------------------------------------------------------
static uint8_t busy = 0;
static uint8_t XLAT_needed = 0;
static uint8_t pos = 0;
static bool transfer = false;
static uint8_t data[5];
//------------------------------------------------------------------------------
//      __   __   __  ___  __  ___      __   ___  __
//     |__) |__) /  \  |  /  \  |  \ / |__) |__  /__`
//     |    |  \ \__/  |  \__/  |   |  |    |___ .__/
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//      __        __          __
//     |__) |  | |__) |    | /  `
//     |    \__/ |__) |___ | \__,
//
//------------------------------------------------------------------------------

//==============================================================================
void spi_init()
{
  //////////////////////////////////////////////////////////////////////////////
  // Configure the PORTS
  //////////////////////////////////////////////////////////////////////////////

  // MOSI
  // Configure the appropriate peripheral
#if (SPI_MOSI_PIN % 2) // Odd Pin
  PORT->Group[SPI_MOSI_GROUP].PMUX[SPI_MOSI_PMUX].bit.PMUXO = PORT_PMUX_PMUXO_D_Val;
#else                  // Even Pin
  PORT->Group[SPI_MOSI_GROUP].PMUX[SPI_MOSI_PMUX].bit.PMUXE = PORT_PMUX_PMUXE_D_Val;
#endif
  // Enable the PMUX
  PORT->Group[SPI_MOSI_GROUP].PINCFG[SPI_MOSI_PIN].bit.PMUXEN = 1;

  // MISO
  // Configure the appropriate peripheral
#if (SPI_MISO_PIN % 2) // Odd Pin
  PORT->Group[SPI_MISO_GROUP].PMUX[SPI_MISO_PMUX].bit.PMUXO = PORT_PMUX_PMUXO_D_Val;
#else                  // Even Pin
  PORT->Group[SPI_MISO_GROUP].PMUX[SPI_MISO_PMUX].bit.PMUXE = PORT_PMUX_PMUXE_D_Val;
#endif
  // Enable the PMUX
  PORT->Group[SPI_MISO_GROUP].PINCFG[SPI_MISO_PIN].bit.PMUXEN = 1;

  // SCK
  // Configure the appropriate peripheral
#if (SPI_SCK_PIN % 2) // Odd Pin
  PORT->Group[SPI_SCK_GROUP].PMUX[SPI_SCK_PMUX].bit.PMUXO = PORT_PMUX_PMUXO_D_Val;
#else                  // Even Pin
  PORT->Group[SPI_SCK_GROUP].PMUX[SPI_SCK_PMUX].bit.PMUXE = PORT_PMUX_PMUXE_D_Val;
#endif
  // Enable the PMUX
  PORT->Group[SPI_SCK_GROUP].PINCFG[SPI_SCK_PIN].bit.PMUXEN = 1;
  
  //SS
#if (SPI_SS_PIN % 2) // Odd Pin
  PORT->Group[SPI_SS_GROUP].PMUX[SPI_SS_PMUX].bit.PMUXO = PORT_PMUX_PMUXO_D_Val;
#else                  // Even Pin
  PORT->Group[SPI_SS_GROUP].PMUX[SPI_SS_PMUX].bit.PMUXE = PORT_PMUX_PMUXE_D_Val;
#endif
  // Enable the PMUX
  PORT->Group[SPI_SS_GROUP].PINCFG[SPI_SS_PIN].bit.PMUXEN = 1;


  //Enable
  PORT->Group[SPI_SCK_GROUP].PINCFG[SPI_SCK_PIN].bit.INEN = 1;
  PORT->Group[SPI_MOSI_GROUP].PINCFG[SPI_MOSI_PIN].bit.INEN = 1;
  PORT->Group[SPI_SS_GROUP].PINCFG[SPI_SS_PIN].bit.INEN = 1;

  PORT->Group[SPI_MISO_GROUP].DIRSET.reg = SPI_MISO;
  

  //////////////////////////////////////////////////////////////////////////////
  // Disable the SPI - 26.6.2.1
  //////////////////////////////////////////////////////////////////////////////
  SERCOM4->SPI.CTRLA.bit.ENABLE = 0;
  // Wait for it to complete
  while (SERCOM4->SPI.SYNCBUSY.bit.ENABLE);




  //////////////////////////////////////////////////////////////////////////////
  // Set up the PM (default on, but let's just do it) and the GCLK
  //////////////////////////////////////////////////////////////////////////////  
  PM->APBCMASK.reg |= PM_APBCMASK_SERCOM4;

  // Initialize the GCLK
  // Setting clock for the SERCOM4_CORE clock
  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(GCLK_CLKCTRL_ID_SERCOM4_CORE) | 
    GCLK_CLKCTRL_GEN_GCLK0                        | 
    GCLK_CLKCTRL_CLKEN ;

  // Wait for the GCLK to be synchronized
  while(GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);




  //////////////////////////////////////////////////////////////////////////////
  // Initialize the SPI
  //////////////////////////////////////////////////////////////////////////////

  // Reset the SPI
  SERCOM4->SPI.CTRLA.bit.SWRST = 1;
  // Wait for it to complete
  while (SERCOM4->SPI.CTRLA.bit.SWRST || SERCOM4->SPI.SYNCBUSY.bit.SWRST);

  // Set up CTRLA 
  SERCOM4->SPI.CTRLA.bit.DIPO = 0x02; // MOSI on PAD2
  SERCOM4->SPI.CTRLA.bit.DOPO = 0x03; // MISO on PAD0
  SERCOM4->SPI.CTRLA.bit.DORD = 0; // MSB Transferred first
  SERCOM4->SPI.CTRLA.bit.CPOL = 0; // SCK Low when Idle
  SERCOM4->SPI.CTRLA.bit.CPHA = 0; // Data sampled on leading edge and change on trailing edge
  SERCOM4->SPI.CTRLA.bit.MODE = 0x02; // Set MODE as SPI Slave
  // Set up CTRLB
  SERCOM4->SPI.CTRLB.bit.RXEN = 1; // Enable the receiver
  SERCOM4->SPI.CTRLB.bit.PLOADEN = 1; //Enable preload
  
  // Set up the BAUD rate	
  SERCOM4->SPI.BAUD.reg = 239; //100KHz - too slow, but easy to see on the Logic Analyzer


  //////////////////////////////////////////////////////////////////////////////
  // Enable the SPI
  ////////////////////////////////////////////////////////////////////////////// 
  SERCOM4->SPI.CTRLA.bit.ENABLE = 1;
  // Wait for it to complete
  while (SERCOM4->SPI.SYNCBUSY.bit.ENABLE);

//   //Enable interrupt
//   NVIC_DisableIRQ(SERCOM4_IRQn);
//   NVIC_ClearPendingIRQ(SERCOM4_IRQn);
//   NVIC_SetPriority(SERCOM4_IRQn, 0);
//   NVIC_EnableIRQ(SERCOM4_IRQn);
// 
//   SERCOM4->SPI.INTENSET.bit.TXC = 1; //Transfer complete byte

}


//==============================================================================
uint8_t spi_read()
{
  // Wait for something to show up in the data register
  while( SERCOM4->SPI.INTFLAG.bit.DRE == 0 || SERCOM4->SPI.INTFLAG.bit.RXC == 0 );
  // Read it and return it. 
  return SERCOM4->SPI.DATA.bit.DATA;
}

//==============================================================================
void spi(uint8_t* in_array)
{
	if (SERCOM4->SPI.INTFLAG.bit.DRE == 1)
	{
		SERCOM4->SPI.DATA.bit.DATA = in_array[0];
		//SERCOM4->SPI.INTENCLR.bit.DRE; done automatically by writing data
	}
}
//------------------------------------------------------------------------------
//      __   __              ___  ___
//     |__) |__) | \  /  /\   |  |__
//     |    |  \ |  \/  /~~\  |  |___
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//      __                  __        __        __
//     /  `  /\  |    |    |__)  /\  /  ` |__/ /__`
//     \__, /~~\ |___ |___ |__) /~~\ \__, |  \ .__/
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//        __   __  , __
//     | /__` |__)  /__`   
//     | .__/ |  \  .__/
//
//------------------------------------------------------------------------------
void SERCOM4_Handler()
{
	if (pos < sizeof data)
	{
		// Send the data
		SERCOM4->SPI.DATA.bit.DATA = data[pos++];
	}
	else
	{
		pos = 0;
	}
}