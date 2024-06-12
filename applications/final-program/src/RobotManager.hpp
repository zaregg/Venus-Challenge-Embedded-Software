#ifndef ROBOT_MANAGER_HPP
#define ROBOT_MANAGER_HPP

// Include any necessary headers
#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <cstdlib> // Include for EXIT_FAILURE and EXIT_SUCCESS

#include "threads/StepperThread.hpp"
#include "threads/ComManagerThread.hpp" // Include the header file for ComManagerThread
#include "threads/SensorManager.hpp"
#include "threads/sensors/DistanceSensor.hpp"
#include "threads/sensors/ColorSensor.hpp"
#include "utils/structs.hpp"

class RobotManager {
public:
    SensorManager sensorManager;
    CommunicationManager comManager;

    DistanceSensor distanceSensor;
    ColorSensor colorSensor;

    Stepper stepperThread;
    
    RobotManager();
    ~RobotManager();

    void start();
    void stop();

    void join();
    // Add your class members and methods here

private:

    SensorManagerQueue comToSensorQueue; // Create a queue to store pointers to s_StepperThread
    SensorManagerQueue sensorToComQueue; // Create a queue to store pointers to s_StepperThread

    StepperQueue comToStepperQueue; // Create a queue to store pointers to s_StepperThread
    StepperQueue stepperToComQueue; // Create a queue to store pointers to s_StepperThread
    // Add your private members and methods here

    void addSensors();

};

#endif // ROBOT_MANAGER_HPP