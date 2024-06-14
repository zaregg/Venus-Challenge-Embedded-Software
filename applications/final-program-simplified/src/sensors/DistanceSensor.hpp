#ifndef DISTANCESENSOR_HPP
#define DISTANCESENSOR_HPP

#include <iostream>
#include <chrono>
#include <thread>
#include <boost/lockfree/queue.hpp> // Include Boost.Lockfree
#include "RobotParams.hpp"

extern "C" {
    #include <libpynq.h>
    #include <iic.h>
    #include <vl53l0x.h>
}

class DistanceSensor {
public:
    // Constructor
    DistanceSensor();

    // Destructor
    ~DistanceSensor();

    int setup();

    // Function to get the distance measurement
    TOFData getDistance();

private:
    

    vl53x sensorA;
    vl53x sensorB;

    uint8_t sensorA_address = 0x69;
    uint8_t sensorB_address = 0x29;

    int setupSensors();
    // Private member variables

};

#endif // DISTANCESENSOR_HPP