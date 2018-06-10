#ifndef _COMANDOS_VIA_SERIAL_H
#define _COMANDOS_VIA_SERIAL_H

#if defined(ARDUINO) && ARDUINO >= 100
   #include "Arduino.h"
#else
   #include "WProgram.h"
#endif   

#include "botao.h"

#define MAX_NUM_COMANDOS 10

class ComandosViaSerial {
private:
  Botao *botaoModo, *botaoPausaContinua, *botaoAntiHorario, *botaoHorario;
  
public:
   ComandosViaSerial(Botao &botaoModo, Botao &botaoPausaContinua, Botao &botaoAntiHorario, Botao &botaoHorario): 
          botaoModo(&botaoModo), botaoPausaContinua(&botaoPausaContinua),
          botaoAntiHorario(&botaoAntiHorario), botaoHorario(&botaoHorario) { }

   /**
    * Deve ser chamado no Loop para testar se algum comando chegou via Serial
    */
   void atualiza();
};
#endif
