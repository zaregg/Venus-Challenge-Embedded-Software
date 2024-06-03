#include "stepper_thread.hpp"


extern "C" {
    #include "../utils/structs.h"
}


Stepper::Stepper() 
{
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

uint16_t Stepper::angleToSteps(uint16_t angle)
{
    return -1;
}

uint16_t Stepper::distanceToSteps(uint16_t distance)
{
    return -1;
}


void *stepperThreadFunction(boost::lockfree::queue<s_StepperThread*>& threadQueue) 
{
    printf("Stepper thread started\n");
    s_StepperThread* threadStruct = nullptr;
    if (threadQueue.pop(threadStruct)) {
        // Process the received s_StepperThread object
        std::cout << "Received s_StepperThread object from main thread." << std::endl;
        // Here you can perform any actions required for stepper motor control
        // For demonstration purposes, we'll just print the values of the struct fields
        std::cout << "Step Count: " << threadStruct->stepCount << std::endl;
        std::cout << "Direction: " << threadStruct->angle << std::endl;
        std::cout << "Speed: " << threadStruct->speed << std::endl;
        // Free the memory allocated for the s_StepperThread object
        delete threadStruct;
    }

    return NULL;
}

