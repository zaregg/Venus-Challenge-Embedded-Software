#ifndef IRSENSOR_HPP
#define IRSENSOR_HPP

// Include any necessary libraries or headers
#include "Sensor.hpp"

// Define the class for the IR sensor
class IRSensor : Sensor<float>{
public:
    // Constructor
    IRSensor();

    // Destructor
    ~IRSensor();

    // Add any necessary member functions or variables

private:
    // Add any necessary private member functions or variables
};

#endif // IRSENSOR_HPP