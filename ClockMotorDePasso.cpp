#include "ClockMotorDePasso.h"

ClockMotorDePasso::ClockMotorDePasso(WD2404 *wd2404): Clock(),
    wd2404(wd2404), passosAcumulados(0), 
    callbackOnSegundo(0), callbackOnMinuto(0), callbackOnHora(0),
    wd2404OnPulChange(this, &ClockMotorDePasso::contaPassos)
    {
      
  this->wd2404->setCallbackPulChange(&wd2404OnPulChange);
}

void ClockMotorDePasso::contaPassos(WD2404 *source) {

  if (source->getFaseDoPulso()==HIGH) {
     // conta os passos enviados para o motor quando há a transicao LOW->HIGH do PULSO do WD2404
     if (source->getDirecao()==LOW) {
          // sentido horario
         this->passosAcumulados++;
     } else {
          // sentido antihorario
         this->passosAcumulados--;
     }
     // a cada 24 horas, reseta o contador para 0:
     this->passosAcumulados = (this->passosAcumulados%(PASSOS_POR_HORA*24));
     
     // Chamar eventos a cada minuto, hora e segundo
     if (this->callbackOnSegundo && this->passosAcumulados%(PASSOS_POR_HORA/3600)==0)
        this->callbackOnSegundo->call(this);
     
  }
  
}

unsigned int ClockMotorDePasso::getHora() {
  return (this->passosAcumulados/PASSOS_POR_HORA);
}

unsigned int ClockMotorDePasso::getMinuto() {
  return ((this->passosAcumulados%PASSOS_POR_HORA)*60)/PASSOS_POR_HORA;
}

unsigned int ClockMotorDePasso::getSegundo() {
  return (((this->passosAcumulados%PASSOS_POR_HORA)*3600)/PASSOS_POR_HORA)%60;
}
  
/** ajusta este clock com o ponteiro do relogio mecanico */
void ClockMotorDePasso::set(unsigned int hora, unsigned int minuto, unsigned int segundo) {
  this->passosAcumulados = (hora*PASSOS_POR_HORA)+(minuto*PASSOS_POR_HORA/60)+(segundo*PASSOS_POR_HORA/3600);
}

void ClockMotorDePasso::setPassosAcumulados(unsigned long value) {
  this->passosAcumulados = value;
}

