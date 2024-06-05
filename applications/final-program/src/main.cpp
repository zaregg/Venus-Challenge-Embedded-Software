#include <iostream>
#include <boost/lockfree/queue.hpp> // Include Boost.Lockfree
#include <thread>
#include <cstdlib> // Include for EXIT_FAILURE and EXIT_SUCCESS
#include <memory> // Include for std::unique_ptr

#include "threads/StepperThread.hpp"
#include "threads/ComManagerThread.hpp"
#include "SensorManager.hpp"
#include "threads/sensors/DistanceSensor.hpp"
#include "utils/structs.hpp"
// #include <libpynq.h>


/**
 * Maybe add a robot class to keep track of the robot's state and position
*/

int main(void) {
  // Initialize the Pynq library
  // pynq_init();

  // Create a queue to store pointers to s_StepperThread
  boost::lockfree::queue<s_StepperThread*> comToSensorQueue(100); // Create a queue to store pointers to s_StepperThread
  boost::lockfree::queue<s_StepperThread*> sensorToComQueue(100); // Create a queue to store pointers to s_StepperThread

  SensorManager sensorManager(comToSensorQueue, sensorToComQueue);

  DistanceSensor distanceSensor;

  sensorManager.addSensor(&distanceSensor);

  sensorManager.amountOfSensors();

  sensorManager.joinThreads();


  // Create a queue to store pointers to s_StepperThread
  // boost::lockfree::queue<s_StepperThread*> comThreadQueue(100); // Create a queue to store pointers to s_StepperThread
  // boost::lockfree::queue<s_StepperThread*> stepperThreadQueue(100); // Create a queue to store pointers to s_StepperThread

  // Create a thread to handle the stepper motor
  // Stepper stepperThread(comThreadQueue);
  // stepperThread.start();

  // Create a thread to handle the communication manager
  // CommunicationManager comManager(comThreadQueue, stepperThreadQueue);
  // comManager.start();

  // join the threads
  // stepperThread.join();
  // comManager.joinThreads();

  // std::thread comManagerThread(Com, std::ref(threadQueue));

 

  // Clean up the Pynq library resources
  // pynq_destroy();

  // Return a success status code
  return EXIT_SUCCESS;
}
