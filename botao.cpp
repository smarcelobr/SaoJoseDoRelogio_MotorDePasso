#include "botao.h"

Botao::Botao(int pinButton): pin(pinButton), lastDebounceTime(0), callbackOnHigh(0), callbackOnLow(0) {
  this->pin = pinButton;
  pinMode(this->pin, INPUT_PULLUP);
  
  int reading = digitalRead(this->pin);
  this->lastButtonState = reading;
  this->buttonState = reading;
  
}

void Botao::setCallbackOnHIGH(Callback<Botao> *callback) {
  callback->setNext(this->callbackOnHigh);
  this->callbackOnHigh = callback;
}

void Botao::setCallbackOnLOW(Callback<Botao> *callback) {
  callback->setNext(this->callbackOnLow);
  this->callbackOnLow = callback;
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
      if (this->buttonState == HIGH && this->callbackOnHigh) {
          this->callbackOnHigh->call(this);
      } else if (this->buttonState == LOW && this->callbackOnLow) {
        this->callbackOnLow->call(this);
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

