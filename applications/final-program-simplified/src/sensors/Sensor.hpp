#ifndef SENSOR_HPP
#define SENSOR_HPP

#include "RobotParams.hpp"
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
    ThreadParams& params;
    // Add your member variables here

};

#endif // SENSOR_HPP