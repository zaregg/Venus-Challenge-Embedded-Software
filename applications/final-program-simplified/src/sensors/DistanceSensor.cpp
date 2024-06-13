// #include "DistanceSensor.hpp"

// DistanceSensor::DistanceSensor()
// {
//     std::cout << "DistanceSensor created" << std::endl;
//     // switchbox_set_pin(IO_AR_SCL, SWB_IIC0_SCL);
//     // switchbox_set_pin(IO_AR_SDA, SWB_IIC0_SDA);
//     // iic_init(IIC0);
// }

// DistanceSensor::~DistanceSensor()
// {
//     // iic_destroy(IIC0);
// }

// int DistanceSensor::setup()
// {
//     int i;
//     std::cout << "Setting up sensors..." << std::endl;

//     std::cout << "Initializing sensor A..." << std::endl;
//     i = tofSetAddress(IIC0,0x29, sensorA_address);
//     if (i != 0) {
//         std::cerr << "Failed to set sensor A address" << std::endl;
//         return -1;
//     }
//     i = tofPing(IIC0, sensorA_address);
//     if (i != 0) {
//         std::cerr << "Failed to ping sensor A" << std::endl;
//         return -1;
//     }
//     i = tofInit(&sensorA, IIC0, sensorA_address, 0);
//     if (i != 0)
//     {
//         std::cerr << "Failed to initialize sensor A" << std::endl;
//         return -1;
//     }

//     uint8_t model, revision;

//     tofGetModel(&sensorA, &model, &revision);
//     std::cout << "---Model ID - " << static_cast<int>(model) << std::endl;
//     std::cout << "---Revision ID - " << static_cast<int>(revision) << std::endl;
//     std::cout << "---Init: Success" << std::endl;
//     std::cout.flush();

//     std::cout << std::endl << std::endl << "Now Power Sensor B!" << std::endl << std::endl;
//     std::cout << "Press Enter to continue..." << std::endl;
//     std::cin.get();

//     std::cout << "Initializing sensor B..." << std::endl;
//     // i = tofSetAddress(IIC0,0x29, sensorB_address);
//     // if (i != 0) {
//     //     std::cerr << "Failed to set sensor B address" << std::endl;
//     //     return -1;
//     // }
//     i = tofPing(IIC0, sensorB_address);
//     if (i != 0) {
//         std::cerr << "Failed to ping sensor B" << std::endl;
//         return -1;
//     }
//     i = tofInit(&sensorB, IIC0, sensorB_address, 0);
//     if (i != 0)
//     {
//         std::cerr << "Failed to initialize sensor B" << std::endl;
//         return -1;
//     }

//     tofGetModel(&sensorB, &model, &revision);
//     std::cout << "---Model ID - " << static_cast<int>(model) << std::endl;
//     std::cout << "---Revision ID - " << static_cast<int>(revision) << std::endl;
//     std::cout << "---Init: Success" << std::endl;
//     std::cout.flush();

//     return 0;
// }


// DistanceSensor DistanceSensor::getDistance()
// {
//     DistanceSensor distanceSensor;
//     int i;
//     uint16_t distanceA, distanceB;
//     i = tofReadDistance(&sensorA, &distanceA);
//     if (i != 0)
//     {
//         std::cerr << "Failed to read distance from sensor A" << std::endl;
//         return distanceSensor;
//     }
//     i = tofReadDistance(&sensorB, &distanceB);
//     if (i != 0)
//     {
//         std::cerr << "Failed to read distance from sensor B" << std::endl;
//         return distanceSensor;
//     }

//     distanceSensor.sensorA = distanceA;
//     distanceSensor.sensorB = distanceB;

//     return distanceSensor;
// }