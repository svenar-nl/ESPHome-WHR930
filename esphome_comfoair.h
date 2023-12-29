#pragma once

#include "comfoair.h"
#include "esphome.h"
#include "esphome/components/api/custom_api_device.h"
#include "esphome/components/climate/climate.h"
#include "esphome/components/climate/climate_mode.h"
#include "esphome/components/climate/climate_traits.h"
#include "esphome/components/custom/climate/custom_climate.h"
#include "esphome/components/uart/uart.h"
#include "esphome/core/component.h"

namespace esphome {
namespace comfoair {

static const bool DEBUG = false;

static const char *TAG = "comfoair";
static const int POLLING_UPDATE_INTERVAL = 1000;

bool pollingDoHalfUpdateTick = false;

class ComfoAirComponent : public climate::Climate, public CustomAPIDevice, PollingComponent, uart::UARTDevice {
   public:
    void setup() override {
        register_service(&ComfoAirComponent::setOperationMode, "climate_set_operation_mode", {"exhaust_fan", "supply_fan"});
        register_service(&ComfoAirComponent::resetFilter, "reset_filter");
    }

    void setOperationMode(bool exhaust, bool supply) {
        ESP_LOGI(TAG, "Setting operation mode target exhaust: %i, supply: %i", exhaust, supply);
        uint8_t commandData[9] = {
            exhaust ? comfoair.getVentilationlevel(0) : (uint8_t)0,
            exhaust ? comfoair.getVentilationlevel(2) : (uint8_t)0,
            exhaust ? comfoair.getVentilationlevel(4) : (uint8_t)0,
            supply ? comfoair.getVentilationlevel(1) : (uint8_t)0,
            supply ? comfoair.getVentilationlevel(3) : (uint8_t)0,
            supply ? comfoair.getVentilationlevel(5) : (uint8_t)0,
            exhaust ? comfoair.getVentilationlevel(6) : (uint8_t)0,
            supply ? comfoair.getVentilationlevel(7) : (uint8_t)0,
            (uint8_t)0};
        comfoair.queueCommand(CMD_SET_VENTINATION_LEVEL, commandData, sizeof(commandData));
    }

    void resetFilter() {
        ESP_LOGI(TAG, "Reset filter service called");
        uint8_t commandData[4] = {0, 0, 0, 1};
        comfoair.queueCommand(CMD_RESET_AND_SELF_TEST, commandData, sizeof(commandData));
    }

    ComfoAirComponent(UARTComponent *parent) : PollingComponent(POLLING_UPDATE_INTERVAL), UARTDevice(parent) {}

    climate::ClimateTraits traits() override {
        auto traits = climate::ClimateTraits();
        traits.set_supports_current_temperature(true);
        traits.set_supported_modes({});
        traits.set_supported_presets({});
        traits.set_supports_two_point_target_temperature(false);
        traits.set_supports_action(false);
        traits.set_supported_fan_modes({
            climate::CLIMATE_FAN_AUTO,
            climate::CLIMATE_FAN_LOW,
            climate::CLIMATE_FAN_MEDIUM,
            climate::CLIMATE_FAN_HIGH,
            climate::CLIMATE_FAN_OFF,
        });
        traits.set_visual_min_temperature(12);
        traits.set_visual_max_temperature(29);
        traits.set_visual_temperature_step(1);
        return traits;
    }

    void control(const climate::ClimateCall &call) override {
        if (call.get_target_temperature().has_value()) {
            this->target_temperature = *call.get_target_temperature();

            if (this->target_temperature < 12.0f || this->target_temperature > 29.0f) {
                ESP_LOGE(TAG, "Ignoring invalid temperature request: %i", this->target_temperature);
                return;
            }

            ESP_LOGI(TAG, "Setting comfort temperature to: %f", this->target_temperature);
            uint8_t command[1] = {(uint8_t)((this->target_temperature + 20.0f) * 2.0f)};
            comfoair.queueCommand(CMD_SET_COMFORT_TEMPERATURE, command, sizeof(command));
        }

        if (call.get_fan_mode().has_value()) {
            uint8_t level = 0;

            this->fan_mode = *call.get_fan_mode();
            switch (this->fan_mode.value()) {
                case climate::CLIMATE_FAN_HIGH:
                    level = 0x04;
                    break;
                case climate::CLIMATE_FAN_MEDIUM:
                    level = 0x03;
                    break;
                case climate::CLIMATE_FAN_LOW:
                    level = 0x02;
                    break;
                case climate::CLIMATE_FAN_OFF:
                    level = 0x01;
                    break;
                case climate::CLIMATE_FAN_AUTO:
                    level = 0x00;
                    break;
                case climate::CLIMATE_FAN_ON:
                case climate::CLIMATE_FAN_MIDDLE:
                case climate::CLIMATE_FAN_FOCUS:
                case climate::CLIMATE_FAN_DIFFUSE:
                default:
                    level = -1;
                    break;
            }

            if (level >= 0) {
                ESP_LOGI(TAG, "Setting level to: %i", level);
                uint8_t command[1] = {level};
                comfoair.queueCommand(CMD_SET_LEVEL, command, sizeof(command));
            }
        }

        this->publish_state();
    }

    void update() override {
        if (pollingDoHalfUpdateTick) {
            comfoair.update();
        }
        pollingDoHalfUpdateTick = !pollingDoHalfUpdateTick;

        std::vector<uint8_t> packet = comfoair.getPacket();
        if (packet.size() > 0) {
            // ESP_LOGI(TAG, "Sending packet of size %i", packet.size());

            char packet_str[packet.size() * 3 + 1];
            for (int i = 0; i < packet.size(); i++) {
                sprintf(packet_str + i * 3, "%02X ", packet[i]);
            }
            if (DEBUG) {
                ESP_LOGI(TAG, ">>> OUT Packet: %s (%i)", packet_str, packet.size());
            }

            this->write_array(packet.data(), packet.size());
        }
    }

    void loop() override {
        uint8_t buffer[32];
        uint8_t bufferSize = 0;
        while (this->available() != 0) {
            buffer[bufferSize++] = this->read();
        }

        if (bufferSize > 0) {
            char recv_packet_str[bufferSize * 3 + 1];
            for (int i = 0; i < bufferSize; i++) {
                sprintf(recv_packet_str + i * 3, "%02X ", buffer[i]);
            }
            if (DEBUG) {
                ESP_LOGI(TAG, "<<< IN Packet: %s (%i)", recv_packet_str, bufferSize);
            }

            comfoair.handleIncommingData(buffer, bufferSize);
        }

        std::string lastMessage = comfoair.getLastMessage();
        if (lastMessage.length() > 0) {
            if (DEBUG) {
                ESP_LOGI(TAG, ">>> %s", lastMessage.c_str());
            }
        }

        ComfoAir::RETURN_DATA_TYPE lastPacketType = comfoair.getLastReceivedPacketType();
        if (lastPacketType != ComfoAir::RETURN_DATA_TYPE::NONE) {
            if (DEBUG) {
                ESP_LOGI(TAG, "^^^ Received packet update of type %i", lastPacketType);
            }
            switch (lastPacketType) {
                case ComfoAir::RETURN_DATA_TYPE::FAN_SUPPLY_AIR_PERCENTAGE:
                    this->fan_supply_air_percentage->publish_state(comfoair.getData_uint8_t(comfoair.RETURN_DATA_TYPE::FAN_SUPPLY_AIR_PERCENTAGE));
                    break;

                case ComfoAir::RETURN_DATA_TYPE::FAN_EXHAUST_AIR_PERCENTAGE:
                    this->fan_exhaust_air_percentage->publish_state(comfoair.getData_uint8_t(comfoair.RETURN_DATA_TYPE::FAN_EXHAUST_AIR_PERCENTAGE));
                    break;

                case ComfoAir::RETURN_DATA_TYPE::FAN_SUPPLY_AIR_RPM:
                    this->fan_supply_speed->publish_state(comfoair.getData_float(comfoair.RETURN_DATA_TYPE::FAN_SUPPLY_AIR_RPM));
                    break;

                case ComfoAir::RETURN_DATA_TYPE::FAN_EXHAUST_AIR_RPM:
                    this->fan_exhaust_speed->publish_state(comfoair.getData_float(comfoair.RETURN_DATA_TYPE::FAN_EXHAUST_AIR_RPM));
                    break;

                case ComfoAir::RETURN_DATA_TYPE::FAN_SUPPLY_AIR_LEVEL:
                    this->supply_air_level->publish_state(comfoair.getData_uint8_t(comfoair.RETURN_DATA_TYPE::FAN_SUPPLY_AIR_LEVEL));
                    break;

                case ComfoAir::RETURN_DATA_TYPE::FAN_RETURN_AIR_LEVEL:
                    this->return_air_level->publish_state(comfoair.getData_uint8_t(comfoair.RETURN_DATA_TYPE::FAN_RETURN_AIR_LEVEL));
                    break;

                case ComfoAir::RETURN_DATA_TYPE::VENTILATION_LEVEL:
                    this->ventilation_level->publish_state(comfoair.getData_uint8_t(comfoair.RETURN_DATA_TYPE::VENTILATION_LEVEL));
                    break;

                case ComfoAir::RETURN_DATA_TYPE::IS_BYPASS_VALVE_OPEN:
                    this->is_bypass_valve_open->publish_state(comfoair.getData_bool(comfoair.RETURN_DATA_TYPE::IS_BYPASS_VALVE_OPEN));
                    break;

                case ComfoAir::RETURN_DATA_TYPE::IS_PREHEATING:
                    this->is_preheating->publish_state(comfoair.getData_bool(comfoair.RETURN_DATA_TYPE::IS_PREHEATING));
                    break;

                case ComfoAir::RETURN_DATA_TYPE::IS_SUMMER_MODE:
                    this->is_summer_mode->publish_state(comfoair.getData_bool(comfoair.RETURN_DATA_TYPE::IS_SUMMER_MODE));
                    break;

                case ComfoAir::RETURN_DATA_TYPE::IS_SUPPLY_FAN_ACTIVE:
                    this->is_supply_fan_active->publish_state(comfoair.getData_bool(comfoair.RETURN_DATA_TYPE::IS_SUPPLY_FAN_ACTIVE));
                    break;

                case ComfoAir::RETURN_DATA_TYPE::IS_FILTER_FULL:
                    this->is_filter_full->publish_state(comfoair.getData_bool(comfoair.RETURN_DATA_TYPE::IS_FILTER_FULL));
                    break;

                case ComfoAir::RETURN_DATA_TYPE::IS_T1_OUTSIDE_AIR_PRESENT:
                    this->is_t1_outside_air_present->publish_state(comfoair.getData_bool(comfoair.RETURN_DATA_TYPE::IS_T1_OUTSIDE_AIR_PRESENT));
                    break;

                case ComfoAir::RETURN_DATA_TYPE::IS_T2_SUPPLY_AIR_PRESENT:
                    this->is_t2_supply_air_present->publish_state(comfoair.getData_bool(comfoair.RETURN_DATA_TYPE::IS_T2_SUPPLY_AIR_PRESENT));
                    break;

                case ComfoAir::RETURN_DATA_TYPE::IS_T3_EXHAUST_AIR_IN_PRESENT:
                    this->is_t3_exhaust_air_in_present->publish_state(comfoair.getData_bool(comfoair.RETURN_DATA_TYPE::IS_T3_EXHAUST_AIR_IN_PRESENT));
                    break;

                case ComfoAir::RETURN_DATA_TYPE::IS_T4_EXHAUST_AIR_OUT_PRESENT:
                    this->is_t4_exhaust_air_out_present->publish_state(comfoair.getData_bool(comfoair.RETURN_DATA_TYPE::IS_T4_EXHAUST_AIR_OUT_PRESENT));
                    break;

                case ComfoAir::RETURN_DATA_TYPE::IS_EWT_PRESENT:
                    this->is_ewt_present->publish_state(comfoair.getData_bool(comfoair.RETURN_DATA_TYPE::IS_EWT_PRESENT));
                    break;

                case ComfoAir::RETURN_DATA_TYPE::IS_POST_HEATING_PRESENT:
                    this->is_post_heating_present->publish_state(comfoair.getData_bool(comfoair.RETURN_DATA_TYPE::IS_POST_HEATING_PRESENT));
                    break;

                case ComfoAir::RETURN_DATA_TYPE::IS_KITCHEN_HOOD_PRESENT:
                    this->is_kitchen_hood_present->publish_state(comfoair.getData_bool(comfoair.RETURN_DATA_TYPE::IS_KITCHEN_HOOD_PRESENT));
                    break;

                case ComfoAir::RETURN_DATA_TYPE::IS_ENTHALPY_PRESENT:
                    this->is_enthalpy_present->publish_state(comfoair.getData_bool(comfoair.RETURN_DATA_TYPE::IS_ENTHALPY_PRESENT));
                    break;

                case ComfoAir::RETURN_DATA_TYPE::IS_BYPASS_PRESENT:
                    this->is_bypass_present->publish_state(comfoair.getData_bool(comfoair.RETURN_DATA_TYPE::IS_BYPASS_PRESENT));
                    break;

                case ComfoAir::RETURN_DATA_TYPE::IS_PREHEATER_PRESENT:
                    this->is_preheater_present->publish_state(comfoair.getData_bool(comfoair.RETURN_DATA_TYPE::IS_PREHEATER_PRESENT));
                    break;

                case ComfoAir::RETURN_DATA_TYPE::COMFORT_TEMPERATURE:
                    this->comfort_temperature->publish_state(comfoair.getData_float(comfoair.RETURN_DATA_TYPE::COMFORT_TEMPERATURE));
                    break;

                case ComfoAir::RETURN_DATA_TYPE::OUTSIDE_AIR_TEMPERATURE:
                    this->outside_air_temperature->publish_state(comfoair.getData_float(comfoair.RETURN_DATA_TYPE::OUTSIDE_AIR_TEMPERATURE));
                    break;

                case ComfoAir::RETURN_DATA_TYPE::SUPPLY_AIR_TEMPERATURE:
                    this->supply_air_temperature->publish_state(comfoair.getData_float(comfoair.RETURN_DATA_TYPE::SUPPLY_AIR_TEMPERATURE));
                    break;

                case ComfoAir::RETURN_DATA_TYPE::RETURN_AIR_TEMPERATURE:
                    this->return_air_temperature->publish_state(comfoair.getData_float(comfoair.RETURN_DATA_TYPE::RETURN_AIR_TEMPERATURE));
                    break;

                case ComfoAir::RETURN_DATA_TYPE::EXHAUST_AIR_TEMPERATURE:
                    this->exhaust_air_temperature->publish_state(comfoair.getData_float(comfoair.RETURN_DATA_TYPE::EXHAUST_AIR_TEMPERATURE));
                    break;

                case ComfoAir::RETURN_DATA_TYPE::EWT_TEMPERATURE:
                    this->ewt_temperature->publish_state(comfoair.getData_float(comfoair.RETURN_DATA_TYPE::EWT_TEMPERATURE));
                    break;

                case ComfoAir::RETURN_DATA_TYPE::REHEATING_TEMPERATURE:
                    this->reheating_temperature->publish_state(comfoair.getData_float(comfoair.RETURN_DATA_TYPE::REHEATING_TEMPERATURE));
                    break;

                case ComfoAir::RETURN_DATA_TYPE::KITCHEN_HOOD_TEMPERATURE:
                    this->kitchen_hood_temperature->publish_state(comfoair.getData_float(comfoair.RETURN_DATA_TYPE::KITCHEN_HOOD_TEMPERATURE));
                    break;

                case ComfoAir::RETURN_DATA_TYPE::ENTHALPY_TEMPERATURE:
                    this->enthalpy_temperature->publish_state(comfoair.getData_float(comfoair.RETURN_DATA_TYPE::ENTHALPY_TEMPERATURE));
                    break;

                case ComfoAir::RETURN_DATA_TYPE::ENTHALPY_HUMIDITY:
                    this->enthalpy_humidity->publish_state(comfoair.getData_uint8_t(comfoair.RETURN_DATA_TYPE::ENTHALPY_HUMIDITY));
                    break;

                case ComfoAir::RETURN_DATA_TYPE::BYPASS_FACTOR:
                    this->bypass_factor->publish_state(comfoair.getData_uint8_t(comfoair.RETURN_DATA_TYPE::BYPASS_FACTOR));
                    break;

                case ComfoAir::RETURN_DATA_TYPE::BYPASS_STEP:
                    this->bypass_step->publish_state(comfoair.getData_uint8_t(comfoair.RETURN_DATA_TYPE::BYPASS_STEP));
                    break;

                case ComfoAir::RETURN_DATA_TYPE::BYPASS_CORRECTION:
                    this->bypass_correction->publish_state(comfoair.getData_uint8_t(comfoair.RETURN_DATA_TYPE::BYPASS_CORRECTION));
                    break;

                default:
                    break;
            }
        }

        if (comfoair.doPublishState()) {
            // Triggered if the core has finished with a full update cycle
        }
    }

   private:
    ComfoAir comfoair = ComfoAir();

   public:
    sensor::Sensor *fan_supply_air_percentage{nullptr};
    sensor::Sensor *fan_exhaust_air_percentage{nullptr};
    sensor::Sensor *fan_supply_speed{nullptr};
    sensor::Sensor *fan_exhaust_speed{nullptr};
    sensor::Sensor *comfort_temperature{nullptr};
    sensor::Sensor *outside_air_temperature{nullptr};  // Supply to WHR
    sensor::Sensor *supply_air_temperature{nullptr};   // Supply to house
    sensor::Sensor *return_air_temperature{nullptr};   // Return from house
    sensor::Sensor *exhaust_air_temperature{nullptr};  // Return from WHR
    sensor::Sensor *enthalpy_temperature{nullptr};
    sensor::Sensor *enthalpy_humidity{nullptr};
    sensor::Sensor *ewt_temperature{nullptr};
    sensor::Sensor *reheating_temperature{nullptr};
    sensor::Sensor *kitchen_hood_temperature{nullptr};
    sensor::Sensor *return_air_level{nullptr};
    sensor::Sensor *supply_air_level{nullptr};
    sensor::Sensor *ventilation_level{nullptr};

    sensor::Sensor *bypass_factor{nullptr};
    sensor::Sensor *bypass_step{nullptr};
    sensor::Sensor *bypass_correction{nullptr};

    binary_sensor::BinarySensor *is_t1_outside_air_present{nullptr};
    binary_sensor::BinarySensor *is_t2_supply_air_present{nullptr};
    binary_sensor::BinarySensor *is_t3_exhaust_air_in_present{nullptr};
    binary_sensor::BinarySensor *is_t4_exhaust_air_out_present{nullptr};
    binary_sensor::BinarySensor *is_ewt_present{nullptr};
    binary_sensor::BinarySensor *is_post_heating_present{nullptr};
    binary_sensor::BinarySensor *is_kitchen_hood_present{nullptr};
    binary_sensor::BinarySensor *is_bypass_present{nullptr};
    binary_sensor::BinarySensor *is_preheater_present{nullptr};
    binary_sensor::BinarySensor *is_enthalpy_present{nullptr};

    binary_sensor::BinarySensor *is_bypass_valve_open{nullptr};
    binary_sensor::BinarySensor *is_preheating{nullptr};
    binary_sensor::BinarySensor *is_summer_mode{nullptr};
    binary_sensor::BinarySensor *is_supply_fan_active{nullptr};
    binary_sensor::BinarySensor *is_filter_full{nullptr};
};

};  // namespace comfoair
};  // namespace esphome