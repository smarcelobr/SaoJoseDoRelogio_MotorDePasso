#ifndef _UTIL_H
#define _UTIL_H

#if defined(ARDUINO) && ARDUINO >= 100
   #include "Arduino.h"
#else
   #include "WProgram.h"
#endif   

/**
 * Interface para os Relogios
 */
class Clock {
public:
  virtual unsigned int getHora() = 0;
  virtual unsigned int getMinuto() = 0;
  virtual unsigned int getSegundo() = 0;  
}; 

/**
 * Interface dos Callbacks.
 * Funciona como uma lista encadeada onde vários callbacks podem ser registrados para um mesmo evento.
 */
template <class S> class Callback {
protected:  
 Callback<S> *next;

 virtual void specificCall(S *source)=0;

public:
 Callback():next(0) { };

 void call(S *source) {
    this->specificCall(source);
    if (this->next) {
      this->next->call(source);
    }
 }
 
 void setNext(Callback<S> *next) {
     this->next = next;
 }
};

/**
 * Implementação de Callback que chama uma function
 */
template <class S> class FuncaoCallback: public Callback<S> {
private:
  void (*funcao)(S *source);
  
public:
  FuncaoCallback(void (*funcao)(S *source)): Callback<S>(), funcao(funcao) { }
  
  void setFuncao(void (*funcao)(S *source)) { this->funcao = funcao; }  
   
  void specificCall(S *source) { 
    funcao(source); 
  }
} ; 

/**
 * Implementação de Callback que chama um método de uma instância
 */
template <class S, class T> class MetodoCallback: public Callback<S> {
private:
  T* instancia;
  void (T::*metodo)(S *source);

public:  
   MetodoCallback(T *instancia, void (T::*metodo)(S *source)): Callback<S>(), metodo(metodo), instancia(instancia) { }
   
   void setMetodo(void (T::*metodo)(S *source)) { this->metodo = metodo; }

   void setInstancia(T *instancia) { this->instancia = instancia; }
   T* getInstancia() { return this->instancia; }
   
   void specificCall(S *source) { (instancia->*metodo)(source); }
};


#endif
