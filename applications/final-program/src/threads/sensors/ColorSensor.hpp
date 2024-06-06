#ifndef COLORSENSOR_HPP
#define COLORSENSOR_HPP

// Include any necessary libraries or headers
#include "Sensor.hpp"
#include <atomic>
#include <condition_variable>
#include <thread>
#include <iostream>
#include <boost/lockfree/queue.hpp>
#include "../../utils/structs.hpp"

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <arpa/inet.h>
// #include <signal.h>

#define PORT 65432
#define CAPTURE_SIGNAL "CAPTURE"

// Define the ColorSensor class
class ColorSensor : public Sensor<float> {
public:
    // Constructor(s)
    ColorSensor();
    // Destructor
    ~ColorSensor() override;

    void start(std::thread& thread, RobotQueue* managerToSensorQueue, RobotQueue* sensorToManagerQueue) override;

    void stop() override;

    void requestCapture();
    
    // Public member functions
    // void start(std::thread& thread, RobotQueue* managerToSensorQueue, RobotQueue* sensorToManagerQueue) override;
    // void requestCapture(RobotQueue* managerToSensorQueue, RobotQueue* sensorToManagerQueue);
    
private:
    std::atomic<bool> running_{false};
    std::atomic<bool> ready_{false};
    std::atomic<bool> stop_{false};
    std::condition_variable cv_;
    std::mutex cv_m; 
    std::thread worker_;

    RobotQueue* managerToSensorQueue_;
    RobotQueue* sensorToManagerQueue_;

    // Private member variables
    // send_capture_signal();
    void workerThread();
    void receive_from_python();
    
    // Private member functions
};

#endif // COLORSENSOR_HPP