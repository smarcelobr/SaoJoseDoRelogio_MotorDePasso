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
#include "ClockMotorDePasso.h"

#include "painel_IndicaPulsos.h"
#include "painel_AcaoCursor.h"

#include "LogWD2404_Serial.h"

#include "ComandosViaSerial.h"
 
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

// variaveis locais
FuncaoTemporizada ledModoPiscando(500, acendeLedModo);

WD2404 wd2404_1(2, 3, 4); // pinos do Enable, Direcao e Pulso
WD2404 wd2404_2(5, 6, 7); // pinos do Enable, Direcao e Pulso
Relogio relogio_1(wd2404_1);
Relogio relogio_2(wd2404_2);
ClockInterno clockInterno(0,0,0);
ClockMotorDePasso clockMotorDePasso1(&wd2404_1);
ClockMotorDePasso clockMotorDePasso2(&wd2404_2);

AcaoCursorNenhuma acaoCursorNenhuma=AcaoCursorNenhuma();
AcaoCursorClockInterno acaoCursorClockInterno(clockInterno);
AcaoCursorClockMotorDePasso acaoCursorClockMotorDePasso1(clockMotorDePasso1);
AcaoCursorClockMotorDePasso acaoCursorClockMotorDePasso2(clockMotorDePasso2);
AcaoCursorRelogio acaoCursorRelogio1(relogio_1,acaoCursorClockMotorDePasso1);
AcaoCursorRelogio acaoCursorRelogio2(relogio_2,acaoCursorClockMotorDePasso2);
AcaoCursorPulsadorRelogio acaoCursorPulsadorR1_1(relogio_1, 0),acaoCursorPulsadorR1_2(relogio_1, 1);
AcaoCursorPulsadorRelogio acaoCursorPulsadorR2_1(relogio_2, 0),acaoCursorPulsadorR2_2(relogio_2, 1);
AcaoCursorAcionaPulsadorRelogio acaoCursorAcionaPulsadorExtraRelogio1(relogio_1,1), acaoCursorAcionaPulsadorExtraRelogio2(relogio_2,1);

/* Os botoes direita/esquerda mudam de funcao dependendo do modo: */
#define NUM_MODOS 12
AcaoCursor *acoesCursor[NUM_MODOS] = {&acaoCursorNenhuma,
     &acaoCursorRelogio1,&acaoCursorClockMotorDePasso1,&acaoCursorPulsadorR1_1,&acaoCursorPulsadorR1_2,&acaoCursorAcionaPulsadorExtraRelogio1,
     &acaoCursorRelogio2,&acaoCursorClockMotorDePasso2,&acaoCursorPulsadorR2_1,&acaoCursorPulsadorR2_2,&acaoCursorAcionaPulsadorExtraRelogio2,
     &acaoCursorClockInterno};     

// Objetos que conectam um wd2404 a um LED para indicar quando estes pulsam.
IndicaPulsos indicaPulsosRelogio1(wd2404_1, ledRelogio1);
IndicaPulsos indicaPulsosRelogio2(wd2404_2, ledRelogio2);

LogRelogio_Serial relogio_1_SerialStatus;
LogRelogio_Serial relogio_2_SerialStatus;

/* botao cursor direita e esquerda */
Botao botaoAntiHorario(A3);
Botao botaoHorario(A4);
BotoesCursor botoesCursor(botaoAntiHorario, botaoHorario, acaoCursorNenhuma);

/* Botao Modo */
Botao botaoModo(A2);
byte modo;
void botaoMudaModoOnClick(Botao *botao) { 
  modo++;
  Serial.print(F("\nMODO:"));
  Serial.println(modo%NUM_MODOS);
  botoesCursor.setAcaoCursor(*acoesCursor[modo%NUM_MODOS]);
}
FuncaoCallback<Botao> botaoMudaModo_onLow(botaoMudaModoOnClick);
FuncaoCallback<Relogio> callbackOnLigado(relogio_onLigado);
FuncaoCallback<Relogio> callbackOnDesligado(relogio_onDesligado);

/* Botao Pausa/Continua: liga ou desliga todos os relógios. */
Botao botaoPausaContinua(A5);
void botaoPausaContinuaOnClick(Botao *botao) { 
  Serial.println(F("\nRelogios "));
  if (relogio_1.isLigado()) {
    Serial.println(F("OFF"));
    relogio_1.desligar(); relogio_2.desligar();
  } else {
    Serial.println(F("ON"));
    relogio_1.ligar(); 
    relogio_2.ligar();
  }
}
FuncaoCallback<Botao> botaoPausaContinua_onLow(botaoPausaContinuaOnClick);


ComandosViaSerial comandosViaSerial(botaoModo, botaoPausaContinua, botaoAntiHorario, botaoHorario);

// the setup routine runs once when you press reset:
void setup() {
  // initialize the digital pin as an output.
  pinMode(ledModo, OUTPUT);
  pinMode(ledDirecao, OUTPUT);
  
  digitalWrite(ledModo, LOW);
  digitalWrite(ledDirecao, LOW);
  
  ledModoPiscando.pausar();
  
  temporizador.add(ledModoPiscando);

  wd2404_1.setCallbackOnDirChange(wd2404_onDirChange);
  wd2404_2.setCallbackOnDirChange(wd2404_onDirChange);

  relogio_1.setCallbackOnLigado(&callbackOnLigado);
  relogio_1.setCallbackOnDesligado(&callbackOnDesligado);
  relogio_1.ligar();

  relogio_2.setCallbackOnLigado(&callbackOnLigado);
  relogio_2.setCallbackOnDesligado(&callbackOnDesligado);
  relogio_2.ligar();

  botaoModo.setCallbackOnLOW(&botaoMudaModo_onLow);
  botaoPausaContinua.setCallbackOnLOW(&botaoPausaContinua_onLow);
  
  Serial.begin(9600);
  Serial.println(F("Sao Jose do Relogio - v.1.1"));

  relogio_1_SerialStatus.begin(F("R1"), relogio_1);
  relogio_2_SerialStatus.begin(F("R2"), relogio_2);

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

  // trata os botoes direita e esquerda (cursors) do painel
  botoesCursor.atualiza();
  
  botaoModo.atualiza();
  botaoPausaContinua.atualiza();
  
  comandosViaSerial.atualiza();
}

void acendeLedModo(ItemTemporizado *source) {
  digitalWrite(ledModo, HIGH);
  ledModoPiscando.setFuncao(apagaLedModo);
}

void apagaLedModo(ItemTemporizado *source) {
  digitalWrite(ledModo, LOW);
  ledModoPiscando.setFuncao(acendeLedModo);
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
  digitalWrite(ledDirecao, dir);
}

