#ifndef __BASE_STEPPER_H__
#define __BASE_STEPPER_H__

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "msp.h"

#define BASE_STEP_PIN GPIO_PIN6  // Specifica il pin per il segnale STEP
#define BASE_DIR_PIN GPIO_PIN7   // Specifica il pin per il segnale DIR
#define SLOWER_DELAY 90000
#define FASTER_DELAY 45000
#define FINAL_STEPS 5

extern void init_baseStepper();
static void stepBaseMotor();
extern void moveBase(int steps);

#endif
