#ifndef _LOGRELOGIO_SERIAL_H
#define _LOGRELOGIO_SERIAL_H

#if defined(ARDUINO) && ARDUINO >= 100
   #include "Arduino.h"
#else
   #include "WProgram.h"
#endif   

#include "Relogio.h"

/**
 * Faz o led piscar quando há pulsos no WD2404 associado.
 * 
 */
class LogRelogio_Serial {
private:
  const __FlashStringHelper *id;

  MetodoCallback<WD2404,LogRelogio_Serial> callbackEnable;
  MetodoCallback<WD2404,LogRelogio_Serial> callbackDisable;
  MetodoCallback<Relogio,LogRelogio_Serial> callbackLigado;
  MetodoCallback<Relogio,LogRelogio_Serial> callbackDesligado;

  void onEnabled(WD2404 *source) {
    log(F("Enabled"));
  }

  void onDisabled(WD2404 *source) {
    log(F("Disabled"));
  }

  void onLigado(Relogio *source) {
    log(F("Ligado"));
  }

  void onDesligado(Relogio *source) {
    log(F("Desligado"));
  }

  void log(const __FlashStringHelper *msg) {
    Serial.print(this->id);
    Serial.print(F("-"));
    Serial.println(msg);    
  }

public:  
  LogRelogio_Serial():
            callbackEnable (MetodoCallback<WD2404,LogRelogio_Serial>(this, &LogRelogio_Serial::onEnabled)),
            callbackDisable(MetodoCallback<WD2404,LogRelogio_Serial>(this, &LogRelogio_Serial::onDisabled)),
            callbackLigado(MetodoCallback<Relogio,LogRelogio_Serial>(this, &LogRelogio_Serial::onLigado)),
            callbackDesligado(MetodoCallback<Relogio,LogRelogio_Serial>(this, &LogRelogio_Serial::onDesligado))
            {  }

  void begin(const __FlashStringHelper *id, Relogio &relogio) {

    this->id = id;
    relogio.getWD2404()->setCallbackEnable(&callbackEnable);
    relogio.getWD2404()->setCallbackDisable(&callbackDisable);
    relogio.setCallbackOnLigado(&callbackLigado);
    relogio.setCallbackOnDesligado(&callbackDesligado);
  }
   
};

#endif
