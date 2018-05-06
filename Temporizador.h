#ifndef _TEMPORIZADOR_H
#define _TEMPORIZADOR_H

#if defined(ARDUINO) && ARDUINO >= 100
   #include "Arduino.h"
#else
   #include "WProgram.h"
#endif   

#define LIMITE_TEMPORIZADORES 10

class ItemTemporizado {
protected:
  unsigned long intervalo;
  unsigned long proximaChamada;
  unsigned long qtdChamadas;
  boolean long emPausa;

public:
  ItemTemporizado(unsigned long intervalo): intervalo(intervalo), emPausa(false) { reseta(); }

  virtual void executar(ItemTemporizado *source) = 0;
  
  void setIntervalo(int intervalo) { this->intervalo = intervalo; } 
  int getIntervalo() { return this->intervalo; } 
  
  /* quantidade de chamadas do executar deste o último reset() */
  int getQtdChamadas() { return this->qtdChamadas; } 

 /** 
  * reseta() - Reinicia a contagem do intervalo.
  */
  void reseta() { this->proximaChamada = millis() + this->intervalo; this->qtdChamadas = 0;}
  
  /**
   * dá uma pausa nas chamadas.
   */
  void pausar() { this->emPausa = true; }
  
  /**
   * continua após a pausa
   */
  void continuar() { this->emPausa = false; }
 
  /**
   * reinicia o temporizador. O intervalo passará a ser contado a partir desta chamada.
   * Se estiver em pausa, este método retira da pausa.
   */
  void reiniciar() { this->reseta(); this->continuar(); }
 

  boolean isEmPausa() { return this->emPausa; }
  
  friend class Temporizador;
};

/**
 * funcao temporizada
 */
class FuncaoTemporizada: public ItemTemporizado {
private:
  void (*funcao)(ItemTemporizado *source);
  
public:
  FuncaoTemporizada(unsigned long intervalo, void (*funcao)(ItemTemporizado *source)): ItemTemporizado(intervalo), funcao(funcao) { }
  
  void setFuncao(void (*funcao)(ItemTemporizado *source)) { this->funcao = funcao; }  
   
  void executar(ItemTemporizado *source) { funcao(source); }
} ;

/** 
 * Registra um Metodo temporizado no temporizador. A cada intervalo em milisegundos, esse metodo deverá ser executado.
 */
template <class T> class MetodoTemporizado : public ItemTemporizado {
private:
  T* instancia;
  void (T::*metodo)(ItemTemporizado *source);

public:  

   /**
    * Cria o temporizador especificando o intevalo (em milisegundos) e o metodo a ser chamado a cada periodo.
    */
   MetodoTemporizado(unsigned long intervalo, T *instancia, void (T::*metodo)(ItemTemporizado *source)): ItemTemporizado(intervalo), metodo(metodo), instancia(instancia) { }
   
   void setMetodo(void (T::*metodo)(ItemTemporizado *source)) { this->metodo = metodo; }
   
   void executar(ItemTemporizado *source) { (instancia->*metodo)(source); }
};

/**
  Classe que chama um metodo cada vez que um certo tempo em milisegundos ocorre.
 */
class Temporizador {
private:
  
  unsigned int qtdItens;
  ItemTemporizado *itens[LIMITE_TEMPORIZADORES];
  
  unsigned long lastCurrentMillis;
  
public:

   /**
    * Cria o temporizador especificando o intevalo (em milisegundos) e o metodo a ser chamado a cada periodo.
    */
   Temporizador(): qtdItens(0), lastCurrentMillis(0) { }
   
   /**
    * atualiza() deve ser chamado no loop para a atualização do botão e execução do callback se houver. 
    */
   void atualiza();
  
  /**
   * adiciona um temporizador generico
   */
   void add(ItemTemporizado &item);
   
};

extern Temporizador temporizador;  // indica que existe uma instancia globalzona para essa classe que está definida no Temporizador.cpp

#endif

