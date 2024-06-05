#ifndef SENSOR_H
#define SENSOR_H

#include <boost/lockfree/queue.hpp>

// Abstract base class for sensors
template<typename T>
class Sensor {
public:
    virtual ~Sensor() {}
    virtual void start(boost::lockfree::queue<std::function<void()>*>* managerToSensorQueue,
                       boost::lockfree::queue<std::function<void()>*>* sensorToManagerQueue) = 0;
    virtual void stop() = 0;
    virtual void readData() = 0;
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
