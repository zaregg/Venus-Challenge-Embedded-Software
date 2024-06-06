#include "ColorSensor.hpp"

ColorSensor::ColorSensor()
{
    std::cout << "Color Sensor created" << std::endl;
}

ColorSensor::~ColorSensor()
{
}

void ColorSensor::start(std::thread &thread, RobotQueue *managerToSensorQueue, RobotQueue *sensorToManagerQueue)
{
    if (!running_.load(std::memory_order_relaxed))
    {
        running_.store(true, std::memory_order_relaxed);
        managerToSensorQueue_ = managerToSensorQueue;
        sensorToManagerQueue_ = sensorToManagerQueue;

        worker_ = std::thread(&ColorSensor::workerThread, this);
        thread = std::move(worker_); // Move the thread to the parameter
    }
}

void ColorSensor::stop()
{
    if (running_.load(std::memory_order_relaxed))
    {
        running_.store(false, std::memory_order_relaxed);
        stop_.store(true, std::memory_order_relaxed);
        if (worker_.joinable())
        {
            worker_.join();
        }
    }
}

void ColorSensor::requestCapture()
{
    std::lock_guard<std::mutex> lk(cv_m);
    ready_.store(true, std::memory_order_relaxed);
    cv_.notify_one();
}

void ColorSensor::workerThread()
{
    while (true)
    {
        std::unique_lock<std::mutex> lk(cv_m);
        cv_.wait(lk, [this] { return ready_.load(); });

        if(stop_.load())
        {
            break;
        }

        // Capture image
        // send_capture_signal();
        // receive_from_python();

        // Simulate reading data
        s_ColorSensorTest *data = new s_ColorSensorTest();
        data->colour = 1;
        
        if (!sensorToManagerQueue_->push(data))
        {
            std::cerr << "Failed to push to queue" << std::endl;
            delete data; // Clean up if push fails
        }

        ready_.store(false, std::memory_order_relaxed);
    }
    
}

void ColorSensor::receive_from_python()
{
}
