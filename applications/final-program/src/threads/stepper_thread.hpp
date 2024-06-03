#ifndef STEPPER_THREAD_H
#define STEPPER_THREAD_H

// Include any necessary libraries here

#include <iostream>
#include <boost/lockfree/queue.hpp> // Include Boost.Lockfree
#include <thread>
#include <cstdlib> // Include for EXIT_FAILURE and EXIT_SUCCESS
#include <memory> // Include for std::unique_ptr

#include "../utils/structs.h"

extern "C" {
    #include <stdio.h>
    // #include <stdint.h>
    #include <libpynq.h>
    #include <platform.h>
    #include <stepper.h>
}

// Declare the Stepper class
class Stepper {
public:
    Stepper();
    ~Stepper();
    // Declare any member variables or functions here

    // 
    uint16_t angleToSteps(uint16_t angle);

    //
    uint16_t distanceToSteps(uint16_t distance);

};

// Declare any function prototypes here
void *stepperThreadFunction(boost::lockfree::queue<s_StepperThread*>& threadQueue);

#endif // STEPPER_THREAD_H