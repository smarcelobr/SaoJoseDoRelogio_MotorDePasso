#include <HID.h>

/*
  Aplicação principal para controle do relógio da torre da Basílica.

  Esta parte do código é responsável por instanciar e configurar os principais componentes
  para movimentação e controle do relógio.

  Também é responsável em responder os comandos do usuário e acionar os LEDs no painel
  de controle.

  - Consulte 'Temporizador.h' caso queira observar as chamadas que são realizadas em intervalos pré-definidos.
  - Consulte 'Relogio.h', se estiver procurando a classe responsável por controlar o tempo certo para enviar pulsos para
  o driver do motor de passo do relógio.
  - Consulte 'WD2404.h', se estiver procurando a classe responsável por efetivamente enviar pulsos para o driver do motor de
    passo do relógio.

*/

#include "temporizador.h"
#include "WD2404.h"
#include "relogio.h"
#include "botao.h"
#include "ClockInterno.h"
 
// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int ledModo = 13;
int ledPulso = A0;
int ledSentido = A1;

#define MODO_AJUSTE_CLOCKINTERNO 0
#define MODO_AJUSTE_RELOGIO_1 1
#define MODO_AJUSTE_RELOGIO_2 2

// Forward declarations
void acendeLedModo(ItemTemporizado *source);
void movimentarMotor(ItemTemporizado *source);
void incrementarClockInterno(ItemTemporizado *source);

// variaveis locais
FuncaoTemporizada ledModoPiscando(500, acendeLedModo);
FuncaoTemporizada movePonteiroTimer(100, movimentarMotor);

WD2404 wd2404_1(2, 3, 4); // pinos do Enable, Direcao e Pulso
WD2404 wd2404_2(5, 6, 7); // pinos do Enable, Direcao e Pulso
Relogio relogio_1(wd2404_1);
Relogio relogio_2(wd2404_2);

Relogio *relogioAtivo;

Botao botaoAntiHorario(A2);
Botao botaoHorario(A3);

/* Clock Interno */
Botao botaoResetClockInterno(A4);
Botao botaoAjustaClockInterno(A5);
ClockInterno clockInterno(0,0,0);
FuncaoTemporizada clockInternoAjustarTimer(150, incrementarClockInterno);

// the setup routine runs once when you press reset:
void setup() {
  // initialize the digital pin as an output.
  pinMode(ledPulso, OUTPUT);
  pinMode(ledModo, OUTPUT);
  pinMode(ledSentido, OUTPUT);
  
  digitalWrite(ledPulso, LOW);
  digitalWrite(ledModo, LOW);
  digitalWrite(ledSentido, LOW);
  
  movePonteiroTimer.pausar();
  ledModoPiscando.pausar();
  clockInternoAjustarTimer.pausar();  
  
  temporizador.add(ledModoPiscando);
  temporizador.add(movePonteiroTimer);
  temporizador.add(clockInternoAjustarTimer);

  wd2404_1.setCallbackOnDirChange(wd2404_onDirChange);
  wd2404_1.setCallbackOnPulChange(wd2404_onPulChange);
  wd2404_1.setCallbackOnEnable(wd2404_onEnable);
  wd2404_1.setCallbackOnDisable(wd2404_onDisable);

  wd2404_2.setCallbackOnDirChange(wd2404_onDirChange);
  wd2404_2.setCallbackOnPulChange(wd2404_onPulChange);
  wd2404_2.setCallbackOnEnable(wd2404_onEnable);
  wd2404_2.setCallbackOnDisable(wd2404_onDisable);

  relogio_1.setCallbackOnLigado(relogio_onLigado);
  relogio_1.setCallbackOnDesligado(relogio_onDesligado);
  relogio_1.ligar();

  relogio_2.setCallbackOnLigado(relogio_onLigado);
  relogio_2.setCallbackOnDesligado(relogio_onDesligado);
  relogio_2.ligar();

  setRelogioAtivo(&relogio_1);  
  
  botaoAntiHorario.setCallbackOnLOW(iniciarMovimentoAntiHorario);
  botaoAntiHorario.setCallbackOnHIGH(pararMovimento);

  botaoHorario.setCallbackOnLOW(iniciarMovimentoHorario);
  botaoHorario.setCallbackOnHIGH(pararMovimento);

  botaoResetClockInterno.setCallbackOnLOW(botaoResetClockInterno_onClick);
  botaoAjustaClockInterno.setCallbackOnLOW(botaoAjustaClockInterno_onDown);
  botaoAjustaClockInterno.setCallbackOnHIGH(botaoAjustaClockInterno_onUp);
  clockInterno.setCallbackOnSegundo(printHora);
  

  Serial.begin(115200);
  Serial.println(F("Sao Jose do Relogio - v.1.1"));
}

// the loop routine runs over and over again forever:
void loop() {
  // trata o tempo
  temporizador.atualiza();

  // trata os botoes
  botaoAntiHorario.atualiza();
  botaoHorario.atualiza();    
  
  botaoResetClockInterno.atualiza();
  botaoAjustaClockInterno.atualiza();  
}

void setRelogioAtivo(Relogio *relogio) {  
  relogioAtivo = relogio;
}

void acendeLedModo(ItemTemporizado *source) {
  digitalWrite(ledModo, HIGH);
  ledModoPiscando.setFuncao(apagaLedModo);
}

void apagaLedModo(ItemTemporizado *source) {
  digitalWrite(ledModo, LOW);
  ledModoPiscando.setFuncao(acendeLedModo);
}

void movimentarMotor(ItemTemporizado *source) {
  relogioAtivo->getWD2404()->sendPulsos(5);  
}

void iniciarMovimentoAntiHorario(Botao *source) {
  relogioAtivo->desligar();
  relogioAtivo->getWD2404()->sentidoAntiHorario();
  movePonteiroTimer.reiniciar();
  Serial.println(F("Ajuste anti-horario iniciado"));
}

void iniciarMovimentoHorario(Botao *source) {
  relogioAtivo->desligar();
  relogioAtivo->getWD2404()->sentidoHorario();
  movePonteiroTimer.continuar();
  Serial.println(F("Ajuste horario iniciado"));
}

void pararMovimento(Botao *source) {
  movePonteiroTimer.pausar();
  relogioAtivo->ligar();
  Serial.println(F("Ajuste terminado"));
}

void relogio_onLigado(Relogio *source) {
  // indica no led que está o relógio funcionado
  ledModoPiscando.reiniciar();  
}

void relogio_onDesligado(Relogio *source) {
  // indica no led que está o relógio parado
  ledModoPiscando.pausar();
  acendeLedModo(NULL);
}

void wd2404_onDirChange(WD2404 *source, int dir) {  
  if (source == relogioAtivo->getWD2404()) {
    digitalWrite(ledSentido, dir);
  }
}

void wd2404_onPulChange(WD2404 *source, int pul) {  
  if (source == relogioAtivo->getWD2404()) {
    digitalWrite(ledPulso, pul);
  }
}

void wd2404_onEnable(WD2404 *source) {
  if (source == relogioAtivo->getWD2404()) {
    Serial.println(F("Motor de passo ENABLED"));
    digitalWrite(ledPulso, HIGH);
  }
}

void wd2404_onDisable(WD2404 *source) {
  if (source == relogioAtivo->getWD2404()) {
    Serial.println(F("Motor de passo DISABLED"));
    digitalWrite(ledPulso, LOW);
  }
}

void botaoResetClockInterno_onClick(Botao *source) {
  clockInterno.set(0,0,0);
}

void printHora(ClockInterno *clockInterno) {
  Serial.print('\r');
  Serial.print(clockInterno->getHora());
  Serial.print(':');
  Serial.print(clockInterno->getMinuto());
  Serial.print(':');
  Serial.print(clockInterno->getSegundo());
  Serial.print("    ");
}

void botaoAjustaClockInterno_onDown(Botao *source) {
  clockInterno.pausar();
  clockInternoAjustarTimer.reiniciar();
}

void botaoAjustaClockInterno_onUp(Botao *source) {
  clockInternoAjustarTimer.pausar();
  clockInterno.continuar();
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

  clockInterno.addSegundos(seg);
  printHora(&clockInterno);
}
