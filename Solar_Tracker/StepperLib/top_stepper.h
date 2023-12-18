#ifndef __TOP_STEPPER_H__
#define __TOP_STEPPER_H__

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "msp.h"

#define STEP_PIN GPIO_PIN6  // Specifica il pin per il segnale STEP
#define DIR_PIN GPIO_PIN5   // Specifica il pin per il segnale DIR

extern void init_topStepper();
static void stepTopMotor();
extern void moveTop(int steps);

#endif
