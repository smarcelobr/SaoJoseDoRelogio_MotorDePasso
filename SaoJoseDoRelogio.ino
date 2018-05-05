/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */

#include "temporizador.h"
#include "WD2404.h"
#include "relogio.h"
#include "botao.h"
#include "ClockInterno.h"
 
// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led = 13;
int ledModo = A0;
int ledSentido = A1;

#define RELOGIO_MODO_NORMAL 0
#define RELOGIO_MODO_AJUSTE_HORARIO 1
#define RELOGIO_MODO_AJUSTE_ANTI_HORARIO 2

FuncaoTemporizada ledModoPiscando(500, acendeLedModo);
FuncaoTemporizada movePonteiroTimer(100, movimentarMotor);

WD2404 wd2404(2, 3, 4); // pinos do Enable, Direcao e Pulso
Relogio relogio(wd2404);

Botao botaoAntiHorario(5);
Botao botaoHorario(6);

/* Clock Interno */
Botao botaoResetClockInterno(7);
Botao botaoAjustaClockInterno(8);
ClockInterno clockInterno(0,0,0);
FuncaoTemporizada clockInternoAjustarTimer(150, incrementarClockInterno);

// the setup routine runs once when you press reset:
void setup() {
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);
  pinMode(ledModo, OUTPUT);
  pinMode(ledSentido, OUTPUT);
  
  digitalWrite(led, LOW);
  digitalWrite(ledModo, LOW);
  digitalWrite(ledSentido, LOW);
  
  movePonteiroTimer.pausar();
  ledModoPiscando.pausar();
  clockInternoAjustarTimer.pausar();  
  
  temporizador.add(ledModoPiscando);
  temporizador.add(movePonteiroTimer);
  temporizador.add(clockInternoAjustarTimer);
  
  wd2404.setCallbackOnDirChange(wd2404_onDirChange);
  wd2404.setCallbackOnPulChange(wd2404_onPulChange);
  wd2404.setCallbackOnEnable(wd2404_onEnable);
  wd2404.setCallbackOnDisable(wd2404_onDisable);

  relogio.setCallbackOnLigado(relogio_onLigado);
  relogio.setCallbackOnDesligado(relogio_onDesligado);
  
  relogio.ligar();
  
  botaoAntiHorario.setCallbackOnLOW(iniciarMovimentoAntiHorario);
  botaoAntiHorario.setCallbackOnHIGH(pararMovimento);

  botaoHorario.setCallbackOnLOW(iniciarMovimentoHorario);
  botaoHorario.setCallbackOnHIGH(pararMovimento);

  botaoResetClockInterno.setCallbackOnLOW(botaoResetClockInterno_onClick);
  botaoAjustaClockInterno.setCallbackOnLOW(botaoAjustaClockInterno_onDown);
  botaoAjustaClockInterno.setCallbackOnLOW(botaoAjustaClockInterno_onUp);
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

void acendeLedModo(ItemTemporizado *source) {
  digitalWrite(ledModo, HIGH);
  ledModoPiscando.setFuncao(apagaLedModo);
}

void apagaLedModo(ItemTemporizado *source) {
  digitalWrite(ledModo, LOW);
  ledModoPiscando.setFuncao(acendeLedModo);
}

void movimentarMotor(ItemTemporizado *source) {
  wd2404.sendPulsos(5);  
}

void iniciarMovimentoAntiHorario(Botao *source) {
  relogio.desligar();
  wd2404.sentidoAntiHorario();
  movePonteiroTimer.reiniciar();
  Serial.println(F("Ajuste anti-horario iniciado"));
}

void iniciarMovimentoHorario(Botao *source) {
  relogio.desligar();
  wd2404.sentidoHorario();
  movePonteiroTimer.continuar();
  Serial.println(F("Ajuste horario iniciado"));
}

void pararMovimento(Botao *source) {
  movePonteiroTimer.pausar();
  relogio.ligar();
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
  digitalWrite(ledSentido, dir);
}

void wd2404_onPulChange(WD2404 *source, int pul) {  
  digitalWrite(led, pul);
}

void wd2404_onEnable(WD2404 *source) {
  Serial.println(F("Motor de passo ENABLED"));
}

void wd2404_onDisable(WD2404 *source) {
  Serial.println(F("Motor de passo DISABLED"));
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
  clockInternoAjustarTimer.reiniciar();
}

void botaoAjustaClockInterno_onUp(Botao *source) {
  clockInternoAjustarTimer.pausar();
}

void incrementarClockInterno(ItemTemporizado *source) {
  clockInterno.addSegundos(1);
  printHora(&clockInterno);
}
