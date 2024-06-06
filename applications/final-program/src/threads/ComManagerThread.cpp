#include "ComManagerThread.hpp"

CommunicationManager::CommunicationManager(StepperQueue &comToStepperQueue, StepperQueue &stepperToComQueue, SensorManagerQueue &comToSensorQueue, SensorManagerQueue &sensorToComQueue)
    : comToStepperQueue(comToStepperQueue), stepperToComQueue(stepperToComQueue), comToSensorQueue(comToSensorQueue), sensorToComQueue(sensorToComQueue), running_(false)
{
    // Print that the COM Manager Thread has started
    std::cout << "COM Manager Thread Started" << std::endl;
}

CommunicationManager::~CommunicationManager()
{
    // Print that the COM Manager Thread has stopped
    std::cout << "COM Manager Thread Stopped" << std::endl;
    stop();
}
void CommunicationManager::start()
{
    if (!running_.load(std::memory_order_relaxed))
    {
        running_.store(true, std::memory_order_relaxed);
        readThread = std::thread(&CommunicationManager::readFromUART, this);
        writeThread = std::thread(&CommunicationManager::writeToUART, this);
    }
}

void CommunicationManager::stop()
{
    std::cout << "Stopping COM Manager Thread" << std::endl;
    
    if (running_.load(std::memory_order_relaxed))
    {
        running_.store(false, std::memory_order_relaxed);
        if (readThread.joinable())
        {
            readThread.join();
        }
    }
}

void CommunicationManager::joinThreads()
{
    if (readThread.joinable())
    {
        readThread.join();
    }
}

void CommunicationManager::readFromUART() {
    while (running_.load(std::memory_order_relaxed)) {
        try {
            std::string message = jsonTest();
            // std::cout << message << std::endl;

            json parsedJson = json::parse(message);
            // std::cout << "Deserialized JSON object:\n" << parsedJson.dump(4) << std::endl;

            // Create a new instance of s_StepperThread struct
            s_StepperThread* stepperThreadStruct = new s_StepperThread;

            // Set the values of the s_StepperThread struct
            stepperThreadStruct->stepCount  = parsedJson["stepCount"].get<int>();
            stepperThreadStruct->angle      = parsedJson["angle"].get<double>();
            stepperThreadStruct->speed      = parsedJson["speed"].get<double>();

            // Push the s_StepperThread struct to the queue
            if (!comToStepperQueue.push(stepperThreadStruct)) {
                std::cerr << "Failed to push to queue" << std::endl;
                delete stepperThreadStruct; // Clean up if push fails
            }

            // Sleep for one second
            std::this_thread::sleep_for(std::chrono::seconds(1));

        } catch (const json::exception& e) {
            std::cerr << "JSON parsing error: " << e.what() << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}

void CommunicationManager::writeToUART()
{
    // Create a new instance of s_StepperThread struct
    s_StepperThread* stepperThreadStruct = nullptr;
    CombinedSensorData* combinedData = nullptr;

    while (running_.load(std::memory_order_relaxed)) {
        // Pop the s_StepperThread struct from the queue
        if (stepperToComQueue.pop(stepperThreadStruct)) {
            // Print the values of the s_StepperThread struct
            // std::cout << "Step Count: " << stepperThreadStruct->stepCount << std::endl;
            // std::cout << "Angle: " << stepperThreadStruct->angle << std::endl;
            // std::cout << "Speed: " << stepperThreadStruct->speed << std::endl;

            delete stepperThreadStruct;
            stepperThreadStruct = nullptr;
        }
        if (sensorToComQueue.pop(combinedData)) {
            if (combinedData->distanceSensorData != nullptr) {
                // Print the values of the distance sensor data
                std::cout << "Distance 1: " << combinedData->distanceSensorData->distance1 << std::endl;
                std::cout << "Distance 2: " << combinedData->distanceSensorData->distance2 << std::endl;
                delete combinedData->distanceSensorData;
                combinedData->distanceSensorData = nullptr;
            }
            else if (combinedData->colorSensorData != nullptr) {
                // Print the values of the color sensor data
                std::cout << "Colour: " << combinedData->colorSensorData->colour << std::endl;
                delete combinedData->colorSensorData;
                combinedData->colorSensorData = nullptr;
            }
            else if (combinedData->irSensorData != nullptr) {
                // Print the values of the IR sensor data
                std::cout << "Detected: " << combinedData->irSensorData->detected << std::endl;
                delete combinedData->irSensorData;
                combinedData->irSensorData = nullptr;
            }
            // Print the values of the s_StepperThread struct
            // std::cout << "Step Count: " << stepperThreadStruct->stepCount << std::endl;
            // std::cout << "Angle: " << stepperThreadStruct->angle << std::endl;
            // std::cout << "Speed: " << stepperThreadStruct->speed << std::endl;

            delete combinedData;
            combinedData = nullptr;
        }
    }
}

std::string CommunicationManager::jsonTest()
{
    json j;
    j["stepCount"] = rand() % 1000;
    j["angle"] = rand() % 360;
    j["speed"] = rand() % 100;

    return j.dump();
}

