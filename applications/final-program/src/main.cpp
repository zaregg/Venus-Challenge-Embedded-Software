#include <iostream>
#include <boost/lockfree/queue.hpp> // Include Boost.Lockfree
#include <thread>
#include <cstdlib> // Include for EXIT_FAILURE and EXIT_SUCCESS
#include <memory> // Include for std::unique_ptr

#include "threads/stepper_thread.hpp"
#include "threads/com_manager_thread.hpp"
#include "utils/structs.h"
#include <libpynq.h>


/**
 * Maybe add a robot class to keep track of the robot's state and position
*/


int main(void) {
  pynq_init();

  // Allocate memory for the thread structure
  // std::unique_ptr<s_StepperThread> stepperThreadStruct(new s_StepperThread);

  // if (!stepperThreadStruct) {
  //     std::cerr << "Error allocating memory for stepper thread structure" << std::endl;
  //     return EXIT_FAILURE;
  // }

  std::cout << "Starting stepper thread..." << std::endl;
  
  boost::lockfree::queue<s_StepperThread*> threadQueue(100); // Capacity of 100 pointers to s_StepperThread

  // pthread_t stepperThreadId;
  // Create the threads
  // if (pthread_create(&stepperThreadId, NULL, stepperThread, (void *)stepperThreadStruct) != 0) {
  //   printf("Error creating Stepper thread\n");
  //   return EXIT_FAILURE;
  // }
  std::thread stepperThread(stepperThreadFunction, std::ref(threadQueue));
  
  s_StepperThread* stepperThreadStruct = new s_StepperThread;
  stepperThreadStruct->stepCount = 1000; // Example step count
  stepperThreadStruct->angle = 1;    // Example direction
  stepperThreadStruct->speed = 50;       // Example speed
  threadQueue.push(stepperThreadStruct);

  // Join the thread
  if (stepperThread.joinable()) {
    stepperThread.join();
  } else {
    std::cerr << "Error joining Stepper thread" << std::endl;
    pynq_destroy();
    return EXIT_FAILURE;
  }

  pynq_destroy();
  return EXIT_SUCCESS;
}
