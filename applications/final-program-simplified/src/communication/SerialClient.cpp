#include "SerialClient.hpp"

SerialClient::SerialClient(ThreadParams& params)
    : params_(params)
{

}

SerialClient::~SerialClient()
{
    
}

void SerialClient::start()
{
    // Open the serial port


    // Start the thread
    thread_ = std::thread(&SerialClient::communicationThread, this);
}

void SerialClient::uart_read_array(const int uart, uint8_t *buf, uint8_t l) {
  for (uint8_t x = 0; x < l; x++) {
    buf[x] = uart_recv(uart);
  }
}