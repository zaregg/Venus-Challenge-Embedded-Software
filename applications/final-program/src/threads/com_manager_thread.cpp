#include "com_manager_thread.hpp"

CommunicationManager::CommunicationManager(boost::lockfree::queue<s_StepperThread*>& comToStepperQueue, boost::lockfree::queue<s_StepperThread*>& stepperToComQueue)
    : comToStepperQueue(comToStepperQueue), stepperToComQueue(stepperToComQueue), running(false)
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
    if (!running)
    {
        running = true;
        readThread = std::thread(&CommunicationManager::readFromUART, this);
    }
}

void CommunicationManager::stop()
{
    if (running)
    {
        running = false;
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

void CommunicationManager::readFromUART()
{
    // Create a new instance of s_StepperThread struct
    s_StepperThread* stepperThreadStruct = new s_StepperThread;

    while (running) {
        std::string message = jsonTest();
        std::cout << message << std::endl;

        json parsedJson = json::parse(message);
        std::cout << "Deserialized JSON object:\n" << parsedJson.dump(4) << std::endl;

        // Set the values of the s_StepperThread struct with random numbers
        stepperThreadStruct->stepCount  = parsedJson["stepCount"];
        stepperThreadStruct->angle      = parsedJson["angle"];
        stepperThreadStruct->speed      = parsedJson["speed"];

        // Push the s_StepperThread struct to the queue
        comToStepperQueue.push(stepperThreadStruct);

        // Sleep for one second
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void CommunicationManager::writeToUART()
{
    // Create a new instance of s_StepperThread struct
    s_StepperThread* stepperThreadStruct = nullptr;

    while (running) {
        // Pop the s_StepperThread struct from the queue
        if (stepperToComQueue.pop(stepperThreadStruct)) {
            // Print the values of the s_StepperThread struct
            std::cout << "Step Count: " << stepperThreadStruct->stepCount << std::endl;
            std::cout << "Angle: " << stepperThreadStruct->angle << std::endl;
            std::cout << "Speed: " << stepperThreadStruct->speed << std::endl;

            delete stepperThreadStruct;
            stepperThreadStruct = nullptr;
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

