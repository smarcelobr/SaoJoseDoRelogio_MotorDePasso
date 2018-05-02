#include "botao.h"

Botao::Botao(int pinButton): pin(pinButton), lastDebounceTime(0), onHighState(0), onLowState(0) {
  this->pin = pinButton;
  pinMode(this->pin, INPUT_PULLUP);
  
  int reading = digitalRead(this->pin);
  this->lastButtonState = reading;
  this->buttonState = reading;
  
}

void Botao::setCallbackOnHIGH(void (*onHighState)()) {
  this->onHighState = onHighState;
}

void Botao::setCallbackOnLOW(void (*onLowState)()) {
  this->onLowState = onLowState;
}

void Botao::atualiza() {
 
  int reading = digitalRead(this->pin);

  if (reading != this->lastButtonState) {
    // reset the debouncing timer
    this->lastDebounceTime = millis();
  }

  if ((millis() - this->lastDebounceTime) > DEBOUNCE_DELAY) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != this->buttonState) {
      // agora eh oficial. Houve uma transição do botão.
      this->buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (this->buttonState == HIGH && this->onHighState) {
          this->onHighState();
      } else if (this->buttonState == LOW && this->onLowState) {
        this->onLowState();
      }
    }
  }

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  this->lastButtonState = reading;   
  
}

int Botao::get() {
  this->atualiza();
  return this->buttonState;  
}

