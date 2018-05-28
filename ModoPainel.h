#ifndef _MODO_NORMAL_H
#define _MODO_NORMAL_H

#if defined(ARDUINO) && ARDUINO >= 100
   #include "Arduino.h"
#else
   #include "WProgram.h"
#endif   
#include "Relogio.h"
#include "Temporizador.h"

class AcaoCursor {
public:
  virtual void botaoDireitaOnLow(Botao *botao)=0;
  virtual void botaoDireitaOnHigh(Botao *botao)=0;
  virtual void botaoEsquerdaOnLow(Botao *botao)=0;
  virtual void botaoEsquerdaOnHigh(Botao *botao)=0;
};

/**
 * Acao onde os botoes de esquerda e direita nao fazem nada.
 */
class AcaoCursorNenhuma: public AcaoCursor {
private:

public:  
  AcaoCursorNenhuma(): AcaoCursor() { }

  void botaoDireitaOnLow(Botao *botao) { };
  void botaoDireitaOnHigh(Botao *botao) { };
  void botaoEsquerdaOnLow(Botao *botao) { };
  void botaoEsquerdaOnHigh(Botao *botao) { };
    
};

/**
 * Acao onde os botoes de esquerda e direita mexem no ponteiro do relogio.
 */
class AcaoCursorRelogio: public AcaoCursor {
private:

Relogio *relogio;
MetodoTemporizado<AcaoCursorRelogio> movePonteiroTimer;

void movimentarMotor(ItemTemporizado *source) {
  relogio->getWD2404()->sendPulsos(5);  
}

void pararMovimento() {
  movePonteiroTimer.pausar();
  relogio->ligar();
  Serial.println(F("Ajuste terminado"));
}

public:  
  AcaoCursorRelogio(Relogio &relogio): AcaoCursor(), relogio(&relogio),
                        movePonteiroTimer(75, this, &AcaoCursorRelogio::movimentarMotor) { 
     movePonteiroTimer.pausar();
     temporizador.add(movePonteiroTimer);
  }

  void botaoDireitaOnLow(Botao *botao) { 
    relogio->desligar();
    relogio->getWD2404()->sentidoHorario();
    movePonteiroTimer.continuar();
    Serial.println(F("Ajuste horario iniciado"));
  }
  
  void botaoEsquerdaOnLow(Botao *botao) {
     relogio->desligar();
     relogio->getWD2404()->sentidoAntiHorario();
     movePonteiroTimer.reiniciar();
     Serial.println(F("Ajuste anti-horario iniciado"));
  }
  
  void botaoDireitaOnHigh(Botao *botao)  { this->pararMovimento(); }
  void botaoEsquerdaOnHigh(Botao *botao) { this->pararMovimento(); }
    
};

class AcaoCursorClockInterno: public AcaoCursor {

ClockInterno *clockInterno;
MetodoTemporizado<AcaoCursorClockInterno> clockInternoAjustarTimer;
MetodoCallback<ClockInterno,AcaoCursorClockInterno> callbackOnSegundo;

void printHora(ClockInterno *clockInterno) {
  Serial.print('\r');
  Serial.print(clockInterno->getHora());
  Serial.print(':');
  Serial.print(clockInterno->getMinuto());
  Serial.print(':');
  Serial.print(clockInterno->getSegundo());
  Serial.print("    ");
}

void incrementarClockInterno(ItemTemporizado *source) {
  unsigned int seg;
  if (source->getQtdChamadas()<10) {
    seg = 1;
  } else if (source->getQtdChamadas()<60) {
    seg = 10;
  } else if (source->getQtdChamadas()<120) {
    seg = 30;
  } else if (source->getQtdChamadas()<180) {
    seg = 60; // por minuto
  } else {
    seg = 3600; // por hora
  }

  clockInterno->addSegundos(seg);
  this->printHora(clockInterno);
}

public:
  AcaoCursorClockInterno(ClockInterno &clockInterno):
           AcaoCursor(),
           clockInterno(&clockInterno),
           clockInternoAjustarTimer(150, this, &AcaoCursorClockInterno::incrementarClockInterno),
           callbackOnSegundo(this, &ClockInterno::printHora)
           {
              clockInternoAjustarTimer.pausar();
              temporizador.add(clockInternoAjustarTimer);

              clockInterno->setCallbackOnSegundo(printHora);
           }

  void botaoDireitaOnLow(Botao *botao) {
      clockInterno->pausar();
      clockInternoAjustarTimer.reiniciar();
  };
  void botaoDireitaOnHigh(Botao *botao) {
      clockInternoAjustarTimer.pausar();
      clockInterno->continuar();
  };
  void botaoEsquerdaOnLow(Botao *botao) {
      clockInterno->set(0,0,0);
  };
  void botaoEsquerdaOnHigh(Botao *botao) { };
  
};

#endif
