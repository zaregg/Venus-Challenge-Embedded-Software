#ifndef SERIALCLIENT_HPP
#define SERIALCLIENT_HPP

#include "RobotParams.hpp"
#include "json.hpp"

#include <thread>
#include <iostream>

using json = nlohmann::json;

extern "C" {
    #include <arm_shared_memory_system.h>
    #include <libpynq.h>
    #include <platform.h>
    #include <stepper.h>
}

class SerialClient {
public:
    // Constructor
    SerialClient(ThreadParams& params);

    // Destructor
    ~SerialClient();

    void start();

    void stop();

    void join();

    // TODO: Add your member functions here

private:
    // TODO: Add your member variables here
    ThreadParams& params_;

    std::thread comThread_;

    void transmitData();
    void receiveData();

    void uart_read_array(const int uart, uint8_t *buf, uint8_t l);

    void uart_write_array(const int uart, const uint8_t *buf, uint8_t l);

    void parseData(std::string& json_str);

    void communicationThread();
};

#endif // SERIALCLIENT_HPP