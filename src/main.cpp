/**
 * DIYSCIP (c) by Geoffroy HUBERT - yorffoeg@gmail.com
 * This file is part of DIYSCIP <https://github.com/yorffoeg/diyscip>.
 * 
 * DIYSCIP is licensed under a
 * Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 * 
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nc-sa/4.0/>.
 * 
 * DIYSCIP is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY;
 */ 


#include <Arduino.h>

#include "CFGSettings.h"
#include "CTRLPanel.h"
#include "MQTTClient.h"
#include "WIFIManager.h"
#include "TEMPSensor.h"
#include "LEDManager.h"
#include "OTAUpdate.h"
#include "Debug.h"
#include "config.h"

#ifdef OTA_ENABLED
  #include <ArduinoOTA.h>
#endif
#ifdef HOMEKIT
  #include <arduino_homekit_server.h>
  #include "HAPAccessory.h"
#endif


/******************************************************************/
CFGSettings   cfgSettings;
LEDManager    ledBuiltin(LED_BUILTIN);
WIFIManager   wifiManager(cfgSettings);

CTRLPanel*    controlPanel  = CTRLPanel::getInstance();
#ifdef PCB_OPTIONS_TEMP_SENSOR
TEMPSensor*   tempSensor    = TEMPSensor::getInstance();
#endif
MQTTClient*   mqttClient    = NULL;


#ifndef NODEBUG
  #ifdef DBG_TCP_ENABLED
    WiFiServer  serverDebug(DBG_TCP_PORT);
    WiFiClient clientDebug;
  #endif // DBG_TCP
#endif // NODEBUG


void setup() {
  Serial.begin(SERIAL_DEBUG_SPEED);

  DBG("\n\n\nSETUP: starting");
  
  wifiManager.setup();

  #ifdef OTA_ENABLED
    ArduinoOTA.setPassword(PASSWORD_OTA);
     
    ArduinoOTA.onStart([]() {
      DBG("OTA: Start [%d]", ArduinoOTA.getCommand());
    });
    
    ArduinoOTA.onEnd([]() {
      DBG("OTA: End");
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      DBG("OTA: Progress: %u%%\r", (progress / (total / 100)));
    });

    ArduinoOTA.onError([](ota_error_t error) {
      DBG("OTA: Error [%u]", error);
    });

    ArduinoOTA.begin();
    DBG("SETUP: OTA Enabled");

  #endif // OTA_ENABLED
  
  #if !defined(NODEBUG) && defined(DBG_TCP_ENABLED)
    serverDebug.begin();
  #endif // NODEBUG

  if (wifiManager.isSTA()) {
    ledBuiltin.setMode(BLINK_1);
#ifdef MQTT
    mqttClient = new MQTTClient(cfgSettings);
    mqttClient->connect();

    mqttClient->addPublisher("spa/sys/wifi",           []() -> uint16_t { return WIFIManager::getWifiQuality(); });
    mqttClient->setLastAddedPublisherUpdateInterval(WIFIQUALITY_UPDATE_INTERVAL);

    #ifdef PCB_OPTIONS_TEMP_SENSOR
    mqttClient->addPublisher("spa/temp/board",         []() -> uint16_t { return tempSensor->getAverageTemperatureCelsius(); });
    #endif
    mqttClient->setLastAddedPublisherUpdateInterval(TEMP_UPDATE_INTERVAL);

    mqttClient->addPublisher("spa/error",              []() -> uint16_t { return controlPanel->getError(); });

    mqttClient->addPublisher("spa/temp/water",         []() -> uint16_t { return controlPanel->getWaterTemperatureCelsius(); });
    mqttClient->addPublisher("spa/temp/desired",       []() -> uint16_t { return controlPanel->getDesiredTemperatureCelsius(); });
    mqttClient->addPublisher("spa/state/power",        []() -> uint8_t  { return controlPanel->isPowerOn(); });
    mqttClient->addPublisher("spa/state/filter",       []() -> uint8_t  { return controlPanel->isFilterOn(); });
    mqttClient->addPublisher("spa/state/heater",       []() -> uint8_t  { return controlPanel->isHeaterOn(); });
    mqttClient->addPublisher("spa/state/bubble",       []() -> uint8_t  { return controlPanel->isBubbleOn(); });

#ifdef SJB_HS
    mqttClient->addPublisher("spa/state/jet",          []() -> uint8_t  { return controlPanel->isJetOn(); });
    mqttClient->addPublisher("spa/state/clean",        []() -> uint8_t  { return controlPanel->isCleanOn(); });
#ifdef PCB_DESIGN_3
    mqttClient->addSubscriber("spa/state/bubble/set",  [](bool v) -> bool { return controlPanel->setBubbleOn(v); });
    mqttClient->addSubscriber("spa/state/jet/set",     [](bool v) -> bool { return controlPanel->setJetOn(v); });
    mqttClient->addSubscriber("spa/state/clean/set",   [](bool v) -> bool { return controlPanel->setCleanOn(v); });
#endif
#endif

    mqttClient->addPublisher("spa/state/heatreached",  []() -> uint8_t  { return controlPanel->isHeatReached(); });
    mqttClient->addPublisher("spa/state",              []() -> uint16_t { return controlPanel->getRawStatus(); });
    mqttClient->addPublisher("spa/sys/powerconsumption",[]() -> uint16_t { return controlPanel->getPowerConsumption(); });

    mqttClient->addSubscriber("spa/temp/desired/set",  [](uint16_t v) -> bool { return controlPanel->setDesiredTemperatureCelsius(v); });
    mqttClient->addSubscriber("spa/state/power/set",   [](bool v) -> bool { return controlPanel->setPowerOn(v); });
    mqttClient->addSubscriber("spa/state/filter/set",  [](bool v) -> bool { return controlPanel->setFilterOn(v); });
    mqttClient->addSubscriber("spa/state/heater/set",  [](bool v) -> bool { return controlPanel->setHeaterOn(v); });

    if (cfgSettings.isUpdateEnabled()) {
      mqttClient->addSubscriber("spa/sys/update",   [](const char* v) -> bool { return OTAUpdate::fwImageURL(v, mqttClient); });
    }

    mqttClient->setSetupModeTrigger([]() -> bool { return controlPanel->isSetupModeTriggered(); });
  #ifdef HOMEKIT
    mqttClient->addSubscriber("spa/reset",  [](uint16_t v) -> bool 
    {
      switch(v) {
        case 1: ESP.restart(); break;
        case 2: cfgSettings.enterMode(CFGSettings::SETUP); break;
        case 3: homekit_server_reset(); ESP.restart(); break;
      } 
    return true;
    });
  #endif
#endif
#ifdef HOMEKIT

    accessory_name.value = HOMEKIT_STRING_CPP( const_cast<char*>(cfgSettings.getDeviceID()) );
  #ifdef SSP_H
    accessory_model.value = HOMEKIT_STRING_CPP( "SSP_H" );
  #else
    accessory_model.value = HOMEKIT_STRING_CPP( "SJB_HS" );
  #endif

    switch_pump_on.getter = 
      []() -> homekit_value_t { return HOMEKIT_BOOL_CPP( controlPanel->isFilterOn() == 0x01 ); }; 
    switch_pump_on.setter = 
      [](const homekit_value_t v) -> void { controlPanel->setFilterOnEx(v.bool_value); switch_pump_on.value = v; };

    thermostat_current_temperature.getter = 
      []() -> homekit_value_t { return HOMEKIT_FLOAT_CPPX(controlPanel->getWaterTemperatureCelsius(),0.0,100.0 );  };

    thermostat_target_temperature.getter = 
      []() -> homekit_value_t { return HOMEKIT_FLOAT_CPPX(controlPanel->getDesiredTemperatureCelsiusEx(),20.0,40.0 ); };
    thermostat_target_temperature.setter = 
      [](const homekit_value_t v) -> void { controlPanel->setDesiredTemperatureCelsius(v.float_value); thermostat_target_temperature.value = v; };

    thermostat_current_heating_cooling_state.getter = //isHeating
      []() -> homekit_value_t { return HOMEKIT_UINT8_CPP( (controlPanel->isHeaterOn()    == 0x01
                                                        && controlPanel->isHeatReached() != 0x01 ) ? 1 : 0 ); };

    thermostat_target_heating_cooling_state.getter = 
      []() -> homekit_value_t { return HOMEKIT_UINT8_CPP( controlPanel->isHeaterOn() == 0x01 ? 1 : 0 ); };
    thermostat_target_heating_cooling_state.setter =
      [](const homekit_value_t v) -> void { controlPanel->setHeaterOnEx(v.uint8_value != 0); thermostat_target_heating_cooling_state.value = v; };
#ifdef PCB_OPTIONS_TEMP_SENSOR
    board_temperature_sensor_temperature.getter = 
      []() -> homekit_value_t { return HOMEKIT_FLOAT_CPPX(tempSensor->getAverageTemperatureCelsius(),0.0,100.0 ); };
#endif
    arduino_homekit_setup(&config);

#endif

  } else {
    
    ledBuiltin.setMode(BLINK_3);
#ifdef HOMEKIT
    homekit_server_reset();
#endif
  }
}

int i = 0;
char buf[50];

void loop() {


  #ifdef OTA_ENABLED
    ArduinoOTA.handle();
  #endif

  #if !defined(NODEBUG) && defined(DBG_TCP_ENABLED)
    if (!clientDebug.connected()) {
      clientDebug.flush();
      clientDebug.stop();
      clientDebug = serverDebug.available();
    } else {
      if (clientDebug.available() && clientDebug.readString().equals(DBG_TCP_CLIENT_PING)) {
        clientDebug.write(DBG_TCP_CLIENT_PONG);
      }
    }
  #endif // NODEBUG

  ledBuiltin.loop();

  if (wifiManager.isSTAConnected()) { // Wifi STA mode
#ifdef MQTT
    if (mqttClient != NULL)  
      mqttClient->loop();
#endif      
#ifdef HOMEKIT
    arduino_homekit_loop();
    if (arduino_homekit_is_paired()) 
      homekit_notify_loop();
  #ifndef NODEBUG      
    static uint32_t next_heap_millis = 0;
    uint32_t time = millis();
    if (time > next_heap_millis) {
      DBG("heap: %d, sockets: %d",
          ESP.getFreeHeap(), arduino_homekit_connected_clients_count());
      next_heap_millis = time + 5000;
      i++;
      sprintf(buf,"Verbrauch %d kWh",i );
    power_sensor_name.value = HOMEKIT_STRING_CPP( buf );
    power_sensor_temperature.value = HOMEKIT_FLOAT_CPPX(i,0.0,100.0);
    homekit_characteristic_notify(&power_sensor_temperature,power_sensor_temperature.value); 
    }
  #endif 
#endif 
  } else { // Wifi AP mode
    wifiManager.loop();
  }

  delay(100);
}