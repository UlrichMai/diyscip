#include "Debug.h"
#include "config.h"
#include "CTRLPanel.h"

// Automation data
uint16_t lastState = UNSET_VALUE;
uint32_t lastStateTime=0;

void Automation_loop(CTRLPanel* controlPanel) {

    // Automation
    uint16_t currentState = controlPanel->getRawStatus();
    uint32_t currentStateTime = millis();

    if (lastState != currentState) {
    //DBG("lastState != currentState 1: currentState =%d, currentStateTime=%d, lastState=%d, lastStateTime=%d",currentState, currentStateTime, lastState, lastStateTime);
    lastState = currentState;
    lastStateTime = millis();
    //DBG("lastState != currentState 2: currentState =%d, currentStateTime=%d, lastState=%d, lastStateTime=%d",currentState, currentStateTime, lastState, lastStateTime);
    }
    // power off when idle for 10min
    if (currentStateTime > lastStateTime+(AUTOMATION_POWER_OFF_IDLE) 
    && currentState == (FRAME_LED|FRAME_LED_POWER)) {
    //DBG("Auto Power Off triggered: currentState =%d, currentStateTime=%d, lastState=%d, lastStateTime=%d",currentState, currentStateTime, lastState, lastStateTime);
    controlPanel->setPowerOn(false);
    delay(1000);
    }
    // filter off after 2h 
    if (currentStateTime > lastStateTime+(AUTOMATION_FILTER_OFF)  
    && currentState == (FRAME_LED|FRAME_LED_POWER|FRAME_LED_FILTER)) {
    //DBG("Auto Filter Off triggered: currentState =%d, currentStateTime=%d, lastState=%d, lastStateTime=%d",currentState, currentStateTime, lastState, lastStateTime);
    controlPanel->setFilterOn(false);
    delay(1000);
    }
/*
    static uint32_t next_debug_millis = 0;
    uint32_t time = millis();
    if (time > next_debug_millis) {
      next_debug_millis = time + 5000;
      DBG("currentState =%d, currentStateTime=%d, lastState=%d, lastStateTime=%d",currentState, currentStateTime, lastState, lastStateTime);
    }
*/
}



