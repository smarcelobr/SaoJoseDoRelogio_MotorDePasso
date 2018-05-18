#ifndef _INDICA_PULSOS_H
#define _INDICA_PULSOS_H

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
class IndicaPulsos {
private:
  WD2404 *wd2404;
  int ledPin;

  MetodoCallback<WD2404,IndicaPulsos> callbackPulChange;
  MetodoCallback<WD2404,IndicaPulsos> callbackEnable;
  MetodoCallback<WD2404,IndicaPulsos> callbackDisable;

  void piscaLed(WD2404 *source) {
    digitalWrite(ledPin, source->getFaseDoPulso());
  }

  void onEnabled(WD2404 *source) {
    digitalWrite(ledPin, HIGH);
  }

  void onDisabled(WD2404 *source) {
    digitalWrite(ledPin, LOW);
  }

public:  

  IndicaPulsos(WD2404 &wd2404, int ledPin): wd2404(&wd2404), ledPin(ledPin),
            callbackPulChange(MetodoCallback<WD2404,IndicaPulsos>(this,&IndicaPulsos::piscaLed)),
            callbackEnable(MetodoCallback<WD2404,IndicaPulsos>(this,&IndicaPulsos::onEnabled)),
            callbackDisable(MetodoCallback<WD2404,IndicaPulsos>(this,&IndicaPulsos::onDisabled)){
              
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW);
    
    this->wd2404->setCallbackPulChange(&callbackPulChange);
    this->wd2404->setCallbackEnable(&callbackEnable);
    this->wd2404->setCallbackDisable(&callbackDisable);
    
  }
   
  WD2404 *getWD2404() { return this->wd2404; }
};

#endif
