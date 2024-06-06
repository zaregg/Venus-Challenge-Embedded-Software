#pragma once
#ifndef SENSOR_MANAGER_HPP
#define SENSOR_MANAGER_HPP


#include <thread>
#include <vector>
#include <boost/lockfree/queue.hpp> // Include Boost.Lockfree
#include <iostream>
#include "sensors/Sensor.hpp"
#include "../utils/structs.hpp"

using QueueType = boost::lockfree::queue<Robot*>;

class SensorManager {
public:
    SensorManager(QueueType &comToSensorQueue, QueueType &sensorToComQueue);

    ~SensorManager();

    void start();
    void stop();
    void join();

    /**
     * Adds a sensor to the sensor manager.
     * 
     * @tparam SensorType The type of sensor to add.
     * @param sensor The sensor object to add.
     * @note The SensorType must have a start() function.
     */
    template <typename SensorType>
    void addSensor(const SensorType& sensor);

    void stopSensorThreads();

    template <typename SensorType>
    void stopSensorThread(const SensorType& sensor) {
        sensor.stop();
    }

    void joinSensors();

    void amountOfSensors();


private:
    // using QueueType = boost::lockfree::queue<Robot*>;
    using QueueTypePtr = std::unique_ptr<QueueType>;

    QueueType& comToManagerQueue;
    QueueType& ManagerToComQueue;

    std::vector<QueueTypePtr> ManagerToSensorQueues;
    std::vector<QueueTypePtr> SensorToManagerQueues;

    std::vector<std::thread> sensorThreads;
    std::vector<bool> sensorThreadsRunning;

    std::thread managerThread;
    bool running_;

    // std::vector<std::function<void()>> sensorFunctions;
    std::vector<SensorBase*> sensors_;


    void processSensorData();

};

template <typename SensorType>
void SensorManager::addSensor(const SensorType &sensor) {
    try {
        auto managerToSensorQueue = std::make_unique<QueueType>(128);
        auto sensorToManagerQueue = std::make_unique<QueueType>(128);

        // Add the queues to the lists of queues
        ManagerToSensorQueues.push_back(std::move(managerToSensorQueue));
        SensorToManagerQueues.push_back(std::move(sensorToManagerQueue));

        // Here, we assume that `sensor` has a `run` method that accepts two `QueueType*` parameters
        auto& mtSensorQueue = ManagerToSensorQueues.back();
        auto& stManagerQueue = SensorToManagerQueues.back();

        std::thread thread;

        sensor->start(thread, mtSensorQueue.get(), stManagerQueue.get());


        sensorThreads.push_back(std::move(thread));

        sensors_.push_back(sensor);

        // If you want to manage threads, uncomment and modify the code below:
        // std::thread sensorThread(&SensorType::run, &sensor, mtSensorQueue.get(), stManagerQueue.get());
        // sensorThreads.push_back(std::move(sensorThread));

        // Set the flag to indicate that the sensor thread is running
        sensorThreadsRunning.push_back(true);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

#endif // SENSOR_MANAGER_HPP