#include <math.h>
#include <libpynq.h>
#include "QMC5883L.h"

#define QMC5883L_ADDR 0x0D

#define QMC5883L_X_LSB 0
#define QMC5883L_X_MSB 1
#define QMC5883L_Y_LSB 2
#define QMC5883L_Y_MSB 3
#define QMC5883L_Z_LSB 4
#define QMC5883L_Z_MSB 5
#define QMC5883L_STATUS 6
#define QMC5883L_TEMP_LSB 7
#define QMC5883L_TEMP_MSB 8
#define QMC5883L_CONFIG 9
#define QMC5883L_CONFIG2 10
#define QMC5883L_RESET 11
#define QMC5883L_RESERVED 12
#define QMC5883L_CHIP_ID 13

#define QMC5883L_STATUS_DRDY 1
#define QMC5883L_STATUS_OVL 2
#define QMC5883L_STATUS_DOR 4

#define QMC5883L_CONFIG_OS512 0b00000000
#define QMC5883L_CONFIG_OS256 0b01000000
#define QMC5883L_CONFIG_OS128 0b10000000
#define QMC5883L_CONFIG_OS64  0b11000000

#define QMC5883L_CONFIG_2GAUSS 0b00000000
#define QMC5883L_CONFIG_8GAUSS 0b00010000

#define QMC5883L_CONFIG_10HZ   0b00000000
#define QMC5883L_CONFIG_50HZ   0b00000100
#define QMC5883L_CONFIG_100HZ  0b00001000
#define QMC5883L_CONFIG_200HZ  0b00001100

#define QMC5883L_CONFIG_STANDBY 0b00000000
#define QMC5883L_CONFIG_CONT    0b00000001

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif

// static void write_register(int addr, int reg, int value) {
// //     Wire.beginTransmission(addr);
// //     Wire.write(reg);
// //     Wire.write(value);
// //     Wire.endTransmission();
// }
static void write_register(int addr, int reg, int value) {
    iic_write_register(IIC0, (uint8_t) addr, (uint8_t) reg, (uint8_t *) &value, (uint16_t) 1);
}

// static int read_register(int addr, int reg, int count) {
//     // Wire.beginTransmission(addr);
//     // Wire.write(reg);
//     // Wire.endTransmission();
  
//     // Wire.requestFrom(addr, count);
//     // int n = Wire.available();
//     // if(n != count) return 0;

//     // return n;
//     return 1;
// }

static int read_register(int addr, int reg, int count) {
    int buffer[count];
    return iic_read_register(IIC0, (uint8_t) addr, (uint8_t) reg, (uint8_t *) buffer, (uint16_t) count);
    
}

void QMC5883L_init(QMC5883L* qmc) {
    qmc->addr = QMC5883L_ADDR;
    qmc->oversampling = QMC5883L_CONFIG_OS512;
    qmc->range = QMC5883L_CONFIG_2GAUSS;
    qmc->rate = QMC5883L_CONFIG_50HZ;
    qmc->mode = QMC5883L_CONFIG_CONT;
    QMC5883L_reset(qmc);
}

void QMC5883L_reset(QMC5883L* qmc) {
    write_register(qmc->addr, QMC5883L_RESET, 0x01);
    write_register(qmc->addr, QMC5883L_CONFIG, qmc->oversampling | qmc->range | qmc->rate | qmc->mode);
    QMC5883L_resetCalibration(qmc);
}

int QMC5883L_ready(QMC5883L* qmc) {
    // if(!read_register(qmc->addr, QMC5883L_STATUS, 1)) return 0;
    int status = !read_register(qmc->addr, QMC5883L_STATUS, 1);
    // printf("status: %d\n", status);
    // uint8_t status = Wire.read();
    return status & QMC5883L_STATUS_DRDY;
}

int QMC5883L_readRaw(QMC5883L* qmc, int16_t *x, int16_t *y, int16_t *z, int16_t *t) {
    while(!QMC5883L_ready(qmc)) {}

    // if(!read_register(qmc->addr, QMC5883L_X_LSB, 6)) return 0;
    uint8_t buffer[6];
    if (iic_read_register(IIC0, qmc->addr, QMC5883L_X_LSB, buffer, 6)) {
        return 0;
    }

    *x = buffer[0] | (buffer[1] << 8);
    *y = buffer[2] | (buffer[3] << 8);
    *z = buffer[4] | (buffer[5] << 8);

    // printf("x: %d, y: %d, z: %d\n", *x, *y, *z);

    // *x = Wire.read() | (Wire.read() << 8);
    // *y = Wire.read() | (Wire.read() << 8);
    // *z = Wire.read() | (Wire.read() << 8);

    return 1;
}

void QMC5883L_resetCalibration(QMC5883L* qmc) {
    qmc->xhigh = qmc->yhigh = 0;
    qmc->xlow = qmc->ylow = 0;
}

int QMC5883L_readHeading(QMC5883L* qmc) {
    int16_t x, y, z, t;

    if(!QMC5883L_readRaw(qmc, &x, &y, &z, &t)) return 0;

    if(x < qmc->xlow) qmc->xlow = x;
    if(x > qmc->xhigh) qmc->xhigh = x;
    if(y < qmc->ylow) qmc->ylow = y;
    if(y > qmc->yhigh) qmc->yhigh = y;

    // printf("xlow: %d, xhigh: %d, ylow: %d, yhigh: %d\n", qmc->xlow, qmc->xhigh, qmc->ylow, qmc->yhigh);

    if(qmc->xlow == qmc->xhigh || qmc->ylow == qmc->yhigh) return 0;

    // printf("x: %d, y: %d\n", x, y);

    x -= (qmc->xhigh + qmc->xlow) / 2;
    y -= (qmc->yhigh + qmc->ylow) / 2;

    float fx = (float)x / (qmc->xhigh - qmc->xlow);
    float fy = (float)y / (qmc->yhigh - qmc->ylow);

    int heading = 180.0 * atan2(fy, fx) / M_PI;
    if(heading <= 0) heading += 360;

    return heading;
}

void QMC5883L_setSamplingRate(QMC5883L* qmc, int rate) {
    switch(rate){
        case 10:
            qmc->rate = QMC5883L_CONFIG_10HZ;
            break;
        case 50:
            qmc->rate = QMC5883L_CONFIG_50HZ;
            break;
        case 100:
            qmc->rate = QMC5883L_CONFIG_100HZ;
            break;
        case 200:
            qmc->rate = QMC5883L_CONFIG_200HZ;
            break;
  }
  QMC5883L_reconfig(qmc);
}

void QMC5883L_setRange(QMC5883L* qmc, int range) {
    qmc->range = range;
    QMC5883L_reset(qmc);
}

void QMC5883L_setOversampling(QMC5883L* qmc, int ovl) {
    qmc->oversampling = ovl;
    QMC5883L_reset(qmc);
}

void QMC5883L_reconfig(QMC5883L* qmc) {
    write_register(qmc->addr,QMC5883L_CONFIG,qmc->oversampling|qmc->range|qmc->rate|qmc->mode);  
}
