#ifndef _ACAO_CURSOR_H
#define _ACAO_CURSOR_H

#if defined(ARDUINO) && ARDUINO >= 100
   #include "Arduino.h"
#else
   #include "WProgram.h"
#endif   
#include "Relogio.h"
#include "Temporizador.h"
#include "ClockMotorDePasso.h"

class AcaoCursor {
public:
  virtual void botaoDireitaOnLow(Botao *botao)=0;
  virtual void botaoDireitaOnHigh(Botao *botao)=0;
  virtual void botaoEsquerdaOnLow(Botao *botao)=0;
  virtual void botaoEsquerdaOnHigh(Botao *botao)=0;
  virtual void setFocus(boolean focus)=0;
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
        acaoCursor.setFocus(true);
  }
  
  void setAcaoCursor(AcaoCursor &acaoCursor) {
    AcaoCursor *Old_AcaoCursor = botaoEsquerdaOnLowCallback.getInstancia();
    if (Old_AcaoCursor) {
      Old_AcaoCursor->setFocus(false);
    }
    botaoEsquerdaOnLowCallback.getInstancia()->setFocus(false);
    botaoEsquerdaOnLowCallback.setInstancia(&acaoCursor);
    botaoEsquerdaOnHighCallback.setInstancia(&acaoCursor);
    botaoDireitaOnLowCallback.setInstancia(&acaoCursor);
    botaoDireitaOnHighCallback.setInstancia(&acaoCursor);
    acaoCursor.setFocus(true);
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
  void setFocus(boolean focus) { };  
};

class AcaoCursorClockInterno: public AcaoCursor {

boolean focus;
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
  if (focus) {
    Serial.print(source->getHora());
    Serial.print(':');
    Serial.print(source->getMinuto());
    Serial.print(':');
    Serial.print(source->getSegundo());
    Serial.print("    \r\n");
  }
}

public:
  AcaoCursorClockInterno(ClockInterno &clockInterno):
           AcaoCursor(),
           clockInterno(&clockInterno),
           clockInternoAjustarTimer(400, this, &AcaoCursorClockInterno::incrementarClockInterno),
           callbackOnSegundo(this, &AcaoCursorClockInterno::printHora),
           focus(false)
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

  void setFocus(boolean focus) { 
    this->focus = focus;
  };  

};

class AcaoCursorClockMotorDePasso: public AcaoCursor {

ClockMotorDePasso *clockMotorDePasso;
MetodoTemporizado<AcaoCursorClockMotorDePasso> clockMotorDePassoAjustarTimer;
MetodoCallback<ClockMotorDePasso, AcaoCursorClockMotorDePasso> callbackOnSegundo;
boolean focus;

void incrementarClockMotorDePasso(ItemTemporizado *source) {
  ajuste(source, 1);
}

void decrementarClockMotorDePasso(ItemTemporizado *source) {
  ajuste(source, -1);
}

void ajuste(ItemTemporizado *source, int sinal) {
  int seg;
  if (source->getQtdChamadas()<10) {
    seg = 1;
  } else if (source->getQtdChamadas()<20) {
    seg = PASSOS_POR_HORA/3600; // por segundo
  } else if (source->getQtdChamadas()<80) {
    seg = (PASSOS_POR_HORA/3600)*10; // por 10 segundos
  } else if (source->getQtdChamadas()<140) {
    seg = PASSOS_POR_HORA/60; // por minuto
  } else {
    seg = PASSOS_POR_HORA; // por hora
  }

  this->clockMotorDePasso->setPassosAcumulados(((long) this->clockMotorDePasso->getPassosAcumulados())+(seg*sinal));
  printHora(this->clockMotorDePasso);
}

public:
  AcaoCursorClockMotorDePasso(ClockMotorDePasso &clockMotorDePasso):
           AcaoCursor(),
           clockMotorDePasso(&clockMotorDePasso),
           clockMotorDePassoAjustarTimer(400, this, &AcaoCursorClockMotorDePasso::incrementarClockMotorDePasso),
           callbackOnSegundo(this, &AcaoCursorClockMotorDePasso::printHora),
           focus(false)
           {
              clockMotorDePassoAjustarTimer.pausar();
              temporizador.add(clockMotorDePassoAjustarTimer);

              this->clockMotorDePasso->setCallbackOnSegundo(&callbackOnSegundo);
           }

  void botaoDireitaOnLow(Botao *botao) {
      clockMotorDePassoAjustarTimer.setMetodo(&AcaoCursorClockMotorDePasso::incrementarClockMotorDePasso);
      clockMotorDePassoAjustarTimer.reiniciar();
  };
  void botaoDireitaOnHigh(Botao *botao) {
      clockMotorDePassoAjustarTimer.pausar();
  };
  void botaoEsquerdaOnLow(Botao *botao) {
      clockMotorDePassoAjustarTimer.setMetodo(&AcaoCursorClockMotorDePasso::decrementarClockMotorDePasso);
      clockMotorDePassoAjustarTimer.reiniciar();
  };
  void botaoEsquerdaOnHigh(Botao *botao) { 
      clockMotorDePassoAjustarTimer.pausar();
  };

  void setFocus(boolean focus) { 
    this->focus = focus;
    if (focus) 
       printHora(this->clockMotorDePasso);
  };  

void printHora(ClockMotorDePasso* source) {
  if (focus) {
    Serial.print(source->getHora());
    Serial.print(':');
    Serial.print(source->getMinuto());
    Serial.print(':');
    Serial.print(source->getSegundo());
    Serial.print("    \r\n");
  }
}

};

/**
 * Acao onde os botoes de esquerda e direita mexem no ponteiro do relogio.
 */
class AcaoCursorRelogio: public AcaoCursor {
private:

Relogio *relogio;
MetodoTemporizado<AcaoCursorRelogio> movePonteiroTimer;
AcaoCursorClockMotorDePasso *acaoCursorClockMotorDePasso;

void movimentarMotor(ItemTemporizado *source) {
  relogio->getWD2404()->sendPulsos(5);  
}

void pararMovimento() {
  movePonteiroTimer.pausar();
  relogio->ligar();
  Serial.println(F("Ajuste terminado"));
}

public:  
  AcaoCursorRelogio(Relogio &relogio, AcaoCursorClockMotorDePasso &acaoCursorClockMotorDePasso): AcaoCursor(), relogio(&relogio),
                        movePonteiroTimer(75, this, &AcaoCursorRelogio::movimentarMotor),
                        acaoCursorClockMotorDePasso(&acaoCursorClockMotorDePasso)
                         { 
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
  void setFocus(boolean focus) { 
    acaoCursorClockMotorDePasso->setFocus(focus);
  };  
    
};

/**
 * Permite ajuste dos pulsadores do Relogio
 */
class AcaoCursorPulsadorRelogio: public AcaoCursor {

boolean focus;
Relogio *relogio;
int idxPulsador;

MetodoTemporizado<AcaoCursorPulsadorRelogio> ajustaMillisDoPulsadorTimer;

void printValor() {
  if (this->focus)
    Serial.println(this->relogio->getMillisPorPulso(idxPulsador), DEC);
}

void incrementarPulsador(ItemTemporizado *source) {
   this->relogio->setMillisPorPulso(idxPulsador, this->relogio->getMillisPorPulso(idxPulsador)+1);
   printValor();
}

void decrementarPulsador(ItemTemporizado *source) {
   this->relogio->setMillisPorPulso(idxPulsador, ((long)this->relogio->getMillisPorPulso(idxPulsador))-1);
   printValor();
}

public:
 AcaoCursorPulsadorRelogio(Relogio &relogio, int idxPulsador): AcaoCursor(), relogio(&relogio), idxPulsador(idxPulsador), focus(false),
      ajustaMillisDoPulsadorTimer(400,this,&AcaoCursorPulsadorRelogio::incrementarPulsador) {
              ajustaMillisDoPulsadorTimer.pausar();
              temporizador.add(ajustaMillisDoPulsadorTimer);
      };
  
  void botaoDireitaOnLow(Botao *botao) {
      ajustaMillisDoPulsadorTimer.setMetodo(&AcaoCursorPulsadorRelogio::incrementarPulsador);
      ajustaMillisDoPulsadorTimer.reiniciar();
  };
  void botaoDireitaOnHigh(Botao *botao) {
      ajustaMillisDoPulsadorTimer.pausar();
  };
  void botaoEsquerdaOnLow(Botao *botao) {
      ajustaMillisDoPulsadorTimer.setMetodo(&AcaoCursorPulsadorRelogio::decrementarPulsador);
      ajustaMillisDoPulsadorTimer.reiniciar();
  };
  void botaoEsquerdaOnHigh(Botao *botao) { 
      ajustaMillisDoPulsadorTimer.pausar();
  };

  void setFocus(boolean focus) { 
    this->focus = focus;
    printValor();
  };  
  
}; 

/**
 * Permite ajuste dos pulsadores do Relogio
 */
class AcaoCursorAcionaPulsadorRelogio: public AcaoCursor {

boolean focus;
Relogio *relogio;
int idxPulsador;

void printValor() {
  if (this->focus) {
    if (this->relogio->isEmPausa(idxPulsador)) {
      Serial.println(F("OFF"));
    } else {
      Serial.println(F("ON"));
    }
  }
}

public:
 AcaoCursorAcionaPulsadorRelogio(Relogio &relogio, int idxPulsador): AcaoCursor(), relogio(&relogio), idxPulsador(idxPulsador), focus(false)
 { };
  
  void botaoDireitaOnLow(Botao *botao) {
      this->relogio->acionarPulsador(this->idxPulsador, true);
      printValor();
  };
  void botaoDireitaOnHigh(Botao *botao) {
  };
  void botaoEsquerdaOnLow(Botao *botao) {
      this->relogio->acionarPulsador(this->idxPulsador, false);
      printValor();
  };
  void botaoEsquerdaOnHigh(Botao *botao) { 
  };

  void setFocus(boolean focus) { 
    this->focus = focus;
    printValor();
  };  
  
}; 

#endif
