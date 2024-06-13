#ifndef SENSOR_HPP
#define SENSOR_HPP

#include "RobotParams.hpp"
#include "ColourSensor.hpp"
#include "DistanceSensor.hpp"
#include "IRSensor.hpp"

#include <iostream>
#include <thread>

class Sensor {
public:
    Sensor(ThreadParams& params);
    ~Sensor();

    void start();
    void stop();
    void join();

    // Add your member functions here

private:
    void sensorThread();

    ThreadParams& params_;
    ColourSensor colourSensor_;
    DistanceSensor distanceSensor_;
    IRSensor irSensor_;
    
    std::thread sensorThread_;
    // Add your member variables here

};

#endif // SENSOR_HPP