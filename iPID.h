#ifndef iPID_H
#define iPID_H

#include <stdint.h>

//iPID
typedef struct {
    int32_t Kp, Ki, Kd;         //gain
    int32_t ei;                 //current error
    int32_t di;                 //difference of errors
    int32_t sumi;               //sum of errors
    int32_t pid;                //action
} iPID_TypeDef;

//#define iPID_MAX        (1l<<30)   //max / min value for error - ***MUST*** be a signed type
//#define iPID_MIN        (-iPID_MAX)   //max / min value for error

//initialize iPID
void iPID_init(iPID_TypeDef *pid, int32_t Kp, int32_t Ki, int32_t Kd);

//calculate action value
int32_t iPID_calc(iPID_TypeDef *pid, int32_t tgt, int32_t val);

#endif
