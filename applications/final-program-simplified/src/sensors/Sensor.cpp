#include "Sensor.hpp"

Sensor::Sensor(ThreadParams& params) 
    : params_(params) { // Initialize colourSensor_
    // Initialize sensor
}

Sensor::~Sensor() {
    // Cleanup sensor
}

void Sensor::start() {
    // Start sensor thread
    if (!params_.sensorParams.sensorRunning.load(std::memory_order_acquire)) {
        params_.sensorParams.sensorRunning.store(true, std::memory_order_release);

        // Setup Sensors
        int i = distanceSensor_.setup();
        while (i != 0){
            i = distanceSensor_.setup();
        }
        colourSensor_.setup();

        sensorThread_ = std::thread(&Sensor::sensorThread, this);
    }
}

void Sensor::stop() {
    // Stop sensor thread
    if (params_.sensorParams.sensorRunning.load(std::memory_order_acquire)) {
        // Notify sensor thread to stop
        params_.sensorParams.sensorRunning.store(false, std::memory_order_release);
    }
}

void Sensor::join() {
    // Join sensor thread
    if (sensorThread_.joinable()) {
        sensorThread_.join();
    }
}

void Sensor::sensorThread()
{
    std::cout << "Sensor thread started." << std::endl;
    int i = 0;
    while(params_.sensorParams.sensorRunning.load(std::memory_order_acquire)) {
        // Perform sensor operations here
        // std::cout << "Sensor thread running." << std::endl;
        // std::cout << "i: " << i << std::endl;
        // if (i == 5) {
        //     params_.motorParams.stopSignal.store(true, std::memory_order_release);
        // }
        // if (i == 10) {
        //     params_.motorParams.stopSignal.store(false, std::memory_order_release);
        //     params_.motorParams.cv.notify_one();
        // }
        // i++;
        SensorData sensorData;
        sensorData.distanceData = distanceSensor_.getDistance();
        sensorData.irData = irSensor_.getIRData();
        std::cout << "DistanceA: " << sensorData.distanceData.distance1 << std::endl;
        std::cout << "DistanceB: " << sensorData.distanceData.distance2 << std::endl;
        std::cout << "IR1: " << sensorData.irData.ir1 << std::endl;
        std::cout << "IR2: " << sensorData.irData.ir2 << std::endl;

        if ((sensorData.distanceData.distance1 < 30 || sensorData.distanceData.distance2 < 30) && !params_.motorParams.stopSignal.load(std::memory_order_acquire)) {
            params_.motorParams.stopSignal.store(true, std::memory_order_release);
            sensorData.colourData = colourSensor_.requestCapture();
            std::cout << "Colour: " << sensorData.colourData.colour << std::endl;
        }
        // else {
        //     params_.motorParams.stopSignal.store(false, std::memory_order_release);
        //     params_.motorParams.cv.notify_one();
        // }

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}
