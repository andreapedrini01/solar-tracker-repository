#ifndef __DELAY_H__
#define __DELAY_H__

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "msp.h"

extern void initTimer();
extern void delayMilliseconds(uint32_t ms);

#endif
