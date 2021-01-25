#include <arduino.h>
#include <FastLED.h>
#include <stdio.h>

#define NUM_LEDS 144

int loopTime = 10;

CRGB leds[NUM_LEDS];

//Color Change Forward Button 
int colorChangeForwardBtnState = LOW;
int colorChangeForwardBtnLastDebounceTime = 0;  // the last time the output pin was toggled
int colorChangeForwardBtnDebounceDelay = 50;            
int colorChangeForwardBtnLastState = LOW;
int colorChangeForwardBtnDownPending = 0;
int colorChangeForwardBtnPending = 0;

//Color Change Backward Button 
int colorChangeBackwardBtnState = LOW;
int colorChangeBackwardBtnLastDebounceTime = 0;  // the last time the output pin was toggled
int colorChangeBackwardBtnDebounceDelay = 50;            
int colorChangeBackwardBtnLastState = LOW;
int colorChangeBackwardBtnDownPending = 0;
int colorChangeBackwardBtnPending = 0;

//Dimming Brighter Button 
int dimmingBrighterBtnState = LOW;
int dimmingBrighterBtnLastDebounceTime = 0;  // the last time the output pin was toggled
int dimmingBrighterBtnDebounceDelay = 50;            
int dimmingBrighterBtnLastState = LOW;
int dimmingBrighterBtnDownPending = 0;
int dimmingBrighterBtnPending = 0;

//Dimming Darker Button 
int dimmingDarkerBtnState = LOW;
int dimmingDarkerBtnLastDebounceTime = 0;  // the last time the output pin was toggled
int dimmingDarkerBtnDebounceDelay = 50;            
int dimmingDarkerBtnLastState = LOW;
int dimmingDarkerBtnDownPending = 0;
int dimmingDarkerBtnPending = 0;

#define COLOR_CHANGE_BACKWARD_PIN 2
#define COLOR_CHANGE_FORWARD_PIN 6
#define LED_CONTROL_PIN 17
#define DIMMING_DARKER_PIN 10
#define DIMMING_BRIGHTER_PIN 12
  
int outputValue = 0;
CRGB presetColors[] = {CRGB(255, 147, 44), CRGB(255, 161, 72), CRGB(255, 0, 0), CRGB(0, 0, 255), CRGB(0, 255, 0), CRGB(255, 20, 0), CRGB(255, 0, 255)};
// Strangely the color code set here doen't correspons actual color
// It seems R->G / G->R / B->B

int totalPresetColors = (sizeof(presetColors)/sizeof(CRGB));
int selectedColorIndex = 0;
double brightness = 0.42;
double brightnessStep = 0.03;
double maxBrightness = 0.6;

void setup() {
  delay(500); // sanity delay

  pinMode(COLOR_CHANGE_BACKWARD_PIN, INPUT);
  pinMode(COLOR_CHANGE_FORWARD_PIN, INPUT);
  pinMode(DIMMING_DARKER_PIN, INPUT);
  pinMode(DIMMING_BRIGHTER_PIN, INPUT);
  pinMode(LED_CONTROL_PIN, OUTPUT);

  FastLED.addLeds<WS2812, LED_CONTROL_PIN, GRB>(leds, NUM_LEDS);
  Serial.begin(9600);
  char buffer[50];
  sprintf(buffer, "totalPresetColors: %d", totalPresetColors);
  Serial.println(buffer);
  // Set initial color 
  CRGB selectedColor = getColor(presetColors[selectedColorIndex], brightness);
  setLEDColor(leds, NUM_LEDS, selectedColor);

 // wait up to 3 seconds for the Serial device to become available
  long unsigned debug_start = millis ();
  while (!Serial && ((millis () - debug_start) <= 3000))
    ;
}


void setLEDColor(CRGB *leds, int numLEDs, CRGB color) {
  for (int i = 0; i < numLEDs; i++) {
    leds[i] = color;
  }
}
// void powerUpBlade(){
//   isAnimating = 1;
//   bladeState = 1;
//   bladeStateReal = 1;
//   Serial.println("Turn on blade");
//   bladeOn = 1;
// }
 
// void powerDownBlade(){
//  //animate DOWN 
//  bladeState = 3;
//  bladeStateReal = 3;
//  fadeStep = NUM_LEDS;
//  isAnimating = 1;
//  Serial.println("Turn off blade");
//  bladeOn = 0;
// }
  
// void bladeIsAnimatingUp(){
//   Serial.println("bladeIsAnimatingUp");
//   int midpoint = NUM_LEDS/2;
//   int newSection = fadeStep+fadeStepSize;
//   for( int j = fadeStep; j < newSection; j++) {
//     leds[j] = selectedColor;
//     leds[NUM_LEDS - 1 - j] =  selectedColor;
//   }
//   fadeStep = newSection;
    
//   if (fadeStep >= midpoint+fadeStepSize){
//     fadeStep = NUM_LEDS;
//     isAnimating=0;
//     bladeState = 2; 
//     bladeStateReal = 2;
//     Serial.println("blade up complete");
//   }
//   Serial.println(bladeStateReal);
// }
    
// void bladeIsAnimatingDown(){
//   Serial.println("bladeIsAnimatingDown");
//   int midpoint = NUM_LEDS/2;
//   int newSection = fadeStep-fadeStepSize;
//   for( int j = fadeStep; j > newSection; j--) {
//     if (j - midpoint - 1 >= 0) {
//       leds[j - midpoint - 1] = CRGB(0,0,0);
//     }
//     if (midpoint - j < 0) {
//       leds[midpoint + NUM_LEDS - j] = CRGB(0,0,0);
//     }
//   }
//   fadeStep = newSection;
  
//   if (fadeStep <=midpoint-fadeStepSize){
//       fadeStep = 0;
//       isAnimating=0;
//       bladeStateReal =0;
//       bladeState = 0;  
//       Serial.println("balde down complete");
//   }
// }


void checkButtonDebounceDelayAndChangeState(
  int reading, int *lastBtnState, int *lastDebounceTime, 
  int debounceDelay, int *btnState, void (*callback)()){
  /**
   * Check if the push button is definitely pressed for the given period of time. 
   * If so, change the state and call callback function
   */
  // Serial.println(debounceDelay);
  char buffer[100];
  if (reading == HIGH) {
    // Serial.println("Button pressed");
  }
  if (reading != *lastBtnState) {
    // reset the debouncing timer
    *lastDebounceTime = millis();
    // sprintf(buffer, "Start debouncing counter: %d", *lastDebounceTime);
    // Serial.println(buffer);
  }
  // Serial.println(debounceDelay);

  if ((millis() - *lastDebounceTime) > debounceDelay) {
    if (reading != *btnState) {
      // sprintf(buffer, "Switchching the state to %d", reading);
      // Serial.println(buffer);
      *btnState = reading;
      if (*btnState == HIGH) {
        (*callback)();
      }
    }
  }
  // Serial.println(debounceDelay);
  // save the reading. Next time through the loop, it wil be the lastState
  *lastBtnState = reading;
  // sprintf(buffer, "reading: %d\tlast state: %d\tstate: %d", reading, *lastBtnState, *btnState);
  // Serial.println(buffer);
  // sprintf(buffer, "Last debounce time: %d\ttime now: %d\tDebounce delay: %d", 
  //   *lastDebounceTime, millis(), debounceDelay);
  // Serial.println(buffer);
}


void loop(){
  
  //handle color change backward button
  int reading = digitalRead(COLOR_CHANGE_BACKWARD_PIN);
  checkButtonDebounceDelayAndChangeState(
    reading, &colorChangeBackwardBtnLastState, &colorChangeBackwardBtnLastDebounceTime, 
    colorChangeBackwardBtnDebounceDelay, &colorChangeBackwardBtnState, previousColor);
  
  //handle color change forward button
  reading = digitalRead(COLOR_CHANGE_FORWARD_PIN);
  checkButtonDebounceDelayAndChangeState(
    reading, &colorChangeForwardBtnLastState, &colorChangeForwardBtnLastDebounceTime, 
    colorChangeForwardBtnDebounceDelay, &colorChangeForwardBtnState, nextColor);

  //handle brightness change darker button
  reading = digitalRead(DIMMING_DARKER_PIN);
  checkButtonDebounceDelayAndChangeState(
    reading, &dimmingDarkerBtnLastState, &dimmingDarkerBtnLastDebounceTime, 
    dimmingDarkerBtnDebounceDelay, &dimmingDarkerBtnState, darker);

  //handle brightness change brighter button
  reading = digitalRead(DIMMING_BRIGHTER_PIN);
  checkButtonDebounceDelayAndChangeState(
    reading, &dimmingBrighterBtnLastState, &dimmingBrighterBtnLastDebounceTime, 
    dimmingBrighterBtnDebounceDelay, &dimmingBrighterBtnState, brighter);

  // digitalWrite(LED_CONTROL_PIN, HIGH);  // enable access to LEDs
  FastLED.show();                     // Refresh strip
  // digitalWrite(LED_CONTROL_PIN, LOW);
  
  delay(loopTime);
}

void nextColor(){
  Serial.println("Next color");
  selectedColorIndex++;
  if(selectedColorIndex >= totalPresetColors){
    selectedColorIndex = totalPresetColors - 1;
  }
  CRGB color = getColor(presetColors[selectedColorIndex], brightness);
  printColor(color);
  setLEDColor(leds, NUM_LEDS, color);
}

void previousColor(){
  Serial.println("Previous color");
  selectedColorIndex--;
  if(selectedColorIndex < 0){
    selectedColorIndex = 0;
  }
  CRGB color = getColor(presetColors[selectedColorIndex], brightness);
  printColor(color);
  setLEDColor(leds, NUM_LEDS, color);
}

void darker(){
  Serial.println("Darker");
  brightness = brightness - brightnessStep;
  if(brightness < 0){
    brightness = 0;
  }
  CRGB color = getColor(presetColors[selectedColorIndex], brightness);
  printColor(color);
  setLEDColor(leds, NUM_LEDS, color);
}
   
void brighter(){
  Serial.println("Brighter");
  brightness = brightness + brightnessStep;
  if(brightness > maxBrightness){
    brightness = maxBrightness;
  }
  CRGB color = getColor(presetColors[selectedColorIndex], brightness);
  printColor(color);
  setLEDColor(leds, NUM_LEDS, color);
}

CRGB getColor(CRGB presetColor, double brightness) {
  CRGB color = CRGB(0, 0, 0);
  color.r = (uint8_t) presetColor.r * brightness;
  color.g = (uint8_t) presetColor.g * brightness;
  color.b = (uint8_t) presetColor.b * brightness;
  return color;
}

void printColor(CRGB color) {
  char buffer[100];
  sprintf(buffer, "Color changed: R%d, G%d, B%d", color.red, color.green, color.blue);
  Serial.println(buffer);
}
