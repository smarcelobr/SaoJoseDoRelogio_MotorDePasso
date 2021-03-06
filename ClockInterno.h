#ifndef _CLOCK_INTERNO_H
#define _CLOCK_INTERNO_H

#if defined(ARDUINO) && ARDUINO >= 100
   #include "Arduino.h"
#else
   #include "WProgram.h"
#endif   

#include "Temporizador.h"
#include "util.h"
/**
 * Conta as horas, minutos e segundos usando o clock do arduino.
 * Fiz essa classe apenas para testar a precisão da classe Temporizador 
 * com um relógio externo.
 * 
 */
class ClockInterno : public Clock {
private:
  unsigned int hora;
  unsigned int minuto;
  unsigned int segundo;
  
  MetodoTemporizado<ClockInterno> aCadaSegundoTimer;
  void doEachSegundo(ItemTemporizado *source);  

  Callback<ClockInterno> *callbackOnSegundo;
  void (*onMinuto)(ClockInterno *source); // funcao callback quando um minuto
  void (*onHora)(ClockInterno *source); // funcao callback quando uma hora
    
public:  

  /**
   * Cria e configura os pinos como OUTPUT e coloca valores iniciais
   */
  ClockInterno(unsigned int horaInicial, unsigned int minutoInicial, unsigned int segundoInicial);
   
  unsigned int getHora();
  unsigned int getMinuto();
  unsigned int getSegundo();
  
  void addSegundos(unsigned int segundos);
  void set(unsigned int hora, unsigned int minuto, unsigned int segundo);

  void setCallbackOnSegundo( Callback<ClockInterno> *callback ) {
      callback->setNext(this->callbackOnSegundo);
      this->callbackOnSegundo = callback;
  }
  void setCallbackOnMinuto( void (*onMinuto)(ClockInterno *source) ) { this->onMinuto = onMinuto; }
  void setCallbackOnHora( void (*onDesligado)(ClockInterno *source) ) { this->onHora = onHora; }

  void pausar() { this->aCadaSegundoTimer.pausar(); }
  void continuar() { this->aCadaSegundoTimer.continuar(); }
};

#endif


