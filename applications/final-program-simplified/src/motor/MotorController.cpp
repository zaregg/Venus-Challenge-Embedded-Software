#include "MotorController.hpp"

MotorController::MotorController(MotorParams &params)
    : params_(params)
{
    switchbox_set_pin(IO_AR0, SWB_UART0_RX);
    switchbox_set_pin(IO_AR1, SWB_UART0_TX);
    gpio_set_direction(IO_AR2, GPIO_DIR_INPUT);
    gpio_set_direction(IO_AR3, GPIO_DIR_INPUT);
    // printf("AR2: %d\n", gpio_get_level(IO_AR2));
    // printf("AR3: %d\n", gpio_get_level(IO_AR3));

    stepper_init();

}

MotorController::~MotorController()
{
    if (params_.motorEnabled.load(std::memory_order_acquire)){
        params_.motorEnabled.store(false, std::memory_order_release);
        stepper_disable();
    }
}

void MotorController::enable()
{
    if (!params_.motorEnabled.load(std::memory_order_acquire)){
        params_.motorEnabled.store(true, std::memory_order_release);
        stepper_enable();
    }
}

void MotorController::disable()
{
    if (params_.motorEnabled.load(std::memory_order_acquire)){
        params_.motorEnabled.store(false, std::memory_order_release);
        stepper_disable();
    }
}

void MotorController::turn(int degrees)
{
    enable();
    // std::cout << "Turning " << degrees << " degrees" << std::endl;
    // std::cout << "Motor enabled: " << params_.motorEnabled.load(std::memory_order_acquire) << std::endl;
    if (params_.motorEnabled.load(std::memory_order_acquire)){
        stepper_set_speed(25000, 25000); //FIXME hardocded for now
        stepper_steps(degrees*STEPS_PER_DEGREE, -degrees*STEPS_PER_DEGREE);
        while (!stepper_steps_done());
        std::this_thread::sleep_for(std::chrono::milliseconds(1));;
    }
    disable();

}

void MotorController::forward(int distance)
{
    enable();
    // std::cout << "Moving forward " << distance << " cm" << std::endl;
    // std::cout << "Motor enabled: " << params_.motorEnabled.load(std::memory_order_acquire) << std::endl;
    if (params_.motorEnabled.load(std::memory_order_acquire)){
        stepper_set_speed(15000, 15000); //FIXME hardocded for now
        stepper_steps(distance*STEPS_PER_CM, distance*STEPS_PER_CM);
        while (!stepper_steps_done());
        std::this_thread::sleep_for(std::chrono::milliseconds(1));;
    }
    disable();
}

void MotorController::backward(int distance)
{
    enable();
    std::cout << "Moving backward " << distance << " cm" << std::endl;
    std::cout << "Motor enabled: " << params_.motorEnabled.load(std::memory_order_acquire) << std::endl;
    if (params_.motorEnabled.load(std::memory_order_acquire)){
        stepper_set_speed(15000, 15000); //FIXME hardocded for now
        stepper_steps(-distance*STEPS_PER_CM, -distance*STEPS_PER_CM);
        while (!stepper_steps_done());
        std::this_thread::sleep_for(std::chrono::milliseconds(1));;
    }
    disable();
}

void MotorController::scan(int degrees)
{
    enable();
    // int i = 0;
    std::cout << "Scanning " << degrees << " degrees" << std::endl;
    std::cout << "Motor enabled: " << params_.motorEnabled.load(std::memory_order_acquire) << std::endl;
    if (params_.motorEnabled.load(std::memory_order_acquire)){
        std::cout << "Scanning " << -degrees << " degrees" << std::endl;
        stepper_set_speed(30000, 30000); //FIXME hardocded for now
        // if (i % 10)
        // {
        //     stepper_steps(degrees*STEPS_PER_DEGREE, -degrees*STEPS_PER_DEGREE);
        //     while (!stepper_steps_done());
        //     std::this_thread::sleep_for(std::chrono::milliseconds(1));;
        //     std::cout << "Done turning 60 degrees" << std::endl;
        //     stepper_steps( -2*degrees*STEPS_PER_DEGREE, 2*degrees*STEPS_PER_DEGREE);
        //     while (!stepper_steps_done());
        //     std::this_thread::sleep_for(std::chrono::milliseconds(1));;
        //     std::cout << "Done turning -180 degrees" << std::endl;
        //     stepper_steps(degrees*STEPS_PER_DEGREE, -degrees*STEPS_PER_DEGREE);
        //     while (!stepper_steps_done());
        //     std::cout << "Done turning 60 degrees" << std::endl;
        //     std::this_thread::sleep_for(std::chrono::milliseconds(1));;
        //     while (!stepper_steps_done());
        //     sleep_msec(10);
        // }
        stepper_steps(STEPS_PER_CM,STEPS_PER_CM);
        while (!stepper_steps_done());
        std::this_thread::sleep_for(std::chrono::milliseconds(1));;
    }
    disable();
}

float MotorController::degreesToRads(int degrees) {
    return degrees * (M_PI / 180.0);
}