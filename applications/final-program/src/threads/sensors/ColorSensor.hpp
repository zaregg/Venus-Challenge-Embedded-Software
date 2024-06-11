#ifndef COLORSENSOR_HPP
#define COLORSENSOR_HPP

// Include any necessary libraries or headers
#include <atomic>
#include <boost/lockfree/queue.hpp>
#include <condition_variable>
#include <iostream>
#include <string>
#include <thread>
#include <zmq.hpp>

#include "Sensor.hpp"
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

    zmq::context_t context_;
    zmq::socket_t socket_;

    RobotQueue* managerToSensorQueue_;
    RobotQueue* sensorToManagerQueue_;

    // Private member variables
    // send_capture_signal();
    void workerThread();

    void sendCaptureSignal();
    std::string processColorData(const std::string& color_str);
    void receiveFromPython(s_ColorSensorTest* data);
    
    // Private member functions
};

#endif // COLORSENSOR_HPP