#ifndef __CONSTANTS_H_INCLUDED
#define __CONSTANTS_H_INCLUDED


//////////////////////////////////////////////////
// Heartbeat constants

#define DEFAULT_HEARTBEAT_PERIOD	60

#define MIN_HEARTBEAT_PERIOD		5
#define MAX_HEARTBEAT_PERIOD		600

#define INVALID_HEARTBEAT_PERIOD(__VAL__) \
    ((__VAL__) > MAX_HEARTBEAT_PERIOD || (__VAL__) < MIN_HEARTBEAT_PERIOD)
#define ADJUST_HEARTBEAT_PERIOD(__VAL__) \
    if (INVALID_HEARTBEAT_PERIOD(__VAL__)) __VAL__ = DEFAULT_HEARTBEAT_PERIOD;



#endif // __CONSTANTS_H_INCLUDED
