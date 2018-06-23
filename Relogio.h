#ifndef _RELOGIO_H
#define _RELOGIO_H

#if defined(ARDUINO) && ARDUINO >= 100
   #include "Arduino.h"
#else
   #include "WProgram.h"
#endif   

#include "WD2404.h"

#define NUM_PULSADORES 2

/**
 * Responsável por enviar ao motor de passo (vir driver WD-2404) os pulsos necessários
 * para movimentar o relógio a cada minuto.
 * 
 */
class Relogio {
private:
  WD2404 *wd2404;

  boolean ligado;
  unsigned long pulsosPendentes, pulsosDe3Minutos;

  MetodoTemporizado<Relogio> aCadaMillisPorPulsoExecuta_1, aCadaMillisPorPulsoExecuta_2;

  void doEachMillisPorPulso(ItemTemporizado *source);
  void doEachMinuto(ItemTemporizado *source);  
  
  Callback<Relogio> *callbackOnLigado; // funcao callback quando o relogio é ligado
  Callback<Relogio> *callbackOnDesligado; // funcao callback quando o relogio é desligado
  
  MetodoTemporizado<Relogio> * getPulsador(int idxPulsador);
public:
  /**
   * Cria e configura os pinos como OUTPUT e coloca valores iniciais
   */
  Relogio(WD2404 &wd2404);
   
  void setMillisPorPulso(int idxPulsador, unsigned long millisPorPulso); 
  unsigned long getMillisPorPulso(int idxPulsador);
  
  void ligar();
  void desligar();
  
  boolean isLigado() { return ligado; }
  WD2404 *getWD2404() { return this->wd2404; }
  
   void setCallbackOnLigado(Callback<Relogio> *callback ) { 
      callback->setNext(this->callbackOnLigado);
      this->callbackOnLigado = callback;
   }
   void setCallbackOnDesligado(  Callback<Relogio> *callback ) { 
     callback->setNext(this->callbackOnDesligado);
     this->callbackOnDesligado = callback; 
   }
  
  void acionarPulsador(int idxPulsador, boolean ativar);
  boolean isEmPausa(int idxPulsador);
};

#endif

