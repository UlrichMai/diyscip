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

#ifndef CTRLPANEL_H
#define CTRLPANEL_H

#include "CFGSettings.h"
#include "Arduino.h"

#include "config.h"

class CTRLPanel {
  
  public:
    static CTRLPanel* getInstance();
    
    uint32_t  getFrameCounter();
    uint32_t  getFrameDropped();

    uint16_t  getWaterTemperatureCelsius();
    uint16_t  getDesiredTemperatureCelsius();
    uint16_t  getDesiredTemperatureCelsiusEx();

    uint16_t  getRawStatus();
    
    uint8_t   isPowerOn();
    uint8_t   isFilterOn();
    uint8_t   isBubbleOn();
    uint8_t   isHeaterOn();
    uint8_t   isHeatReached();

#ifdef SJB_HS
    uint8_t   isJetOn();
    uint8_t   isCleanOn();
#ifdef PCB_DESIGN_3
    bool      setBubbleOn(bool v);
    bool      setJetOn(bool v);
    bool      setCleanOn(bool v);
#endif    
#endif    

    bool      hasError();
    uint16_t  getError();

    bool      setDesiredTemperatureCelsius(uint16_t temp);
    bool      setPowerOn(bool v);
    bool      setFilterOn(bool v);
    bool      setFilterOnEx(bool v);
    bool      setHeaterOn(bool v);
    bool      setHeaterOnEx(bool v);

    bool      isSetupModeTriggered();
    uint16_t  getPowerConsumption();

  private:
    static CTRLPanel*   instance;
    
    static volatile uint16_t  frameValue;
    static volatile uint16_t  frameShift;
    static volatile uint32_t  frameCounter;
    static volatile uint32_t  frameDropped;
    
    static volatile uint16_t  ledStatus;
    static volatile uint16_t  displayValue;
    static volatile uint16_t  waterTemp;
    static volatile uint16_t  desiredTemp;

    static volatile uint16_t  lastWaterTemp;

    static volatile uint32_t  lastBlackDisplayFrameCounter;
    static volatile bool      isDisplayBlink;

    static volatile uint32_t  lastWaterTempChangeFrameCounter;
    static volatile uint32_t  lastDesiredTempChangeFrameCounter;
    static volatile uint32_t  lastLedStatusChangeFrameCounter;
    
    static volatile uint16_t  unsetDigits;

    static volatile uint8_t   lastTempUnit;
    static volatile uint32_t  lastTempUnitChangeFrameCounter;
    static volatile uint16_t  counterTempUnitChanged;
#ifdef PCB_DESIGN_3
    #define BUTTON_COUNT    9
    static volatile bool    buttonPushRequest[BUTTON_COUNT];
    static volatile int     buttonPushCycles[BUTTON_COUNT];
    static volatile bool    buttonPushActive;
#endif
    CTRLPanel();
    static ICACHE_RAM_ATTR void clckRisingInterrupt();
    static ICACHE_RAM_ATTR void holdRisingInterrupt();

    void  pushButton(int8_t button);

    uint16_t  convertDisplayToCelsius(uint16_t displayValue);
};



#endif // CTRLPANEL_H
