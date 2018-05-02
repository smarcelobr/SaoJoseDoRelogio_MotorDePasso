#include "Temporizador.h"

// instancia aqui o globalzão declarado como extern no Temporizador.h
Temporizador temporizador; // globazao que será usado por todo mundo para chamadas de funcoes ou metodos em intervalos regulares.

/** metodo que deve ser chamado a cada iteracao do loop() 
 * e verifica quais itens temporizados precisam ser executados no momento.
 *
 * Para manter a acuracidade, este metodo deve ser chamado pelo menos uma vez a cada milisegundo. Se demorar mais
 * que isso, haverá perdas de precisão no momento do overflow do millis(). O overflow do millis() ocorre a cada
 * 50 dias.
 */
void Temporizador::atualiza() {
  unsigned long currentMillis = millis();

  /* A cada 50 dias, aproximadamente, ocorre um overflow no contador millis().
     Preciso tratar essa condição do overflow para evitar problemas e paradas no relógio.
  */
  boolean houveOverflow = currentMillis < this->lastCurrentMillis;
  this->lastCurrentMillis = currentMillis;

  for (int ix=0; ix<this->qtdItens; ix++) {
    
      ItemTemporizado *it = this->itens[ix];

      if (houveOverflow) {
         if (it->aguardaOverflow) {
            // não preciso aguardar o overflow. Ele ocorreu agora.
            it->aguardaOverflow=false;
         } else {
            /* Esse bloco executou porque o overflow do millis() ocorreu antes do overflow do itemTemporizado.
              Isso não deveria ser comum, mas, pode ocorrer quando o loop() do programa principal estiver demorando mais que
              1 milisegundo para executar.

              Nesse caso, eu reinicio a proximaChamada para 0. Isso afeta a acuracidade do intervalo.
            */
            it->proximaChamada = 0;
            /* TODO chamar CALLBACK para situacao de ALERTA do temporizador*/
         }
      }

      if (!it->aguardaOverflow && currentMillis >= it->proximaChamada) {
         if (!it->emPausa) 
             it->executar();


         unsigned long ultimaChamada = it->proximaChamada;
         it->proximaChamada = it->proximaChamada + it->intervalo;
         // houve overflow? Se sim, só poderrei comparar com millis() novamente quando ele fizer overflow tb.
         it->aguardaOverflow = !houveOverflow && (ultimaChamada>it.proximaChamada);
      }
  }
  
}

void Temporizador::add(ItemTemporizado &item) {
    if (this->qtdItens<LIMITE_TEMPORIZADORES) {
       this->itens[this->qtdItens++]=&item;
    } else {
       /* TODO chamar CALLBACK para situacao de ALERTA do temporizador*/
    }
 }

