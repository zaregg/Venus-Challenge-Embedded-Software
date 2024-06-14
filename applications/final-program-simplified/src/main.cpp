#include <iostream>
#include <boost/lockfree/queue.hpp>
#include "sensors/Sensor.hpp"
#include "motor/Motor.hpp"

#include "utils/RobotParams.hpp"

int main() {
    pynq_init();
    // Create shared parameters
    // Motor parameters
    std::atomic<bool> motorRunning  { false };      // Flag indicating if the motor is running
    std::atomic<bool> stopSignal    { false };      // Flag indicating if the motor should stop
    std::mutex mtx;                                 // Mutex for motor synchronization
    std::condition_variable cv;                     // Condition variable for motor synchronization

    // Sensor parameters
    std::atomic<bool> sensorRunning { false };      // Flag indicating if the sensor is running

    ThreadParams params {  MotorParams { motorRunning, stopSignal, mtx, cv }, SensorParams { sensorRunning } };

    // Create sensor and motor objects with shared parameters
    Sensor sensor(params);
    Motor motor(params);

    // Start threads
    motor.start();
    sensor.start();


    // Wait for threads to finish
    motor.join();
    sensor.join();

    std::cout << "Threads finished execution." << std::endl;
    pynq_destroy();
    return EXIT_SUCCESS;
}
