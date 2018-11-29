#include <asf.h>
#include "system_clock.h"
#include "debug.h"
#include "usb.h"
#include "lane_sensor.h"
#include "position_sensor.h"
#include "interrupts.h"


int main (void)
{
	system_clock_init ();
	debug_init ();
	usb_init ();
	
	lane_sensor_init ();
	position_sensor_init ();
	
	interrupts_enable ();	
		
	while (true)
	{
		usb_process_queue();		
	}
}





