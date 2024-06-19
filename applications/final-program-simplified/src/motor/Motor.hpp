#ifndef MOTOR_HPP
#define MOTOR_HPP

#include <iostream>
#include <thread>
#include <cmath>
#include "RobotParams.hpp"
#include "MotorController.hpp"
#include "json.hpp"

class Motor {
public:
    // Constructor
    Motor(ThreadParams& params);

    // Destructor
    ~Motor();

    // Methods
    void start();
    void stop();
    void join();

    void setSpeed(int speed);

    // Add your member functions here

private:
    void motorThread();


    ThreadParams& params_;
    MotorController motorController_;
    Coordinate currentCoords_;
    std::thread motorThread_;
    // Add your member variables here
};

#endif // MOTOR_HPP