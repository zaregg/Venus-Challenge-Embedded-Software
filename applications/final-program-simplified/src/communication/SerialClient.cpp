#include "SerialClient.hpp"

SerialClient::SerialClient(ThreadParams& params)
    : params_(params)
{
  switchbox_set_pin(IO_AR0, SWB_UART0_RX);
  switchbox_set_pin(IO_AR1, SWB_UART0_TX);
}

SerialClient::~SerialClient()
{
    
}

void SerialClient::start()
{
  if (!params_.comParams.comRunning.load(std::memory_order_acquire)) {
    params_.comParams.comRunning.store(true, std::memory_order_release);
    // Open the serial port
    uart_init(UART0);

    uart_reset_fifos(UART0);
    
    // Start the thread
    comThread_ = std::thread(&SerialClient::communicationThread, this);
  }
}

void SerialClient::stop()
{
  params_.comParams.comRunning.store(false, std::memory_order_release);
  join();
}

void SerialClient::join()
{
  if(comThread_.joinable())
  {
    comThread_.join();
  }
}

void SerialClient::uart_read_array(const int uart, uint8_t *buf, uint8_t l) {
  for (uint8_t x = 0; x < l; x++) {
    buf[x] = uart_recv(uart);
  }
}

void SerialClient::uart_write_array(const int uart, const uint8_t *buf, uint8_t l) {
  for (uint8_t x = 0; x < l; x++) {
    uart_send(uart, buf[x]);
  }
}

void SerialClient::parseData(std::string& json_str)
{
  try {
    auto root = nlohmann::json::parse(json_str);

    int16_t l = 0, r = 0;

    if (root.contains("left")) {
      l = root["left"].get<int16_t>();
    }
        
    if (root.contains("right")) {
      r = root["right"].get<int16_t>();
    }
    
    if (root.contains("lspeed") && root.contains("rspeed")) {
      uint16_t s = root["lspeed"].get<uint16_t>();
      uint16_t sr = root["rspeed"].get<uint16_t>();
      // stepper_set_speed(s, sr);
    }
    
    std::cout << l << " " << r << std::endl;
    // stepper_steps(l, r);
  } catch (nlohmann::json::parse_error& e) {
    std::cerr << "Failed to parse JSON: " << e.what() << std::endl;
  }
}

void SerialClient::communicationThread()
{
  std::cout << "Communication thread started" << std::endl;
  std::cout << "comRunning: " << params_.comParams.comRunning.load(std::memory_order_acquire) << "\n";
  while (params_.comParams.comRunning.load(std::memory_order_acquire))
  {
    // Receiving
    if(uart_has_data(UART0))
    {
      std::cout << "Data available" << std::endl;
      uint32_t size = 0;
      uart_read_array(UART0, (uint8_t *) &size, 4);
      char array[size];
      uart_read_array(UART0, (uint8_t *) &array, (uint8_t) size);
      std::string message(array, size);
      std::cout << "Received: " << message << std::endl;
    }

    // Transmitting
    SensorData data;
    if (params_.sensorQueue.pop(data))
    {
      // nlohmann::json data_json;

      // Populate the JSON object with data
      // data_json["type"] = "robot";
      // data_json["x"] = rand() % 101 - 50;
      // data_json["y"] = rand() % 101 - 50;
      // data_json["tag"] = "delete";

      // std::string json_str = data_json.dump();
      // std::cout << "Sending: " << json_str << std::endl;
      // uint32_t size = json_str.size();
      // uart_write_array(UART0, (uint8_t *) &size, 4);
      // uart_write_array(UART0, (uint8_t *) json_str.c_str(), (uint8_t) size);
    }

    MotorToComData receivedData;
    if (params_.motorToComQueue.pop(receivedData))
    {

      std::string json_str = receivedData.toJson().dump();
      std::cout << "Sending: " << json_str << std::endl;
      uint32_t size = json_str.size();
      uart_write_array(UART0, (uint8_t *) &size, 4);
      uart_write_array(UART0, (uint8_t *) json_str.c_str(), (uint8_t) size);
    }
  }
}