#pragma once
#ifndef SENSOR_MANAGER_HPP
#define SENSOR_MANAGER_HPP


#include <thread>
#include <vector>
#include <boost/lockfree/queue.hpp> // Include Boost.Lockfree


class SensorManager {
public:
    SensorManager(boost::lockfree::queue<std::function<void()>*> &comToSensorQueue, boost::lockfree::queue<std::function<void()>*> &sensorToComQueue);

    ~SensorManager();

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



private:
    std::vector<std::thread> sensorThreads;
    boost::lockfree::queue<std::function<void()>*>& comToSensorTasks;
    boost::lockfree::queue<std::function<void()>*>& sensorToComTasks;
    std::vector<bool> sensorThreadsRunning;
    // Private member variables

    void sensorThreadFunction(std::function<void()> *task);

};

#endif // SENSOR_MANAGER_HPP