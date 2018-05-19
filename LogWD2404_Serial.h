#ifndef _LOGWD2404_SERIAL_H
#define _LOGWD2404_SERIAL_H

#if defined(ARDUINO) && ARDUINO >= 100
   #include "Arduino.h"
#else
   #include "WProgram.h"
#endif   

#include "WD2404.h"

/**
 * Faz o led piscar quando há pulsos no WD2404 associado.
 * 
 */
class LogWD2404_Serial {
private:
  const __FlashStringHelper *id;
  WD2404 *wd2404;

  MetodoCallback<WD2404,LogWD2404_Serial> callbackEnable;
  MetodoCallback<WD2404,LogWD2404_Serial> callbackDisable;

  void onEnabled(WD2404 *source) {
    log(F("Enabled"));
  }

  void onDisabled(WD2404 *source) {
    log(F("Disabled"));
  }

  void log(const __FlashStringHelper *msg) {
    Serial.print(this->id);
    Serial.print(F("-"));
    Serial.println(msg);    
  }

public:  
  LogWD2404_Serial():
            callbackEnable (MetodoCallback<WD2404,LogWD2404_Serial>(this, &LogWD2404_Serial::onEnabled)),
            callbackDisable(MetodoCallback<WD2404,LogWD2404_Serial>(this, &LogWD2404_Serial::onDisabled))
            {  }

  void begin(const __FlashStringHelper *id, WD2404 &wd2404) {
    this->wd2404 = &wd2404;
    this->id = id;              
    this->wd2404->setCallbackEnable(&callbackEnable);
    this->wd2404->setCallbackDisable(&callbackDisable);
  }
   
  WD2404 *getWD2404() { return this->wd2404; }
};

#endif
