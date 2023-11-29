#ifndef __BASE_STEPPER_H__
#define __BASE_STEPPER_H__

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "msp.h"

#define ENABLE_PIN GPIO_PIN5  // Specifica il pin per il segnale ENABLE
#define STEP_PIN GPIO_PIN6  // Specifica il pin per il segnale STEP
#define DIR_PIN GPIO_PIN7   // Specifica il pin per il segnale DIR

extern void init_baseStepper();
static void stepBaseMotor();
extern void moveBaseForward(int steps);
extern void moveBaseBackward(int steps);
extern void lock_baseEnable();
extern void unlock_baseEnable();

#endif /*stepperLib.h*/
