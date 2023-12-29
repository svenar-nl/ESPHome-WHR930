#pragma once

#include <stdint.h>

#include <list>
#include <string>
#include <vector>

#include "registers/registers_bootloader.h"
#include "registers/registers_cc_ease.h"
#include "registers/registers_core.h"
#include "registers/registers_general.h"
#include "registers/registers_read.h"
#include "registers/registers_test.h"
#include "registers/registers_write.h"

class ComfoAir {
   public:
    enum UPDATE_TASK {
        get_bootloader_version = -3,
        get_firmware_version = -2,
        get_connectorboard_version = -1,
        get_fan_status = 0,
        get_valve_status = 1,
        get_sensor_data = 2,
        get_ventilation_level = 3,
        get_temperatures = 4,
        get_status = 5,
        get_error_status = 6,
        get_bypass_control_status = 7
    };

    UPDATE_TASK getFirstTask();
    UPDATE_TASK getCurrentTask();
    UPDATE_TASK nextTask();

    enum CLIMATE_FAN_SPEED {
        FAN_SPEED_AUTO,
        FAN_SPEED_OFF,
        FAN_SPEED_LOW,
        FAN_SPEED_MEDIUM,
        FAN_SPEED_HIGH
    };

    enum CLIMATE_FAN_MODE {
        FAN_MODE_AUTO,
        FAN_MODE_OFF,
        FAN_MODE_FAN_ONLY
    };

    enum RETURN_DATA_TYPE {
        NONE,

        BOOTLOADER_VERSION,
        FIRMWARE_VERSION,
        CONNECTORBOARD_VERSION,

        FAN_SUPPLY_AIR_PERCENTAGE,
        FAN_EXHAUST_AIR_PERCENTAGE,
        FAN_SUPPLY_AIR_RPM,
        FAN_EXHAUST_AIR_RPM,

        FAN_SUPPLY_AIR_LEVEL,
        FAN_RETURN_AIR_LEVEL,
        VENTILATION_LEVEL,

        IS_T1_OUTSIDE_AIR_PRESENT,
        IS_T2_SUPPLY_AIR_PRESENT,
        IS_T3_EXHAUST_AIR_IN_PRESENT,
        IS_T4_EXHAUST_AIR_OUT_PRESENT,
        IS_EWT_PRESENT,
        IS_POST_HEATING_PRESENT,
        IS_KITCHEN_HOOD_PRESENT,
        IS_ENTHALPY_PRESENT,
        IS_BYPASS_PRESENT,
        IS_PREHEATER_PRESENT,

        IS_BYPASS_VALVE_OPEN,
        IS_PREHEATING,
        IS_SUMMER_MODE,
        IS_SUPPLY_FAN_ACTIVE,
        IS_FILTER_FULL,

        COMFORT_TEMPERATURE,
        OUTSIDE_AIR_TEMPERATURE,
        SUPPLY_AIR_TEMPERATURE,
        RETURN_AIR_TEMPERATURE,
        EXHAUST_AIR_TEMPERATURE,
        EWT_TEMPERATURE,
        REHEATING_TEMPERATURE,
        KITCHEN_HOOD_TEMPERATURE,

        ENTHALPY_TEMPERATURE,
        ENTHALPY_HUMIDITY,

        BYPASS_FACTOR,
        BYPASS_STEP,
        BYPASS_CORRECTION,
    };

    ComfoAir();
    void update();
    void handleIncommingData(uint8_t packet[], uint8_t packetSize);

    std::vector<uint8_t> getPacket();
    bool doPublishState();

    uint8_t getData_uint8_t(RETURN_DATA_TYPE type);
    float getData_float(RETURN_DATA_TYPE type);
    bool getData_bool(RETURN_DATA_TYPE type);

    void queueCommand(uint8_t cmd, uint8_t data[], uint8_t dataSize);
    uint8_t getVentilationlevel(uint8_t index);

    std::string getLastMessage();
    RETURN_DATA_TYPE getLastReceivedPacketType();

   protected:
    void create_packet(uint8_t cmd[], uint8_t cmdSize, uint8_t data[], uint8_t dataSize, uint8_t *packet, uint8_t *packetSize);
    uint8_t calculate_checksum(uint8_t data[], uint8_t dataSize);
    uint8_t calculate_return_checksum(const uint8_t *data, uint8_t dataLength);

   private:
    int8_t minTaskIndex = -3;
    int8_t maxTaskIndex = 6;
    int8_t currentTaskIndex = -3;
    int8_t previousTaskIndex = currentTaskIndex;
    bool publishState = false;

    uint8_t bootloaderVersion[13]{0};
    uint8_t firmwareVersion[13]{0};
    uint8_t connectorBoardVersion[14]{0};

    uint8_t fan_supply_air_percentage = 0;
    uint8_t fan_exhaust_air_percentage = 0;
    float fan_supply_air_rpm = 0.0f;
    float fan_exhaust_air_rpm = 0.0f;

    bool is_t1_outside_air_present = false;
    bool is_t2_supply_air_present = false;
    bool is_t3_exhaust_air_in_present = false;
    bool is_t4_exhaust_air_out_present = false;
    bool is_ewt_present = false;
    bool is_post_heating_present = false;
    bool is_kitchen_hood_present = false;
    bool is_enthalpy_present = false;
    bool is_preheater_present = false;
    bool is_bypass_present = false;

    bool is_bypass_valve_open = false;
    bool is_preheating = false;
    bool is_supply_fan_active = false;
    bool is_filter_full = false;
    bool is_summer_mode = false;

    uint8_t bypass_factor = 0;
    uint8_t bypass_step = 0;
    uint8_t bypass_correction = 0;

    float comfort_temperature = 0.0f;
    float outside_air_temperature = 0.0f;
    float supply_air_temperature = 0.0f;
    float return_air_temperature = 0.0f;
    float exhaust_air_temperature = 0.0f;
    float ewt_temperature = 0.0f;
    float reheating_temperature = 0.0f;
    float kitchen_hood_temperature = 0.0f;

    float enthalpy_temperature = 0.0f;
    uint8_t enthalpy_humidity = 0;

    uint8_t ventilation_levels[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    uint8_t return_air_level = 0;
    uint8_t supply_air_level = 0;
    CLIMATE_FAN_SPEED fan_speed = CLIMATE_FAN_SPEED::FAN_SPEED_OFF;
    CLIMATE_FAN_MODE fan_mode = CLIMATE_FAN_MODE::FAN_MODE_OFF;

    void prepareGetBootloaderVersionPacket();
    void prepareGetFirmwareVersionPacket();
    void prepareGetConnectorboardboardVersionPacket();

    void prepareGetFanStatusPacket();
    void prepareGetValveStatusPacket();
    void prepareGetSensorDataPacket();
    void prepareGetVentilationLevelPacket();
    void prepareGetTemperaturesPacket();
    void prepareGetStatusPacket();
    void prepareGetErrorStatusPacket();
    void prepareGetBypassControlStatusPacket();

    void prepareBaseGetPacket(uint8_t cmd);

    std::list<std::vector<uint8_t>> packetSendQueue;
    std::string lastMessage = "";

    std::list<RETURN_DATA_TYPE> lastReceivedPacketType;
};