/**
 * @file com_manager_thread.hpp
 * @brief Header file for the communication manager thread.
 * 
 * This file contains the declaration of the communication manager thread function and includes the necessary dependencies.
 */
#pragma once
#ifndef COM_MANAGER_THREAD_H
#define COM_MANAGER_THREAD_H

#include <iostream>
#include <boost/lockfree/queue.hpp> // Include Boost.Lockfree
#include <thread>
#include <cstdlib> // Include for EXIT_FAILURE and EXIT_SUCCESS
#include <memory> // Include for std::unique_ptr


#include "../utils/json.hpp" // Include for JSON parsing
using json = nlohmann::json;

#include "../utils/structs.hpp"

extern "C" {
    // #include <arm_shared_memory_system.h>
    // // #include <json-c/json.h>
    // // #include <json-c/json_object.h>
    // #include <libpynq.h>
    // #include <platform.h>
    // #include <stdint.h>
    // #include <stepper.h>
}

/**
 * @class CommunicationManager
 * @brief Manages the communication between threads using UART.
 *
 * The CommunicationManager class is responsible for managing the communication
 * between threads using UART (Universal Asynchronous Receiver-Transmitter).
 * It provides functionality to start and stop the communication, as well as
 * joining the threads. The class uses a lock-free queue to store the threads
 * that need to be processed.
 *
 * @note This class assumes that the boost::lockfree::queue and std::thread
 *       classes are available.
 */
class CommunicationManager {
public:
    /**
     * @brief Constructs a CommunicationManager object.
     * @param threadQueue A reference to a lock-free queue of s_StepperThread pointers.
     *
     * This constructor initializes a CommunicationManager object with the given
     * lock-free queue of s_StepperThread pointers. The queue is used to store
     * the threads that need to be processed.
     */
    CommunicationManager(StepperQueue &comToStepperQueue, StepperQueue &stepperToComQueue, SensorManagerQueue &comToSensorQueue, SensorManagerQueue &sensorToComQueue);

    /**
     * @brief Destroys the CommunicationManager object.
     *
     * This destructor cleans up any resources associated with the CommunicationManager
     * object.
     */
    ~CommunicationManager();

    /**
     * @brief Starts the communication.
     *
     * This function starts the communication by creating and launching the read
     * and write threads.
     */
    void start();

    /**
     * @brief Stops the communication.
     *
     * This function stops the communication by setting the running flag to false,
     * which will cause the read and write threads to exit their loops.
     */
    void stop();

    /**
     * @brief Joins the read and write threads.
     *
     * This function waits for the read and write threads to finish their execution
     * and joins them back to the main thread.
     */
    void joinThreads();

private:
    std::thread readThread_;                             ///< The read thread.
    std::thread writeThread_;                            ///< The write thread.
    // boost::lockfree::queue<s_StepperThread*>& comToStepperQueue; ///< The lock-free queue for communication thread to stepper thread.
    // boost::lockfree::queue<s_StepperThread*>& stepperToComQueue; ///< The lock-free queue for stepper thread to communication thread.
    StepperQueue& comToStepperQueue;                    ///< The lock-free queue for communication thread to stepper thread.
    StepperQueue& stepperToComQueue;                    ///< The lock-free queue for stepper thread to communication thread.
    SensorManagerQueue& comToSensorQueue;                       ///< The lock-free queue for communication thread to sensor thread.
    SensorManagerQueue& sensorToComQueue;                       ///< The lock-free queue for sensor thread to communication thread.

    std::atomic<bool> running_; ///< Flag indicating if the communication is running.

    /**
     * @brief Reads data from UART.
     *
     * This function is executed by the read thread. It continuously reads data
     * from UART and processes it.
     */
    void readFromUART();

    /**
     * @brief Writes data to the UART.
     * 
     * This function is responsible for writing data to the UART (Universal Asynchronous Receiver/Transmitter).
     * It performs the necessary operations to send the data over the UART interface.
     * 
     * @note This function assumes that the UART has been properly initialized before calling this function.
     * 
     * @return void
     */
    void writeToUART();

    /**
     * @brief Returns a test JSON string.
     * @return A test JSON string.
     *
     * This function returns a test JSON string for testing purposes.
     */
    std::string jsonTest();
};

#endif // COM_MANAGER_THREAD_H