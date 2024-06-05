#pragma once
#ifndef SENSOR_MANAGER_HPP
#define SENSOR_MANAGER_HPP


#include <thread>
#include <vector>
#include <boost/lockfree/queue.hpp> // Include Boost.Lockfree
#include <iostream>


class SensorManager {
public:
    SensorManager(boost::lockfree::queue<std::function<void()>*> &comToSensorQueue, boost::lockfree::queue<std::function<void()>*> &sensorToComQueue);

    ~SensorManager();

    void start();
    void stop();

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
    void joinThreads();

    void amountOfSensors();


private:
    std::vector<std::thread> sensorThreads;
    std::vector<boost::lockfree::queue<std::function<void()>*>> ManagerToSensorQueues;
    std::vector<boost::lockfree::queue<std::function<void()>*>> SensorToManagerQueues;
    boost::lockfree::queue<std::function<void()>*>& comToManagerQueue;
    boost::lockfree::queue<std::function<void()>*>& ManagerToComQueue;
    std::vector<bool> sensorThreadsRunning;
    // Private member variables

    void processSensorData();

};

#endif // SENSOR_MANAGER_HPP