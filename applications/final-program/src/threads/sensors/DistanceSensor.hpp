#ifndef DISTANCE_SENSOR_HPP
#define DISTANCE_SENSOR_HPP

// Include any necessary headers here
#include "Sensor.hpp"
#include <iostream>
#include <chrono>
#include <thread>

class DistanceSensor : public Sensor<float> {
    public:
        // Constructor
        DistanceSensor();

        // Destructor
        ~DistanceSensor();

        void start(boost::lockfree::queue<std::function<void()>*>* managerToSensorQueue,
                   boost::lockfree::queue<std::function<void()>*>* sensorToManagerQueue) override;
        void stop() override;

    private:
        bool running_ = false;
        std::thread thread_;

        void readData();
        // Add any necessary private variables here
};
#endif // DISTANCE_SENSOR_HPP