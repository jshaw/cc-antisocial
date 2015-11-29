#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#include <Ultrasonic.h>
#include <Array.h>


// NEOPIXEL init
// ===========================
// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

#define PIN 11

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(30, PIN, NEO_GRB + NEO_KHZ800);

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
// TODO: Put these inits and declarations into an array
Ultrasonic ultrasonicOne(9, 8);
Ultrasonic ultrasonicTwo(7, 6);
Ultrasonic ultrasonicThree(5, 4);
Ultrasonic ultrasonicFour(3, 2);
//Ultrasonic ultrasonicFive(10, 9);

int distanceOne = 0;
int distanceTwo = 0;
int distanceThree = 0;
int distanceFour = 0;
int distanceFive = 0;

int distanceMapOne = 0;
int distanceMapTwo = 0;
int distanceMapThree = 0;
int distanceMapFour = 0;
int distanceMapFive = 0;

//const byte size = 10;
//int sensorArrayValue[3];
//Array<int> array = Array<int>(sensorArrayValue, size);

const byte size = 4;
int rawArray[size] = {1,2,3};
int sensorArrayValue[size];
Array<int> array = Array<int>(sensorArrayValue, size);

const long averageDifferenceLow = 5;
const long averageDifferenceHigh = 20;
unsigned long previousAverage = 0;
unsigned long currentAverage = 0;

// Timer init
// ===========================
const long interval = 500;
unsigned long previousMillis = 0;

char val; // Data received from the serial port
int ledPin = 13; // Set the pin to digital I/O 13
boolean ledState = LOW; //to toggle our LED


void setup() {
  //initialize serial communications at a 9600 baud rate
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT); // Set pin as OUTPUT

  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  
  // send a byte to establish contact until receiver responds 
  //  establishContact();

   for (byte i=0; i<array.size(); i++){
  //   Serial.print(array);
  //   Serial.print(", ");
   }
   Serial.println("\nSpecial functionality:");
   Serial.print("\tMinimum value:");
   Serial.print(array.getMin());
   Serial.println();
   Serial.print("\tMaximum value:");
   Serial.print(array.getMax());
   Serial.println();
   Serial.print("\tAverage value:");
   Serial.print(array.getAverage());
   Serial.println();
}

void loop() {
  
  unsigned long currentMillis = millis();
//  Serial.println(currentMillis);
  
  // SENSORS
  distanceOne = ultrasonicOne.Ranging(CM);
  distanceTwo = ultrasonicTwo.Ranging(CM);
  distanceThree = ultrasonicThree.Ranging(CM);
  distanceFour = ultrasonicFour.Ranging(CM);
//  distanceFive = ultrasonicFive.Ranging(CM);

  int j;
  int minIndex = array.getMinIndex();
  int minNum = array.getMin();
  int disAverage = array.getAverage();

  Serial.println("==================");
  Serial.println(disAverage);
  Serial.println(maxAverage);
  Serial.println("==================");
  

  if(disAverage > 120){
    // switch modes
    // loop through and dop fun things like no one is watching
    // low yourself from the ceiling
    int j;
   
    // Draw 5 pixels centered on pos.  setPixelColor() will clip any
    // pixels off the ends of the strip, we don't need to watch for that.
    strip.setPixelColor(pos2 - 3, strip.Color(defaultColor[0], 0, 0)); // Dark red
    strip.setPixelColor(pos2 - 2, strip.Color(defaultColor[1], 0, 0)); // Dark red
    strip.setPixelColor(pos2 - 1, strip.Color(defaultColor[2], 0, 0)); // Medium red
    strip.setPixelColor(pos2    , strip.Color(defaultColor[3], 0 , 0)); // Center pixel is brightest
    strip.setPixelColor(pos2 + 1, strip.Color(defaultColor[2], 0, 0)); // Medium red
    strip.setPixelColor(pos2 + 2, strip.Color(defaultColor[1], 0, 0)); // Dark red
    strip.setPixelColor(pos2 + 3, strip.Color(defaultColor[0], 0, 0)); // Dark red
    
    strip.show();
//    delay(30);
    
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

//  #TODO: put this into a function as well as the thing below
// when the distance gets too large, should either fade out or reset by 
// going to position one and than do default mode stuff    
    sensorArrayValue[0] = checkDistance(distanceOne);
    sensorArrayValue[1] = checkDistance(distanceTwo);
    sensorArrayValue[2] = checkDistance(distanceThree);
    sensorArrayValue[3] = checkDistance(distanceFour);
    
    return;
  } else {
  
  
  //  Serial.println();
  //  Serial.println(currentColor[3]);
  //  Serial.println();
    
    //  Waiting for the sonars to start to work
    if (minNum == 0){
      Serial.println("Waiting for the sonars to start detecting");
    } else if (minNum > 0 && minNum < 20){
      // roll up the motors
      
      // fade the lights
      // TODO: put this in a function
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
      
    } else if (minNum > 19) {
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
      
    }  
  
    strip.setPixelColor(pos - 3, strip.Color(currentColor[0], 0, 0)); // Dark red
    strip.setPixelColor(pos - 2, strip.Color(currentColor[1], 0, 0)); // Dark red
    strip.setPixelColor(pos - 1, strip.Color(currentColor[2], 0, 0)); // Medium red
    strip.setPixelColor(pos    , strip.Color(currentColor[3], 0 , 0)); // Center pixel is brightest
    strip.setPixelColor(pos + 1, strip.Color(currentColor[2], 0, 0)); // Medium red
    strip.setPixelColor(pos + 2, strip.Color(currentColor[1], 0, 0)); // Dark red
    strip.setPixelColor(pos + 3, strip.Color(currentColor[0], 0, 0)); // Dark red
   
    strip.show();
    //  delay(30);
   
    // Rather than being sneaky and erasing just the tail pixel,
    // it's easier to erase it all and draw a new one next time.
    for(j=-3; j<= 3; j++) { 
      strip.setPixelColor(pos+j, 0);
    }
  
    currentAverage = array.getAverage();
    if((abs(currentAverage - previousAverage) >= averageDifferenceLow) || (abs(currentAverage - previousAverage) <= averageDifferenceHigh)) {
      previousAverage = currentAverage;
      
      if(minIndex == 0){
        lowBase = 3;
      } else if (minIndex == 1){
        lowBase = 10;
      } else if(minIndex == 2) {
        lowBase = 19;
      } else if(minIndex == 3) {
        lowBase = 26;
      } else {
        lowBase = 26;
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
      
      Serial. print("d1: ");
      Serial.print(checkDistance(distanceOne));
      Serial.print(", ");
      Serial.print("d2: ");
      Serial.print(checkDistance(distanceTwo));
      Serial.print(", ");
      Serial.print("d3: ");
      Serial.print(checkDistance(distanceThree));
      Serial.print(", ");
      Serial.print("d4: ");
      Serial.println(checkDistance(distanceFour));
      
      sensorArrayValue[0] = checkDistance(distanceOne);
      sensorArrayValue[1] = checkDistance(distanceTwo);
      sensorArrayValue[2] = checkDistance(distanceThree);
      sensorArrayValue[3] = checkDistance(distanceFour);
  
      Array<int> array = Array<int>(sensorArrayValue, size);
      Serial.print("MIN VALUE: ");
      Serial.println(array.getMin());
      Serial.print("MIN INDEX: ");
      Serial.println(array.getMinIndex());
      Serial.print("AVERAGE: ");
      Serial.println(array.getAverage());
  
  // PIXEL FOLLOW SONAR
      currentAverage = array.getAverage();
  
  //    if(abs(currentAverage - previousAverage) >= averageDifference) {
  //      previousAverage = currentAverage;
  //      Serial.println("PREVIOUS");
  //      Serial.println(abs(currentAverage - previousAverage));
  //    } else {
  //      Serial.println("========PREVIOUS=======");
  //      Serial.println(abs(currentAverage - previousAverage));
  //      return;
  //    }
  //
  //    mapSonarPosition(array.getMinIndex(), 50);
  
  //    fade(10);
      
  //    Serial.print(", ");
  //    Serial.print("d5: ");
  //    Serial.println(distanceFive);
      
      }
  
  //  Serial.println("Hello, world!");
  //  
  //  if (Serial.available()) { 
  //    val = Serial.read(); // read it and store it in val
  //  
  //    if (val == '1') {
  //      // digitalWrite(ledPin, HIGH); // turn the LED on
  //      ledState = !ledState; //flip the ledState
  //      digitalWrite(ledPin, ledState);
  //    }
  //    delay(10); // Wait 10 milliseconds for next reading
  //  } else {
  //    Serial.println("Hello, world!"); //send back a hello world
  //    delay(50);
  //  }

  }
}

int checkDistance(int distance) {
  if(distance > 400){
    return 400;
  } else {
    return distance;
  }
}

void establishContact() {
  while (Serial.available() <= 0) {
    Serial.println("A");   // send a capital A
//    delay(300);
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

void mapSonarPosition(int quator, uint8_t wait) {
  Serial.print("============");
  Serial.print(quator);
  Serial.println("------------");
  
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    if(i >= (quator * 7) && i <= (quator * 7 + 7)){
      strip.setPixelColor(i, strip.Color(127, 127, 127));
      strip.show();
//      delay(wait);
    } else {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
      strip.show();
    }

  }
    
//    strip.setPixelColor(, strip.Color(127, 127, 127));
//    strip.show();
//    delay(wait);
//  for(uint16_t i=0; i<strip.numPixels(); i++) {
//    strip.setPixelColor(i, c);
//    strip.show();
//    delay(wait);
//  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
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
    delay(wait);
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


//  Original hex values 
// ========================
//  strip.setPixelColor(pos - 3, 0x100000); // Dark red
//  strip.setPixelColor(pos - 2, 0x800000); // Dark red
//  strip.setPixelColor(pos - 1, 0xF00000); // Medium red
//  strip.setPixelColor(pos    , 0xFF0000); // Center pixel is brightest
//  strip.setPixelColor(pos + 1, 0xF00000); // Medium red
//  strip.setPixelColor(pos + 2, 0x800000); // Dark red
//  strip.setPixelColor(pos + 3, 0x100000); // Dark red

