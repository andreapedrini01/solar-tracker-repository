#include <StepperLib/base_stepper.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "msp.h"

void init_baseStepper() {
    // Inizializza la port e i pin
    MAP_WDT_A_holdTimer();

    // Configura i pin come output
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, BASE_STEP_PIN | BASE_DIR_PIN);

    // Imposta i pin a livello basso iniziale
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, BASE_STEP_PIN | BASE_DIR_PIN);
}

void stepBaseMotor() {
    // Invia un impulso STEP
        MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P1, BASE_STEP_PIN);
        __delay_cycles(5000); //Aggiungi un piccolo ritardo
        MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, BASE_STEP_PIN);
}

void moveBase(int steps) {
    if (steps > 0) {
        // moveBaseForward
        MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, BASE_DIR_PIN);
    } else {
        // moveBaseBackward
        steps = 0 - steps;
        MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P1, BASE_DIR_PIN);
    }
    // Genera i passi
    int i;
    for (i = 0; i < steps; i++) {
        stepBaseMotor();
        __delay_cycles(250); // Aggiungi un ritardo tra i passi
    }
}
