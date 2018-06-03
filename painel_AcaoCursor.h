#ifndef _ACAO_CURSOR_H
#define _ACAO_CURSOR_H

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


class BotoesCursor {
  Botao *botaoEsquerda, *botaoDireita;
  MetodoCallback<Botao, AcaoCursor> botaoEsquerdaOnLowCallback, botaoEsquerdaOnHighCallback, botaoDireitaOnLowCallback, botaoDireitaOnHighCallback;
  AcaoCursor *acaoCursor;
  
public:
  BotoesCursor(Botao &botaoEsquerda, Botao &botaoDireita, AcaoCursor &acaoCursor): 
             botaoEsquerda(&botaoEsquerda), botaoDireita(&botaoDireita),
             botaoEsquerdaOnLowCallback(&acaoCursor, &AcaoCursor::botaoEsquerdaOnLow),
             botaoEsquerdaOnHighCallback(&acaoCursor, &AcaoCursor::botaoEsquerdaOnHigh),             
             botaoDireitaOnLowCallback(&acaoCursor, &AcaoCursor::botaoDireitaOnLow),
             botaoDireitaOnHighCallback(&acaoCursor, &AcaoCursor::botaoDireitaOnHigh)
             { 
        this->botaoEsquerda->setCallbackOnLOW(&botaoEsquerdaOnLowCallback);
        this->botaoEsquerda->setCallbackOnHIGH(&botaoEsquerdaOnHighCallback);
        this->botaoDireita->setCallbackOnLOW(&botaoDireitaOnLowCallback);        
        this->botaoDireita->setCallbackOnHIGH(&botaoDireitaOnHighCallback);
  }
  
  void setAcaoCursor(AcaoCursor &acaoCursor) {
    botaoEsquerdaOnLowCallback.setInstancia(&acaoCursor);
    botaoEsquerdaOnHighCallback.setInstancia(&acaoCursor);
    botaoDireitaOnLowCallback.setInstancia(&acaoCursor);
    botaoDireitaOnHighCallback.setInstancia(&acaoCursor);
  }
  
  void atualiza() {
    botaoEsquerda->atualiza();
    botaoDireita->atualiza();
  }
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

  this->clockInterno->addSegundos(seg);
  this->printHora(this->clockInterno);
}

void printHora(ClockInterno* source) {
  Serial.print('\r');
  Serial.print(source->getHora());
  Serial.print(':');
  Serial.print(source->getMinuto());
  Serial.print(':');
  Serial.print(source->getSegundo());
  Serial.print("    ");
}

public:
  AcaoCursorClockInterno(ClockInterno &clockInterno):
           AcaoCursor(),
           clockInterno(&clockInterno),
           clockInternoAjustarTimer(150, this, &AcaoCursorClockInterno::incrementarClockInterno),
           callbackOnSegundo(this, &AcaoCursorClockInterno::printHora)
           {
              clockInternoAjustarTimer.pausar();
              temporizador.add(clockInternoAjustarTimer);

              this->clockInterno->setCallbackOnSegundo(&callbackOnSegundo);
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
