#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdint.h>

typedef struct s_StepperThread {
    uint16_t angle;
    uint16_t stepCount;
    int16_t speed;

} s_StepperThread;


#endif /* STRUCTS_H */