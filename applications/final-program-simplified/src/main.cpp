#include <iostream>
#include <boost/lockfree/queue.hpp>
#include "sensors/Sensor.hpp"
#include "motor/Motor.hpp"
#include "communication/SerialClient.hpp"

#include "utils/RobotParams.hpp"

int main() {
    pynq_init();
    // Create shared parameters
    // Motor parameters
    std::atomic<bool> motorEnabled   { false };      // Flag indicating if the motor is enabled
    std::atomic<bool> motorRunning  { false };      // Flag indicating if the motor is running
    std::atomic<bool> stopSignal    { false };      // Flag indicating if the motor should stop
    std::atomic<bool> irSignal      { false };      // Flag indicating if the motor should stop
    std::mutex mtx;                                 // Mutex for motor synchronization
    std::condition_variable cv;                     // Condition variable for motor synchronization

    // Sensor parameters
    std::atomic<bool> sensorRunning { false };      // Flag indicating if the sensor is running

    // Communication parameters
    std::atomic<bool> comRunning    { false };      // Flag indicating if the communication is running

    ThreadParams params {  MotorParams { motorEnabled, motorRunning, stopSignal, irSignal, mtx, cv }, SensorParams { sensorRunning }, ComParams{comRunning} };

    // Create sensor and motor objects with shared parameters
    Sensor sensor(params);
    Motor motor(params);
    SerialClient com(params);

    // Start threads
    sensor.start();

    while (!sensorRunning) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    com.start();

    while (!comRunning) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    motor.start();


    // Wait for threads to finish
    motor.join();
    sensor.join();
    com.join();

    std::cout << "Threads finished execution." << std::endl;
    pynq_destroy();
    return EXIT_SUCCESS;
}
