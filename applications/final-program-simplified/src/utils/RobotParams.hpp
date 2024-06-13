#ifndef ROBOTPARAMS_HPP
#define ROBOTPARAMS_HPP

#include <atomic>
#include <mutex>
#include <condition_variable>
#include <boost/lockfree/queue.hpp>

// Define the maximum size of the lock-free queues
#define QUEUE_CAPACITY 100

struct TOFData {
    uint16_t distance1;                      // Distance measurement 1
    uint16_t distance2;                      // Distance measurement 2
};

struct ColourData {
    uint8_t colour;                          // Colour value
};

struct IRData {
    uint8_t ir1;                             // Infrared value 1
    uint8_t ir2;                             // Infrared value 2
};

struct MotorParams {
    std::atomic<bool>& motorRunning;       // Flag indicating if the motor is running
    std::mutex& mtx;                        // Mutex for motor synchronization
    std::condition_variable& cv;            // Condition variable for motor synchronization
};

struct MotorData {
    int speed;                              // Motor speed
    int angle;                              // Motor angle
    int distance;                           // Motor distance
};

struct SensorData {
    TOFData distanceData;                   // Time-of-Flight sensor data
    ColourData colourData;                  // Colour sensor data
    IRData irData;                          // Infrared sensor data
};

struct ThreadParams {
    MotorParams motorParams;                // Motor parameters
    
    // Add your shared variables here
    // TODO Maybe the comToMotorQueue and motorToComQueue should have different structs
    boost::lockfree::queue<MotorData> comToMotorQueue { QUEUE_CAPACITY };    // Lock-free queue for motor data
    boost::lockfree::queue<MotorData> motorToComQueue { QUEUE_CAPACITY };    // Lock-free queue for motor data
    boost::lockfree::queue<SensorData> sensorQueue { QUEUE_CAPACITY };    // Lock-free queue for sensor data
};

#endif // ROBOTPARAMS_HPP