#include "StepperThread.hpp"


Stepper::Stepper(boost::lockfree::queue<s_StepperThread*>& threadQueue)
    : threadQueue(threadQueue), running(false)
{
    // Initialize the stepper motor
    std::cout << "Initializing stepper motor" << std::endl;

    // switchbox_set_pin(IO_AR0, SWB_UART0_RX);
    // switchbox_set_pin(IO_AR1, SWB_UART0_TX);
    // gpio_set_direction(IO_AR2, GPIO_DIR_INPUT);
    // gpio_set_direction(IO_AR3, GPIO_DIR_INPUT);

    // stepper_init();
    // stepper_enable();
}

Stepper::~Stepper() {
    stop();
    // stepper_disable();
    std::cout << "Stepper motor disabled" << std::endl;
}

void Stepper::start() {
    if (!running)
    {
        running = true;
        stepperThread = std::thread(&Stepper::move, this);
    }
}

void Stepper::stop() {
    if (running)
    {
        running = false;
        if (stepperThread.joinable())
        {
            stepperThread.join();
            // stepper_disable();
        }
    }
}

void Stepper::join() {
    if (stepperThread.joinable())
    {
        stepperThread.join();
    }
}

void Stepper::move() {
    printf("Stepper thread started\n");
    s_StepperThread* threadStruct = nullptr;
    while (running) {
        try {
            if (threadQueue.pop(threadStruct)) {
                // Process the received s_StepperThread object
                std::cout << "Received s_StepperThread object from main thread." << std::endl;
                // Here you can perform any actions required for stepper motor control
                // For demonstration purposes, we'll just print the values of the struct fields
                std::cout << "Step Count: " << threadStruct->stepCount << std::endl;
                std::cout << "Direction: " << threadStruct->angle << std::endl;
                std::cout << "Speed: " << threadStruct->speed << std::endl;

                // Free the memory allocated for the s_StepperThread object
                delete threadStruct;
                threadStruct = nullptr;
            }
        } catch (const std::exception& e) {
            // Handle any exceptions that might occur during pop operation
            std::cerr << "Exception caught: " << e.what() << std::endl;
            // Clean up allocated memory before continuing
            if (threadStruct != nullptr) {
                delete threadStruct;
                threadStruct = nullptr;
            }
        }
    }
}

