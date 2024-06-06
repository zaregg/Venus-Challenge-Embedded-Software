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

