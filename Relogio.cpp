
#include "Relogio.h"

Relogio::Relogio(WD2404 &wd2404):
    wd2404(&wd2404),
    ligado(false),
    pulsosPendentes(0), pulsosDe3Minutos(0),
    aCadaMillisPorPulsoExecuta_1(225, this, &Relogio::doEachMillisPorPulso),
    aCadaMillisPorPulsoExecuta_2(50850, this, &Relogio::doEachMillisPorPulso)
{ 
  // pausa todos os pulsadores, exceto o primeiro.
  for(int i = 1; i < NUM_PULSADORES; ++i) {
    this->getPulsador(i)->pausar();
  }
 
  for(int i = 0; i < NUM_PULSADORES; ++i) {
    MetodoTemporizado<Relogio> *pulsador = this->getPulsador(i);
    temporizador.add(*pulsador); // a cada 20ms, o metodo mudarFaseDoPulso será executado.
 }
}

void Relogio::doEachMillisPorPulso(ItemTemporizado *source) {

    this->pulsosPendentes = this->pulsosPendentes + 1;
    this->pulsosDe3Minutos = this->pulsosDe3Minutos + 1;


    bool descarregaPulsos = this->pulsosPendentes>=267; // // 267 é um pouquinho mais que 1 minuto e 266 é um pouquinho menos que 1 minuto. Os pulsos só batem quando chega a tres minutos (800 pulsos).

    if (this.pulsosDe3Minutos>=800 ) { // a cada 3 minutos. Descarrega para resincronizar com o minuto exato
        this.pulsosDe3Minutos=0;
        descarregaPulsos=true;
    }

    if (this->ligado && descarregaPulsos) {
      wd2404->sendPulsos(this->pulsosPendentes);
      this->pulsosPendentes = 0;
    }
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
}

void Relogio::desligar() {
  ligado=false;
  if (onDesligado)
     onDesligado(this);
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

