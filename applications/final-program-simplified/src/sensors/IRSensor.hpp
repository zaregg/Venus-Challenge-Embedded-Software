#ifndef IRSENSOR_HPP
#define IRSENSOR_HPP

#include "RobotParams.hpp"
#include <iostream>

extern "C" {
    #include <libpynq.h>

}

class IRSensor {
public:
    // Constructor
    IRSensor();

    // Destructor
    ~IRSensor();

    IRData getIRData();

    // Function to initialize the sensor


private:
    // Private member variables

};

#endif // IRSENSOR_HPP