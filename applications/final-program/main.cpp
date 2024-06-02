#include "threads/stepper_thread.hpp"

#include <libpynq.h>

#include <pthread.h>

#include "structs.h"


int main(void) {
  pynq_init();

  s_StepperThread *stepperThreadStruct = (s_StepperThread *)malloc(sizeof(s_StepperThread));

  pthread_t stepperThreadId;

  printf("Starting stepper thread...\n");

  // Create the threads
  if (pthread_create(&stepperThreadId, NULL, stepperThread, (void *)stepperThreadStruct) != 0) {
    printf("Error creating Stepper thread\n");
    return EXIT_FAILURE;
  }
  
  // Join the threads
  if (pthread_join(stepperThreadId, NULL) != 0) {
    fprintf(stderr, "Error joining Stepper thread\n");
    return EXIT_FAILURE;
  }

  pynq_destroy();
  return EXIT_SUCCESS;
}
