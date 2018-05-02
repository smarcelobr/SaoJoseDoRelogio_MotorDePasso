
#ifndef _BOTAO_H
#define _BOTAO_H

#if defined(ARDUINO) && ARDUINO >= 100
   #include "Arduino.h"
#else
   #include "WProgram.h"
#endif   

#define DEBOUNCE_DELAY 100

class Botao {
private:
  int pin;
  
  int buttonState;             // the current reading from the input pin
  int lastButtonState;
  unsigned long lastDebounceTime;  // the last time the output pin was toggled
  
  void (*onHighState)(); // funcao callback quando o botão for clicado (necessário chamar o updateStatus() no loop)
  void (*onLowState)(); // funcao callback quando o botão for clicado (necessário chamar o updateStatus() no loop)
  
public:  

   /**
    * Cria e configura o pino do botáo para INPUT com PULL_UP.
    */
   Botao(int pinButton);
   
   /**
    * Durante o setup, vocë pode definir uma função de callback para ser chamada quando o botão mudar para o estado HIGH.
   */
   void setCallbackOnHIGH( void (*onHighState)() );

   /**
    * Durante o setup, vocë pode definir uma função de callback para ser chamada quando o botão mudar para o estado LOW.
   */
   void setCallbackOnLOW( void (*onLowState)() );

   /** 
    * get() retorna o status do botao. Pode ser LOW ou HIGH
    **/
   int get();
   
   /**
    * atualiza() deve ser chamado no loop para a atualização do botão e execução do callback se houver. 
    */
  void atualiza();

};


#endif


