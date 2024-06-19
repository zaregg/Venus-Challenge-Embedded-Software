#include "DistanceSensor.hpp"

DistanceSensor::DistanceSensor()
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

int DistanceSensor::setup()
{
    int i;
    std::cout << "Scanning I2C bus..." << std::endl;
    std::vector<uint8_t> address_list;
    for (uint8_t address = 1; address <= 127; ++address) {
        i = tofPing(IIC0, address);
        if (i == 0) {
            if (address == sensorA_address || address == sensorB_address) {
                std::cout << "Sensor found at address 0x" << std::hex << static_cast<int>(address) << std::dec << std::endl;
                address_list.push_back(address);
            }
        }
    }
    if (address_list.size() >= 2) {
        std::cout << "Both Sensor A and Sensor B found" << std::endl;
        // return 0;
    }else if (address_list.size() == 1 && address_list[0] == sensorB_address) {

        // int i;
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
        std::cout << std::endl << std::endl << "Now Power Sensor B!" << std::endl << std::endl;
        std::cout << "Press Enter to continue..." << std::endl;
        std::cin.get();
    } else if (address_list.size() == 1 && address_list[0] == sensorA_address) {
        std::cout << std::endl << std::endl << "Now Power Sensor B!" << std::endl << std::endl;
        std::cout << "Press Enter to continue..." << std::endl;
        std::cin.get();
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


TOFData DistanceSensor::getDistance()
{
    TOFData tofData = {0,0};
    uint16_t distanceA, distanceB;
    distanceA = tofReadDistance(&sensorA);
    distanceB = tofReadDistance(&sensorB);
    auto start_time = std::chrono::steady_clock::now();
    while (distanceA < 20 || distanceB < 20) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        distanceA = tofReadDistance(&sensorA);
        distanceB = tofReadDistance(&sensorB);

        auto current_time = std::chrono::steady_clock::now();
        auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count();
        if (elapsed_time >= 500) {
        std::cerr << "Timeout occurred" << std::endl;
        return tofData;
        }
    }

    tofData.distance1 = distanceA;
    tofData.distance2 = distanceB;

    return tofData;
}