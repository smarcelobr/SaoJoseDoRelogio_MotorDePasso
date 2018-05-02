#include "Temporizador.h"

// instancia aqui o globalzão declarado como extern no Temporizador.h
Temporizador temporizador; // globazao que será usado por todo mundo para chamadas de funcoes ou metodos em intervalos regulares.

/** metodo que deve ser chamado a cada iteracao do loop() 
 * e verifica quais itens temporizados precisam ser executados no momento.
 */
void Temporizador::atualiza() {
  unsigned long currentMillis = millis();
  
  for (int ix=0; ix<this->qtdItens; ix++) {
    
      ItemTemporizado *it = this->itens[ix];

      if (currentMillis >= it->proximaChamada) {
         if (!it->emPausa) 
             it->executar();
         it->proximaChamada = it->proximaChamada + it->intervalo;
      }
    
  }
  
  this->lastCurrentMillis = currentMillis;
}

void Temporizador::add(ItemTemporizado &item) {
    if (this->qtdItens<LIMITE_TEMPORIZADORES) {
       this->itens[this->qtdItens++]=&item;
    }
 }

