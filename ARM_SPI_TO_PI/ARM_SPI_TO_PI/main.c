//------------------------------------------------------------------------------
//             __             __   ___  __
//     | |\ | /  ` |    |  | |  \ |__  /__`
//     | | \| \__, |___ \__/ |__/ |___ .__/
//
//------------------------------------------------------------------------------

#include "sam.h"
#include "stdint.h"
#include "stdlib.h"
#include "stdbool.h"
#include "spi.h"
#include "adc.h"

//-----------------------------------------------------------------------------
//      __   ___  ___         ___  __
//     |  \ |__  |__  | |\ | |__  /__`
//     |__/ |___ |    | | \| |___ .__/
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//     ___      __   ___  __   ___  ___  __
//      |  \ / |__) |__  |  \ |__  |__  /__`
//      |   |  |    |___ |__/ |___ |    .__/
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//                __          __        ___  __
//     \  /  /\  |__) |  /\  |__) |    |__  /__`
//      \/  /~~\ |  \ | /~~\ |__) |___ |___ .__/
//
//-----------------------------------------------------------------------------
static int adc_val;
//-----------------------------------------------------------------------------
//      __   __   __  ___  __  ___      __   ___  __
//     |__) |__) /  \  |  /  \  |  \ / |__) |__  /__`
//     |    |  \ \__/  |  \__/  |   |  |    |___ .__/
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//      __        __          __
//     |__) |  | |__) |    | /  `
//     |    \__/ |__) |___ | \__,
//
//-----------------------------------------------------------------------------

//=============================================================================
int main(void)
{
    /* Initialize the SAM system */
    SystemInit();
	spi_init();
	adc_init(); 	
	
   //When reading from Pi, last byte of data will be read first before beginning from data[0]
   while(1)
   {
 	   while (ADC->STATUS.bit.SYNCBUSY == 1) {};
 	   //Set the ADC Value to ADC Val
       adc_val = ADC->RESULT.reg;
 	   
		//Then pass to SPI
		//Send high byte
		if (SERCOM4->SPI.INTFLAG.bit.DRE == 1)
		{
 			SERCOM4->SPI.DATA.reg = adc_val;
		}
   }
   
}

//-----------------------------------------------------------------------------
//      __   __              ___  ___
//     |__) |__) | \  /  /\   |  |__
//     |    |  \ |  \/  /~~\  |  |___
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//        __   __   __
//     | /__` |__) /__`
//     | .__/ |  \ .__/
//
//-----------------------------------------------------------------------------