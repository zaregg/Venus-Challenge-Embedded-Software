#include "stepper_thread.hpp"


extern "C" {
    #include "../structs.h"
}


Stepper::Stepper() : xCoordinate(0), yCoordinate(0) {
    switchbox_set_pin(IO_AR0, SWB_UART0_RX);
    switchbox_set_pin(IO_AR1, SWB_UART0_TX);
    gpio_set_direction(IO_AR2, GPIO_DIR_INPUT);
    gpio_set_direction(IO_AR3, GPIO_DIR_INPUT);

    stepper_init();
    stepper_enable();
}

Stepper::~Stepper() {
    stepper_disable();
}

void *stepperThread(void *vargp) {
    s_StepperThread *stepperThread = (s_StepperThread *)vargp;

    Stepper stepper;

    printf("Stepper thread started\n");

    return NULL;
}