#pragma once

#include "registers_core.h"

// ======================================================================
// ========== Command list (PC to ComfoAir) / reading commands ==========
// ======================================================================

/*
Data: -

Response: 2 bytes
Byte[1] = Step switch: (1 = active / 0 = inactive)
    0x01 = L1
    0x02 = L2

Byte[2] = Switching inputs: (1 = active / 0 = inactive)
    0x01 = bathroom switch
    0x02 = kitchen hood switch
    0x04 = External filter
    0x08 = heat recovery (WTW)
    0x10 = bathroom switch 2 (luxe)
*/
#define CMD_GET_INPUTS 0x03
#define RES_GET_INPUTS 0x04

/*
Data: -

Response: 6 bytes
Byte[1] = Supply air (%)
Byte[2] = Exhaust air (%)
Byte[3..4] = Supply air fan speed (rpm**)
Byte[5..6] = Exhaust air fan speed (rpm**)
*/
#define CMD_GET_FAN_STATUS 0x0B
#define RES_GET_FAN_STATUS 0x0C

/*
Data: -

Response: 4 bytes
Byte[1] = Bypass (%) (0xFF = undefined)
Byte[2] = Preheating (1 = Open / 0 = Closed / 2 = Unknown)
Byte[3] = Bypass motor current (ADC raw data)
Byte[4] = Preheating motor current (ADC raw data)
*/
#define CMD_GET_VALVE_STATUS 0x0D
#define RES_GET_VALVE_STATUS 0x0E

/*
Data: -

Response: 4 bytes
Byte[1] = T1 / outside air (°C*)
Byte[2] = T2 / supply air (°C*)
Byte[3] = T3 / exhaust air (in) (°C*)
Byte[4] = T4 / exhaust air (out) (°C*)
*/
#define CMD_GET_TEMPERATURE_STATUS 0x0F
#define RES_GET_TEMPERATURE_STATUS 0x10

/*
Data: -

Response: 1 byte
0x00 = Nothing pressed
0xFF = Error
*/
#define CMD_GET_BUTTON_STATUS 0x11
#define RES_GET_BUTTON_STATUS 0x12

/*
Data: -

Response: 4 bytes
Byte[1] = Analog 1 (0..255 = 0..10V)
Byte[2] = Analog 2 (0..255 = 0..10V)
Byte[3] = Analog 3 (0..255 = 0..10V)
Byte[4] = Analog 4 (0..255 = 0..10V)
*/
#define CMD_GET_ANALOG_INPUTS 0x13
#define RES_GET_ANALOG_INPUTS 0x14

/*
Data: -

Response: 17 bytes
Byte[1] = Enthalpie Sensor Temperatur (°C*)
Byte[2] = Enthalpie Sensor Feuchtigkeit (%)
Byte[3] = Analog 1 (%)
Byte[4] = Analog 2 (%)
Byte[5] = Enthalpie Koeffizient (%)
Byte[6] = Enthalpie Timer (0..240 = 0..2880 Min)
Byte[7] = 0x00
Byte[8] = Analog 1 zu gewünscht (%)
Byte[9] = Analog 1 ab gewünscht (%)
Byte[10] = Analog 2 zu gewünscht (%)
Byte[11] = Analog 2 ab gewünscht (%)
Byte[12] = Analog 3 (%)
Byte[13] = Analog 4 (%)
Byte[14] = Analog 3 zu gewünscht (%)
Byte[15] = Analog 3 ab gewünscht (%)
Byte[16] = Analog 4 zu gewünscht (%)
Byte[17] = Analog 4 ab gewünscht (%)
*/
#define CMD_GET_SENSOR_DATA 0x97
#define RES_GET_SENSOR_DATA 0x98

/*
Data: -

Response: 19 bytes
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
*/
#define CMD_GET_ANALOG_VALUES 0x9D
#define RES_GET_ANALOG_VALUES 0x9E

/*
Data: -

Response: 8 bytes
Byte[1] = Bathroom switch switch-on delay (min)
Byte[2] = Bathroom switch switch-off delay (min)
Byte[3] = L1 switch-off delay (min)
Byte[4] = Shock ventilation (min)
Byte[5] = Filter counter (weeks)
Byte[6] = RF high time short (min)
Byte[7] = RF high time long (min)
Byte[8] = Kitchen hood switch-off delay (min)
*/
#define CMD_GET_TIME_DELAY 0xC9
#define RES_GET_TIME_DELAY 0xCA

/*
Data: -

Response: 14 bytes
Byte[1] = Exhaust air absent (%)
Byte[2] = Exhaust air low / level 1 (%)
Byte[3] = Exhaust air medium / level 2 (%)
Byte[4] = Supply air level absent (%)
Byte[5] = Supply air low / level 1 (%)
Byte[6] = Supply air medium / level 2 (%)
Byte[7] = Current exhaust air (%)
Byte[8] = Current supply air (%)
Byte[9] = Current level (see command 0x00 0x99)
Byte[10] = Supply air fan active (1 = active / 0 = inactive)
Byte[11] = Exhaust air high / level 3 (%)
Byte[12] = Supply air high / level 3 (%)
Byte[13] = ..
Byte[14] = ..
*/
#define CMD_GET_VENTILATION_LEVEL 0xCD
#define RES_GET_VENTILATION_LEVEL 0xCE

/*
Data: -

Response: 9 bytes
Byte[1] = Comfort temperature (°C*)
Byte[2] = T1 / outside air (°C*)
Byte[3] = T2 / supply air (°C*)
Byte[4] = T3 / exhaust air (in) (°C*)
Byte[5] = T4 / exhaust air (out) (°C*)
Byte[6] = Sensor present: (1 = present / 0 = absent)
    0x01 = T1 / outside air
    0x02 = T2 / supply air
    0x04 = T3 / exhaust air (in)
    0x08 = T4 / exhaust air (out)
    0x10 = EWT
    0x20 = post-heating
    0x40 = kitchen hood
Byte[7] = Temperature EWT (°C*)
Byte[8] = Temperature afterheating (°C*)
Byte[9] = Temperature kitchen hood (°C*)
*/
#define CMD_GET_TEMPERATURES 0xD1
#define RES_GET_TEMPERATURES 0xD2

/*
Data: -

Response: 11 bytes
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
Byte[7] = Active status 1:
    0x01 = P10
    :
    0x80 = P17
Byte[8] = Active status 2:
    0x01 = P18
    0x02 = P19
Byte[9] = Active status 3:
    0x01 = P90
    :
    0x40 = P96
Byte[10] = Enthalpy present (1 = present / 0 = absent / 2 = without Sensor)
Byte[11] = EWT present (1 = regulated / 0 = absent / 2 = unregulated)

*/
#define CMD_GET_STATUS 0xD5
#define RES_GET_STATUS 0xD6

/*
Data: -

Response: 17 bytes
Byte[1] = Current error A:
    0x01 = A1
    :
    0x80 = A8
Byte[2] = Current error E:
    0x01 = E1
    :
    0x80 = E8
Byte[3] = Last mistake A
    0x01 = A1
    :
    0x80 = A8
Byte[4] = Last mistake E
    0x01 = E1
    :
    0x80 = E8
Byte[5] = Penultimate mistake A
    0x01 = A1
    :
    0x80 = A8
Byte[6] = Penultimate error E
    0x01 = E1
    :
    0x80 = E8
Byte[7] = Penultimate error A
    0x01 = A1
    :
    0x80 = A8
Byte[8] = Penultimate error E
    0x01 = E1
    :
    0x80 = E8
Byte[9] = 0x00 = Filter OK
          0x01 = Filter full
Byte[10] = Current error EA:
    0x01 = EA1
    :
    0x80 = EA8
Byte[11] = Last error EA:
    0x01 = EA1
    :
    0x80 = EA8
Byte[12] = Second to last error EA:
    0x01 = EA1
    :
    0x80 = EA8
Byte[13] = Last but one error EA:
    0x01 = EA1
    :
    0x80 = EA8
Byte[14] = Current error A (high):
    0x01 = A9
    :
    0x40 = A15
    0x80 = A0
Byte[15] = Last error A (high):
    0x01 = A9
    :
    0x40 = A15
    0x80 = A0
Byte[16] = Second to last error A (high):
    0x01 = A9
    :
    0x40 = A15
    0x80 = A0
Byte[17] = Last but one error A (high):
    0x01 = A9
    :
    0x40 = A15
    0x80 = A0
*/
#define CMD_GET_FAULTS 0xD9
#define RES_GET_FAULTS 0xDA

/*
Data: -

Response: 20 bytes
Bytes[1-3] = Operating hours absent (h) (Byte[3] = Low Byte)
Bytes[4-6] = Operating hours low / level 1 (h) (Byte[6] = Low Byte)
Bytes[7-9] = Operating hours medium / level 2 (h) (Byte[9] = Low Byte)
Bytes[10-11] = Operating hours of frost protection (h) (Byte[11] = Low Byte)
Bytes[12-13] = Operating hours preheating (h) (Byte[13] = Low Byte)
Bytes[14-15] = Operating hours bypass open (h) (Byte[15] = Low Byte)
Bytes[16-17] = Filter operating hours (h) (Byte[17] = Low Byte)
Bytes[18-20] = Operating hours high / level 3 (h) (Byte[20] = Low Byte)
*/
#define CMD_GET_OPERATION_HOURS 0xDD
#define RES_GET_OPERATION_HOURS 0xDE

/*
Data: -

Response: 7 bytes
Byte[1] = 0x00
Byte[2] = 0x00
Byte[3] = Bypass factor
Byte[4] = Bypass stage
Byte[5] = Bypass correction
Byte[6] = 0x00
Byte[7] = Summer mode (1 = yes / 0 = no (winter))
*/
#define CMD_GET_BYPASS_CONTROL_STATUS 0xDF
#define RES_GET_BYPASS_CONTROL_STATUS 0xE0

/*
Data: -

Response: 6 bytes
Byte[1] = Status valve (1 = open / 0 = closed / 2 = unknown)
Byte[2] = Frost protection (1 = active / 0 = inactive)
Byte[3] = Preheating (1 = active / 0 = inactive)
Byte[4..5] = Frost minutes (min)
Byte[6] = Frost protection (1 = extra safe / 4 = safe)
*/
#define CMD_GET_PREHEATING_STATUS 0xE1
#define RES_GET_PREHEATING_STATUS 0xE2

/*
Data: -

Response: 7 bytes
Byte[1] = RF address 4 (low byte)
Byte[2] = RF address 3
Byte[3] = RF address 2
Byte[4] = RF address 1 (high byte)
Byte[5] = RF ID
Byte[6] = Module present
Byte[7] = Self-learning mode active
*/
#define CMD_GET_RF_STATUS 0xE5
#define RES_GET_RF_STATUS 0xE6

/*
Data: -

Response: 8 bytes
Byte[1] = Oldest value (°C)
Byte[2] =
Byte[3] =
Byte[4] =
Byte[5] =
Byte[6] =
Byte[7] =
Byte[8] = Latest value (°C)
*/
#define CMD_GET_LAST_8_TIMES_PREHEATING 0xE9
#define RES_GET_LAST_8_TIMES_PREHEATING 0xEA

/*
Data: -

Response: 7 byets
Byte[1] = EWT low (°C)
Byte[2] = EWT high (°C)
Byte[3] = EWT speed up (%)
Byte[4] = Kitchen hood speed up (%)
Byte[5] = Post-heating performance
Byte[6] = Post-heating power I parameter
Byte[7] = Reheating T desired (°C)
*/
#define CMD_EWT_POST_HEATING 0xEB
#define RES_EWT_POST_HEATING 0xEC