#pragma once

#include "registers_core.h"

// ================================================
// ========== Test mode (PC to ComfoAir) ==========
// ================================================

/*
Data: -

Response: -
Confirmation test mode
*/
#define CMD_SET_TEST_MODE 0x01
#define RES_SET_TEST_MODE 0x02

/*
Data: 2 bytes
Byte[1] = relay
    0x01 = Preheating relay
    0x02 = Triac preheating
    0x04 = EWT supply
    0x08 = EWT direction
    0x10 = kitchen hood
    0x20 = error
Byte[2] = Return message
    0x01 = Filter full LED

Response: -
*/
#define CMD_SET_OUTPUTS 0x05
#define RES_SET_OUTPUTS []

/*
Data: 3 bytes
Byte[1] = Supply air (%)
Byte[2] = Exhaust air (%)
Byte[3] = Post-heating (%)

Response: -
*/
#define CMD_SET_ANALOG_OUTPUTS 0x07
#define RES_SET_ANALOG_OUTPUTS []

/*
Data: 2 bytes
Byte[1] = Bypass (1 = open / 0 = closed / 3 = stop)
Byte[2] = Preheating (1 = open / 0 = closed / 3 = stop)

Response:
*/
#define CMD_SET_FLAPS 0x09
#define RES_SET_FLAPS []

/*
Data: -

Response: -
Confirmation end of test mode
*/
#define CMD_EXIT_TEST_MODE 0x19
#define RES_EXIT_TEST_MODE 0x1A