#include "ClockInterno.h"

ClockInterno::ClockInterno(unsigned int horaInicial, unsigned int minutoInicial, unsigned int segundoInicial):
         hora(horaInicial), minuto(minutoInicial), segundo(segundoInicial), callbackOnSegundo(0), onMinuto(0), onHora(0),
        aCadaSegundoTimer(1000, this, &ClockInterno::doEachSegundo) { 
         
  temporizador.add(aCadaSegundoTimer);
  
}

void ClockInterno::doEachSegundo(ItemTemporizado *source) {
   
  this->segundo++;
   
  if (this->segundo>=60) {
     this->segundo=0;
     this->minuto++;
     
     if (this->minuto>=60) {
       this->minuto=0;
       this->hora++;
       if (onHora)
          onHora(this);
     } // if hora
     
     if (onMinuto)
         onMinuto(this);
  } // if minuto
   
  if (this->callbackOnSegundo)
     this->callbackOnSegundo->call(this);
     
}
 
unsigned int ClockInterno::getHora() {
  return this->hora;
}
unsigned int ClockInterno::getMinuto() {
  return this->minuto;
}
unsigned int ClockInterno::getSegundo() {
  return this->segundo;
}
  
void ClockInterno::addSegundos(unsigned int segundos) {
  
  unsigned int newSegundo, newMinuto, newHora;
  
  newSegundo = (this->segundo + segundos) % 60;
  newMinuto = (this->minuto + ( (this->segundo + segundos) / 60 ) ) % 60;
  newHora = this->hora + ( (this->minuto + ( (this->segundo + segundos) / 60 ) ) / 60 );
  
  this->set(newHora, newMinuto, newSegundo);
  
}

void ClockInterno::set(unsigned int hora, unsigned int minuto, unsigned int segundo) {
  this->hora = hora;
  this->minuto = minuto;
  this->segundo = segundo;
}

