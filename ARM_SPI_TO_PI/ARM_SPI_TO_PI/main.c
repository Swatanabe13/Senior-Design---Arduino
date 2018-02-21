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
//----------------------------------------------------------------------------
#define ADC_BUFFER_SIZE 2
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

//ADC_queue and variables
int adc_queue[ADC_BUFFER_SIZE];
int head = 0;
int tail = 0;
int val = 0;

int high = 0;
int low = 0;

int i = 0;
int queueNewVal = 0;
int waitForTransfer = 0;
int lowByteSent = 0;
//-----------------------------------------------------------------------------
//      __   __   __  ___  __  ___      __   ___  __
//     |__) |__) /  \  |  /  \  |  \ / |__) |__  /__`
//     |    |  \ \__/  |  \__/  |   |  |    |___ .__/
//
//-----------------------------------------------------------------------------
void enqueue_val(int * queue, int value);
int dequeue_val(int * queue);

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
	
   //Interrupt driven ADC and SPI will do all the work
   while(1)
   {
		//Just keep spinning, just keep spinning...
   }
}

void enqueue_val (int * queue, int value)
{
	if (tail < ADC_BUFFER_SIZE)
	{
		tail++;
	}
	else
	{
		tail = 0;
	}
	queue[tail] = value;
}

int dequeue_val (int * queue)
{
	int value = queue[head];
	if (head < ADC_BUFFER_SIZE)
	{
		head++;
	}
	else
	{
		head = 0;
	}
	return value;
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
void SERCOM4_Handler()
{
	//New value
	SERCOM4->SPI.DATA.bit.DATA = dequeue_val(adc_queue);
	i++;
	if (i > 1)
	{
		queueNewVal = 1;
		waitForTransfer = 1;
		i = 0;
	}
}

void ADC_Handler()
{
	if (queueNewVal)
	{
		waitForTransfer = 1;
		val = ADC->RESULT.reg;
		queueNewVal = 0;
	}
	//If the SPI has transferred both bytes, queue up a new value
	if (waitForTransfer == 1)
	{
		//If the low byte has not been queued yet, queue it up
		if (lowByteSent == 0)
		{
			enqueue_val(adc_queue, val & 0xFF);
			lowByteSent = 1;
		}
		//Send the high byte and set queueNewVal to true
		else
		{
			enqueue_val(adc_queue, val >> 8);
			lowByteSent = 0;
			waitForTransfer = 0;
		}
	}
}