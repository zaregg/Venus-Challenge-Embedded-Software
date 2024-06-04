#include <iostream>
#include <boost/lockfree/queue.hpp> // Include Boost.Lockfree
#include <thread>
#include <cstdlib> // Include for EXIT_FAILURE and EXIT_SUCCESS
#include <memory> // Include for std::unique_ptr

#include "threads/stepper_thread.hpp"
#include "threads/com_manager_thread.hpp"
#include "utils/structs.hpp"
#include <libpynq.h>


/**
 * Maybe add a robot class to keep track of the robot's state and position
*/

int main(void) {
  // Initialize the Pynq library
  pynq_init();

  // Print a message indicating that the stepper thread is starting
  std::cout << "Starting stepper thread..." << std::endl;

  // Create a queue to store pointers to s_StepperThread
  boost::lockfree::queue<s_StepperThread*> comThreadQueue(100); // Create a queue to store pointers to s_StepperThread
  boost::lockfree::queue<s_StepperThread*> stepperThreadQueue(100); // Create a queue to store pointers to s_StepperThread
  // Create a thread to handle the stepper motor
  // std::thread stepperThread(stepperThreadFunction, std::ref(threadQueue));

  // Create a thread to handle the stepper motor
  Stepper stepperThread(comThreadQueue);
  stepperThread.start();

  // Create a thread to handle the communication manager
  CommunicationManager comManager(comThreadQueue, stepperThreadQueue);
  comManager.start();

  // join the threads
  stepperThread.join();
  comManager.joinThreads();

  // std::thread comManagerThread(Com, std::ref(threadQueue));

  // // Create a new instance of s_StepperThread struct
  // s_StepperThread* stepperThreadStruct = new s_StepperThread;

  // // Set the values of the s_StepperThread struct
  // stepperThreadStruct->stepCount = 1000; // Example step count
  // stepperThreadStruct->angle = 1;    // Example direction
  // stepperThreadStruct->speed = 50;       // Example speed

  // Push the pointer to the s_StepperThread struct into the thread queue
  // threadQueue.push(stepperThreadStruct);

  // Join the thread, which means waiting for it to finish executing
  // if (stepperThread.joinable()) {
  //   stepperThread.join();
  // } else {
  //   // Print an error message if the thread cannot be joined
  //   std::cerr << "Error joining Stepper thread" << std::endl;

  //   // Clean up the Pynq library resources
  //   pynq_destroy();

  //   // Return a failure status code
  //   return EXIT_FAILURE;
  // }

  // Clean up the Pynq library resources
  pynq_destroy();

  // Return a success status code
  return EXIT_SUCCESS;
}
