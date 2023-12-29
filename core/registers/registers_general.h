#pragma once

#include "registers_core.h"

// =============================================================
// ========== Command list (PC to ComfoAir) / General ==========
// =============================================================

/*
Data: -

Response: 13 bytes
Byte[1] = Version Major
Byte[2] = Version Minor
Byte[3] = Beta
Byte[4-13] = Devicename (ASCII String)
*/
#define CMD_GET_BOOTLOADER_VERSION 0x67
#define RES_GET_BOOTLOADER_VERSION 0x68

/*
Data: -

Response: 13 bytes
Byte[1] = Version Major
Byte[2] = Version Minor
Byte[3] = Beta
Byte[4-13] = Devicename (ASCII String)
*/
#define CMD_GET_FIRMWARE_VERSION 0x69
#define RES_GET_FIRMWARE_VERSION 0x6A

/*
Data: -

Response: 14 bytes
Byte[1] = Version Major
Byte[2] = Version Minor
Byte[3-12] = Devicename (ASCII String)
Byte[13] = Version CC-Ease
    Bit[7..4] Version Major
    Bit[3..0] = Version Minor
Byte[14] = Version CC-Luxe
    Bit[7..4] Version Major
    Bit[3..0] = Version Minor
*/
#define CMD_GET_CONNECTOR_BOARD_VERSION 0xA1
#define RES_GET_CONNECTOR_BOARD_VERSION 0xA2

/*
Data: 1 byte
0x00 = end
0x01 = PC only
0x03 = PC master
0x04 = PC log mode

Response: 1 byte
0x00 = Without connection
0x01 = PC only
0x02 = CC Ease only
0x03 = PC master
0x04 = PC log mode
*/
#define CMD_SET_RS232_MODE 0x9B
#define RES_SET_RS232_MODE 0x9C