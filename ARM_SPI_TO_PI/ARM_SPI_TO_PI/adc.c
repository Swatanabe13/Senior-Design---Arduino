//------------------------------------------------------------------------------
//             __             __   ___  __
//     | |\ | /  ` |    |  | |  \ |__  /__`
//     | | \| \__, |___ \__/ |__/ |___ .__/
//
//------------------------------------------------------------------------------

#include "ADC.h"
#include "sam.h"

//------------------------------------------------------------------------------
//      __   ___  ___         ___  __
//     |  \ |__  |__  | |\ | |__  /__`
//     |__/ |___ |    | | \| |___ .__/
//
//------------------------------------------------------------------------------
#define ADC_PORT (PORT_PA02)
#define ADC_PIN (PIN_PA02)
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
//Initialize the ADC
void adc_init()
{
	PORT->Group[0].PINCFG[PIN_PA02B_ADC_AIN0].bit.INEN = 1;
	while(ADC->STATUS.bit.SYNCBUSY == 1 ) { };
	PORT->Group[0].DIRCLR.reg = PORT_PA02;
	
	// Enable the GCLK
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID_ADC;
	while(ADC->STATUS.bit.SYNCBUSY == 1 ) { };
	
	// Enable the ADC
	ADC->CTRLA.bit.ENABLE = 1;
	while(ADC->STATUS.bit.SYNCBUSY == 1 ) { };
	
	// CONFIGURATION --------------------------------------------
	ADC->REFCTRL.bit.REFSEL = 2; // 1/2 VDDANA Halves voltage so we don't blow up
	while(ADC->STATUS.bit.SYNCBUSY == 1 ) { };

	ADC->CTRLB.bit.PRESCALER = 07;
	while(ADC->STATUS.bit.SYNCBUSY == 1 ) { };

	ADC->SAMPCTRL.bit.SAMPLEN = 0x00;
	while (ADC->STATUS.bit.SYNCBUSY == 1 ) { };
	
	
	ADC->CTRLB.bit.RESSEL = 3;		// 0 == 12-bit resolution. 
									// 1 == 16-bit resolution. 
									// 2 == 10-bit resolution. 
									// 3 ==  8-bit resolution. 
	while(ADC->STATUS.bit.SYNCBUSY == 1 ) { };	

	ADC->CTRLB.bit.FREERUN = 1; // Freerunning mode. KEEP THIS ON 1
	while(ADC->STATUS.bit.SYNCBUSY == 1 ) { };

	ADC->INPUTCTRL.bit.GAIN = 0x0; // pure 1x gain
	while(ADC->STATUS.bit.SYNCBUSY == 1 ) { };
	
	ADC->INPUTCTRL.bit.MUXPOS = 00; //Using analog pin 0, MUXPOS should be 0. Change to 1 to simulate no input.
	while(ADC->STATUS.bit.SYNCBUSY == 1 ) { };
	
	ADC->INPUTCTRL.bit.MUXNEG = 0x19; //Internal Ground 
	while(ADC->STATUS.bit.SYNCBUSY == 1 ) { };
		
    ADC->DBGCTRL.bit.DBGRUN = 0; //Use for DEBUGGING ONLY. TURN OFF when running for reals.
	while(ADC->STATUS.bit.SYNCBUSY == 1 ) { };

	//Enable double-stuffed Oreo. Interrupts. It's interrupts.
	NVIC_DisableIRQ(ADC_IRQn);
	NVIC_ClearPendingIRQ(ADC_IRQn);
	NVIC_SetPriority(ADC_IRQn, 0);
	NVIC_EnableIRQ(ADC_IRQn);
	
	ADC->INTENSET.bit.RESRDY = 1;
	ADC->INTENSET.bit.OVERRUN = 1;
	//ADC->INTENSET.bit.SYNCRDY = 1;
}

//==============================================================================
//Return the value of the ADC
uint16_t adc_get()
{
	uint16_t adc_val = 0;
	//Wait to sync
	while(ADC->STATUS.bit.SYNCBUSY == 1) { };
	//Set the ADC Value to ADC Val
	adc_val = ADC->RESULT.reg;
	
	//Return ADC Value
	return adc_val;
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
