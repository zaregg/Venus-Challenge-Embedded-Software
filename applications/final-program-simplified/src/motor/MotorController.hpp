#ifndef MOTOR_CONTROLLER_HPP
#define MOTOR_CONTROLLER_HPP

#include "RobotParams.hpp"
#include <iostream>
#include <cmath>
#include <thread>

extern "C" {
    #include <libpynq.h>
    #include <stepper.h>
    #include <arm_shared_memory_system.h>
}

#define STEPS_PER_CM 65
#define STEPS_PER_DEGREE 7

// Motor controller class for controlling the stepper motor
class MotorController {
public:
    // Constructor
    MotorController(MotorParams& params);

    // Destructor
    ~MotorController();

    void enable();

    void disable();

    void turn(int degrees); // Added parameter for degrees

    void forward(int distance); // Added parameter for steps

    void backward(int distance); // Added parameter for steps

    void scan(int degrees); // Added parameter for degrees


    float degreesToRads(int degrees); // Added parameter for degrees
private:
    MotorParams& params_;

    // int steps_up = cm*65; // Added variable for steps_up
    // int turn = degrees*108; // terible variable naming

};;

#endif // MOTOR_CONTROLLER_HPP