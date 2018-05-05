
#ifndef _WD2404_H
#define _WD2404_H

#if defined(ARDUINO) && ARDUINO >= 100
   #include "Arduino.h"
#else
   #include "WProgram.h"
#endif   

#include "Temporizador.h"

class WD2404 {
private:
  int pinEna;
  int pinDir;
  int pinPul;

  boolean isEnable;
  
  unsigned long pulsosAFazer;
  MetodoTemporizado<WD2404> aCadaMudancaFaseDoPulso;
  int faseDoPulso;
  int direcaoAtual;

  void (*onEnable)(WD2404 *source); // funcao callback quando o motor é habilitado
  void (*onDisable)(WD2404 *source); // funcao callback quando o motor é desabilitado
  void (*onDirChange)(WD2404 *source, int dir); // funcao callback quando a direção muda
  void (*onPulChange)(WD2404 *source, int pul); // funcao callback quando o pulso muda

  void mudarFaseDoPulso(ItemTemporizado *source);
  void mudarDirecao(int direcaoNova);
public:  

   /**
    * Cria e configura os pinos como OUTPUT e coloca valores iniciais
    */
   WD2404(int pinEnable, int pinDirection, int pinPulso);
   
   void setCallbackOnEnable( void (*onEnable)(WD2404 *source) ) { this->onEnable = onEnable; }
   void setCallbackOnDisable( void (*onDisable)(WD2404 *source) ) { this->onDisable = onDisable; }
   void setCallbackOnDirChange( void (*onDirChange)(WD2404 *source, int dir) ) { this->onDirChange = onDirChange; }
   void setCallbackOnPulChange( void (*onPulChange)(WD2404 *source, int pul) ) { this->onPulChange = onPulChange; }

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

};


#endif


