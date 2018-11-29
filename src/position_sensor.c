#include "position_sensor.h"
#include "usb.h"
#include "debug.h"


int pin3_start;
int pin3_lastprobe_stamp;
int pin3_lastprobe_step;
int pin3_inUse = 0;
unsigned char portd_history;

void position_sensor_init (void)
{
	// Interrupts für Flanken des Positionssensors
	PORTD_INT0MASK |= PIN3_bm;
	PORTD_INTCTRL |= PORT_INT0LVL_HI_gc;
	PORTD_PIN3CTRL |= PORT_ISC_RISING_gc;

	TCC1_CTRLA = TC_CLKSEL_DIV64_gc;
}


ISR (PORTD_INT0_vect)
{
	DBG_OUT(PIN3_bm,0xff);
	
	uint8_t changedPins = PORTD_IN ^ portd_history;
	portd_history = PORTD_IN;

	
	//if (changedPins & PIN3_bm)
	if (PORTD_IN & PIN3_bm)
	{
		DBG_OUT(PIN2_bm,0xff);
		
		int currentStamp = TCC1_CNT;
		int currentStep = currentStamp - pin3_lastprobe_stamp;
		
		if ((currentStep - pin3_lastprobe_step)<3 && (pin3_lastprobe_step - currentStep)<3)
		{
			if (!pin3_inUse)
			{
				DBG_OUT(PIN1_bm,0xff);
				usb_send(currentStep);
				DBG_OUT(PIN1_bm,0x00);
			}
			
			pin3_inUse = 1;
		}
		else
		{
			pin3_inUse = 0;
		}
		
		pin3_lastprobe_stamp = currentStamp;
		pin3_lastprobe_step = currentStep;
		
		DBG_OUT(PIN2_bm,0x00);
	}
	
	DBG_OUT(PIN3_bm,0x00);
}
