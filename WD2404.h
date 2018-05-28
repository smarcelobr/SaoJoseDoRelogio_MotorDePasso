
#ifndef _WD2404_H
#define _WD2404_H

#if defined(ARDUINO) && ARDUINO >= 100
   #include "Arduino.h"
#else
   #include "WProgram.h"
#endif   

#include "util.h"
#include "Temporizador.h"

class WD2404 {
private:
  int pinEna;
  int pinDir;
  int pinPul;

  boolean isEnable;
  
  unsigned long pulsosAFazer;
  MetodoTemporizado<WD2404> aCadaMudancaFaseDoPulso;
  MetodoTemporizado<WD2404> desabilitarAposTempo;
  int faseDoPulso;
  int direcaoAtual;

  void (*onDirChange)(WD2404 *source, int dir); // funcao callback quando a direção muda
  Callback<WD2404> *callbackPulChange; // funcao callback quando o pulso muda
  Callback<WD2404> *callbackEnable; // funcao callback quando o pulso muda
  Callback<WD2404> *callbackDisable; // funcao callback quando o pulso muda

  /* métodos callback do temporizador */
  void mudarFaseDoPulso(ItemTemporizado *source);
  void horaParaDesabilitar(ItemTemporizado *source);
  
  void mudarDirecao(int direcaoNova);
public:  

   /**
    * Cria e configura os pinos como OUTPUT e coloca valores iniciais
    */
   WD2404(int pinEnable, int pinDirection, int pinPulso);
   
   void setCallbackEnable(Callback<WD2404> *callback ) { 
      callback->setNext(this->callbackEnable);
      this->callbackEnable = callback;
   }
   void setCallbackDisable(  Callback<WD2404> *callback ) { 
     callback->setNext(this->callbackDisable);
     this->callbackDisable = callback; 
   }
   void setCallbackOnDirChange( void (*onDirChange)(WD2404 *source, int dir) ) { this->onDirChange = onDirChange; }
   void setCallbackPulChange( Callback<WD2404> *callback ) { 
      callback->setNext(this->callbackPulChange);
      this->callbackPulChange = callback; 
   }

   /**
    * atualiza() deve ser chamado no loop para a atualização do botão e execução do callback se houver. 
    */
  void atualiza();
  
  void sendPulsos(unsigned long pulsos);
  void resetPulsos();
  
  void sentidoHorario();
  void sentidoAntiHorario();
  
  void enable();
  void disable();

  int getFaseDoPulso() { return this->faseDoPulso; }
  int getDirecao() { return this->direcaoAtual; }

};


#endif


