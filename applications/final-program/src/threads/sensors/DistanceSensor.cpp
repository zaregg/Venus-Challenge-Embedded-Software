#include "DistanceSensor.hpp"

DistanceSensor::DistanceSensor() : running_(false)
{
    std::cout << "DistanceSensor created" << std::endl;
}

DistanceSensor::~DistanceSensor()
{
}

void DistanceSensor::start(std::thread& thread, QueueType* managerToSensorQueue, QueueType* sensorToManagerQueue)
{
    if (!running_.load(std::memory_order_relaxed)) {
        running_.store(true, std::memory_order_relaxed);
        managerToSensorQueue_ = managerToSensorQueue;
        sensorToManagerQueue_ = sensorToManagerQueue;
        
        thread_ = std::thread(&DistanceSensor::readData, this);
        thread = std::move(thread_); // Move the thread to the parameter
    }
}


void DistanceSensor::stop()
{
    if (running_.load(std::memory_order_relaxed)) {
        running_.store(false, std::memory_order_relaxed);
        if (thread_.joinable()) {
            thread_.join();
        }
    }
}

S_DistanceSensorTest* DistanceSensor::testData()
{
    S_DistanceSensorTest* test = new S_DistanceSensorTest();
    test->distance1 = 10;
    test->time = std::chrono::system_clock::now();
    
    return test; // Replace with actual data
}

void DistanceSensor::readData()
{
    while (running_.load(std::memory_order_relaxed)){
        // int distance = readDistance(); // Calls private helper function
        // sendData(distance);
        std::cout << "Reading distance data..." << std::endl;

        // Simulate reading data
        S_DistanceSensorTest* data = testData();
        sensorToManagerQueue_->push(data);

        std::this_thread::sleep_for(std::chrono::seconds(1)); // Simulate reading every second
    }
}

// int DistanceSensor::readDistance() {
//     // Simulated function to read distance data
//     // Replace with actual code to read from sensor
//     static int distance = 0;
//     distance += 10; // Incrementing distance for demonstration
//     return distance;
// }
