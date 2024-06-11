#include "SensorManager.hpp"
#include <functional>

SensorManager::SensorManager(SensorManagerQueue &comToSensorQueue, SensorManagerQueue &sensorToComQueue)
    : comToManagerQueue(comToSensorQueue), ManagerToComQueue(sensorToComQueue), running_(false)
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
    running_.store(true, std::memory_order_relaxed);
}

void SensorManager::stop()
{
    running_.store(false, std::memory_order_relaxed);
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
    std::unique_lock<std::mutex> lock(vectorMutex);
    cv.wait(lock, [this]{ return !processing; });
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
    std::unique_lock<std::mutex> lock(vectorMutex);
    std::cout << "Amount of sensors: " << sensorThreads.size() << std::endl;
}

void SensorManager::processSensorData()
{
    std::cout << "Processing sensor data..." << std::endl;

    while (running_.load(std::memory_order_relaxed))
    {
        std::unique_lock<std::mutex> lock(vectorMutex);
        processing = true;

        // Check if SensorToManagerQueues and sensorThreadsRunning have the same size
        CombinedSensorData* combinedData = new CombinedSensorData;
        combinedData->distanceSensorData = nullptr;
        combinedData->colorSensorData = nullptr;
        combinedData->irSensorData = nullptr;

        if (SensorToManagerQueues.size() != sensorThreadsRunning.size()) {
            std::cerr << "Error: Size mismatch between SensorToManagerQueues and sensorThreadsRunning" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            continue;
        }

        for (size_t i = 0; i < SensorToManagerQueues.size(); ++i)
        {
            if (i < 0 || i >= SensorToManagerQueues.size()) {
                std::cerr << "Error: Index " << i << " is out of bounds" << std::endl;
                break;
            }
            if (i >= sensorThreadsRunning.size()) {
                std::cerr << "Error: index " << i << " is out of bounds for sensorThreadsRunning" << std::endl;
                continue;
            }
            // Check if sensorThreadsRunning[i] is true
            if (!sensorThreadsRunning[i]) {
                if (running_.load(std::memory_order_relaxed)) {
                    break;
                }
                std::cerr << "Error: sensorThreadsRunning[" << i << "] is false" << std::endl;
                continue;
            }

            Robot *item = nullptr;

            if (sensorThreadsRunning[i])
            {
                // std::cout << "Processing sensor " << i << std::endl;
                // Check if SensorToManagerQueues[i] is a valid pointer
                if (!SensorToManagerQueues[i]) {
                    std::cerr << "Error: SensorToManagerQueues[" << i << "] is null" << std::endl;
                    break;
                }

                while(SensorToManagerQueues[i]->pop(item))
                {

                    if (!item) {
                        std::cerr << "Error: Popped null item from SensorToManagerQueues[" << i << "]" << std::endl;
                        continue;
                    }


                    // Process the sensor data
                    std::cout << "Received sensor data from sensor " << i << std::endl;
                    if (s_DistanceSensorTest* s1 = dynamic_cast<s_DistanceSensorTest*>(item))
                    {
                        combinedData->distanceSensorData = s1;
                    }
                    else if (s_ColorSensorTest* s2 = dynamic_cast<s_ColorSensorTest*>(item)) {
                        // Add the sensor data to the combinedData struct
                        combinedData->colorSensorData = s2;
                    }
                    else if (s_IRSensorTest* s3 = dynamic_cast<s_IRSensorTest*>(item)) {
                        // Add the sensor data to the combinedData struct
                        combinedData->irSensorData = s3;
                    }
                    else {
                        std::cout << "Received unknown sensor data" << std::endl;
                    }

                    // delete item;
                    item = nullptr;
                }
            }
        }

        // Send the combined sensor data to the CommunicationManager
        if (!ManagerToComQueue.push(combinedData))
        {
            std::cerr << "Failed to push to ManagerToComQueue" << std::endl;
            delete combinedData;
        }
        processing = false;
        cv.notify_all();
        lock.unlock();
    }
}