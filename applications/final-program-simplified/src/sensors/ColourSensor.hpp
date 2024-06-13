#ifndef COLOURSENSOR_HPP
#define COLOURSENSOR_HPP

// Include any necessary libraries or headers
#include <atomic>
#include <boost/lockfree/queue.hpp>
#include <condition_variable>
#include <iostream>
#include <string>
#include <thread>
#include <zmq.hpp>

#include "RobotParams.hpp"

#define PORT 65432
#define CAPTURE_SIGNAL "CAPTURE"

// Define the ColorSensor class
class ColourSensor {
public:
    // Constructor(s)
    ColourSensor();
    // Destructor
    ~ColourSensor();
    void setup();

    ColourData requestCapture();
    
private:

    zmq::context_t context_;
    zmq::socket_t socket_;

    void sendCaptureSignal();
    std::string processColorData(const std::string& color_str);
    void receiveFromPython(ColourData* colourData);
    
    // Private member functions
};

#endif // COLOURSENSOR_HPP