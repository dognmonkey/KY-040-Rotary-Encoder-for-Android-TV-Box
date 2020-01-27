/* ===========================
 *  IAmOrion aka James Tanner
 * ===========================
 * 
 * Version 1.0RC
 * 
 * FUNCTIONS
 * Turn knob left: Volume down
 * Turn knob right: Volume up
 * Press knob: Mute
 * 
 * PINOUT, Digispark to KY-040
 * P0   to  DT
 * P1   to  SW
 * P2   to  CLK
 * VCC  to  5V
 * GND  to  GND
 */

#include "TrinketKeyboard.h" 
#include "Settings.h" 
#define TRINKET

#ifdef TRINKET
#include <avr/power.h>
#endif

#define LATCHSTATE 3
int buttonState = HIGH, lastButtonState = HIGH;
long lastDebounceTime = 0, debounceDelay = 50, lastAction = 0;
int _position = 0, _positionExt = 0, buttonCounter = 0;
int8_t _oldState; bool shouldActionButton=true, btnReset=true, rotaryMode=false;

const int8_t encoderStates[] = {
  0, -1, 1, 0,
  1, 0, 0, -1,
  -1, 0, 0, 1,
  0, 1, -1, 0
};

void setup() {
  #ifdef TRINKET
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  
  pinMode(encoderPinA, INPUT);
  pinMode(encoderPinB, INPUT);
  pinMode(encoderButton, INPUT);
  digitalWrite(encoderPinA, HIGH);
  digitalWrite(encoderPinA, HIGH);
  digitalWrite(encoderButton, LOW);
  _oldState = 3; 
  TrinketKeyboard.begin();
}

void loop() {
  static int pos = 0;
  tick();
  int newPos = getPosition();
  if (pos != newPos) {
     lastAction = millis(); shouldActionButton=false; btnReset=false; rotaryMode=true;
    if (newPos < pos) {
      switch (buttonCounter) {
        case 0:
          // Default Left mode
          TrinketKeyboard.pressKey(0, LEFT_ACTION);
          TrinketKeyboard.pressKey(0, 0);
          break;
        case 1:
          // Mode 1 Left
          TrinketKeyboard.pressKey(0, LEFT_ACTION_MODE1);
          TrinketKeyboard.pressKey(0, 0);
          break;
        case 2:
          // Mode 2 Left
          TrinketKeyboard.pressKey(0, LEFT_ACTION_MODE2);
          TrinketKeyboard.pressKey(0, 0);
          break;
        case 3:
          // Mode 3 Left
          TrinketKeyboard.pressKey(0, LEFT_ACTION_MODE3);
          TrinketKeyboard.pressKey(0, 0);
          break;
        default:
          break;
      }
    }
    else if (newPos > pos){
      switch (buttonCounter) {
        case 0:
          // Default Right mode
          TrinketKeyboard.pressKey(0, RIGHT_ACTION);
          TrinketKeyboard.pressKey(0, 0);
          break;
        case 1:
          // Mode 1 Right
          TrinketKeyboard.pressKey(0, RIGHT_ACTION_MODE1);
          TrinketKeyboard.pressKey(0, 0);
          break;
        case 2:
          // Mode 2 Right
          TrinketKeyboard.pressKey(0, RIGHT_ACTION_MODE2);
          TrinketKeyboard.pressKey(0, 0);
          break;
        case 3:
          // Mode 3 Right
          TrinketKeyboard.pressKey(0, RIGHT_ACTION_MODE3);
          TrinketKeyboard.pressKey(0, 0);
          break;
        default:
          break;
      }
    }
    pos = newPos;
  }
  int reading = digitalRead(encoderButton);
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == HIGH) {
        shouldActionButton=true; btnReset=false;
        if (allowMenuLoop){ buttonCounter++; } else { if (!rotaryMode) { buttonCounter++; } else { doBtnReset(); } } 
        if (buttonCounter > buttonModes){buttonCounter=0;}
        lastAction = millis();
      } else if (buttonState == LOW){
        // may do something with onRelease at a later date...
      }
    } 
  }
  lastButtonState = reading;

  if ((millis() - lastAction) > buttonPressTimeout) {
    if (shouldActionButton) {
      switch (buttonCounter) {
        case 0:
          break;
        case 1:
          TrinketKeyboard.pressKey(0, BUTTON_ACTION_SINGLE);
          TrinketKeyboard.pressKey(0, 0);
          break;
        case 2:
          TrinketKeyboard.pressKey(0, BUTTON_ACTION_DOUBLE);
          TrinketKeyboard.pressKey(0, 0);
          break;
        case 3:
          TrinketKeyboard.pressKey(0, BUTTON_ACTION_TRIPLE);
          TrinketKeyboard.pressKey(0, 0);
          break;
        default:
          break;
      }
      shouldActionButton=false;
    }
    if (!btnReset){
      doBtnReset();
    }
  }
  TrinketKeyboard.poll();
}

void doBtnReset() {
    buttonCounter = 0;rotaryMode=false;
    btnReset=true;
}

int  getPosition() {
  return _positionExt;
}

void setPosition(int newPosition) {
  _position = ((newPosition<<2) | (_position & 0x03));
  _positionExt = newPosition;
}

void tick(void) {
  int sig1 = digitalRead(encoderPinA);
  int sig2 = digitalRead(encoderPinB);
  int8_t thisState = sig1 | (sig2 << 1);
  if (_oldState != thisState) {
    _position += encoderStates[thisState | (_oldState<<2)];
    if (thisState == LATCHSTATE)
      _positionExt = _position >> 2;
    _oldState = thisState;
  }
} 
