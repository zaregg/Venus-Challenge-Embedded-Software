#include "ColourSensor.hpp"

ColourSensor::ColourSensor()
    : context_(1), socket_(context_, ZMQ_REQ)
{
    std::cout << "Color Sensor created" << std::endl;
}

ColourSensor::~ColourSensor()
{
    std::cout << "Color Sensor destroyed" << std::endl;
}

void ColourSensor::setup()
{

    socket_.connect("tcp://localhost:5555");
    const std::string last_endpoint = socket_.get(zmq::sockopt::last_endpoint);
    std::cout << "Connected to: " << last_endpoint << std::endl;

    socket_.set(zmq::sockopt::rcvtimeo, 20000);
    int linger = 0;
    socket_.set(zmq::sockopt::linger, linger);

}

ColourData ColourSensor::requestCapture()
{
    sendCaptureSignal();

    ColourData colorData;
    receiveFromPython(&colorData);

    return colorData;
}

void ColourSensor::sendCaptureSignal() {

    try {
        std::string CAPTURE_SIGNAL_STR = CAPTURE_SIGNAL;
        zmq::message_t request(CAPTURE_SIGNAL_STR.size());
        memcpy(request.data(), CAPTURE_SIGNAL_STR.c_str(), CAPTURE_SIGNAL_STR.size());
        std::cout << "Sending request: " << CAPTURE_SIGNAL_STR << std::endl;
        socket_.send(request, zmq::send_flags::none);
        std::cout << "Capture signal sent" << std::endl;
        
    } catch (std::exception& e) {
        std::cerr << "Connection failed: " << e.what() << std::endl;
        return;
    }
}

std::string ColourSensor::processColorData(const std::string &color_str)
{
    std::string color_str_copy = color_str; // Create a non-const copy of color_str
    size_t pos = 0;
    std::string token;
    while ((pos = color_str_copy.find(',')) != std::string::npos) {
        token = color_str_copy.substr(0, pos);
        std::cout << "Received color: " << token << std::endl;
        color_str_copy.erase(0, pos + 1);
    }
    std::cout << "Received color: " << color_str_copy << std::endl;
    return color_str_copy;
}

void ColourSensor::receiveFromPython(ColourData* colorData)
{
    try {
        zmq::message_t reply;
        zmq::recv_result_t ret = socket_.recv(reply, zmq::recv_flags::none);
        if (!ret.has_value()) {
            std::cout << "Capture failed" << std::endl;
            socket_.close();
            context_.close();
            return; //0;
        }
        std::string recv_msg(static_cast<char*>(reply.data()), reply.size());
        std::cout << "Received reply: " << recv_msg << std::endl;
        if (recv_msg == "OK") {
            std::cout << "Capture successful" << std::endl;
            zmq::recv_result_t ret = socket_.recv(reply, zmq::recv_flags::none);
            if (!ret.has_value()) {
            std::cout << "Capture failed" << std::endl;
            socket_.close();
            context_.close();
            return; //0;
            }
            std::string colour(static_cast<char*>(reply.data()), reply.size());
            std::cout << "Received Colour: " << colour << std::endl;

            std::string color_str = processColorData(colour);
            if (colorData != nullptr) {
                // 1 = RED 2 = GREEN 3 = BLUE
                if (std::strcmp(color_str.c_str(), "Red") == 0) {
                    colorData->colour = 1;
                } else if (std::strcmp(color_str.c_str(), "Green") == 0) {
                    colorData->colour = 2;
                } else if (std::strcmp(color_str.c_str(), "Blue") == 0) {
                    colorData->colour = 3;
                } else {
                    colorData->colour = 0;
                }
            }
        } else {
            std::cout << "Capture failed" << std::endl;
        }
    } catch (std::exception& e) {
        std::cerr << "Exception caught in receiveFromPython: " << e.what() << std::endl;
        return;
    } catch (...) {
        std::cerr << "Unknown exception caught in receiveFromPython" << std::endl;
        return;
    }

}
