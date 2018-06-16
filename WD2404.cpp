#include "WD2404.h"

WD2404::WD2404(int pinEnable, int pinDirection, int pinPulso): 
    pinEna(pinEnable), pinDir(pinDirection), pinPul(pinPulso),
    pulsosAFazer(0), faseDoPulso(HIGH), direcaoAtual(LOW),
    callbackEnable(0), callbackDisable(0), onDirChange(0), callbackPulChange(0),
    aCadaMudancaFaseDoPulso(10, this, &WD2404::mudarFaseDoPulso),
    desabilitarAposTempo(3000, this, &WD2404::horaParaDesabilitar) {
  
  pinMode(pinEna, OUTPUT);
  pinMode(pinDir, OUTPUT);
  pinMode(pinPul, OUTPUT);
  
  digitalWrite(pinEna, LOW); 
  digitalWrite(pinDir, direcaoAtual);
  digitalWrite(pinPul, faseDoPulso);
  
  this->disable();
  
  temporizador.add(this->aCadaMudancaFaseDoPulso); // a cada 20ms, o metodo mudarFaseDoPulso será executado.
  temporizador.add(this->desabilitarAposTempo);
}

void WD2404::enable() {
  this->desabilitarAposTempo.pausar(); // caso o temporizador de desabilitar o motor esteja contando, eu interrompo ele.
  if (!this->isEnable) {
    digitalWrite(this->pinEna, LOW);
    this->isEnable = true;
    if (this->callbackEnable) 
       this->callbackEnable->call(this);
  }
}

void WD2404::disable() {
 /* não desabilito na hora, pois, a inercia pode movimentar o ponteiro indesejadamente.
  *  Então, uso um temporizador para que o motor só seja desligado depois de um tempo.
  */
 desabilitarAposTempo.reiniciar(); // reinicia o tempo para desligar o motor 
}

void WD2404::mudarDirecao(int direcaoNova) {
  if (direcaoNova!=direcaoAtual) {    
    this->resetPulsos(); // reseta pulsos pendentes para evitar uma movimantacao do motor pendente em outra direcao
    digitalWrite(this->pinDir, direcaoNova);
    if (this->onDirChange) 
       this->onDirChange(this, direcaoNova);
     direcaoAtual = direcaoNova;  
  }
}

void WD2404::sentidoHorario() {
  this->mudarDirecao(LOW);
}

void WD2404::sentidoAntiHorario() {
  this->mudarDirecao(HIGH);
}

void WD2404::sendPulsos(unsigned long pulsos) {
   this->pulsosAFazer = pulsos;
   this->enable();
}

void WD2404::resetPulsos() {
  this->pulsosAFazer = 0;
  this->disable(); // desliga motor
}

void WD2404::mudarFaseDoPulso(ItemTemporizado *source) {
  // faz o movimento dos ponteiros
  if (this->pulsosAFazer>0) { // tem pulso pendente?
      this->enable(); // liga motor

      this->faseDoPulso = !this->faseDoPulso; // muda para LOW ou HIGH
      digitalWrite(this->pinPul, this->faseDoPulso);
      if (this->faseDoPulso==HIGH) {
        // decrementa cada vez que faz a transição para HIGH
        this->pulsosAFazer--;
      }
      if (this->callbackPulChange) {
          // executa callback
          this->callbackPulChange->call(this);
      }
     
      if (this->pulsosAFazer==0) {
        this->disable();
      }
  }

}

/**
 * Este método é executando quando chega a hora de desabilitar o motor.
 * O motor não é desabilitado imediatamente após o fim do movimento para 
 * impedir algum movimento involuntário devido a inércia.
 */
void WD2404::horaParaDesabilitar(ItemTemporizado *source) {
  source->pausar(); // interrompo a execução do temporizador
  if (this->isEnable) {
    digitalWrite(this->pinEna, HIGH); // HIGH desabilita a energia do motor.
    this->isEnable = false;
    if (this->callbackDisable) 
       this->callbackDisable->call(this);
  }
}

