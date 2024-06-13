#ifndef MOTOR_CONTROLLER_HPP
#define MOTOR_CONTROLLER_HPP

#include "RobotParams.hpp"

class MotorController {
public:
    // Constructor
    MotorController(MotorParams& params);

    // Destructor
    ~MotorController();

};

#endif // MOTOR_CONTROLLER_HPP