/*
 * HAPAccessory.c
 * Define the homekit accessories for the pool controller
 *
 *  Created on: 2020-05-16
 *      Author: Ulrich Mai
 */

#include <homekit/homekit.h>
#include <homekit/characteristics.h>
#include "HAPAccessory.h"

void accessory_identify(homekit_value_t _value) {
	printf("accessory identify\n");
}

homekit_characteristic_t accessory_name                           = HOMEKIT_CHARACTERISTIC_(NAME, "Pool");
homekit_characteristic_t accessory_manufacturer                   = HOMEKIT_CHARACTERISTIC_(MANUFACTURER, "Intex");
homekit_characteristic_t accessory_serial_number                  = HOMEKIT_CHARACTERISTIC_(SERIAL_NUMBER, "SN_00000001");
homekit_characteristic_t accessory_model                          = HOMEKIT_CHARACTERISTIC_(MODEL, "SSP_H");
homekit_characteristic_t accessory_firmware_revision              = HOMEKIT_CHARACTERISTIC_(FIRMWARE_REVISION, "0.0.0");
homekit_characteristic_t accessory_identify_cb                    = HOMEKIT_CHARACTERISTIC_(IDENTIFY, accessory_identify);

homekit_characteristic_t thermostat_name                          = HOMEKIT_CHARACTERISTIC_(NAME, "Pool Heizung");
homekit_characteristic_t thermostat_current_temperature           = HOMEKIT_CHARACTERISTIC_(CURRENT_TEMPERATURE, 19.0, .min_step  = (float[]) {1.0},  .getter=NULL );
homekit_characteristic_t thermostat_target_temperature            = HOMEKIT_CHARACTERISTIC_(TARGET_TEMPERATURE,  31.0, .min_value = (float[]) {10.0}, .max_value = (float[]) {40.0}, .min_step = (float[]) {1.0}, .getter=NULL, .setter=NULL );
homekit_characteristic_t thermostat_current_heating_cooling_state = HOMEKIT_CHARACTERISTIC_(CURRENT_HEATING_COOLING_STATE, 0, .valid_values = {.count = 2, .values = (uint8_t[]) {0,1} }, .getter=NULL );
homekit_characteristic_t thermostat_target_heating_cooling_state  = HOMEKIT_CHARACTERISTIC_(TARGET_HEATING_COOLING_STATE,  0, .valid_values = {.count = 2, .values = (uint8_t[]) {0,1} }, .getter=NULL, .setter=NULL  );
homekit_characteristic_t thermostat_temperature_display_units     = HOMEKIT_CHARACTERISTIC_(TEMPERATURE_DISPLAY_UNITS, 0); //Celsius

homekit_characteristic_t switch_power_name                        = HOMEKIT_CHARACTERISTIC_(NAME, "Pool Power");
homekit_characteristic_t switch_power_on                          = HOMEKIT_CHARACTERISTIC_(ON, false, .getter=NULL, .setter=NULL );
homekit_characteristic_t switch_pump_name                         = HOMEKIT_CHARACTERISTIC_(NAME, "Pool Pumpe");
homekit_characteristic_t switch_pump_on                           = HOMEKIT_CHARACTERISTIC_(ON, false, .getter=NULL, .setter=NULL );

homekit_accessory_t *accessories[] =
		{
				HOMEKIT_ACCESSORY(
						.id = 1,
						.category = homekit_accessory_category_thermostat,
						.services=(homekit_service_t*[]){
							HOMEKIT_SERVICE(ACCESSORY_INFORMATION,
								.characteristics=(homekit_characteristic_t*[]){
									&accessory_name,
									&accessory_manufacturer,
									&accessory_serial_number,
									&accessory_model,
									&accessory_firmware_revision,
                  &accessory_identify_cb,
									NULL
								}),
             
              HOMEKIT_SERVICE(THERMOSTAT, .primary=true,
                .characteristics=(homekit_characteristic_t*[]) {
                  &thermostat_name,
                  &thermostat_current_temperature,
                  &thermostat_target_temperature,
                  &thermostat_current_heating_cooling_state,
                  &thermostat_target_heating_cooling_state,
                  &thermostat_temperature_display_units,
                  NULL
                }),
              
              HOMEKIT_SERVICE(SWITCH, .primary=false,
                .characteristics=(homekit_characteristic_t*[]){
                  &switch_power_name,
                  &switch_power_on,
                  NULL
                }),
              
              HOMEKIT_SERVICE(SWITCH, .primary=false,
                .characteristics=(homekit_characteristic_t*[]){
                  &switch_pump_name,
                  &switch_pump_on,
                  NULL
                }),
              
							NULL
						}),
				NULL
		};

homekit_server_config_t config = {
		.accessories = accessories,
		.password = "111-11-111",
		//.on_event = on_homekit_event,
		.setupId = "ABCD"
};

static homekit_value_t old_value[6];
static homekit_value_t new_value;

#define NOTIFY_WHEN_CHANGED(no,cha) \
  new_value = (cha).getter(); \
  if (!homekit_value_equal(&old_value[(no)],&new_value)) { \
    old_value[(no)] = new_value; \
    (cha).value = new_value; \
    homekit_characteristic_notify(&(cha),new_value); \
  }  

void homekit_notify_loop() {
  NOTIFY_WHEN_CHANGED(0,switch_power_on);
  NOTIFY_WHEN_CHANGED(1,switch_pump_on);
  NOTIFY_WHEN_CHANGED(2,thermostat_current_temperature);
  NOTIFY_WHEN_CHANGED(3,thermostat_target_temperature);
  NOTIFY_WHEN_CHANGED(4,thermostat_current_heating_cooling_state);
  NOTIFY_WHEN_CHANGED(5,thermostat_target_heating_cooling_state);
}