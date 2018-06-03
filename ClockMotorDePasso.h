#ifndef _CLOCK_MOTOR_PASSO_H
#define _CLOCK_MOTOR_PASSO_H

#if defined(ARDUINO) && ARDUINO >= 100
   #include "Arduino.h"
#else
   #include "WProgram.h"
#endif   

#include "WD2404.h"
#include "util.h"

// essa constante é o numero de passos que eh necessario prover para o relogio movimentar 1 hora.
#define PASSOS_POR_HORA 16000

/**
 * Conta as horas, minutos e segundos baseando-se no movimento do motor de passo ligado a um relogio mecanico.
 * 
 */
class ClockMotorDePasso {
private:
  WD2404 *wd2404;

  unsigned long passosAcumulados;
  
  Callback<ClockMotorDePasso> *callbackOnSegundo;
  Callback<ClockMotorDePasso> *callbackOnMinuto;
  Callback<ClockMotorDePasso> *callbackOnHora;
  
  MetodoCallback<WD2404, ClockMotorDePasso> wd2404OnPulChange;
  
  void contaPassos(WD2404 *wd2404);
    
public:  

  /**
   * Associa esse clock ao WD2404 que gera os pulsos ao motor de passo.
   */
  ClockMotorDePasso(WD2404 *wd2404);
   
  unsigned long getPassosAcumulados() { return this->passosAcumulados; }
  unsigned int getHora();
  unsigned int getMinuto();
  unsigned int getSegundo();
  
  /** ajusta este clock com o ponteiro do relogio mecanico */
  void set(unsigned int hora, unsigned int minuto, unsigned int segundo);

  void setCallbackOnSegundo( Callback<ClockMotorDePasso> *callback ) {
      callback->setNext(this->callbackOnSegundo);
      this->callbackOnSegundo = callback;
  }
  void setCallbackOnMinuto( Callback<ClockMotorDePasso> *callback ) {
      callback->setNext(this->callbackOnMinuto);
      this->callbackOnMinuto = callback;
  }
  void setCallbackOnHora( Callback<ClockMotorDePasso> *callback ) {
      callback->setNext(this->callbackOnHora);
      this->callbackOnHora = callback;
  }

};

#endif


