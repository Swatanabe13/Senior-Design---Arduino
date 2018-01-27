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
#define ADC_BUFFER_SIZE 5000
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
int adc_queue[ADC_BUFFER_SIZE];
int head = 0;
int tail = 0;
//-----------------------------------------------------------------------------
//      __   __   __  ___  __  ___      __   ___  __
//     |__) |__) /  \  |  /  \  |  \ / |__) |__  /__`
//     |    |  \ \__/  |  \__/  |   |  |    |___ .__/
//
//-----------------------------------------------------------------------------
void enqueue_val(int * queue, int value);
int dequeue_val(int * queue);
int queue_empty();
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
		enqueue_val(adc_queue, ADC->RESULT.reg);
		
		//Interrupt driven SPI will do the thing for you and dequeue
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

int queue_empty()
{
	return head == tail;
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
	SERCOM4->SPI.DATA.reg = dequeue_val(adc_queue);
}