#ifndef POSITION_SENSOR_ISR_H_
#define POSITION_SENSOR_ISR_H_

#include "read_access.h"

#include <asf.h>


#define POSITION_PORT_COUNT 5
#define POSITION_SENSOR_COUNT 8*POSITION_PORT_COUNT

#define POSITION_SENSOR_TIMESTAMP_COUNT 3

#if POSITION_SENSOR_TIMESTAMP_COUNT < 3
#error "POSITION_SENSOR_TIMESTAMP_COUNT must be at least 3."
#endif

typedef struct position_sensor_state_struct
{
	// timestamps[0] holds the most recent timestamp, timestamps[1] the previous
	// one and so on.
	uint16_t timestamps[POSITION_SENSOR_TIMESTAMP_COUNT];
	read_access_t read_access;
} position_sensor_state_t;

extern position_sensor_state_t position_sensor_states[POSITION_SENSOR_COUNT];
extern uint8_t position_sensor_port_changed;

void position_sensor_init_isr(void);

#endif /* POSITION_SENSOR_ISR_H_ */