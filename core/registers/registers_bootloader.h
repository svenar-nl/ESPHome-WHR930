#pragma once

#include "registers_core.h"

// =================================================
// ========== Bootloader (PC an ComfoAir) ==========
// =================================================

/*
Data: -

Response: -
Confirmation of bootloader mode
*/
#define CMD_SET_BOOTLOADER_MODE 0x65
#define RES_SET_BOOTLOADER_MODE 0x66

/*
Data: 67 bytes
Byte[1] = Data byte 1
Byte[2..63] = ...
Byte[64] = Data byte 64
Byte[65] = Start address (high byte)
Byte[66] = Start address
Byte[67] = Start address (low byte)

Response: 1 byte
Byte[1] = status
    0x01 = success
    0x02 = Error
    0x04 = Address out of range
    0x08 = data block incomplete
*/
#define CMD_WRITE_FLASH_BLOCK 0x6F
#define RES_WRITE_FLASH_BLOCK 0x70

/*
Data: 3 bytes
Byte[1] = Start address (high byte)
Byte[2] = Start address
Byte[3] = Start address (low byte)

Response: 64 bytes
Byte[1] = Data byte 1
Byte[2..63] = ...
Byte[64] = Data byte 64
*/
#define CMD_READ_FLASH_BLOCK 0x71
#define RES_READ_FLASH_BLOCK 0x72

/*
Data: -

Response: -
Confirmation of end of bootloader mode
*/
#define CMD_EXIT_BOOTLOADER_MODE 0x73
#define RES_EXIT_BOOTLOADER_MODE 0x74