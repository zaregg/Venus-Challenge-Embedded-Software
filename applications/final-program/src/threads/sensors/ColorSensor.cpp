#include "ColorSensor.hpp"

ColorSensor::ColorSensor() : context_(1), socket_(context_, ZMQ_REQ)
{
    std::cout << "Color Sensor created" << std::endl;
}

ColorSensor::~ColorSensor()
{
    std::cout << "Color Sensor destroyed" << std::endl;
}

void ColorSensor::start(std::thread &thread, RobotQueue *managerToSensorQueue, RobotQueue *sensorToManagerQueue)
{
    if (!running_.load(std::memory_order_relaxed))
    {
        running_.store(true, std::memory_order_relaxed);
        managerToSensorQueue_ = managerToSensorQueue;
        sensorToManagerQueue_ = sensorToManagerQueue;

        socket_.connect("tcp://localhost:5555");
        const std::string last_endpoint = socket_.get(zmq::sockopt::last_endpoint);
        std::cout << "Connected to: " << last_endpoint << std::endl;

        socket_.set(zmq::sockopt::rcvtimeo, 20000);
        int linger = 0;
        socket_.set(zmq::sockopt::linger, linger);

        worker_ = std::thread(&ColorSensor::workerThread, this);
        thread = std::move(worker_); // Move the thread to the parameter
    }
}

void ColorSensor::stop()
{
    if (running_.load(std::memory_order_relaxed))
    {
        running_.store(false, std::memory_order_relaxed);
        stop_.store(true, std::memory_order_relaxed);
        ready_.store(true, std::memory_order_relaxed);
        cv_.notify_one();
        if (worker_.joinable())
        {
            worker_.join();
        }
        socket_.close();
        context_.close();
    }
}

void ColorSensor::requestCapture()
{
    std::lock_guard<std::mutex> lk(cv_m);
    ready_.store(true, std::memory_order_relaxed);
    cv_.notify_one();
}

void ColorSensor::workerThread()
{
    while (running_.load(std::memory_order_relaxed))
    {
        std::unique_lock<std::mutex> lk(cv_m);
        cv_.wait(lk, [this] { return ready_.load(); });
        if(stop_.load())
        {
            break;
        }
        sendCaptureSignal();
        std::cout << "Waiting for reply" << std::endl;

        s_ColorSensorTest *data = new s_ColorSensorTest();
        receiveFromPython(data);
        // Capture image
        // send_capture_signal();
        // receive_from_python();

        // Simulate reading data
        
        if (!sensorToManagerQueue_->push(data))
        {
            std::cerr << "Failed to push to queue" << std::endl;
            delete data; // Clean up if push fails
        }

        ready_.store(false, std::memory_order_relaxed);
    }
    
}

std::string ColorSensor::processColorData(const std::string &color_str)
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


void ColorSensor::sendCaptureSignal() {

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

void ColorSensor::receiveFromPython(s_ColorSensorTest* data)
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
            if (data != nullptr) {
                // 1 = RED 2 = GREEN 3 = BLUE
                if (std::strcmp(color_str.c_str(), "Red") == 0) {
                    data->colour = 1;
                } else if (std::strcmp(color_str.c_str(), "Green") == 0) {
                    data->colour = 2;
                } else if (std::strcmp(color_str.c_str(), "Blue") == 0) {
                    data->colour = 3;
                } else {
                    data->colour = 0;
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