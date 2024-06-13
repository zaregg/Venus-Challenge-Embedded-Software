#include "Motor.hpp"

Motor::Motor(ThreadParams& params) 
    : params_(params), motorController_(params.motorParams) {
    // Initialize motor
}

Motor::~Motor() {
    // Cleanup motor
    if (params_.motorParams.motorRunning.load(std::memory_order_acquire)) {
        // Notify motor thread to stop
        params_.motorParams.motorRunning.store(false, std::memory_order_release);
        stop();
        join();
    }
}

void Motor::start() {
    // Start motor thread
    if(!params_.motorParams.motorRunning.load(std::memory_order_acquire)) {
        params_.motorParams.motorRunning.store(true, std::memory_order_release);
        

        motorThread_ = std::thread(&Motor::motorThread, this);
    }
}

void Motor::stop() {
    // Stop motor thread
}

void Motor::join()
{
    // Join motor thread
    if (motorThread_.joinable()) {
        motorThread_.join();
    }
}

void Motor::setSpeed(int speed) {
    // Set motor speed
}

void Motor::motorThread()
{
    while (true) {
        // Check if stop signal is received
        if (params_.motorParams.stopSignal.load(std::memory_order_acquire)) {
            std::cout << "Motor thread stopping." << std::endl;
            std::cout << "Stop Signal: " << params_.motorParams.stopSignal.load(std::memory_order_acquire) << std::endl;
            // Wait until further instructions
            std::unique_lock<std::mutex> lock(params_.motorParams.mtx);
            params_.motorParams.cv.wait(lock, [&]() {
                // std::cout << "Waiting condition: " << !params_.motorParams.stopSignal.load(std::memory_order_acquire) << std::endl;
                
                return !params_.motorParams.stopSignal.load(std::memory_order_acquire);
            });
            // TODO It should wait for some information from the comunication thread when stopping
        }
        std::cout << "Motor thread running." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        // Perform motor operations here
        
    }
}
