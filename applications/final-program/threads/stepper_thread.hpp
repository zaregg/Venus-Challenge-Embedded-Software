#ifndef STEPPER_THREAD_H
#define STEPPER_THREAD_H

// Include any necessary libraries here
extern "C" {
    #include <stdio.h>
    #include <stdint.h>
    #include <libpynq.h>
    #include <platform.h>
    #include <stepper.h>
}

#include <pthread.h>

// Declare the Stepper class
class Stepper {
public:
    Stepper();
    ~Stepper();
    // Declare any member variables or functions here

    

};

// Declare any function prototypes here
void *stepperThread(void *vargp);

#endif // STEPPER_THREAD_H