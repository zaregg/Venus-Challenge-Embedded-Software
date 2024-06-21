#include "Motor.hpp"

Motor::Motor(ThreadParams& params) 
    : params_(params), motorController_(params.motorParams), currentCoords_({0, 0, 0}) {
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
        
        motorController_.enable();

        motorThread_ = std::thread(&Motor::motorThread, this);
    }
}

void Motor::stop() {
    // Stop motor thread
    if (params_.motorParams.motorRunning.load(std::memory_order_acquire)) {
        params_.motorParams.motorRunning.store(false, std::memory_order_release);
        params_.motorParams.stopSignal.store(true, std::memory_order_release);
        params_.motorParams.cv.notify_all();
        // FIXME Maybe wrong
    }
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
    std::this_thread::sleep_for(std::chrono::seconds(5));

    while (true) {
        // Check if stop signal is received
        if (params_.motorParams.stopSignal.load(std::memory_order_acquire)) {
            std::cout << "Motor thread stopping." << std::endl;
            std::cout << "Stop Signal: " << params_.motorParams.stopSignal.load(std::memory_order_acquire) << std::endl;
            // Wait until further instructions
            std::unique_lock<std::mutex> lock(params_.motorParams.mtx);
            params_.motorParams.cv.wait(lock, [&]() {
                std::cout << "Waiting condition: " << !params_.motorParams.stopSignal.load(std::memory_order_acquire) << std::endl;

                return !params_.motorParams.stopSignal.load(std::memory_order_acquire);
            });
            // TODO It should wait for some information from the comunication thread when stopping
            // motorController_.turn(rand() % 231 + 40);
            int dis1 = 0;
            int dis2 = 0;
            bool ir1 = 0;
            bool ir2 = 0;
            SensorData sensorData;
            if (params_.sensorQueue.pop(sensorData)) {
                std::cout << "Sensor Data: " << sensorData.irData.ir1 << std::endl;
                std::cout << "Sensor Data: " << sensorData.irData.ir2 << std::endl;

                ir1 = static_cast<bool>(sensorData.irData.ir1);
                ir2 = static_cast<bool>(sensorData.irData.ir2);
                dis1 = sensorData.distanceData.distance1;
                dis2 = sensorData.distanceData.distance2;

                params_.motorToComQueue.push(MotorToComData("Block", currentCoords_, sensorData));

            }
            else {
                std::cout << "No sensor data." << std::endl;
            }

            int direction = rand() % 2 == 0 ? 1 : -1;
            int angle = direction * (rand() % 141 + 40);
            motorController_.turn(angle);
            currentCoords_.theta += angle;
            if (currentCoords_.theta < 0) {
                currentCoords_.theta += 360;
            }
            else if (currentCoords_.theta >= 360) {
                currentCoords_.theta -= 360;
            }

            // if (degrees != angle) {
            //     if (ir1 && ir2) {
            //         direction = rand() % 2 == 0 ? 1 : -1;
            //     }
            //     else if (ir1) {
            //         direction = -1;
            //     }
            //     else if (ir2) {
            //         direction = 1;
            //     }
            //     angle = direction * (rand() % 141 + 40);
            //     motorController_.turn(angle);
            //     currentCoords_.theta += angle;
            //     if (currentCoords_.theta < 0) {
            //         currentCoords_.theta += 360;
            //     }
            //     else if (currentCoords_.theta >= 360) {
            //         currentCoords_.theta -= 360;
            //     }
            //     params_.motorToComQueue.push(MotorToComData("Robot", currentCoords_));

            // }

            std::cout << "Current Coords: " << currentCoords_.x << " - " << currentCoords_.y << " - " << currentCoords_.theta << std::endl;

        }
        // Check if IR signal is received
        if (params_.motorParams.irSignal.load(std::memory_order_acquire)) {
            std::cout << "Motor thread stopping." << std::endl;
            std::cout << "IR Signal: " << params_.motorParams.irSignal.load(std::memory_order_acquire) << std::endl;
            // Wait until further instructions
            std::unique_lock<std::mutex> lock(params_.motorParams.mtx);
            params_.motorParams.cv.wait(lock, [&]() {
                std::cout << "Waiting condition: " << !params_.motorParams.irSignal.load(std::memory_order_acquire) << std::endl;
                // motorController_.turn(180);

                SensorData sensorData;
                bool ir1 = false;
                bool ir2 = false;
                if (params_.sensorQueue.pop(sensorData)) {
                    std::cout << "Sensor Data: " << sensorData.irData.ir1 << std::endl;
                    std::cout << "Sensor Data: " << sensorData.irData.ir2 << std::endl;

                    ir1 = static_cast<bool>(sensorData.irData.ir1);
                    ir2 = static_cast<bool>(sensorData.irData.ir2);

                }
                else {
                    std::cout << "No sensor data." << std::endl;
                    params_.motorParams.irSignal.store(false, std::memory_order_release);
                    params_.motorParams.cv.notify_all();
                    return !params_.motorParams.irSignal.load(std::memory_order_acquire);
                }
                params_.motorToComQueue.push(MotorToComData("Cliff", currentCoords_));
                int direction = 1;
                if (ir1 && ir2) {
                    direction = rand() % 2 == 0 ? 1 : -1;
                }
                else if (ir1) {
                    direction = -1;
                }
                else if (ir2) {
                    direction = 1;
                }
                int angle = direction * (rand() % 141 + 40);
                motorController_.turn(angle);
                currentCoords_.theta += angle;
                if (currentCoords_.theta < 0) {
                currentCoords_.theta += 360;
                }
                else if (currentCoords_.theta >= 360) {
                    currentCoords_.theta -= 360;
                }

                params_.motorToComQueue.push(MotorToComData("Robot", currentCoords_));
                // if (degrees != angle) {
                //     // if (degrees < 5)
                //     // {
                //     //     // for (int i = 0; i < 5; i++)
                //     //     // {
                //     //     //     motorController_.forward(-i);
                //     //     // }
                //     //     continue;
                //     // }
                //     // std::cout << "params_.motorParams.irSignal: " << params_.motorParams.irSignal.load(std::memory_order_acquire) << std::endl;
                //     // continue;
                //     params_.motorParams.irSignal.store(false, std::memory_order_release);
                //     params_.motorParams.cv.notify_all();
                //     return !params_.motorParams.irSignal.load(std::memory_order_acquire);
                // }

                std::cout << "Current Coords: " << currentCoords_.x << " - " << currentCoords_.y << " - " << currentCoords_.theta << std::endl;
                
                params_.motorParams.irSignal.store(false, std::memory_order_release);
                params_.motorParams.cv.notify_all();
                // std::cout << "params_.motorParams.irSignal: " << params_.motorParams.irSignal.load(std::memory_order_acquire) << std::endl;
                return !params_.motorParams.irSignal.load(std::memory_order_acquire);
            });
            // TODO It should wait for some information from the comunication thread when stopping
        }   
        // std::cout << "Motor thread running." << std::endl;
        motorController_.forward(1); // cm
        float x = currentCoords_.x;
        float y = currentCoords_.y;
        int theta = currentCoords_.theta;

        float x_new = std::cos(motorController_.degreesToRads(theta)) + x; // cos(theta)
        float y_new = std::sin(motorController_.degreesToRads(theta)) + y; // sin(theta)

        // std::cout << "Current Coords: " << x_new << " - " << y_new << " - " << theta << std::endl;
        currentCoords_.x = x_new;
        currentCoords_.y = y_new;

        // MotorToComData motorToComData;
        // motorToComData.type = "robot";
        // motorToComData.coordinate = currentCoords_;

        params_.motorToComQueue.push(MotorToComData("Robot", currentCoords_));
        
        // motorController_.scan(30);
        // std::this_thread::sleep_for(std::chrono::milliseconds(100));
        // Perform motor operations here
        
    }
}
