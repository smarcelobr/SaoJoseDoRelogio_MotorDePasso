#include "WD2404.h"

WD2404::WD2404(int pinEnable, int pinDirection, int pinPulso): 
    pinEna(pinEnable), pinDir(pinDirection), pinPul(pinPulso),
    pulsosAFazer(0), faseDoPulso(HIGH), direcaoAtual(LOW),
    onEnable(0), onDisable(0), onDirChange(0), onPulChange(0),
    aCadaMudancaFaseDoPulso(20, this, &WD2404::mudarFaseDoPulso) {
  
  pinMode(pinEna, OUTPUT);
  pinMode(pinDir, OUTPUT);
  pinMode(pinPul, OUTPUT);
  
  digitalWrite(pinEna, LOW); 
  digitalWrite(pinDir, direcaoAtual);
  digitalWrite(pinPul, faseDoPulso);
  
  this->disable();
  
  temporizador.add(this->aCadaMudancaFaseDoPulso); // a cada 20ms, o metodo mudarFaseDoPulso será executado.
}

void WD2404::enable() {  
  if (!this->isEnable) {
    digitalWrite(this->pinEna, LOW);
    this->isEnable = true;
    if (this->onEnable) 
       this->onEnable(this);
  }
}

void WD2404::disable() {
  if (this->isEnable) {
    digitalWrite(this->pinEna, HIGH);
    this->isEnable = false;
    if (onDisable) 
       onDisable(this);
  }
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

void WD2404::mudarFaseDoPulso() {
  // faz o movimento dos ponteiros
  if (this->pulsosAFazer>0) { // tem pulso pendente?      
      this->enable(); // liga motor

      this->faseDoPulso = !this->faseDoPulso; // muda para LOW ou HIGH
      digitalWrite(this->pinPul, this->faseDoPulso);
      if (this->faseDoPulso==HIGH) {
        // decrementa cada vez que faz a transição para HIGH
        this->pulsosAFazer--;
      }
      if (onPulChange) {
          // executa callback
          onPulChange(this, this->faseDoPulso);
      }
     
      if (this->pulsosAFazer==0) {
         this->disable(); // desliga motor
      }
  }

}

