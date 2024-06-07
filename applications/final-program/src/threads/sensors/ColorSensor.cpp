#include "ColorSensor.hpp"

ColorSensor::ColorSensor()
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
        printf("Received color: {}\n", token);
        color_str_copy.erase(0, pos + 1);
    }
    printf("Received color: {}\n", color_str_copy);
    return color_str_copy;
}


void ColorSensor::sendCaptureSignal() {
    boost::asio::io_context io_context;
    boost::asio::ip::tcp::socket socket(io_context);
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string("127.0.0.1"), PORT);

    try {
        socket.connect(endpoint);
        boost::asio::write(socket, boost::asio::buffer(CAPTURE_SIGNAL, strlen(CAPTURE_SIGNAL)));
    } catch (std::exception& e) {
        std::cerr << "Connection failed: " << e.what() << std::endl;
        return;
    }
}

void ColorSensor::receiveFromPython(s_ColorSensorTest* data)
{
    boost::asio::io_context io_context;
    boost::asio::ip::tcp::acceptor acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), PORT));

    std::cout << "Waiting for connections..." << std::endl;

    boost::asio::ip::tcp::socket socket(io_context);
    acceptor.accept(socket);

    std::array<char, 1024> buffer;
    boost::system::error_code error;
    size_t length = socket.read_some(boost::asio::buffer(buffer), error);
    if (error) {
        std::cerr << "Read error: " << error.message() << std::endl;
        return;
    }
    std::string color_str = processColorData(std::string(buffer.data(), length));
    if (data != nullptr) {
        // 1 = RED 2 = GREEN 3 = BLUE
        if (std::strcmp(color_str.c_str(), "red") == 0) {
            data->colour = 1;
        } else if (std::strcmp(color_str.c_str(), "green") == 0) {
            data->colour = 2;
        } else if (std::strcmp(color_str.c_str(), "blue") == 0) {
            data->colour = 3;
        } else {
            data->colour = 0;
        }
    }
}