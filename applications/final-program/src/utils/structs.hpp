#pragma once
#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdint.h>
#include <chrono>
#include <boost/lockfree/queue.hpp> // Include Boost.Lockfree
// #include <nlohmann/json.hpp> // Include for JSON parsing
#include "../utils/json.hpp" // Include for JSON parsing
using json = nlohmann::json;

// Define a type alias for the queue

struct Robot {
    virtual ~Robot() = default;
    // virtual json toJson() const = 0; // Pure virtual function
};

struct RobotSensor : public Robot {
    virtual ~RobotSensor() = default;
    virtual json toJson() const = 0; // Pure virtual function
};

typedef struct S_DistanceSensorTest : public RobotSensor
{
    int distance1;
    int distance2;

    json toJson() const override {
        return json{{"TOF", {{"d1", distance1}, {"d2", distance2}}}};
    }
} s_DistanceSensorTest;

typedef struct s_ColorSensorTest : public RobotSensor
{
    // 1 = RED 2 = GREEN 3 = BLUE
    int colour;
    json toJson() const override {
        return json{{"CS", {{"c", colour}}}};
    }
} s_ColorSensorTest;

typedef struct s_IRSensorTest : public RobotSensor
{
    int detected; // 1 = detected 0 = not detected
    json toJson() const override {
        return json{{"IR", {{"d", detected}}}};
    }
} s_IRSensorTest;


struct CombinedSensorData : public RobotSensor {
    s_DistanceSensorTest *distanceSensorData;
    s_ColorSensorTest *colorSensorData;
    s_IRSensorTest *irSensorData;

    json toJson() const override {
        return json{
            {"TOF", distanceSensorData->toJson()["TOF"]},
            {"CS", colorSensorData->toJson()["CS"]},
            {"IR", irSensorData->toJson()["IR"]}
        };
    }
};

typedef struct s_StepperThread : public Robot {
    uint16_t angle;
    uint16_t stepCount;
    int16_t speed;

} s_StepperThread;

using RobotQueue = boost::lockfree::queue<Robot*>;
using StepperQueue = boost::lockfree::queue<s_StepperThread*>;
using SensorManagerQueue = boost::lockfree::queue<CombinedSensorData*>;
// TODO add a communication manager queue here

#endif /* STRUCTS_H */