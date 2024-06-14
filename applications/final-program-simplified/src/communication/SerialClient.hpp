#ifndef SERIALCLIENT_HPP
#define SERIALCLIENT_HPP

#include "RobotParams.hpp"

#include <thread>

extern "c" {
    #include <libpynq.h>
    #include <arm_shared_memory_system.h>
    #include <platform.h>
}

class SerialClient {
public:
    // Constructor
    SerialClient(ThreadParams& params);

    // Destructor
    ~SerialClient();

    void start();

    void stop();

    // TODO: Add your member functions here

private:
    // TODO: Add your member variables here
    ThreadParams& params_;

    std::thread comThread_;

    void transmitData();
    void receiveData();

    void uart_read_array(const int uart, uint8_t *buf, uint8_t l);

    void communicationThread();
};

#endif // SERIALCLIENT_HPP