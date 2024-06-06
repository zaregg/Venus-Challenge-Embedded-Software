#ifndef DISTANCE_SENSOR_HPP
#define DISTANCE_SENSOR_HPP

// Include any necessary headers here
#include "Sensor.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <boost/lockfree/queue.hpp> // Include Boost.Lockfree
#include "../../utils/structs.hpp"

class DistanceSensor : public Sensor<float> {
    public:
        // Constructor
        DistanceSensor();

        // Destructor
        ~DistanceSensor() override;

        void start(std::thread& thread, RobotQueue* managerToSensorQueue, RobotQueue* sensorToManagerQueue) override;
        void stop() override;

    private:
        std::atomic<bool> running_;
        std::thread thread_;

        RobotQueue* managerToSensorQueue_;
        RobotQueue* sensorToManagerQueue_;

        S_DistanceSensorTest* testData();
        

        void readData();
        // Add any necessary private variables here
};
#endif // DISTANCE_SENSOR_HPP