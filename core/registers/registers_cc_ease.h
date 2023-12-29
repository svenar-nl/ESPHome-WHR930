#pragma once

#include "registers_core.h"

// ========================================================
// ========== Command list (CC-Ease to ComfoAir) ==========
// ========================================================

/*
Data: -

Response: -

See ComfoAir command list on CC-Ease
*/
#define CMD_GET_DATA 0x33
#define RES_GET_DATA_1 0x38
#define RES_GET_DATA_2 0x3E
#define RES_GET_DATA_3 0x40
#define RES_GET_DATA_4 0x98
#define RES_GET_DATA_5 0x9C
#define RES_GET_DATA_6 0xAA
#define RES_GET_DATA_7 0xCE
#define RES_GET_DATA_8 0xD2
#define RES_GET_DATA_9 0xE0
#define RES_GET_DATA_10 0xE2
#define RES_GET_DATA_11 0xEC

/*
Data: 5 bytes
Byte[1] = RTC day
    0x00 = Saturday
    0x01 = Sunday
    0x02 = Monday
    0x03 = Tuesday
    0x04 = Wednesday
    0x05 = Thursday
    0x06 = Friday
Byte[2] = RTC hour (0..23)
Byte[3] = RTC minutes (0..59)
Byte[4] = Temperature (°C*)
Byte[5] = CC Ease version
    Bit 7..4 = Version Major
    Bit 3..0 = Version Minor

* Value sent is (temperature + 20) * 2

Response: -
See ComfoAir command list on CC-Ease

*/
#define CMD_GET_CC_EASE_PARAMETERS 0x35
#define RES_GET_CC_EASE_PARAMETERS 0x3C

/*
Data: 7 bytes
Byte[1] Fan (0..255 = 0..4080 milliseconds)
Byte[2] Mode (0..255 = 0..4080 milliseconds)
Byte[3] Clock (0..255 = 0..4080 milliseconds)
Byte[4] Temperature (0..255 = 0..4080 milliseconds)
Byte[5] Plus (0..255 = 0..4080 milliseconds)
Byte[6] Minus (0..255 = 0..4080 milliseconds)
Byte[7] Status bits
    0x01 = Flashing on/off

Response: -
See ComfoAir command list on CC-Ease
*/
#define CMD_CCEASE_BUTTON_STATUS 0x37
#define RES_CCEASE_BUTTON_STATUS 0x3C

/*
Data: 16 bytes
Byte[1] = Module type receiver
Byte[2] = Module ID receiver
Byte[3] = Module type transmitter
Byte[4] = Module ID transmitter
Byte[5] = Lifetime
Byte[6] = Data type
Byte[7] = Data byte 1
Byte[8] = Data byte 2
Byte[9] = Data byte 3
Byte[10] = Data byte 4
Byte[11] = Data byte 5
Byte[12] = Data byte 6
Byte[13] = Data byte 7
Byte[14] = Data byte 8
Byte[15] = Data byte 9
Byte[16] = Data byte 10

Response: -
See ComfoAir command list on CC-Ease
*/
#define CMD_GET_RF_COMMAND 0x39
#define RES_GET_RF_COMMAND 0x40

// ========================================================
// ========== Command list (ComfoAir to CC-Ease) ==========
// ========================================================

/*
Data: 5 bytes
Byte[1] = RTC day
    0x00 = Saturday
    0x01 = Sunday
    0x02 = Monday
    0x03 = Tuesday
    0x04 = Wednesday
    0x05 = Thursday
    0x06 = Friday
Byte[2] = RTC hour (0..23)
Byte[3] = RTC minutes (0..59)
Byte[4] = Backlight timeout (Fixed at 30)
Byte[5] = Backlight (fixed at 100%)

Response:
*/
#define CMD_SET_PARAMETER 0x38
#define RES_SET_PARAMETER []

/*
Data: 10 bytes
Byte[1] = (1 = on / 0 = off)
    0x01 = Saturday
    0x02 = Sunday
    0x04 = Monday
    0x08 = Tuesday
    0x10 = Wednesday
    0x20 = Thursday
    0x40 = Friday
    0x80 = colon
Byte[2] = (1 = on / 0 = off)
    0x01 = 1AEGED
    0x02 = 1B
    0x04 = 1C
    0x08 = AUTO symbol
    0x10 = MANUAL symbol
    0x20 = FILTER icon
    0x40 = Symbol I
    0x80 = Symbol E
Byte[3] = (1 = on / 0 = off)
    0x01 = 2A
    0x02 = 2B
    0x04 = 2C
    0x08 = 2D
    0x10 = 2E
    0x20 = 2F
    0x40 = 2G
    0x80 = FAN symbol
Byte[4] = (1 = on / 0 = off)
    0x01 = 3A
    0x02 = 3B
    0x04 = 3C
    0x08 = 3DByte[1] =
    0x10 = 3E
    0x20 = 3F
    0x40 = 3G
    0x80 = kitchen hood symbol
Byte[5] = (1 = on / 0 = off)
    0x01 = 4A
    0x02 = 4B
    0x04 = 4C
    0x08 = 4D
    0x10 = 4E
    0x20 = 4F
    0x40 = 4G
    0x80 = Preheating symbol
Byte[6] = (1 = on / 0 = off)
    0x01 = 5A
    0x02 = 5B
    0x04 = 5C
    0x08 = 5D
    0x10 = 5E
    0x20 = 5F
    0x40 = 5G
    0x80 = Frost symbol
Byte[7] = (1 = on / 0 = off)
    0x01 = 6A
    0x02 = 6B
    0x04 = 6C
    0x08 = 6D
    0x10 = 6E
    0x20 = 6F
    0x40 = 6G
    0x80 = Symbol EWT
Byte[8] = (1 = on / 0 = off)
    0x01 = 7A
    0x02 = 7B
    0x04 = 7C
    0x08 = 7D
    0x10 = 7E
    0x20 = 7F
    0x40 = 7G
    0x80 = Reheating symbol
Byte[9] = (1 = on / 0 = off)
    0x01 = 8A
    0x02 = 8B
    0x04 = 8C
    0x08 = 8D
    0x10 = 8E
    0x20 = 8F
    0x40 = 8G
    0x80 = point
Byte[10] = (1 = on / 0 = off)
    0x01 = Degree symbol
    0x02 = Bypass symbol
    0x04 = X1
    0x08 = X2
    0x10 = X3
    0x20 = House symbol
    0x40 = supply air symbol
    0x80 = Exhaust air symbol

Response: -
*/
#define CMD_SET_DISPLAY 0x39
#define RES_SET_DISPLAY []

/*
Data: 4 bytes
Byte[1] = RF Address 4 (Low Byte)
Byte[2] = RF Address 3
Byte[3] = RF Address 2
Byte[4] = RF Address 1 (High Byte)

Response: -
*/
#define CMD_SET_RF_ADDRESS 0x3E
#define RES_SET_RF_ADDRESS []

/*
Data: 21 bytes
Byte[1] = Module type receiver
Byte[2] = Module ID receiver
Byte[3] = Module type transmitter
Byte[4] = Module ID transmitter
Byte[5] = Lifetime
Byte[6] = Date type
Byte[7] = Data byte 1
Byte[8] = Data byte 2
Byte[9] = Data byte 3
Byte[10] = Data byte 4
Byte[11] = Data byte 5
Byte[12] = Data byte 6
Byte[13] = Data byte 7
Byte[14] = Data byte 8
Byte[15] = Data byte 9
Byte[16] = Data byte 10
Byte[17] = RF address 4 (low byte)
Byte[18] = RF address 3
Byte[19] = RF address 2
Byte[20] = RF address 1 (high byte)
Byte[21] = Control bits
    0x01 = Repeat previous package first
    0x02 = 250ms pause before sending
    0x04 = Received at sender address

Response:
*/
#define CMD_SEND_RF_COMMAND 0x40
#define RES_SEND_RF_COMMAND []