#include "DistanceSensor.hpp"

DistanceSensor::DistanceSensor() : running_(false)
{
    std::cout << "DistanceSensor created" << std::endl;
    switchbox_set_pin(IO_AR_SCL, SWB_IIC0_SCL);
	switchbox_set_pin(IO_AR_SDA, SWB_IIC0_SDA);
	iic_init(IIC0);
}

DistanceSensor::~DistanceSensor()
{
    iic_destroy(IIC0);
}

void DistanceSensor::start(std::thread& thread, RobotQueue* managerToSensorQueue, RobotQueue* sensorToManagerQueue)
{
    if (!running_.load(std::memory_order_relaxed)) {
        running_.store(true, std::memory_order_relaxed);
        managerToSensorQueue_ = managerToSensorQueue;
        sensorToManagerQueue_ = sensorToManagerQueue;

        int i = setupSensors();
        if (i != 0) {
            std::cerr << "Failed to setup sensors" << std::endl;
            return;
        }
        
        thread_ = std::thread(&DistanceSensor::readData, this);
        thread = std::move(thread_); // Move the thread to the parameter
    }
}


void DistanceSensor::stop()
{
    if (running_.load(std::memory_order_relaxed)) {
        running_.store(false, std::memory_order_relaxed);
        if (thread_.joinable()) {
            thread_.join();
        }
    }
}

int DistanceSensor::setupSensors()
{
    int i;
    std::cout << "Setting up sensors..." << std::endl;

    std::cout << "Initializing sensor A..." << std::endl;
    i = tofSetAddress(IIC0,0x29, sensorA_address);
    if (i != 0) {
        std::cerr << "Failed to set sensor A address" << std::endl;
        return -1;
    }
    i = tofPing(IIC0, sensorA_address);
    if (i != 0) {
        std::cerr << "Failed to ping sensor A" << std::endl;
        return -1;
    }
    i = tofInit(&sensorA, IIC0, sensorA_address, 0);
    if (i != 0)
    {
        std::cerr << "Failed to initialize sensor A" << std::endl;
        return -1;
    }

    uint8_t model, revision;

    tofGetModel(&sensorA, &model, &revision);
    std::cout << "---Model ID - " << static_cast<int>(model) << std::endl;
    std::cout << "---Revision ID - " << static_cast<int>(revision) << std::endl;
    std::cout << "---Init: Success" << std::endl;
    std::cout.flush();

    std::cout << std::endl << std::endl << "Now Power Sensor B!" << std::endl << std::endl;
    std::cout << "Press Enter to continue..." << std::endl;
    std::cin.get();

    std::cout << "Initializing sensor B..." << std::endl;
    // i = tofSetAddress(IIC0,0x29, sensorB_address);
    // if (i != 0) {
    //     std::cerr << "Failed to set sensor B address" << std::endl;
    //     return -1;
    // }
    i = tofPing(IIC0, sensorB_address);
    if (i != 0) {
        std::cerr << "Failed to ping sensor B" << std::endl;
        return -1;
    }
    i = tofInit(&sensorB, IIC0, sensorB_address, 0);
    if (i != 0)
    {
        std::cerr << "Failed to initialize sensor B" << std::endl;
        return -1;
    }

    tofGetModel(&sensorB, &model, &revision);
    std::cout << "---Model ID - " << static_cast<int>(model) << std::endl;
    std::cout << "---Revision ID - " << static_cast<int>(revision) << std::endl;
    std::cout << "---Init: Success" << std::endl;
    std::cout.flush();

    return 0;
}

S_DistanceSensorTest *DistanceSensor::testData()
{
    S_DistanceSensorTest* test = new S_DistanceSensorTest();
    test->distance1 = 10;
    test->distance2 = 20;
    
    return test; // Replace with actual data
}

void DistanceSensor::readData()
{
    uint32_t iDistance;
    auto lastSentTime = std::chrono::steady_clock::now();

    while (running_.load(std::memory_order_relaxed)) {
        // Simulate reading data
        S_DistanceSensorTest* data = new S_DistanceSensorTest();
        iDistance = tofReadDistance(&sensorA);
        data->distance1 = iDistance;
        iDistance = tofReadDistance(&sensorB);
        data->distance2 = iDistance;

        std::cout << "Distance 1: " << data->distance1 << " mm" << std::endl;
        std::cout << "Distance 2: " << data->distance2 << " mm" << std::endl;

        auto currentTime = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed = currentTime - lastSentTime;

        if (elapsed.count() >= 0.1) { // 0.1 seconds is 100 milliseconds
            if (!sensorToManagerQueue_->push(data)) {
                std::cerr << "Failed to push to queue" << std::endl;
                delete data; // Clean up if push fails
            } else {
                lastSentTime = currentTime; // Update last sent time
            }
        } else {
            delete data; // Clean up if not sending
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1)); // Small sleep to avoid busy-waiting
    }
}



// int DistanceSensor::readDistance() {
//     // Simulated function to read distance data
//     // Replace with actual code to read from sensor
//     static int distance = 0;
//     distance += 10; // Incrementing distance for demonstration
//     return distance;
// }
