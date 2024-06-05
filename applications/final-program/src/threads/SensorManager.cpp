#include "SensorManager.hpp"
#include <functional>

SensorManager::SensorManager(boost::lockfree::queue<std::function<void()>*> &comToSensorQueue, boost::lockfree::queue<std::function<void()>*> &sensorToComQueue)
    : comToManagerQueue(comToSensorQueue), ManagerToComQueue(sensorToComQueue)
{
    std::cout << "SensorManager created" << std::endl;
}

SensorManager::~SensorManager()
{
    std::cout << "SensorManager destroyed" << std::endl;
}

void SensorManager::start()
{

}

void SensorManager::stop()
{

}

template <typename SensorType>
void SensorManager::addSensor(const SensorType &sensor)
{
    // Create the queues for this sensor
    try {
        auto managerToSensorQueue = new boost::lockfree::queue<std::function<void()>*>(128);
        auto sensorToManagerQueue = new boost::lockfree::queue<std::function<void()>*>(128);

        // Add the queues to the lists of queues
        ManagerToSensorQueues.push_back(managerToSensorQueue);
        SensorToManagerQueues.push_back(sensorToManagerQueue);

        // Create a new thread and pass the sensor.run function as the thread function
        // std::thread sensorThread(&SensorType::start, &sensor, managerToSensorQueue, sensorToManagerQueue);

        sensor.run(managerToSensorQueue, sensorToManagerQueue);

        // Add the thread to the list of sensor threads
        // sensorThreads.push_back(std::move(sensorThread)); maybe add it back later

        // Set the flag to indicate that the sensor thread is running
        sensorThreadsRunning.push_back(true);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void SensorManager::stopSensorThreads()
{
    // Set flag to signal sensor threads to stop
    for (size_t i = 0; i < sensorThreadsRunning.size(); ++i)
    {
        sensorThreadsRunning[i] = false;
    }
}


void SensorManager::joinThreads() 
{
    for (size_t i = 0; i < sensorThreads.size(); i++)
    {
        if (sensorThreads[i].joinable())
        {
            sensorThreads[i].join();
            // sensorThreadsRunning[i] = false;
        }
    }
}

void SensorManager::amountOfSensors()
{
    std::cout << "Amount of sensors: " << sensorThreads.size() << std::endl;
}

void SensorManager::processSensorData()
{
    
}

