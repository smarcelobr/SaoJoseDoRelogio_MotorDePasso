
#include "Relogio.h"

Relogio::Relogio(WD2404 &wd2404):
    wd2404(&wd2404),
    ligado(false),
    pulsosPendentes(0),
    aCadaMinutoExecuta(60000, this, &Relogio::doEachMinuto),
    aCadaMillisPorPulsoExecuta_1(225, this, &Relogio::doEachMillisPorPulso),
    aCadaMillisPorPulsoExecuta_2(60000, this, &Relogio::doEachMillisPorPulso)
{ 
  // pausa todos os pulsadores, exceto o primeiro.
  for(int i = 1; i < NUM_PULSADORES; ++i) {
    this->getPulsador(i)->pausar();
  }
 
 temporizador.add(aCadaMinutoExecuta); // a cada 20ms, o metodo mudarFaseDoPulso será executado.
  for(int i = 0; i < NUM_PULSADORES; ++i) {
    MetodoTemporizado<Relogio> *pulsador = this->getPulsador(i);
    temporizador.add(*pulsador); // a cada 20ms, o metodo mudarFaseDoPulso será executado.
  }
}

void Relogio::doEachMillisPorPulso(ItemTemporizado *source) {
    this->pulsosPendentes = this->pulsosPendentes + 1;
}

void Relogio::doEachMinuto(ItemTemporizado *source) {
    wd2404->sendPulsos(pulsosPendentes);
    Serial.println(pulsosPendentes);
    this->pulsosPendentes = 0;
}

MetodoTemporizado<Relogio> * Relogio::getPulsador(int idxPulsador)
{
  MetodoTemporizado<Relogio> *aCadaMillisPorPulsoExecuta;
  switch (idxPulsador) {
  case 0: aCadaMillisPorPulsoExecuta = &aCadaMillisPorPulsoExecuta_1; 
          break;
  case 1: aCadaMillisPorPulsoExecuta = &aCadaMillisPorPulsoExecuta_2; 
          break;
  }
  return aCadaMillisPorPulsoExecuta;  
}

void Relogio::setMillisPorPulso(int idxPulsador, unsigned long millisPorPulso) {
  this->getPulsador(idxPulsador)->setIntervalo(millisPorPulso);
}

unsigned long Relogio::getMillisPorPulso(int idxPulsador) {
  return this->getPulsador(idxPulsador)->getIntervalo();
}

void Relogio::ligar() {
  wd2404->sentidoHorario();
  wd2404->resetPulsos();
  ligado=true;
  if (onLigado)
     onLigado(this);
  this->pulsosPendentes=0;
  for(int i = 0; i < NUM_PULSADORES; ++i) {
    MetodoTemporizado<Relogio> *pulsador = this->getPulsador(i);
    if (!pulsador->isEmPausa()) { // se o pulsador estiver ativo, 
       pulsador->reiniciar();  // reinicia a contagem
    }
  }
  aCadaMinutoExecuta.reiniciar();
}

void Relogio::desligar() {
  ligado=false;
  if (onDesligado)
     onDesligado(this);
  aCadaMinutoExecuta.pausar();
}

void Relogio::acionarPulsador(int idxPulsador, boolean ativar) {
  MetodoTemporizado<Relogio> *pulsador = this->getPulsador(idxPulsador);
  if (ativar) {
    pulsador->reiniciar();
  } else {
    pulsador->pausar();
  }
}

boolean Relogio::isEmPausa(int idxPulsador) {
  MetodoTemporizado<Relogio> *pulsador = this->getPulsador(idxPulsador);
  return pulsador->isEmPausa();
}

