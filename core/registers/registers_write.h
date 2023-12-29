#pragma once

#include "registers_core.h"

// ====================================================================
// ========== Command list (PC to ComfoAir) / write commands ==========
// ====================================================================

/*
Data: 1 byte
Byte[1] = 0x00 = Auto
          0x01 = Absent
          0x02 = low / level 1
          0x03 = medium / level 2
          0x04 = high / level 3

Response: ACK
*/
#define CMD_SET_LEVEL 0x99
#define RES_SET_LEVEL ACK

/*
Data: 19 bytes
Byte[1] = Analogously present: (1 = present / 0 = absent)
    0x01 = Analog 1
    0x02 = Analog 2
    0x04 = Analog 3
    0x08 = Analog 4
    0x10 = RF
Byte[2] = Regulate / control analogously: (1 = regulate / 0 = control)
    0x01 = Analog 1
    0x02 = Analog 2
    0x04 = Analog 3
    0x08 = Analog 4
    0x10 = RF
Byte[3] = Analogue positive / negative: (1 = negative / 0 = positive)
    0x01 = Analog 1
    0x02 = Analog 2
    0x04 = Analog 3
    0x08 = Analog 4
    0x10 = RF
Byte[4] = Analog 1 Min. Setting (%)
Byte[5] = Analog 1 Max. Setting (%)
Byte[6] = Analog 1 setpoint (%)
Byte[7] = Analogue 2 min. setting (%)
Byte[8] = Analog 2 Max. Setting (%)
Byte[9] = Analog 2 setpoint (%)
Byte[10] = Analogue 3 min. setting (%)
Byte[11] = Analog 3 Max. Setting (%)
Byte[12] = Analog 3 setpoint (%)
Byte[13] = Analogue 4 min. setting (%)
Byte[14] = Analog 4 Max Setting (%)
Byte[15] = Analog 4 setpoint (%)
Byte[16] = Analog RF Min. Setting (%)
Byte[17] = Analog RF Max Setting (%)
Byte[18] = Analog RF setpoint (%)
Byte[19] = Priority control (0 = analog inputs / 1 = weekly program)

Response: ACK
*/
#define CMD_SET_ANALOG_VALUES 0x9F
#define RES_SET_ANALOG_VALUES ACK

/*
Data: 8 bytes
Byte[1] = Bathroom switch switch-on delay (min)
Byte[2] = Bathroom switch switch-off delay (min)
Byte[3] = L1 switch-off delay (min)
Byte[4] = Shock ventilation (min)
Byte[5] = Filter counter (weeks)
Byte[6] = RF high time short (min)
Byte[7] = RF high time long (min)
Byte[8] = Kitchen hood switch-off delay (min)

Response: ACK
*/
#define CMD_SET_TIME_DELAY 0xCB
#define RES_SET_TIME_DELAY ACK

/*
Data: 9 bytes
Byte[1] = Exhaust air absent (%)
Byte[2] = Exhaust air low / level 1 (%)
Byte[3] = Exhaust air medium / level 2 (%)
Byte[4] = Supply air level absent (%)
Byte[5] = Supply air low / level 1 (%)
Byte[6] = Supply air medium / level 2 (%)
Byte[7] = Exhaust air high / level 3 (%)
Byte[8] = Supply air high / level 3 (%)
Byte[9] =

Response: ACK
*/
#define CMD_SET_VENTINATION_LEVEL 0xCF
#define RES_SET_VENTINATION_LEVEL ACK

/*
Data: 1 byte
Byte[1] = Comfort temperature (°C*)

Response: ACK
*/
#define CMD_SET_COMFORT_TEMPERATURE 0xD3
#define RES_SET_COMFORT_TEMPERATURE ACK

/*
Data: 8 bytes
Byte[1] = Preheater present (1 = present / 0 = absent)
Byte[2] = Bypass present (1 = present / 0 = absent)
Byte[3] = Type (1 = left / 2 = right)
Byte[4] = Size (1 = large / 2 = small)
Byte[5] = Options: (1 = present / 0 = absent)
    0x01 = fireplace
    0x02 = kitchen hood
    0x04 = post-heating
    0x40 = Afterheating PWM mode
    0x80 =
Byte[6] = 0x00
Byte[7] = Enthalpy present (1 = present / 0 = absent / 2 = without Sensor)
Byte[8] = EWT present (1 = regulated / 0 = absent / 2 = unregulated)

Response: ACK
*/
#define CMD_SET_STATUS 0xD7
#define RES_SET_STATUS ACK

/*
Data: 4 bytes
Byte[1] = Reset faults (1 = reset / 0 = -)
Byte[2] = Reset settings (1 = reset / 0 = -)
Byte[3] = Start self-test (1 = start / 0 = -)
Byte[4] = Reset filter operating hours (1 = reset / 0 = -)

Response: ACK
*/
#define CMD_RESET_AND_SELF_TEST 0xDB
#define RES_RESET_AND_SELF_TEST ACK

/*
Data: 5 bytes
Byte[1] = EWT low (°C)
Byte[2] = EWT high (°C)
Byte[3] = EWT speed up (%)
Byte[4] = Kitchen hood speed up (%)
Byte[5] = Reheating T desired (°C)

*  Value sent is (temperature + 20) * 2
** Sent value is 1875000 / (RPM)

Response: ACK
*/
#define CMD_SET_EWT_POSTHEATING 0xED
#define RES_SET_EWT_POSTHEATING ACK