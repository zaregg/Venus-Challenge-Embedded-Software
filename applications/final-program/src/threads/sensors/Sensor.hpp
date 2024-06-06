#ifndef SENSOR_H
#define SENSOR_H

#include <boost/lockfree/queue.hpp>
#include <memory>
#include <thread>
#include <functional>

class SensorBase {
public:
    virtual ~SensorBase() = default;
    // virtual void start() = 0;
    virtual void stop() = 0;
};

// Abstract base class for sensors
template<typename T>
class Sensor : public SensorBase {
public:
    using QueueType = boost::lockfree::queue<std::function<void()>*>;

    Sensor() = default;
    virtual ~Sensor() = default;

    virtual void start(std::thread& thread, QueueType* managerToSensorQueue, QueueType* sensorToManagerQueue) = 0;
    // virtual void stop() = 0;
    // virtual void readData() = 0;
    void setDataQueue(boost::lockfree::queue<T>& dataQueue);

protected:
    void sendData(const T& data) {
        if (dataQueue_) {
            dataQueue_->push(data);
        }
    }

private:
    boost::lockfree::queue<T>* dataQueue_;
};

template<typename T>
void Sensor<T>::setDataQueue(boost::lockfree::queue<T>& dataQueue) {
    dataQueue_ = &dataQueue;
}

#endif // SENSOR_H
