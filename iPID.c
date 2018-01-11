#include "iPID.h"				//we use iPID

#define iPID_MAX        (1l<<30)   //max / min value for error - ***MUST*** be a signed type
#define iPID_MIN        (-iPID_MAX)   //max / min value for error

//initialize iPID
void iPID_init(iPID_TypeDef *pid, int32_t Kp, int32_t Ki, int32_t Kd) {
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->ei = pid->di = pid->sumi = 0;
}

//calculate action value
int32_t iPID_calc(iPID_TypeDef *pid, int32_t tgt, int32_t val) {
    int32_t ei;

    ei = tgt - val;                 //update ei
    //update sum
    pid->sumi += ei;                //update sumi
    //bound sumi to [iPID_MIN, iPID_MAX]
    pid->sumi = (pid->sumi > iPID_MAX)?iPID_MAX:((pid->sumi < iPID_MIN)?(iPID_MIN):pid->sumi);
    //if (sumi > iPID_MAX) sumi = iPID_MAX; else if (sumi < iPID_MIN) sumi = iPID_MIN;
    pid->di = ei - pid->ei;         //update di
    pid->ei = ei;                   //update ei
    pid->pid = pid->Kp * pid->ei + pid->Kd * pid->di + pid->Ki * pid->sumi;
    return pid->pid;                //no boundary check for pid output
}
