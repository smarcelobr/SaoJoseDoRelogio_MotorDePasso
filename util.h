#ifndef _UTIL_H
#define _UTIL_H

#if defined(ARDUINO) && ARDUINO >= 100
   #include "Arduino.h"
#else
   #include "WProgram.h"
#endif   

/**
 * Interface dos Callbacks
 * 
 */
template <class S> class Callback {
public:  

 virtual void call(S *source);

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
   
  void call(S *source) { funcao(source); }
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
   
   void call(S *source) { (instancia->*metodo)(source); }
};


#endif
