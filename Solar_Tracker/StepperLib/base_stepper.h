#ifndef __BASE_STEPPER_H__
#define __BASE_STEPPER_H__

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "msp.h"

#define STEP_PIN GPIO_PIN6  // Specifica il pin per il segnale STEP
#define DIR_PIN GPIO_PIN7   // Specifica il pin per il segnale DIR
#define BASE_STEP_DELAY 100000

extern void init_baseStepper();
static void stepBaseMotor();
extern void moveBase(int steps);

#endif
