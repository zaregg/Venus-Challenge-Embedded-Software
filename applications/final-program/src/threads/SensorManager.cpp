#include "SensorManager.hpp"
#include <functional>

SensorManager::SensorManager(QueueType &comToSensorQueue, QueueType &sensorToComQueue)
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
    managerThread = std::thread(&SensorManager::processSensorData, this);
    running_ = true;
}

void SensorManager::stop()
{
    running_ = false;
    stopSensorThreads();
    joinSensors();
    if (managerThread.joinable())
    {
        managerThread.join();
    }

}

void SensorManager::join()
{
    if (managerThread.joinable())
    {
        managerThread.join();
    }
}



void SensorManager::stopSensorThreads()
{
    // Set flag to signal sensor threads to stop
    for (size_t i = 0; i < sensorThreadsRunning.size(); ++i)
    {
        sensorThreadsRunning[i] = false;
        sensors_[i]->stop();
    }
}


void SensorManager::joinSensors() 
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
    std::cout << "Processing sensor data..." << std::endl;
    while (running_)
    {
    
        for (size_t i = 0; i < SensorToManagerQueues.size(); ++i)
        {
            std::cout << "Processing sensor " << i << std::endl;
            Robot* item;
            while (sensorThreadsRunning[i])
            {
                while(SensorToManagerQueues[i]->pop(item))
                {
                    std::cout << "Received sensor data from sensor " << i << std::endl;
                    if (s_DistanceSensorTest* s1 = dynamic_cast<s_DistanceSensorTest*>(item))
                    {
                        std::cout << "Received distance sensor data" << std::endl;
                        std::cout << "Distance: " << s1->distance << std::endl;
                        std::time_t time = std::chrono::system_clock::to_time_t(s1->time);
                        std::cout << "Time: " << std::ctime(&time) << std::endl;
                    }
                    else
                    {
                        std::cout << "Received unknown sensor data" << std::endl;
                    }
                }
            }
        }

    }
}

