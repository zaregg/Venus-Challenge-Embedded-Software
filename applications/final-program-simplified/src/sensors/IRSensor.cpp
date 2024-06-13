#include "IRSensor.hpp"

IRSensor::IRSensor() {
    // Initialize the sensor
    // gpio_set_direction(IO_AR6, GPIO_DIR_INPUT);
    // gpio_set_direction(IO_AR7, GPIO_DIR_INPUT);
}

IRSensor::~IRSensor() {
    // Cleanup sensor
}

IRData IRSensor::getIRData() {
    // Get IR data
    IRData irData;
    irData.ir1 = 0; // gpio_read(IO_AR6);
    irData.ir2 = 0; // gpio_read(IO_AR7);
    return irData;
}