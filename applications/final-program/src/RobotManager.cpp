#include "RobotManager.hpp"


RobotManager::RobotManager() 
: sensorManager(comToSensorQueue, sensorToComQueue), 
    comManager(comToStepperQueue, stepperToComQueue, comToSensorQueue, sensorToComQueue), 
    stepperThread(comToStepperQueue, stepperToComQueue),
    comToSensorQueue(100),
    sensorToComQueue(100),
    comToStepperQueue(100),
    stepperToComQueue(100)
{

}

RobotManager::~RobotManager()
{

}

void RobotManager::start()
{
    addSensors();

    sensorManager.start();
    sensorManager.amountOfSensors();

    stepperThread.start();
    comManager.start();

}

void RobotManager::stop()
{
    sensorManager.stop();
    stepperThread.stop();
    comManager.stop();
}

void RobotManager::join()
{
    sensorManager.join();
    sensorManager.joinSensors();
    stepperThread.join();
    comManager.joinThreads();
}

void RobotManager::addSensors()
{
    sensorManager.addSensor(&distanceSensor);
    sensorManager.addSensor(&colorSensor);

}