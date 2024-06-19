#ifndef ROBOTPARAMS_HPP
#define ROBOTPARAMS_HPP

#include <atomic>
#include <mutex>
#include <condition_variable>
#include <boost/lockfree/queue.hpp>
#include "json.hpp"
#include <optional>

using json = nlohmann::json;

// Define the maximum size of the lock-free queues
#define QUEUE_CAPACITY 100

struct TOFData {
    uint16_t distance1;                      // Distance measurement 1
    uint16_t distance2;                      // Distance measurement 2

    json toJson() const  {
        return json{{"TOF", {{"d1", distance1}, {"d2", distance2}}}};
    }
};

struct ColourData {
    uint8_t colour;                          // Colour value

    json toJson() const {
        return json{{"CS", {{"c", colour}}}};
    }
};

struct IRData {
    // IDK why but it cannot be uint8_t, it will not print to stdout   
    int ir1;                             // Infrared value 1
    int ir2;                             // Infrared value 2

    json toJson() const {
        return json{{"IR", {{"ir1", ir1}, {"ir2", ir2}}}};
    }
};

struct MotorParams {
    std::atomic<bool>& motorEnabled;        // Flag indicating if the motor is enabled
    std::atomic<bool>& motorRunning;       // Flag indicating if the motor is running
    std::atomic<bool>& stopSignal;          // Flag indicating if the motor should stop
    std::atomic<bool>& irSignal;            // Flag indicating if the motor should stop
    std::mutex& mtx;                        // Mutex for motor synchronization
    std::condition_variable& cv;            // Condition variable for motor synchronization
};

struct MotorData {
    int speed = 10000;                      // Motor speed
    int angle;                              // Motor angle
    int distance;                           // Motor distance
};

struct Coordinate {
    float x;
    float y;
    int theta;
};

struct SensorData {
    TOFData distanceData;                   // Time-of-Flight sensor data
    ColourData colourData;                  // Colour sensor data
    IRData irData;                          // Infrared sensor data

    json toJson() const {
        return json{
            // {"TOF", distanceData ? distanceData.toJson()["TOF"] : json{}},
            // {"CS", colourData ? colourData.toJson()["CS"] : json{}},
            // {"IR", irData ? irData.toJson()["IR"] : json{}}
            {"TOF", distanceData.toJson()["TOF"] },
            {"CS", colourData.toJson()["CS"] },
            {"IR", irData.toJson()["IR"] }
        };
    }
};

struct MotorToComData {
    // std::string type;                       // Type of data
    std::array<char, 100> type;
    Coordinate coordinate;

    std::optional<SensorData> sensorData;

    MotorToComData() : type{}, coordinate{}, sensorData{} {}

    // Constructor that takes a std::string and a Coordinate
    MotorToComData(const std::string& s, const Coordinate& c, const std::optional<SensorData>& sd = std::nullopt) : type{}, coordinate{c}, sensorData{sd} {
        std::copy(s.begin(), s.end(), type.begin());
    }

    // Conversion to std::string
    operator std::string() const {
        return std::string(type.data());
    }

    json toJson() const {
        json result{
            {"Coords", {{"type", std::string(type.data())}, {"x", coordinate.x}, {"y", coordinate.y}, {"tag", "delete"}}}
        };
    
        if (sensorData.has_value()) {
            result["Sensors"] = sensorData.value().toJson();
        }
    
        return result;
    }
};

struct SensorParams {
    std::atomic<bool>& sensorRunning;       // Flag indicating if the sensor is running
    // TODO Maybe more in the future
};

struct ComParams {
    std::atomic<bool>& comRunning;          // Flag indicating if the communication is running
    // std::mutex& mtx;                        // Mutex for communication synchronization
    // std::condition_variable& cv;            // Condition variable for communication synchronization
};

struct ThreadParams {
    MotorParams motorParams;                // Motor parameters
    SensorParams sensorParams;              // Sensor parameters
    ComParams comParams;                    // Communication parameters
    
    // Add your shared variables here
    // TODO Maybe the comToMotorQueue and motorToComQueue should have different structs
    boost::lockfree::queue<MotorData> comToMotorQueue { QUEUE_CAPACITY };    // Lock-free queue for motor data
    boost::lockfree::queue<MotorToComData> motorToComQueue { QUEUE_CAPACITY };    // Lock-free queue for motor data
    boost::lockfree::queue<SensorData> sensorQueue { QUEUE_CAPACITY };    // Lock-free queue for sensor data
};

#endif // ROBOTPARAMS_HPP