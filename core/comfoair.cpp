#include "comfoair.h"

#include "development.h"

ComfoAir::ComfoAir() {
}

ComfoAir::UPDATE_TASK ComfoAir::getFirstTask() {
    return UPDATE_TASK(minTaskIndex);
}

ComfoAir::UPDATE_TASK ComfoAir::getCurrentTask() {
    return UPDATE_TASK(currentTaskIndex);
}

ComfoAir::UPDATE_TASK ComfoAir::nextTask() {
    previousTaskIndex = currentTaskIndex;
    currentTaskIndex = currentTaskIndex > maxTaskIndex ? -1 : currentTaskIndex;
    currentTaskIndex++;
    return getCurrentTask();
}

void ComfoAir::update() {
    UPDATE_TASK currentTask = nextTask();

    if (currentTaskIndex == 0 && previousTaskIndex == maxTaskIndex) {
        publishState = true;
    }

    switch (currentTask) {
        case get_bootloader_version:
            prepareGetBootloaderVersionPacket();
            break;

        case get_firmware_version:
            prepareGetFirmwareVersionPacket();
            break;

        case get_connectorboard_version:
            prepareGetConnectorboardboardVersionPacket();
            break;

        case get_fan_status:
            prepareGetFanStatusPacket();
            break;

        case get_valve_status:
            prepareGetValveStatusPacket();
            break;

        case get_sensor_data:
            prepareGetSensorDataPacket();
            break;

        case get_ventilation_level:
            prepareGetVentilationLevelPacket();
            break;

        case get_temperatures:
            prepareGetTemperaturesPacket();
            break;

        case get_status:
            prepareGetStatusPacket();
            break;

        case get_error_status:
            prepareGetErrorStatusPacket();
            break;

        case get_bypass_control_status:
            prepareGetBypassControlStatusPacket();
            break;

        default:
            break;
    }
}

void ComfoAir::handleIncommingData(uint8_t packet[], uint8_t packetSize) {
    if (packetSize < 2) {
        lastMessage = "Received invalid packet size";
        return;
    }

    if (packet[0] == 0x07 && packet[1] == ACK && packetSize == 2) {
        lastMessage = "Received ACK";
        return;
    } else if (packet[0] == 0x07 && packet[1] == ACK) {
        uint8_t newPacket[packetSize - 2];
        for (uint8_t i = 0; i < packetSize - 2; ++i) {
            newPacket[i] = packet[i + 2];
        }
        handleIncommingData(newPacket, packetSize - 2);
        return;
    }

    if (packet[0] != 0x07 || packet[1] != 0xF0 || packet[2] != 0x00 || packet[packetSize - 2] != 0x07 || packet[packetSize - 1] != 0x0F) {  // Invalid header / tail
        lastMessage = "Received invalid header / tail";
        return;
    }

    uint8_t dataLength = packet[4];
    if (dataLength != packetSize - 8) {  // Invalid data length
        lastMessage = "Received invalid data length";
        return;
    }

    uint8_t checksum = calculate_return_checksum(packet + 2, dataLength + 3);
    if (checksum != packet[packetSize - 3]) {  // Invalid checksum
        lastMessage = "Received invalid checksum";
        return;
    }

    // Packet is valid
    lastMessage = "Received valid packet";

    uint8_t data[dataLength];
    for (uint8_t i = 0; i < dataLength; ++i) {
        data[i] = packet[5 + i];
    }

    uint8_t ventilationIndex = 0;
    switch (packet[3]) {
        case RES_GET_BOOTLOADER_VERSION:
            lastMessage += " - Bootloader version";
            for (uint8_t i = 0; i < dataLength; ++i) {
                bootloaderVersion[i] = data[i];
            }

            lastReceivedPacketType.push_back(RETURN_DATA_TYPE::BOOTLOADER_VERSION);
            break;

        case RES_GET_FIRMWARE_VERSION:
            lastMessage += " - Firmware version";
            for (uint8_t i = 0; i < dataLength; ++i) {
                firmwareVersion[i] = data[i];
            }

            lastReceivedPacketType.push_back(RETURN_DATA_TYPE::FIRMWARE_VERSION);
            break;

        case RES_GET_CONNECTOR_BOARD_VERSION:
            lastMessage += " - Connector board version";
            for (uint8_t i = 0; i < dataLength; ++i) {
                connectorBoardVersion[i] = data[i];
            }

            lastReceivedPacketType.push_back(RETURN_DATA_TYPE::CONNECTORBOARD_VERSION);
            break;

        case RES_GET_FAN_STATUS:
            lastMessage += " - Fan status";
            fan_supply_air_percentage = data[0];
            fan_exhaust_air_percentage = data[1];
            fan_supply_air_rpm = 1875000.0f / (data[3] | (data[2] << 8));
            fan_exhaust_air_rpm = 1875000.0f / (data[5] | (data[4] << 8));

            char lastMessageBuffer[40];
            sprintf(lastMessageBuffer, " %i %i %f %f", fan_supply_air_percentage, fan_exhaust_air_percentage, fan_supply_air_rpm, fan_exhaust_air_rpm);
            lastMessage += lastMessageBuffer;

            lastReceivedPacketType.push_back(RETURN_DATA_TYPE::FAN_SUPPLY_AIR_PERCENTAGE);
            lastReceivedPacketType.push_back(RETURN_DATA_TYPE::FAN_EXHAUST_AIR_PERCENTAGE);
            lastReceivedPacketType.push_back(RETURN_DATA_TYPE::FAN_SUPPLY_AIR_RPM);
            lastReceivedPacketType.push_back(RETURN_DATA_TYPE::FAN_EXHAUST_AIR_RPM);
            break;

        case RES_GET_VALVE_STATUS:
            lastMessage += " - Valve status";
            is_bypass_valve_open = data[0] != 0;
            is_preheating = data[1] != 0;

            lastReceivedPacketType.push_back(RETURN_DATA_TYPE::IS_BYPASS_VALVE_OPEN);
            lastReceivedPacketType.push_back(RETURN_DATA_TYPE::IS_PREHEATING);
            break;

        case RES_GET_SENSOR_DATA:
            lastMessage += " - Sensor data";
            enthalpy_temperature = (float)data[0] / 2.0f - 20.0f;
            enthalpy_humidity = data[1];

            lastReceivedPacketType.push_back(RETURN_DATA_TYPE::ENTHALPY_TEMPERATURE);
            lastReceivedPacketType.push_back(RETURN_DATA_TYPE::ENTHALPY_HUMIDITY);
            break;

        case RES_GET_VENTILATION_LEVEL:
            lastMessage += " - Ventilation level";
            ventilation_levels[ventilationIndex++] = data[0];
            ventilation_levels[ventilationIndex++] = data[3];
            ventilation_levels[ventilationIndex++] = data[1];
            ventilation_levels[ventilationIndex++] = data[4];
            ventilation_levels[ventilationIndex++] = data[2];
            ventilation_levels[ventilationIndex++] = data[5];
            ventilation_levels[ventilationIndex++] = data[10];
            ventilation_levels[ventilationIndex++] = data[11];

            return_air_level = data[6];
            supply_air_level = data[7];

            switch (data[8]) {
                case 0x00:
                    fan_speed = CLIMATE_FAN_SPEED::FAN_SPEED_AUTO;
                    fan_mode = CLIMATE_FAN_MODE::FAN_MODE_AUTO;
                    break;

                case 0x01:
                    fan_speed = CLIMATE_FAN_SPEED::FAN_SPEED_OFF;
                    fan_mode = CLIMATE_FAN_MODE::FAN_MODE_OFF;
                    break;

                case 0x02:
                    fan_speed = CLIMATE_FAN_SPEED::FAN_SPEED_LOW;
                    fan_mode = CLIMATE_FAN_MODE::FAN_MODE_FAN_ONLY;
                    break;

                case 0x03:
                    fan_speed = CLIMATE_FAN_SPEED::FAN_SPEED_MEDIUM;
                    fan_mode = CLIMATE_FAN_MODE::FAN_MODE_FAN_ONLY;
                    break;

                case 0x04:
                    fan_speed = CLIMATE_FAN_SPEED::FAN_SPEED_HIGH;
                    fan_mode = CLIMATE_FAN_MODE::FAN_MODE_FAN_ONLY;
                    break;

                default:
                    break;
            }

            is_supply_fan_active = data[9] == 1;

            lastReceivedPacketType.push_back(RETURN_DATA_TYPE::IS_SUPPLY_FAN_ACTIVE);
            lastReceivedPacketType.push_back(RETURN_DATA_TYPE::FAN_SUPPLY_AIR_LEVEL);
            lastReceivedPacketType.push_back(RETURN_DATA_TYPE::FAN_RETURN_AIR_LEVEL);
            lastReceivedPacketType.push_back(RETURN_DATA_TYPE::VENTILATION_LEVEL);
            break;

        case RES_GET_TEMPERATURE_STATUS:
            lastMessage += " - Temperature status";
            outside_air_temperature = (float)data[0] / 2.0f - 20.0f;
            supply_air_temperature = (float)data[1] / 2.0f - 20.0f;
            return_air_temperature = (float)data[2] / 2.0f - 20.0f;
            exhaust_air_temperature = (float)data[3] / 2.0f - 20.0f;

            lastReceivedPacketType.push_back(RETURN_DATA_TYPE::OUTSIDE_AIR_TEMPERATURE);
            lastReceivedPacketType.push_back(RETURN_DATA_TYPE::SUPPLY_AIR_TEMPERATURE);
            lastReceivedPacketType.push_back(RETURN_DATA_TYPE::RETURN_AIR_TEMPERATURE);
            lastReceivedPacketType.push_back(RETURN_DATA_TYPE::EXHAUST_AIR_TEMPERATURE);
            break;

        case RES_GET_TEMPERATURES:
            lastMessage += " - Temperatures";
            comfort_temperature = (float)data[0] / 2.0f - 20.0f;
            outside_air_temperature = (float)data[1] / 2.0f - 20.0f;
            supply_air_temperature = (float)data[2] / 2.0f - 20.0f;
            return_air_temperature = (float)data[3] / 2.0f - 20.0f;
            exhaust_air_temperature = (float)data[4] / 2.0f - 20.0f;
            ewt_temperature = (float)data[6] / 2.0f - 20.0f;
            reheating_temperature = (float)data[7] / 2.0f - 20.0f;
            kitchen_hood_temperature = (float)data[8] / 2.0f - 20.0f;

            is_t1_outside_air_present = data[5] & 0x01;
            is_t2_supply_air_present = data[5] & 0x02;
            is_t3_exhaust_air_in_present = data[5] & 0x04;
            is_t4_exhaust_air_out_present = data[5] & 0x08;
            is_ewt_present = data[5] & 0x10;
            is_post_heating_present = data[5] & 0x20;
            is_kitchen_hood_present = data[5] & 0x40;

            lastReceivedPacketType.push_back(RETURN_DATA_TYPE::COMFORT_TEMPERATURE);
            lastReceivedPacketType.push_back(RETURN_DATA_TYPE::OUTSIDE_AIR_TEMPERATURE);
            lastReceivedPacketType.push_back(RETURN_DATA_TYPE::SUPPLY_AIR_TEMPERATURE);
            lastReceivedPacketType.push_back(RETURN_DATA_TYPE::RETURN_AIR_TEMPERATURE);
            lastReceivedPacketType.push_back(RETURN_DATA_TYPE::EXHAUST_AIR_TEMPERATURE);
            lastReceivedPacketType.push_back(RETURN_DATA_TYPE::EWT_TEMPERATURE);
            lastReceivedPacketType.push_back(RETURN_DATA_TYPE::REHEATING_TEMPERATURE);
            lastReceivedPacketType.push_back(RETURN_DATA_TYPE::KITCHEN_HOOD_TEMPERATURE);

            lastReceivedPacketType.push_back(RETURN_DATA_TYPE::IS_T1_OUTSIDE_AIR_PRESENT);
            lastReceivedPacketType.push_back(RETURN_DATA_TYPE::IS_T2_SUPPLY_AIR_PRESENT);
            lastReceivedPacketType.push_back(RETURN_DATA_TYPE::IS_T3_EXHAUST_AIR_IN_PRESENT);
            lastReceivedPacketType.push_back(RETURN_DATA_TYPE::IS_T4_EXHAUST_AIR_OUT_PRESENT);
            lastReceivedPacketType.push_back(RETURN_DATA_TYPE::IS_EWT_PRESENT);
            lastReceivedPacketType.push_back(RETURN_DATA_TYPE::IS_POST_HEATING_PRESENT);
            lastReceivedPacketType.push_back(RETURN_DATA_TYPE::IS_KITCHEN_HOOD_PRESENT);
            break;

        case RES_GET_STATUS:
            lastMessage += " - Status";
            is_preheater_present = data[0] != 0;
            is_bypass_present = data[1] != 0;
            is_enthalpy_present = data[9] != 0;
            is_ewt_present = data[10] != 0;

            lastReceivedPacketType.push_back(RETURN_DATA_TYPE::IS_PREHEATER_PRESENT);
            lastReceivedPacketType.push_back(RETURN_DATA_TYPE::IS_BYPASS_PRESENT);
            lastReceivedPacketType.push_back(RETURN_DATA_TYPE::IS_ENTHALPY_PRESENT);
            lastReceivedPacketType.push_back(RETURN_DATA_TYPE::IS_EWT_PRESENT);
            break;

        case RES_GET_FAULTS:
            lastMessage += " - Faults";
            is_filter_full = data[8] != 0;

            lastReceivedPacketType.push_back(RETURN_DATA_TYPE::IS_FILTER_FULL);
            break;

        case RES_GET_BYPASS_CONTROL_STATUS:
            lastMessage += " - Bypass control status";
            bypass_factor = data[2];
            bypass_step = data[3];
            bypass_correction = data[4];
            is_summer_mode = data[6] != 0;

            lastReceivedPacketType.push_back(RETURN_DATA_TYPE::BYPASS_FACTOR);
            lastReceivedPacketType.push_back(RETURN_DATA_TYPE::BYPASS_STEP);
            lastReceivedPacketType.push_back(RETURN_DATA_TYPE::BYPASS_CORRECTION);
            lastReceivedPacketType.push_back(RETURN_DATA_TYPE::IS_SUMMER_MODE);
            break;

        default:
            lastMessage += " - Unknown response";
            break;
    }
}

std::vector<uint8_t> ComfoAir::getPacket() {
    if (packetSendQueue.empty()) {
        return {};
    }

    std::vector<uint8_t> packet = packetSendQueue.front();
    packetSendQueue.pop_front();

    return packet;
}

bool ComfoAir::doPublishState() {
    if (!publishState) {
        return false;
    }
    publishState = false;
    return true;
}

uint8_t ComfoAir::getData_uint8_t(RETURN_DATA_TYPE type) {
    uint8_t returnValue = 0;
    uint8_t ventilation_level = 0;

    switch (type) {
        case FAN_SUPPLY_AIR_PERCENTAGE:
            returnValue = fan_supply_air_percentage;
            break;

        case FAN_EXHAUST_AIR_PERCENTAGE:
            returnValue = fan_exhaust_air_percentage;
            break;

        case FAN_SUPPLY_AIR_LEVEL:
            returnValue = supply_air_level;
            break;

        case FAN_RETURN_AIR_LEVEL:
            returnValue = return_air_level;
            break;

        case VENTILATION_LEVEL:
            ventilation_level = fan_speed == CLIMATE_FAN_SPEED::FAN_SPEED_OFF ? 0 : ventilation_level;
            ventilation_level = fan_speed == CLIMATE_FAN_SPEED::FAN_SPEED_AUTO ? 0 : ventilation_level;
            ventilation_level = fan_speed == CLIMATE_FAN_SPEED::FAN_SPEED_LOW ? 1 : ventilation_level;
            ventilation_level = fan_speed == CLIMATE_FAN_SPEED::FAN_SPEED_MEDIUM ? 2 : ventilation_level;
            ventilation_level = fan_speed == CLIMATE_FAN_SPEED::FAN_SPEED_HIGH ? 3 : ventilation_level;
            returnValue = ventilation_level;
            break;

        case ENTHALPY_HUMIDITY:
            returnValue = is_enthalpy_present ? enthalpy_humidity : 0;
            break;

        case BYPASS_FACTOR:
            returnValue = bypass_factor;
            break;

        case BYPASS_STEP:
            returnValue = bypass_step;
            break;

        case BYPASS_CORRECTION:
            returnValue = bypass_correction;
            break;

        default:
            break;
    }

    return returnValue;
}

float ComfoAir::getData_float(RETURN_DATA_TYPE type) {
    float returnValue = 0.0f;

    switch (type) {
        case COMFORT_TEMPERATURE:
            returnValue = comfort_temperature;
            break;
        
        case FAN_SUPPLY_AIR_RPM:
            returnValue = fan_supply_air_rpm;
            break;

        case FAN_EXHAUST_AIR_RPM:
            returnValue = fan_exhaust_air_rpm;
            break;

        case OUTSIDE_AIR_TEMPERATURE:
            returnValue = is_t1_outside_air_present ? outside_air_temperature : 0.0f;
            break;

        case SUPPLY_AIR_TEMPERATURE:
            returnValue = is_t2_supply_air_present ? supply_air_temperature : 0.0f;
            break;

        case RETURN_AIR_TEMPERATURE:
            returnValue = is_t3_exhaust_air_in_present ? return_air_temperature : 0.0f;
            break;

        case EXHAUST_AIR_TEMPERATURE:
            returnValue = is_t4_exhaust_air_out_present ? exhaust_air_temperature : 0.0f;
            break;

        case EWT_TEMPERATURE:
            returnValue = is_ewt_present ? ewt_temperature : 0.0f;
            break;

        case REHEATING_TEMPERATURE:
            returnValue = is_post_heating_present ? reheating_temperature : 0.0f;
            break;

        case KITCHEN_HOOD_TEMPERATURE:
            returnValue = is_kitchen_hood_present ? kitchen_hood_temperature : 0.0f;
            break;

        case ENTHALPY_TEMPERATURE:
            returnValue = is_enthalpy_present ? enthalpy_temperature : 0.0f;
            break;

        default:
            break;
    }

    return returnValue;
}

bool ComfoAir::getData_bool(RETURN_DATA_TYPE type) {
    bool returnValue = false;

    switch (type) {
        case IS_BYPASS_VALVE_OPEN:
            returnValue = is_bypass_valve_open;
            break;

        case IS_PREHEATING:
            returnValue = is_preheating;
            break;

        case IS_SUMMER_MODE:
            returnValue = is_summer_mode;
            break;

        case IS_SUPPLY_FAN_ACTIVE:
            returnValue = is_supply_fan_active;
            break;

        case IS_FILTER_FULL:
            returnValue = is_filter_full;
            break;

        case IS_T1_OUTSIDE_AIR_PRESENT:
            returnValue = is_t1_outside_air_present;
            break;

        case IS_T2_SUPPLY_AIR_PRESENT:
            returnValue = is_t2_supply_air_present;
            break;

        case IS_T3_EXHAUST_AIR_IN_PRESENT:
            returnValue = is_t3_exhaust_air_in_present;
            break;

        case IS_T4_EXHAUST_AIR_OUT_PRESENT:
            returnValue = is_t4_exhaust_air_out_present;
            break;

        case IS_EWT_PRESENT:
            returnValue = is_ewt_present;
            break;

        case IS_POST_HEATING_PRESENT:
            returnValue = is_post_heating_present;
            break;

        case IS_KITCHEN_HOOD_PRESENT:
            returnValue = is_kitchen_hood_present;
            break;

        case IS_ENTHALPY_PRESENT:
            returnValue = is_enthalpy_present;
            break;

        case IS_PREHEATER_PRESENT:
            returnValue = is_preheater_present;
            break;

        default:
            break;
    }

    return returnValue;
}

void ComfoAir::create_packet(uint8_t cmd[], uint8_t cmdSize, uint8_t data[], uint8_t dataSize, uint8_t *packet, uint8_t *packetSize) {
    if (cmdSize != 2) {
        return;
    }

    // ----- WRITE SAFETY -----
#if DEVELOPMENT_MODE
    if (dataSize > 0) {
        char cmd_str[cmdSize * 3 + 1];
        char data_str[dataSize * 3 + 1];
        for (int i = 0; i < cmdSize; i++) {
            sprintf(cmd_str + i * 3, "%02X ", cmd[i]);
        }
        for (int i = 0; i < dataSize; i++) {
            sprintf(data_str + i * 3, "%02X ", data[i]);
        }
        lastMessage += "[DEV] - OUT: 07 F0 " + std::string(cmd_str) + std::to_string(dataSize) + " " + std::string(data_str) + "CC 07 0F";
        return;
    }
#endif
    // ----- WRITE SAFETY -----

    uint8_t packetIndex = 0;

    packet[packetIndex++] = 0x07;  // default start bit
    packet[packetIndex++] = 0xF0;  // default start bit

    packet[packetIndex++] = cmd[0];
    packet[packetIndex++] = cmd[1];

    packet[packetIndex++] = dataSize;
    for (uint8_t i = 0; i < dataSize; ++i) {
        packet[packetIndex++] = data[i];
    }

    if (dataSize == 0) {
        uint8_t checksum = calculate_checksum(packet + 2, dataSize + 2);
        packet[packetIndex++] = checksum;
    } else {
        uint8_t cechksumData[cmdSize + dataSize + 1];
        for (uint8_t i = 0; i < cmdSize; ++i) {
            cechksumData[i] = cmd[i];
        }
        for (uint8_t i = 0; i < dataSize; ++i) {
            cechksumData[cmdSize + i] = data[i];
        }
        uint8_t checksum = (calculate_checksum(cechksumData, cmdSize + dataSize) + 0x01) & 0xFF;
        packet[packetIndex++] = checksum;
    }

    packet[packetIndex++] = 0x07;  // default end bit
    packet[packetIndex++] = 0x0F;  // default end bit

    *packetSize = packetIndex;
}

uint8_t ComfoAir::calculate_checksum(uint8_t data[], uint8_t dataSize) {
    uint8_t checksum = 0xAD;
    uint8_t stopByte = 0x07;
    bool foundStopByte = false;

    for (uint8_t i = 0; i < dataSize; ++i) {
        uint8_t b = data[i];

        if ((b == stopByte && !foundStopByte) || b != stopByte) {
            checksum += b;
        }

        if (b == stopByte) {
            foundStopByte = true;
        }

        if (checksum > 0xFF) {
            checksum -= 0xFF + 1;
        }
    }

    return checksum;
}

uint8_t ComfoAir::calculate_return_checksum(const uint8_t *data, uint8_t dataLength) {
    uint8_t sum = 0;
    for (uint8_t i = 0; i < dataLength; i++) {
        sum += data[i];
    }
    return sum + 0xAD;
}

void ComfoAir::prepareGetBootloaderVersionPacket() {
    prepareBaseGetPacket(CMD_GET_BOOTLOADER_VERSION);
}

void ComfoAir::prepareGetFirmwareVersionPacket() {
    prepareBaseGetPacket(CMD_GET_FIRMWARE_VERSION);
}

void ComfoAir::prepareGetConnectorboardboardVersionPacket() {
    prepareBaseGetPacket(CMD_GET_CONNECTOR_BOARD_VERSION);
}

void ComfoAir::prepareGetFanStatusPacket() {
    prepareBaseGetPacket(CMD_GET_FAN_STATUS);
}

void ComfoAir::prepareGetValveStatusPacket() {
    prepareBaseGetPacket(CMD_GET_VALVE_STATUS);
}

void ComfoAir::prepareGetSensorDataPacket() {
    prepareBaseGetPacket(CMD_GET_SENSOR_DATA);
}

void ComfoAir::prepareGetVentilationLevelPacket() {
    prepareBaseGetPacket(CMD_GET_VENTILATION_LEVEL);
}

void ComfoAir::prepareGetTemperaturesPacket() {
    prepareBaseGetPacket(CMD_GET_TEMPERATURES);
}

void ComfoAir::prepareGetStatusPacket() {
    prepareBaseGetPacket(CMD_GET_STATUS);
}

void ComfoAir::prepareGetErrorStatusPacket() {
    prepareBaseGetPacket(CMD_GET_FAULTS);
}

void ComfoAir::prepareGetBypassControlStatusPacket() {
    prepareBaseGetPacket(CMD_GET_BYPASS_CONTROL_STATUS);
}

void ComfoAir::prepareBaseGetPacket(uint8_t targetCmd) {
    uint8_t cmd[2] = {COMMAND_PREFIX, targetCmd};

    uint8_t packet[32];
    uint8_t packetSize = 0;

    create_packet(cmd, sizeof(cmd) / sizeof(cmd[0]), {}, 0, packet, &packetSize);

    std::vector<uint8_t> packetVector(packet, packet + packetSize);
    packetSendQueue.push_back(packetVector);
}

void ComfoAir::queueCommand(uint8_t targetCmd, uint8_t data[], uint8_t dataSize) {
    uint8_t cmd[2] = {COMMAND_PREFIX, targetCmd};

    uint8_t packet[32];
    uint8_t packetSize = 0;

    create_packet(cmd, sizeof(cmd) / sizeof(cmd[0]), data, dataSize, packet, &packetSize);

    std::vector<uint8_t> packetVector(packet, packet + packetSize);
    packetSendQueue.push_back(packetVector);
}

uint8_t ComfoAir::getVentilationlevel(uint8_t index) {
    return ventilation_levels[index];
}

std::string ComfoAir::getLastMessage() {
    std::string msg = lastMessage;
    lastMessage = "";
    return msg;
}

ComfoAir::RETURN_DATA_TYPE ComfoAir::getLastReceivedPacketType() {
    if (lastReceivedPacketType.empty()) {
        return RETURN_DATA_TYPE::NONE;
    }

    RETURN_DATA_TYPE type = lastReceivedPacketType.front();
    lastReceivedPacketType.pop_front();

    return type;
}