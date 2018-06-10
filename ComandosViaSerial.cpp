#include "ComandosViaSerial.h"

/**
 * Deve ser chamado no Loop para testar se algum comando chegou via Serial
 */
void ComandosViaSerial::atualiza() {
  
  // ver comandos via serial
  if (Serial.available() > 0) {
		// read the incoming byte:
    int incomingByte = Serial.read();
    
    switch ((char) incomingByte) {
    case 'M': this->botaoModo->down(); break;
    case 'P': this->botaoPausaContinua->down(); break;
    case '<': this->botaoAntiHorario->down(); break;
    case '[': this->botaoAntiHorario->up(); break;
    case '>': this->botaoAntiHorario->down(); break;
    case ']': this->botaoHorario->up(); break;    
    }

  }
}

