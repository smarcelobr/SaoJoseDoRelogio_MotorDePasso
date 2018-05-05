#ifndef _RELOGIO_H
#define _RELOGIO_H

#if defined(ARDUINO) && ARDUINO >= 100
   #include "Arduino.h"
#else
   #include "WProgram.h"
#endif   

#include "WD2404.h"

/**
 * Responsável por enviar ao motor de passo (vir driver WD-2404) os pulsos necessários
 * para movimentar o relógio a cada minuto.
 * 
 */
class Relogio {
private:
  WD2404 *wd2404;

  boolean ligado;
  unsigned long pulsosPendentes;

  MetodoTemporizado<Relogio> aCadaMinutoExecuta;
  MetodoTemporizado<Relogio> aCadaMillisPorPulsoExecuta;

  void doEachMillisPorPulso(ItemTemporizado *source);
  void doEachMinuto(ItemTemporizado *source);  
  
  void (*onLigado)(Relogio *source); // funcao callback quando o relogio é ligado
  void (*onDesligado)(Relogio *source); // funcao callback quando o relógio é desligado
    
public:  

   /**
    * Cria e configura os pinos como OUTPUT e coloca valores iniciais
    */
   Relogio(WD2404 &wd2404);
   
  void setMillisPorPulso(int millisPorPulso); 
  int getMillisPorPulso();
  
  void ligar();
  void desligar();
  
  boolean isLigado() { return ligado; }
  
   void setCallbackOnLigado( void (*onLigado)(Relogio *source) ) { this->onLigado = onLigado; }
   void setCallbackOnDesligado( void (*onDesligado)(Relogio *source) ) { this->onDesligado = onDesligado; }

};

#endif


