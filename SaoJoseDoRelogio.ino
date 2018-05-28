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
#include "IndicaPulsos.h"
#include "LogWD2404_Serial.h"
#include "ModoPainel.h"
 
// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
#define ledModo 13      // Led Vermelho comum
#define ledRelogio1 A0  // RGB Verde
#define ledRelogio2 8  // RGB Azul
#define ledDirecao  A1  // RGB Vermelho

#define MODO_NORMAL 0
#define MODO_AJUSTE_RELOGIO_1 1
#define MODO_AJUSTE_RELOGIO_2 2
#define MODO_AJUSTE_CLOCKINTERNO 3
#define ULTIMO_MODO MODO_AJUSTE_CLOCKINTERNO


// Forward declarations
void acendeLedModo(ItemTemporizado *source);
void movimentarMotor(ItemTemporizado *source);
void incrementarClockInterno(ItemTemporizado *source);

// variaveis locais
FuncaoTemporizada ledModoPiscando(500, acendeLedModo);

WD2404 wd2404_1(2, 3, 4); // pinos do Enable, Direcao e Pulso
WD2404 wd2404_2(5, 6, 7); // pinos do Enable, Direcao e Pulso
Relogio relogio_1(wd2404_1);
Relogio relogio_2(wd2404_2);
ClockInterno clockInterno(0,0,0);

AcaoCursorNenhuma acaoCursorNenhuma=AcaoCursorNenhuma();
AcaoCursorRelogio acaoCursorRelogio1(relogio_1);
AcaoCursorRelogio acaoCursorRelogio2(relogio_2);
AcaoCursorClockInterno acaoCursorClockInterno(clockInterno);

/* Os botoes direita/esquerda mudam de funcao dependendo do modo: */
AcaoCursor *acoesCursor[] = {&acaoCursorNenhuma,&acaoCursorRelogio1,&acaoCursorRelogio2,&acaoCursorClockInterno};

AcaoCursor *acaoCursorAtual=acoesCursor[0];

// Objetos que conectam um wd2404 a um LED para indicar quando estes pulsam.
IndicaPulsos indicaPulsosRelogio1(wd2404_1, ledRelogio1);
IndicaPulsos indicaPulsosRelogio2(wd2404_2, ledRelogio2);

LogWD2404_Serial wd2404_1_SerialStatus;
LogWD2404_Serial wd2404_2_SerialStatus;

Relogio *relogioAtivo;

Botao botaoAntiHorario(A2);
Botao botaoHorario(A3);

/* Clock Interno */
//Botao botaoResetClockInterno(A4);

//FuncaoTemporizada clockInternoAjustarTimer(150, incrementarClockInterno);


/* Botao Pausa/Continua: liga ou desliga todos os relógios. */
Botao botaoPausaContinua(A5);
void botaoPausaContinuaOnClick(Botao *botao) { 
  if (relogio_1.isLigado()) {
    relogio_1.desligar(); relogio_2.desligar();
  } else {
    relogio_1.ligar(); 
    relogio_2.ligar();
  }
}
FuncaoCallback<Botao> botaoPausaContinua_onLow(botaoPausaContinuaOnClick);

// the setup routine runs once when you press reset:
void setup() {
  // initialize the digital pin as an output.
  pinMode(ledModo, OUTPUT);
  pinMode(ledDirecao, OUTPUT);
  
  digitalWrite(ledModo, LOW);
  digitalWrite(ledDirecao, LOW);
  
  ledModoPiscando.pausar();
  //clockInternoAjustarTimer.pausar();
  
  temporizador.add(ledModoPiscando);
  //temporizador.add(clockInternoAjustarTimer);

  wd2404_1.setCallbackOnDirChange(wd2404_onDirChange);
  wd2404_2.setCallbackOnDirChange(wd2404_onDirChange);

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

//  botaoResetClockInterno.setCallbackOnLOW(botaoResetClockInterno_onClick);
//  botaoAjustaClockInterno.setCallbackOnLOW(botaoAjustaClockInterno_onDown);
//  botaoAjustaClockInterno.setCallbackOnHIGH(botaoAjustaClockInterno_onUp);

  acaoCursorAtual = &acaoCursor[0];
  botaoPausaContinua.setCallbackOnLOW(botaoPausaContinua_onLow);
  
//  clockInterno.setCallbackOnSegundo(printHora);

  // registra as funções dos botões em cada modo:
  // modo 0
  
  Serial.begin(115200);
  Serial.println(F("Sao Jose do Relogio - v.1.1"));

  wd2404_1_SerialStatus.begin(F("W1"),wd2404_1);
  wd2404_2_SerialStatus.begin(F("W2"),wd2404_2);

  unsigned int qtd = temporizador.getQtdItens();
  Serial.print(F("*T:"));
  Serial.println(qtd);
  if (qtd>LIMITE_TEMPORIZADORES) {
    Serial.println(F("/fail"));
  }
}

// the loop routine runs over and over again forever:
void loop() {
  // trata o tempo
  temporizador.atualiza();

  // trata os botoes do painel
  painel->atualiza();
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
    digitalWrite(ledDirecao, dir);
  }
}

//void botaoResetClockInterno_onClick(Botao *source) {
//  clockInterno.set(0,0,0);
//}
/*
void printHora(ClockInterno *clockInterno) {
  Serial.print('\r');
  Serial.print(clockInterno->getHora());
  Serial.print(':');
  Serial.print(clockInterno->getMinuto());
  Serial.print(':');
  Serial.print(clockInterno->getSegundo());
  Serial.print("    ");
}
*/
/*void botaoAjustaClockInterno_onDown(Botao *source) {
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

*/
