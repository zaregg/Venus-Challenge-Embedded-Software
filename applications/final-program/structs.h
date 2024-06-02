#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdint.h>

typedef struct s_StepperThread {
    uint8_t angle;
    uint8_t direction;
    uint8_t speed;

} s_StepperThread;


#endif /* STRUCTS_H */