#pragma once
#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdint.h>

// Define a type alias for the queue

struct Robot {
    virtual ~Robot() = default;
};

typedef struct S_DistanceSensorTest : public Robot
{
    int distance1;
    int distance2;
    std::chrono::time_point<std::chrono::system_clock> time;
} s_DistanceSensorTest;

typedef struct s_StepperThread : public Robot {
    uint16_t angle;
    uint16_t stepCount;
    int16_t speed;

} s_StepperThread;

using RobotQueue = boost::lockfree::queue<Robot*>;
using StepperQueue = boost::lockfree::queue<s_StepperThread*>;

#endif /* STRUCTS_H */