#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#include <NewPing.h>
#include <Array.h>

// MOTOR SHIELD
#include <AccelStepper.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"

Adafruit_MotorShield AFMSbot(0x61); // Rightmost jumper closed
Adafruit_MotorShield AFMStop(0x60); // Default address, no jumpers

// Connect two steppers with 200 steps per revolution (1.8 degree)
// to the top shield
Adafruit_StepperMotor *myStepper1 = AFMStop.getStepper(200, 1);
Adafruit_StepperMotor *myStepper2 = AFMStop.getStepper(200, 2);

// Now we'll wrap the 2 steppers in an AccelStepper object
AccelStepper stepper1(forwardstep1, backwardstep1);
AccelStepper stepper2(forwardstep2, backwardstep2);

// NEOPIXEL init
// ===========================
// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

//#define PIN 11
#define PIN 12

int furthestDist = 420;
int minimumDistance = 0;
int minimumFirstFence = 20;
int lightTop = 1;
int lightBottom = 100;
int controlStep = 12;


// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);

// Position & direction of bar
int pos = 0;
int dir = 1;
int pos2 = 0;
int dir2 = 1;

int previousMinIndex = 0;
int lowBase = 0;
int maxAverage= 120;

int defaultColor[4] = {16, 90, 160, 255};
int currentColor[4] = {16, 90, 160, 255};

// Init an Ultrasonic object
// ===========================
#define SONAR_NUM     5 // Number of sensors.
#define MAX_DISTANCE 500 // Maximum distance (in cm) to ping.
#define PING_INTERVAL 100 // Milliseconds between sensor pings (29ms is about the min to avoid cross-sensor echo).

unsigned long pingTimer[SONAR_NUM]; // Holds the times when the next ping should happen for each sensor.
unsigned int cm[SONAR_NUM];         // Where the ping distances are stored.
uint8_t currentSensor = 0;          // Keeps track of which sensor is active.

// Sensor object array.
// Each sensor's trigger pin, echo pin, and max distance to ping.
NewPing sonar[SONAR_NUM] = {
  NewPing(2, 3, MAX_DISTANCE),
  NewPing(4, 5, MAX_DISTANCE),
  NewPing(6, 7, MAX_DISTANCE),
  NewPing(8, 9, MAX_DISTANCE),
  NewPing(10, 11, MAX_DISTANCE)
};

const byte size = 5;
int rawArray[size] = {1,2,3,4,5};
int sensorArrayValue[size];
Array<int> array = Array<int>(sensorArrayValue, size);

const long averageDifferenceLow = 5;
const long averageDifferenceHigh = 20;
unsigned long previousAverage = 0;
unsigned long currentAverage = 0;

// Length of slider
#define STEPPER_STEPS 1100
int currentValue = 0;
int conf = 0;
int motorMovingUp = 0;
int motorMovingDown = 0;

// Timer init
// ===========================
const long interval = 500;
unsigned long previousMillis = 0;

const long interactionInterval = 10000;
const long interactionPreviousInterval = 0;
unsigned long interactionPreviousMillis = 0;

char val; // Data received from the serial port
int ledPin = 13; // Set the pin to digital I/O 13
boolean ledState = LOW; //to toggle our LED
const long serialInterval = 50;
unsigned long previousSerialMillis = 0;

// To Control the Motor for configuration
int motorStateButton = LOW;
int motorPrevious = LOW;
long time = 0;
long debounce = 200;
int clearBar = 0;

// To turn off the motors incase it gets to busy
int motorState = LOW;
int motorAtBottom = 0;
const long serialMotorInterval = 50;
unsigned long previousMotorMillis = 0;

void setup() {

  Serial.print("S: Current POS");
  Serial.println(stepper1.currentPosition());
  
  //initialize serial communications at a 9600 baud rate
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT); // Set pin as OUTPUT

  // First ping starts at 75ms, gives time for the Arduino to chill before starting.
  pingTimer[0] = millis() + 75;
  // Set the starting time for each sensor.
  for (uint8_t i = 1; i < SONAR_NUM; i++) {
    pingTimer[i] = pingTimer[i - 1] + PING_INTERVAL;
  }

  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  
  // send a byte to establish contact until receiver responds 
  //  establishContact();

  AFMSbot.begin(); // Start the bottom shield
  AFMStop.begin(); // Start the top shield

  //  INCHES
  // 12 steps = 1"
  // 131 steps = 1'

  // CM
  // 5 steps = 1cm
  
  stepper1.setMaxSpeed(200.0);
  stepper1.setAcceleration(100.0);
  stepper1.moveTo(1);
    
  stepper2.setMaxSpeed(200.0);
  stepper2.setAcceleration(100.0);
  stepper2.moveTo(1);
}

void loop() {
  unsigned long currentMillis = millis();

//  Serial.print("L: Current POS");
//  Serial.println(stepper1.currentPosition());

  if (Serial.available()) { 
    val = Serial.read(); // read it and store it in val
    if (val == '1') {
      // digitalWrite(ledPin, HIGH); // turn the LED on
      if(currentMillis - previousSerialMillis >= serialInterval) {
        previousSerialMillis = currentMillis; 
        ledState = !ledState; //flip the ledState
        digitalWrite(ledPin, ledState);
        motorStateButton = !motorStateButton;
        currentValue = 0;
        conf = 0;
        if (motorState == HIGH){
          motorState = LOW;
          motorAtBottom = 0;
          motorMovingUp = 1;
          motorMovingDown = 0;
          stepper1.moveTo(0);
          stepper2.moveTo(0);
        }
      }
    } else if (val == 'u'){
        stepper1.moveTo(stepper1.currentPosition() + controlStep);
        stepper2.moveTo(stepper2.currentPosition() + controlStep);
//        currentValue -= 12;
    } else if (val == 'd'){
        stepper1.moveTo(stepper1.currentPosition() - controlStep);
        stepper2.moveTo(stepper2.currentPosition() - controlStep);
//        currentValue += 12;
    } else if (val == 'l'){
        stepper1.moveTo(stepper1.currentPosition() + controlStep);
    } else if (val == 'r'){
        stepper2.moveTo(stepper2.currentPosition() + controlStep);
    } else if (val == 'c'){
      currentValue = 0;
      conf = 1;
      if (motorState == HIGH){
        motorState = LOW;
        motorAtBottom = 0;
        motorMovingUp = 1;
        motorMovingDown = 0;
        stepper1.moveTo(-lightBottom);
        stepper2.moveTo(-lightBottom);
      }
    } else if (val == 'm'){
      // TOGGLE THE MOTORS
      if(currentMillis - previousMotorMillis >= serialMotorInterval) {
        previousMotorMillis = currentMillis; 
        //flip the motorState
        motorState = !motorState;
        motorAtBottom = 0;
      }
    }
//    delay(10); // Wait 10 milliseconds for next reading
  } else {
//    Serial.println("Hello, world!"); //send back a hello world
//    delay(50);
  }

  if (motorState == HIGH){
      if (motorMovingDown == 0 && motorAtBottom == 0){
        if(stepper1.currentPosition() <= lightTop && stepper1.currentPosition() >= -lightBottom){
          motorMovingDown = 1;
          stepper1.moveTo(stepper1.currentPosition() - lightBottom);
          stepper2.moveTo(stepper2.currentPosition() - lightBottom);
        }
      }

      if (motorMovingDown == 1){
        if (stepper1.distanceToGo() == 0) {
          // slow acceleeration?
//          motorMovingUp = 0;
          motorAtBottom = 1;
          motorMovingDown = 0;
        }
      }

      stepper1.run();
      stepper2.run();
  }

  if (conf == 1){
    for(int i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(127, 127, 127));
    }
    strip.show();
  
    stepper1.run();
    stepper2.run();
    
    return;
  }

  if (motorStateButton == LOW){
    //  rainbow(20);
    rainbowCycle(20);
    clearBar = 1;
    
    stepper1.run();
    stepper2.run();
    
    return;
  } else {
    
    // Only clear bar the first time after showing the rainbow
//    Serial.println(clearBar);
    if (clearBar == 1){
      Serial.println("HOW MUCH GOES IN HERE?");
      for(int i=0; i<strip.numPixels(); i++) {
        strip.setPixelColor(i, strip.Color(0, 0, 0));
      }
      strip.show();
      clearBar = 0;
    }
  }
  
  // SENSORS
  // Loop through all the sensors.
  for (uint8_t i = 0; i < SONAR_NUM; i++) {
    
    // Is it this sensor's time to ping?
    if (millis() >= pingTimer[i]) {
      
      // Set next time this sensor will be pinged.
      pingTimer[i] += PING_INTERVAL * SONAR_NUM;
      
      // Sensor ping cycle complete, do something with the results.
      if (i == 0 && currentSensor == SONAR_NUM - 1) oneSensorCycle();
      
      // Make sure previous timer is canceled before starting a new ping (insurance).
      sonar[currentSensor].timer_stop();
      
      // Sensor being accessed.
      currentSensor = i;
      
      // Make distance zero in case there's no ping echo for this sensor.
      cm[currentSensor] = 0;
      
      // Do the ping (processing continues, interrupt will call echoCheck to look for echo).
      sonar[currentSensor].ping_timer(echoCheck);
    }
  }

  int j;
  int minIndex = array.getMinIndex();
  int minNum = array.getMin();
  int disAverage = array.getAverage();

  // BE CURIOUS
  // =================
  if(disAverage > furthestDist){
    // switch modes
    // loop through and dop fun things like no one is watching
    // low yourself from the ceiling
    int j;

    // Updates and draws new LED positions
    setLEDPosition(pos2);
    
    strip.show();
    
    // Rather than being sneaky and erasing just the tail pixel,
    // it's easier to erase it all and draw a new one next time.
    for(j=-3; j<= 3; j++) { 
      strip.setPixelColor(pos2+j, 0);
    }

    // Bounce off ends of strip
    pos2 += dir2;
    if(pos2 < 0) {
      pos2 = 1;
      dir2 = -dir2;
    } else if(pos2 >= strip.numPixels()) {
      pos2 = strip.numPixels() - 2;
      dir2 = -dir2;
    }

    // Sets the sensor array values
    // sensorArrayValue[n] = checkDistance(cm[n]);
    setSensorArrayValues();
  
    return;
  } else {

    //this is a calling to look come at me if no one has in 10 seconds
    if(currentMillis - interactionPreviousMillis >= interactionInterval) {
        interactionPreviousMillis = currentMillis; 

        int r = random(0, 255);
        int g = random(0, 255);
        int b = random(0, 255);
        
        for(int i=0; i<strip.numPixels(); i++) {
          //  strip.setPixelColor(i, strip.Color(127, 127, 127));
          strip.setPixelColor(i, strip.Color(r, g, b));
        }
        strip.show();
        // peeps won't be liking this 
        delay(500);

        for(int i=0; i<strip.numPixels(); i++) {
          strip.setPixelColor(i, strip.Color(0, 0, 0));
        }
    }
    
    //  Waiting for the sonars to start to work
    if (minNum == 0){
      //  Serial.println("Waiting for the sonars to start detecting");
      // minimumDistance = 0
      // minimumFirstFence = 20
    } else if (minNum > minimumDistance && minNum < minimumFirstFence){

      if (motorState == LOW){
        // roll up the motors
        if (motorMovingUp == 0){
          if (stepper1.currentPosition() <= lightTop && stepper1.currentPosition() >= -lightBottom) {
            motorMovingUp = 1;
            stepper1.moveTo(0);
            stepper2.moveTo(0);
          }
    
          if (motorMovingUp == 1){
            if (stepper1.distanceToGo() == 0) {
              // slow acceleeration?
              motorMovingDown = 0;
            }
          }
        }
      }

      // fade the lights
      if (currentColor[0] > 0){
        currentColor[0] -= 10;
        if(currentColor[0] < 0){
          currentColor[0] = 0;
        }
      }
  
      if (currentColor[1] > 0){
        currentColor[1] -= 10;
        if(currentColor[1] < 0){
          currentColor[1] = 0;
        }
      }
  
      if (currentColor[2] > 0){
        currentColor[2] -= 10;
        if(currentColor[2] < 0){
          currentColor[2] = 0;
        }
      }
  
      if (currentColor[3] > 0){
        currentColor[3] -= 10;
        if(currentColor[3] < 0){
          currentColor[3] = 0;
        }
      }

      if (currentColor[4] > 0){
        currentColor[4] -= 10;
        if(currentColor[4] < 0){
          currentColor[4] = 0;
        }
      }
      
    } else if (minNum > 19) {

      Serial.println(minNum);
      
      if (motorState == LOW){
        if (motorMovingDown == 0){
          if(stepper1.currentPosition() <= lightTop && stepper1.currentPosition() >= -lightBottom){
            motorMovingDown = 1;
            stepper1.moveTo(stepper1.currentPosition() - lightBottom);
            stepper2.moveTo(stepper2.currentPosition() - lightBottom);
          } else {
            
          }
        }
  
        if (motorMovingDown == 1){
          if (stepper1.distanceToGo() == 0) {
            // slow acceleeration?
            motorMovingUp = 0;
          }
        }
      }
      
      if (currentColor[0] < defaultColor[0]){
        currentColor[0] += 5;
      }
  
      if (currentColor[1] < defaultColor[1]){
        currentColor[1] += 5;
      }
  
      if (currentColor[2] < defaultColor[2]){
        currentColor[2] += 1;
      }
  
      if (currentColor[3] < defaultColor[3]){
        currentColor[3] += 1;
      }

      if (currentColor[4] < defaultColor[4]){
        currentColor[4] += 1;
      }
    }  

    // Updates and draws new LED positions    
    setLEDPosition(pos);
   
    strip.show();
   
    // Rather than being sneaky and erasing just the tail pixel,
    // it's easier to erase it all and draw a new one next time.
    for(j=-3; j<= 3; j++) { 
      strip.setPixelColor(pos+j, 0);
    }
  
    currentAverage = array.getAverage();

    if((abs(currentAverage - previousAverage) >= averageDifferenceLow) || (abs(currentAverage - previousAverage) <= averageDifferenceHigh)) {
      previousAverage = currentAverage;

      // 60 / 5 = 12
      // 12 / 2 = = 6
      if(minIndex == 0){
        //  lowBase = 3;
        lowBase = 3;
      } else if (minIndex == 1){
        lowBase = 18;
      } else if(minIndex == 2) {
        lowBase = 30;
      } else if(minIndex == 3) {
        lowBase = 42;
      } else if(minIndex == 4) {
        lowBase = 55;
      } else {
        lowBase = 55;
      }
    }
  
    if(pos < lowBase){
      pos++;
    } else if(pos > lowBase){
      pos--;  
    }
  
    previousMinIndex = minIndex;
  
    if(currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;

      // Sets the sensor array values
      // sensorArrayValue[n] = checkDistance(cm[n]);
      setSensorArrayValues();
    
      Array<int> array = Array<int>(sensorArrayValue, size);
//      Serial.print("MIN VALUE: ");
//      Serial.println(array.getMin());
//      Serial.print("MIN INDEX: ");
//      Serial.println(array.getMinIndex());
//      Serial.print("AVERAGE: ");
//      Serial.println(array.getAverage());
  
  // PIXEL FOLLOW SONAR
      currentAverage = array.getAverage();
  //
  //    Testing LED Patterns
  //    =======================
  //    fade(10);
  //    larson();
  //    rainbow(20);
      
      }
  }

  stepper1.run();
  stepper2.run();
}

// you can change these to DOUBLE or INTERLEAVE or MICROSTEP!
// wrappers for the first motor!
void forwardstep1() {  
  myStepper1->onestep(FORWARD, SINGLE);
}

void backwardstep1() {  
  myStepper1->onestep(BACKWARD, SINGLE);
}

// wrappers for the second motor!
void forwardstep2() {  
  myStepper2->onestep(FORWARD, SINGLE);
}

void backwardstep2() {  
  myStepper2->onestep(BACKWARD, SINGLE);
}

void echoCheck() { // If ping received, set the sensor distance to array.
  if (sonar[currentSensor].check_timer()){
    cm[currentSensor] = sonar[currentSensor].ping_result / US_ROUNDTRIP_CM;
  }
}

void oneSensorCycle() { // Sensor ping cycle complete, do something with the results.
  // The following code would be replaced with your code that does something with the ping results.
  for (uint8_t i = 0; i < SONAR_NUM; i++) {
    sensorArrayValue[i] = cm[i];
    
    Serial.print(i);
    Serial.print("=");
    Serial.print(checkDistance(cm[i]));
    Serial.print("cm ");
    Serial.print(" || ");
    Serial.print(i);
    Serial.print("=");
    Serial.print(cm[i]);
    Serial.print("cm ");
  }
  Serial.println();
}

// Draw 5 pixels centered on pos.  setPixelColor() will clip any
// pixels off the ends of the strip, we don't need to watch for that.
void setLEDPosition(int ledPosition) {

// Red
// ==========
//  strip.setPixelColor(ledPosition - 3, strip.Color(currentColor[0], 0, 0)); // Dark red
//  strip.setPixelColor(ledPosition - 2, strip.Color(currentColor[1], 0, 0)); // Dark red
//  strip.setPixelColor(ledPosition - 1, strip.Color(currentColor[2], 0, 0)); // Medium red
//  strip.setPixelColor(ledPosition    , strip.Color(currentColor[3], 0 , 0)); // Center pixel is brightest
//  strip.setPixelColor(ledPosition + 1, strip.Color(currentColor[2], 0, 0)); // Medium red
//  strip.setPixelColor(ledPosition + 2, strip.Color(currentColor[1], 0, 0)); // Dark red
//  strip.setPixelColor(ledPosition + 3, strip.Color(currentColor[0], 0, 0)); // Dark red

// Green
// ==========
//  strip.setPixelColor(ledPosition - 3, strip.Color(0, currentColor[0], 0)); // Dark red
//  strip.setPixelColor(ledPosition - 2, strip.Color(0, currentColor[1], 0)); // Dark red
//  strip.setPixelColor(ledPosition - 1, strip.Color(0, currentColor[2], 0)); // Medium red
//  strip.setPixelColor(ledPosition    , strip.Color(0, currentColor[3], 0)); // Center pixel is brightest
//  strip.setPixelColor(ledPosition + 1, strip.Color(0, currentColor[2], 0)); // Medium red
//  strip.setPixelColor(ledPosition + 2, strip.Color(0, currentColor[1], 0)); // Dark red
//  strip.setPixelColor(ledPosition + 3, strip.Color(0, currentColor[0], 0)); // Dark red

// yellow
// ==========
//  strip.setPixelColor(ledPosition - 3, strip.Color(currentColor[0], currentColor[0], 0)); // Dark red
//  strip.setPixelColor(ledPosition - 2, strip.Color(currentColor[1], currentColor[1], 0)); // Dark red
//  strip.setPixelColor(ledPosition - 1, strip.Color(currentColor[2], currentColor[2], 0)); // Medium red
//  strip.setPixelColor(ledPosition    , strip.Color(currentColor[3], currentColor[3], 0)); // Center pixel is brightest
//  strip.setPixelColor(ledPosition + 1, strip.Color(currentColor[2], currentColor[2], 0)); // Medium red
//  strip.setPixelColor(ledPosition + 2, strip.Color(currentColor[1], currentColor[1], 0)); // Dark red
//  strip.setPixelColor(ledPosition + 3, strip.Color(currentColor[0], currentColor[0], 0)); // Dark red

//  BLUE
// ==========
//  strip.setPixelColor(ledPosition - 3, strip.Color(0, currentColor[0], currentColor[0])); // Dark red
//  strip.setPixelColor(ledPosition - 2, strip.Color(0, currentColor[1], currentColor[1])); // Dark red
//  strip.setPixelColor(ledPosition - 1, strip.Color(0, currentColor[2], currentColor[2])); // Medium red
//  strip.setPixelColor(ledPosition    , strip.Color(0, currentColor[3], currentColor[3])); // Center pixel is brightest
//  strip.setPixelColor(ledPosition + 1, strip.Color(0, currentColor[2], currentColor[2])); // Medium red
//  strip.setPixelColor(ledPosition + 2, strip.Color(0, currentColor[1], currentColor[1])); // Dark red
//  strip.setPixelColor(ledPosition + 3, strip.Color(0, currentColor[0], currentColor[0])); // Dark red

  //  White
  // ==========
  strip.setPixelColor(ledPosition - 3, strip.Color(currentColor[0], currentColor[0], currentColor[0])); // Dark red
  strip.setPixelColor(ledPosition - 2, strip.Color(currentColor[1], currentColor[1], currentColor[1])); // Dark red
  strip.setPixelColor(ledPosition - 1, strip.Color(currentColor[2], currentColor[2], currentColor[2])); // Medium red
  strip.setPixelColor(ledPosition    , strip.Color(currentColor[3], currentColor[3], currentColor[3])); // Center pixel is brightest
  strip.setPixelColor(ledPosition + 1, strip.Color(currentColor[2], currentColor[2], currentColor[2])); // Medium red
  strip.setPixelColor(ledPosition + 2, strip.Color(currentColor[1], currentColor[1], currentColor[1])); // Dark red
  strip.setPixelColor(ledPosition + 3, strip.Color(currentColor[0], currentColor[0], currentColor[0])); // Dark red

  //  Original hex values 
  // ========================
  //  strip.setPixelColor(pos - 3, 0x100000); // Dark red
  //  strip.setPixelColor(pos - 2, 0x800000); // Dark red
  //  strip.setPixelColor(pos - 1, 0xF00000); // Medium red
  //  strip.setPixelColor(pos    , 0xFF0000); // Center pixel is brightest
  //  strip.setPixelColor(pos + 1, 0xF00000); // Medium red
  //  strip.setPixelColor(pos + 2, 0x800000); // Dark red
  //  strip.setPixelColor(pos + 3, 0x100000); // Dark red
}

void setSensorArrayValues() {
  sensorArrayValue[0] = checkDistance(cm[0]);
  sensorArrayValue[1] = checkDistance(cm[1]);
  sensorArrayValue[2] = checkDistance(cm[2]);
  sensorArrayValue[3] = checkDistance(cm[3]);
  sensorArrayValue[4] = checkDistance(cm[4]);

//    Serial.print("d1: ");
//    Serial.print(checkDistance(cm[0]));
//    Serial.print(", ");
//    Serial.print("d2: ");
//    Serial.print(checkDistance(cm[1]));
//    Serial.print(", ");
//    Serial.print("d3: ");
//    Serial.print(checkDistance(cm[2]));
//    Serial.print(", ");
//    Serial.print("d4: ");
//    Serial.print(checkDistance(cm[3]));
//    Serial.print(", ");
//    Serial.print("d5: ");
//    Serial.println(checkDistance(cm[4]));
}

int checkDistance(int distance) {
  if(distance > 400){
    return 400;
  } else if (distance < 5) {
    return 400;
  } else {
    return distance;
  }
}

void establishContact() {
  while (Serial.available() <= 0) {
    Serial.println("A");   // send a capital A
  }
}


// NEOPIXEL Functions
// ========================
// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void larson(){

  int j;
   
  // Draw 5 pixels centered on pos.  setPixelColor() will clip any
  // pixels off the ends of the strip, we don't need to watch for that.
  strip.setPixelColor(pos - 2, 0x100000); // Dark red
  strip.setPixelColor(pos - 1, 0x800000); // Medium red
  strip.setPixelColor(pos    , 0xFF3000); // Center pixel is brightest
  strip.setPixelColor(pos + 1, 0x800000); // Medium red
  strip.setPixelColor(pos + 2, 0x100000); // Dark red
 
  strip.show();
  delay(30);
 
  // Rather than being sneaky and erasing just the tail pixel,
  // it's easier to erase it all and draw a new one next time.
  for(j=-2; j<= 2; j++) { 
    strip.setPixelColor(pos+j, 0);
  }
  
  // Bounce off ends of strip
  pos += dir;
  if(pos < 0) {
    pos = 1;
    dir = -dir;
  } else if(pos >= strip.numPixels()) {
    pos = strip.numPixels() - 2;
    dir = -dir;
  }
  
}

void fade(uint8_t wait) {
  uint16_t i, j;

  for(j=1; j<200; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(127, 127, 127));
      strip.setBrightness(j);
      delay(wait);
    }
    strip.show();
    delay(wait);
  }

  for(j=200; j>1; j--) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(127, 127, 127));
      strip.setBrightness(j);
      delay(wait);
    }
    strip.show();
    delay(wait);
  }
}

void fadeStep(uint8_t wait) {
  uint16_t i, j;

  for(i=0; i<strip.numPixels(); i++) {
    for(j=1; j<200; j++) {
      strip.setPixelColor(i, strip.Color(j, j, j));
      strip.show();
      delay(wait);
    }
    strip.show();
    delay(wait);
  }

  for(i=0; i<strip.numPixels(); i++) {
    for(j=200; j>1; j--) {
      strip.setPixelColor(i, strip.Color(j, j, j));
      strip.show();
      delay(wait);
    }
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
//    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
//    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights.
void theaterChaseReverse(uint32_t c, uint8_t wait) {
  for (int j=10; j>10; j--) {  //do 10 cycles of chasing
    for (int q=3; q > 3; q--) {
      for (int i=strip.numPixels(); i > strip.numPixels(); i=i-3) {
        strip.setPixelColor(i-q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=strip.numPixels(); i > strip.numPixels(); i=i-3) {
        strip.setPixelColor(i-q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}


// SUDO CODE
// ========================
// Some gravity maths :
// https://codebender.cc/sketch:56734#BouncingBalls2014.ino
// https://github.com/fibonacci162/LEDs
// https://github.com/fibonacci162/LEDs/blob/master/BouncingBalls2014/BouncingBalls2014.ino

// if average is > 325 {
  // if index 0 is the lowest raise the left motor
    // do something with the LED
    // Physics
  // if index 3 is the lowest raise the right motor
    // do something with the LED
    // Physics
// } else {
  // map distance for 0 - 400 cm (13 feet)
  // map the distance to the height it can travel
  // the number of steps the motor needs to take 
  // Total 8 feet
  // 28 steps per rotation
  // lets make it 2 rotations per foot
  // = 56 steps per foot
  // = 448 steps total movement from top to bottom
  // map 400 cm to 448 so each cm someone gets closer to the peice it 
  // directly interacts with their movement
// }

// Also need a set of programs on how it behaves when there isn't anyone there.
// need to make it so that it inches down

// if there isn't anyone near{
  // slowly inch down one , 5, 10 steps at a time and update the leds
// }


// Programatically lighting up static sections of the LED strip
// Doesn't animate though
// ========================
//void mapSonarPosition(int quator, uint8_t wait) {
//  Serial.print("============");
//  Serial.print(quator);
//  Serial.println("------------");
//  
//  for(uint16_t i=0; i<strip.numPixels(); i++) {
//    if(i >= (quator * 7) && i <= (quator * 7 + 7)){
//      strip.setPixelColor(i, strip.Color(127, 127, 127));
//      strip.show();
////      delay(wait);
//    } else {
//      strip.setPixelColor(i, strip.Color(0, 0, 0));
//      strip.show();
//    }
//
//  }
    
//    strip.setPixelColor(, strip.Color(127, 127, 127));
//    strip.show();
//    delay(wait);
//  for(uint16_t i=0; i<strip.numPixels(); i++) {
//    strip.setPixelColor(i, c);
//    strip.show();
//    delay(wait);
//  }
//}

// STEPPER Demo code
  // Change direction at the limits
//  if (stepper1.distanceToGo() == 0){
//    stepper1.moveTo(-stepper1.currentPosition());
//  }
//
//  if (stepper2.distanceToGo() == 0) {
//    stepper2.moveTo(-stepper2.currentPosition());
//  }
