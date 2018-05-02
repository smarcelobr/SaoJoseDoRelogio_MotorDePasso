
#include "Relogio.h"

Relogio::Relogio(WD2404 &wd2404):
    wd2404(&wd2404),
    ligado(false),
    pulsosPendentes(0),
    aCadaMinutoExecuta(60000, this, &Relogio::doEachMinuto),
    aCadaMillisPorPulsoExecuta(225, this, &Relogio::doEachMillisPorPulso)
{ 
 temporizador.add(aCadaMinutoExecuta); // a cada 20ms, o metodo mudarFaseDoPulso será executado.
 temporizador.add(aCadaMillisPorPulsoExecuta); // a cada 20ms, o metodo mudarFaseDoPulso será executado.
}

void Relogio::doEachMillisPorPulso() {
    this->pulsosPendentes = this->pulsosPendentes + 1;
}

void Relogio::doEachMinuto() {
    wd2404->sendPulsos(pulsosPendentes);
    Serial.println(pulsosPendentes);
    this->pulsosPendentes = 0;
}

void Relogio::setMillisPorPulso(int millisPorPulso) {
  aCadaMillisPorPulsoExecuta.setIntervalo(millisPorPulso);
}

int Relogio::getMillisPorPulso() {
  return aCadaMillisPorPulsoExecuta.getIntervalo();
}

void Relogio::ligar() {
  wd2404->sentidoHorario();
  wd2404->resetPulsos();
  ligado=true;
  if (onLigado)
     onLigado(this);
  this->pulsosPendentes=0;
  aCadaMillisPorPulsoExecuta.reiniciar();
  aCadaMinutoExecuta.reiniciar();
}

void Relogio::desligar() {
  ligado=false;
  if (onDesligado)
     onDesligado(this);
  aCadaMinutoExecuta.pausar();
  aCadaMillisPorPulsoExecuta.pausar();
}


