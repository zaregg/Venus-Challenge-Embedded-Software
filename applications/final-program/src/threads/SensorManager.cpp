#include "sensor_manager.hpp"
#include <functional>

SensorManager::SensorManager(boost::lockfree::queue<std::function<void()>*> &comToSensorQueue, boost::lockfree::queue<std::function<void()>*> &sensorToComQueue)
    : comToSensorQueue(comToSensorQueue), sensorToComQueue(sensorToComQueue)
{
    std::cout << "SensorManager created" << std::endl;
}

SensorManager::~SensorManager()
{
    std::cout << "SensorManager destroyed" << std::endl;
}

template <typename SensorType>
void SensorManager::addSensor(SensorType &sensor)
{
    std::function<void()> *sensorFunction = new std::function<void()>(std::bind(&SensorType::run, &sensor));
    sensorThreads.push_back(std::thread([sensorFunction, this]() {
        while (true)
        {
            if (sensorFunction != nullptr)
            {
                (*sensorFunction)();
            }
        }
    }));
    sensorThreadsRunning.push_back(true);
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