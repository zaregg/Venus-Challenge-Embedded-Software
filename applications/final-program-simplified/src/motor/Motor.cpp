#include "Motor.hpp"

using namespace std;

Motor::Motor(ThreadParams& params) 
    : state_("SEARCHING"), params_(params), motorController_(params.motorParams), currentCoords_({0, 0, 0}), stuckCounter_(0) {
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

        ds1_    = params_.ds1.load(std::memory_order_acquire);
        ds2_    = params_.ds2.load(std::memory_order_acquire);
        ir1_    = params_.ir1.load(std::memory_order_acquire);
        ir2_    = params_.ir2.load(std::memory_order_acquire);

        cout << "ds1: " << ds1_ << " ds2: " << ds2_ << endl << " ir1: " << ir1_ << " ir2: " << ir2_ << endl;

        if (state_ == "SEARCHING") {
            // State where the robot searches for paths and obstacles
            searchState();
        }
        else if (state_ == "DETECTION"){
            // State where the robot rocks and hills
            detectionState();
        }
        else if (state_ == "AVOIDING") {
            // State where the robot avoids black tape detected by IR sensors
            avoidState();
        }
        // else if (state_ == "REORIENTING") {
        //     // State where the robot reorients its direction
        //     reorientState();
        // }
        else if (state_ == "STUCK") {
            // State where the robot handles being stuck and attempts recovery
            stuckState();
        }
    }
}

void Motor::searchState() {
    // State where the robot searches for paths and obstacles
    cout << "Searching for path and scanning for blocks..." << endl;
    
    // Example: Read actual distance sensor value
    // int distance = distance_sensor_value.load();

    // State transition conditions based on real sensor values
    if (ir1_ || ir2_) {
        // Detected black tape, perform avoidance
        state_ = "AVOIDING";
    } else if (ds1_ < 150) {
        // Detected obstacle (block), perform avoidance
        state_ = "DETECTION";
    } else {
        // Example: Move forward (replace with actual motor control logic)
        motorController_.forward(1);
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
        // moveForward(1.0); // Move forward for 1 second (example duration)
    }
}


void Motor::detectionState() {
    cout << "Detecting rocks and hills..." << endl;
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

        motorController_.forward(-1);
        float x = currentCoords_.x;
        float y = currentCoords_.y;
        int theta = currentCoords_.theta;

        float x_new = -1 * std::cos(motorController_.degreesToRads(theta)) + x; // cos(theta)
        float y_new = -1 * std::sin(motorController_.degreesToRads(theta)) + y; // sin(theta)

        // std::cout << "Current Coords: " << x_new << " - " << y_new << " - " << theta << std::endl;
        currentCoords_.x = x_new;
        currentCoords_.y = y_new;

        params_.motorToComQueue.push(MotorToComData("robot", currentCoords_));
        

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

        state_ = "SEARCHING";

        std::cout << "Current Coords: " << currentCoords_.x << " - " << currentCoords_.y << " - " << currentCoords_.theta << std::endl;

    }
    
}

void Motor::avoidState() {
    cout << "Avoiding black tape and obstacles..." << endl;

    bool moving_away = true;
    bool block_detected = false;
    bool recovery_successful = true; // Replace with actual recovery logic
    int randomDirection = (rand() % 2 == 0) ? 1 : -1;

    while (moving_away) {

        ds1_    = params_.ds1.load(std::memory_order_acquire);
        ds2_    = params_.ds2.load(std::memory_order_acquire);
        ir1_    = params_.ir1.load(std::memory_order_acquire);
        ir2_    = params_.ir2.load(std::memory_order_acquire);
        

        cout << "ds1: " << ds1_ << " ds2: " << ds2_ << endl;
        cout << "ir1: " << ir1_ << " ir2: " << ir2_ << endl;

        if (ds1_ < 150 ) {
            block_detected = true;
            break;
        }

        if (ir1_ || ir2_) {
            cout << "In if ir1_ || ir2_" << endl;
            // Detected black tape, perform avoidance
            // Example: Move backward (replace with actual motor control logic)
            motorController_.turn(randomDirection * 90);
            currentCoords_.theta += randomDirection * 90;
            if (currentCoords_.theta >= 360) {
                currentCoords_.theta -= 360;
            } else if (currentCoords_.theta < 0) {
                currentCoords_.theta += 360;
            }
            // moveBackward(1.0); // Move backward for 1 second (example duration)
        } else if (ir1_) {
            cout << "In if ir1_" << endl;
            // Detected obstacle (block), perform avoidance
            // Example: Move backward (replace with actual motor control logic)
            motorController_.turn(90);
            currentCoords_.theta += 90;
            if (currentCoords_.theta >= 360) {
                currentCoords_.theta -= 360;
            } else if (currentCoords_.theta < 0) {
                currentCoords_.theta += 360;
            }
            // moveBackward(1.0); // Move backward for 1 second (example duration)
        } else if (ir2_) {
            cout << "In if ir2_" << endl;
            // Detected obstacle (block), perform avoidance
            // Example: Move backward (replace with actual motor control logic)
            motorController_.turn(-90);
            currentCoords_.theta -= 90;
            if (currentCoords_.theta >= 360) {
                currentCoords_.theta -= 360;
            } else if (currentCoords_.theta < 0) {
                currentCoords_.theta += 360;
            }
            // moveBackward(1.0); // Move backward for 1 second (example duration)
        } else {
            // Example: Move forward (replace with actual motor control logic)
            cout << "IDK HOW THIS HAPPENED IN avoidState()" << endl;
            // moveForward(1.0); // Move forward for 1 second (example duration)
        }

        if (stuckCounter_ > STUCK_THRESHOLD) {
            recovery_successful = false;
            break;
        }

        if (!ir1_ && !ir2_) {
            moving_away = false;
        }

    }
    if (block_detected) {
        state_ = "DETECTION";
    }else if (recovery_successful) {
        state_ = "SEARCHING";
    } else {
        // state_ = "STUCK";
        state_ = "SEARCHING";
    }

}

void Motor::reorientState() {
    cout << "Reorienting direction..." << endl;
    
}

void Motor::stuckState() {
    cout << "Robot is stuck, attempting recovery..." << endl;

    motorController_.forward(-1);
    float x = currentCoords_.x;
    float y = currentCoords_.y;
    int theta = currentCoords_.theta;

    
    float x_new = -1 * std::cos(motorController_.degreesToRads(theta)) + x; // cos(theta)
    float y_new = -1 * std::sin(motorController_.degreesToRads(theta)) + y; // sin(theta)

    // std::cout << "Current Coords: " << x_new << " - " << y_new << " - " << theta << std::endl;

    currentCoords_.x = x_new;
    currentCoords_.y = y_new;

    params_.motorToComQueue.push(MotorToComData("robot", currentCoords_));

    stuckCounter_++;
}

