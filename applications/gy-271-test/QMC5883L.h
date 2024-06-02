#ifndef QMC5883L_H
#define QMC5883L_H

#include <stdint.h>

typedef struct {
    int16_t xhigh, xlow;
    int16_t yhigh, ylow;
    uint8_t addr;
    uint8_t mode;
    uint8_t rate;
    uint8_t range;
    uint8_t oversampling;
} QMC5883L;

void QMC5883L_init(QMC5883L* qmc);
void QMC5883L_reset(QMC5883L* qmc);
int QMC5883L_ready(QMC5883L* qmc);
void QMC5883L_reconfig(QMC5883L* qmc);

int QMC5883L_readHeading(QMC5883L* qmc);
int QMC5883L_readRaw(QMC5883L* qmc, int16_t *x, int16_t *y, int16_t *z, int16_t *t);

void QMC5883L_resetCalibration(QMC5883L* qmc);

void QMC5883L_setSamplingRate(QMC5883L* qmc, int rate);
void QMC5883L_setRange(QMC5883L* qmc, int range);
void QMC5883L_setOversampling(QMC5883L* qmc, int ovl);

#endif
