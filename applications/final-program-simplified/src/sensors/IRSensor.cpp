#include "IRSensor.hpp"

IRSensor::IRSensor() {
    // Initialize the sensor
    gpio_set_direction(IO_AR6, GPIO_DIR_INPUT);
    gpio_set_direction(IO_AR7, GPIO_DIR_INPUT);
    gpio_init();
}

IRSensor::~IRSensor() {
    // Cleanup sensor
}

IRData IRSensor::getIRData() {
    // Get IR data
    IRData irData;
    irData.ir1 =  gpio_get_level(IO_AR6);
    // std::cout << "IR1 (AR6): " << gpio_get_level(IO_AR6) << std::endl;
    irData.ir2 =  gpio_get_level(IO_AR7);
    // std::cout << "IR2 (AR7): " << gpio_get_level(IO_AR7) << std::endl;
    return irData;
}