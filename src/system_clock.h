#ifndef SYSTEM_CLOCK_H_
#define SYSTEM_CLOCK_H_

#include "asf.h"

extern uint16_t real_time_clock_adjustment;

void system_clock_init(void);

#define RTC_VALUE (RTC.CNT + real_time_clock_adjustment)

#endif /* SYSTEM_CLOCK_H_ */