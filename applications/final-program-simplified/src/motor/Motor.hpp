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

    // State where the robot searches for paths and obstacles
    void searchState();

    // State where the robot detects rocks and hills
    void detectionState();

    // State where the robot avoids black tape detected by IR sensors
    void avoidState();

    // State where the robot reorients its direction
    void reorientState();

    // State where the robot handles being stuck and attempts recovery
    void stuckState();


    std::string state_;

    ThreadParams& params_;
    MotorController motorController_;
    Coordinate currentCoords_;
    std::thread motorThread_;

    const int STUCK_THRESHOLD = 3;
    int stuckCounter_;

    int ds1_;
    int ds2_;
    int ir1_;
    int ir2_;


    // Add your member variables here
};

#endif // MOTOR_HPP