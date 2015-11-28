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


// Init an Ultrasonic object
// ===========================
// TODO: Put these inits and declarations into an array
Ultrasonic ultrasonicOne(3, 2);
Ultrasonic ultrasonicTwo(5, 4);
Ultrasonic ultrasonicThree(7, 6);
Ultrasonic ultrasonicFour(9, 8);
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

  // Some example procedures showing how to display to the pixels:
  colorWipe(strip.Color(255, 0, 0), 50); // Red
  colorWipe(strip.Color(0, 255, 0), 50); // Green
  colorWipe(strip.Color(0, 0, 255), 50); // Blue

  unsigned long currentMillis = millis();

//  Serial.println(currentMillis);
  
  // SENSORS
  distanceOne = ultrasonicOne.Ranging(CM);
  distanceTwo = ultrasonicTwo.Ranging(CM);
  distanceThree = ultrasonicThree.Ranging(CM);
  distanceFour = ultrasonicFour.Ranging(CM);
//  distanceFive = ultrasonicFive.Ranging(CM);

  if(currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;  
    
    Serial.print("d1: ");
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
