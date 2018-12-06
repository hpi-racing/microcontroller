#ifndef LANE_SENSOR_ISR_H_
#define LANE_SENSOR_ISR_H_

#include "read_access.h"

#include <asf.h>

typedef struct lane_sensor_packet_struct
{
	uint8_t bit_count;
	uint16_t bit_sequence;
	read_access_t read_access;
} lane_sensor_packet_t;

extern lane_sensor_packet_t lane_sensor_packet;


void lane_sensor_init_isr(void);


#endif /* LANE_SENSOR_ISR_H_ */