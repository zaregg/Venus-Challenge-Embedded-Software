#pragma once
#ifndef STEPPER_THREAD_H
#define STEPPER_THREAD_H

// Include any necessary libraries here

#include <iostream>
#include <boost/lockfree/queue.hpp> // Include Boost.Lockfree
#include <thread>
#include <cstdlib> // Include for EXIT_FAILURE and EXIT_SUCCESS
#include <memory> // Include for std::unique_ptr

#include "../utils/structs.hpp"

extern "C" {
    // #include <stdio.h>
    // // #include <stdint.h>
    // #include <libpynq.h>
    // #include <platform.h>
    // #include <stepper.h>
}

/**
 * @class Stepper
 * @brief Represents a stepper motor controller.
 * 
 * The Stepper class provides functionality to control a stepper motor. It allows starting and stopping the motor, as well as converting angles and distances to steps.
 */
class Stepper {
public:
    /**
     * @brief Constructs a Stepper object.
     * @param threadQueue A reference to a boost::lockfree::queue<s_StepperThread*> object representing the thread queue.
     * 
     * This constructor creates a Stepper object and initializes it with a reference to a thread queue. The thread queue is used to communicate with the stepper motor control thread.
     */
    Stepper(StepperQueue &comToStepperQueue, StepperQueue &stepperToComQueue);

    /**
     * @brief Destroys the Stepper object.
     * 
     * This destructor is responsible for cleaning up any resources used by the Stepper object.
     */
    ~Stepper();

    // Declare any member variables or functions here

    /**
     * @brief Starts the stepper motor.
     * 
     * This function starts the stepper motor by creating and launching a new thread responsible for controlling the motor.
     */
    void start();

    /**
     * @brief Stops the stepper motor.
     * 
     * This function stops the stepper motor by setting the running flag to false. The move thread will then stop executing and the stepper motor will stop moving.
     */
    void stop();

    /**
     * @brief Waits for the stepper thread to finish.
     * 
     * This function blocks the calling thread until the stepper motor move thread has finished executing.
     */
    void join();

private:
    std::thread stepperThread; /**< The thread responsible for controlling the stepper motor. */
    StepperQueue &comToStepperQueue; /**< A reference to the queue used to send messages to the stepper motor control thread. */
    StepperQueue &stepperToComQueue; /**< A reference to the queue used to receive messages from the stepper motor control thread. */
    std::atomic<bool> running_; /**< A flag indicating whether the stepper motor is running or not. */

    /**
     * @brief Converts an angle to the corresponding number of steps.
     * @param angle The angle to convert, in degrees.
     * @return The number of steps corresponding to the given angle.
     * 
     * This function converts an angle in degrees to the corresponding number of steps for the stepper motor. It uses a conversion formula specific to the motor being controlled.
     */
    uint16_t angleToSteps(uint16_t angle);

    /**
     * @brief Converts a distance to the corresponding number of steps.
     * @param distance The distance to convert, in millimeters.
     * @return The number of steps corresponding to the given distance.
     * 
     * This function converts a distance in millimeters to the corresponding number of steps for the stepper motor. It uses a conversion formula specific to the motor being controlled.
     */
    uint16_t distanceToSteps(uint16_t distance);

    /**
     * @brief Moves the stepper motor.
     * 
     * This function is called by the control thread to move the stepper motor. It performs the necessary operations to control the motor, such as setting the direction and step count.
     */
    void move();
};

// Declare any function prototypes here
// void *stepperThreadFunction(boost::lockfree::queue<s_StepperThread*>& threadQueue);

#endif // STEPPER_THREAD_H