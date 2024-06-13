#include <iostream>
#include <boost/lockfree/queue.hpp>
#include "sensors/Sensor.hpp"
#include "motor/Motor.hpp"

#include "utils/RobotParams.hpp"

int main() {
    // Create shared parameters
    std::atomic<bool> motor_running(true);
    std::mutex mtx;
    std::condition_variable cv;

    ThreadParams params {  MotorParams { motor_running, mtx, cv } };

    // Create sensor and motor objects with shared parameters
    Sensor sensor(params);
    Motor motor(params);

    // Start threads
    sensor.start();
    motor.start();

    // Wait for threads to finish
    sensor.join();
    motor.join();

    std::cout << "Threads finished execution." << std::endl;
    return 0;
}
