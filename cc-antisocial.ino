#include <Ultrasonic.h>
#include <Array.h>

// Init an Ultrasonic object
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



const long interval = 500;
unsigned long previousMillis = 0;

char val; // Data received from the serial port
int ledPin = 13; // Set the pin to digital I/O 13
boolean ledState = LOW; //to toggle our LED


void setup() {
  //initialize serial communications at a 9600 baud rate
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT); // Set pin as OUTPUT
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

