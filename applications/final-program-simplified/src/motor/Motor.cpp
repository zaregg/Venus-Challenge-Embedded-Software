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
    // std::this_thread::sleep_for(std::chrono::seconds(5));
    // for (int i = 0; i < 50; i++)
    // {
    //     motorController_.forward(1); // cm
    //     float x = currentCoords_.x;
    //     float y = currentCoords_.y;
    //     int theta = currentCoords_.theta;

    //     float x_new = std::cos(motorController_.degreesToRads(theta)) + x; // cos(theta)
    //     float y_new = std::sin(motorController_.degreesToRads(theta)) + y; // sin(theta)

    //     std::cout << "Current Coords: " << x_new << " - " << y_new << " - " << theta << std::endl;
    //     currentCoords_.x = x_new;
    //     currentCoords_.y = y_new;


    //     params_.motorToComQueue.push(MotorToComData("robot", currentCoords_));
    // }
    // motorController_.turn(90);
    // currentCoords_.theta += 90;
    // if (currentCoords_.theta < 0) {
    //     currentCoords_.theta += 360;
    // }
    // else if (currentCoords_.theta >= 360) {
    //     currentCoords_.theta -= 360;
    // }

    // for (int i = 0; i < 50; i++)
    // {
    //     motorController_.forward(1); // cm
    //     float x = currentCoords_.x;
    //     float y = currentCoords_.y;
    //     int theta = currentCoords_.theta;

    //     float x_new = std::cos(motorController_.degreesToRads(theta)) + x; // cos(theta)
    //     float y_new = std::sin(motorController_.degreesToRads(theta)) + y; // sin(theta)

    //     std::cout << "Current Coords: " << x_new << " - " << y_new << " - " << theta << std::endl;
    //     currentCoords_.x = x_new;
    //     currentCoords_.y = y_new;

    //     params_.motorToComQueue.push(MotorToComData("robot", currentCoords_));
    // }  
    // motorController_.turn(90);
    // currentCoords_.theta += 90;
    // if (currentCoords_.theta < 0) {
    //     currentCoords_.theta += 360;
    // }
    // else if (currentCoords_.theta >= 360) {
    //     currentCoords_.theta -= 360;
    // }
    // for (int i = 0; i < 50; i++)
    // {
    //     motorController_.forward(1); // cm
    //     float x = currentCoords_.x;
    //     float y = currentCoords_.y;
    //     int theta = currentCoords_.theta;

    //     float x_new = std::cos(motorController_.degreesToRads(theta)) + x; // cos(theta)
    //     float y_new = std::sin(motorController_.degreesToRads(theta)) + y; // sin(theta)

    //     std::cout << "Current Coords: " << x_new << " - " << y_new << " - " << theta << std::endl;
    //     currentCoords_.x = x_new;
    //     currentCoords_.y = y_new;


    //     params_.motorToComQueue.push(MotorToComData("robot", currentCoords_));
    // }
    // motorController_.turn(180);
    // currentCoords_.theta += 180;
    // if (currentCoords_.theta < 0) {
    //     currentCoords_.theta += 360;
    // }
    // else if (currentCoords_.theta >= 360) {
    //     currentCoords_.theta -= 360;
    // }
    // for (int i = 0; i < 20; i++)
    // {
    //     motorController_.forward(1); // cm
    //     float x = currentCoords_.x;
    //     float y = currentCoords_.y;
    //     int theta = currentCoords_.theta;

    //     float x_new = std::cos(motorController_.degreesToRads(theta)) + x; // cos(theta)
    //     float y_new = std::sin(motorController_.degreesToRads(theta)) + y; // sin(theta)

    //     std::cout << "Current Coords: " << x_new << " - " << y_new << " - " << theta << std::endl;
    //     currentCoords_.x = x_new;
    //     currentCoords_.y = y_new;

    //     params_.motorToComQueue.push(MotorToComData("robot", currentCoords_));
    // }
    // motorController_.turn(45);
    // currentCoords_.theta += 45;
    // if (currentCoords_.theta < 0) {
    //     currentCoords_.theta += 360;
    // }
    // else if (currentCoords_.theta >= 360) {
    //     currentCoords_.theta -= 360;
    // }
    // for (int i = 0; i < 50; i++)
    // {
    //     motorController_.forward(1); // cm
    //     float x = currentCoords_.x;
    //     float y = currentCoords_.y;
    //     int theta = currentCoords_.theta;

    //     float x_new = std::cos(motorController_.degreesToRads(theta)) + x; // cos(theta)
    //     float y_new = std::sin(motorController_.degreesToRads(theta)) + y; // sin(theta)

    //     std::cout << "Current Coords: " << x_new << " - " << y_new << " - " << theta << std::endl;
    //     currentCoords_.x = x_new;
    //     currentCoords_.y = y_new;

    //     params_.motorToComQueue.push(MotorToComData("robot", currentCoords_));
    // }

// }

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
            // motorController_.turn(rand() % 231 + 40);
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

            std::cout << "Current Coords: " << currentCoords_.x << " - " << currentCoords_.y << " - " << currentCoords_.theta << std::endl;

        }
        // Check if IR signal is received
        if (params_.motorParams.irSignal.load(std::memory_order_acquire)) {
            std::cout << "Motor thread stopping." << std::endl;
            std::cout << "IR Signal: " << params_.motorParams.irSignal.load(std::memory_order_acquire) << std::endl;
            // Wait until further instructions
            std::unique_lock<std::mutex> lock(params_.motorParams.mtx);
            params_.motorParams.cv.wait(lock, [&]() {
                // std::cout << "Waiting condition: " << !params_.motorParams.irSignal.load(std::memory_order_acquire) << std::endl;
                // motorController_.turn(180);

                SensorData sensorData;
                if (params_.sensorQueue.pop(sensorData)) {
                    std::cout << "Sensor Data: " << sensorData.irData.ir1 << std::endl;
                    std::cout << "Sensor Data: " << sensorData.irData.ir2 << std::endl;

                    params_.motorToComQueue.push(MotorToComData("Cliff", currentCoords_, sensorData));

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

                std::cout << "Current Coords: " << currentCoords_.x << " - " << currentCoords_.y << " - " << currentCoords_.theta << std::endl;
                
                params_.motorParams.irSignal.store(false, std::memory_order_release);
                params_.motorParams.cv.notify_all();
                std::cout << "params_.motorParams.irSignal: " << params_.motorParams.irSignal.load(std::memory_order_acquire) << std::endl;
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

        params_.motorToComQueue.push(MotorToComData("robot", currentCoords_));
        
        // motorController_.scan(30);
        // std::this_thread::sleep_for(std::chrono::milliseconds(100));
        // Perform motor operations here
        
    }
}
