// University of Warwick School of Engineering
// ES2C6: Electro-mechanical System Design
// 2021/2022 Term 1
//
// Robot Hand Project (Systems stream)
// Project Instructor: James Atkinson
//
// Getting Started sample code
//
// This code let's you control the direction of robot gripper (either tightening or loosening)
// based on feedback from a force sensor.
//
// This code was originally created by Dr Adam Noel

// Initialise variables
const int pwm = 3 ;       // digital pin 3 will be used for PWM (Pulse Width Modulation) output
const int dir = 8 ;       // digital pin 8 will be used for high/low output
const int forcePin = A0;  // analogue pin A0 will be used to read the voltage from the force sensor
const int ledPin = 13;    // digital pin 13 is also wired to a built-in LED
int forceValue = 0;       // value read from force pin
uint8_t motorSpeed = 40;  // 8 bit unsigned integer (0-255) defining motor speed
bool bHigh = false;       // boolean tracking direction
bool bChangeDir = false;  // boolean tracking whether we need to switch directions

const int trigPin = 11;
const int echoPin = 12;

long duration; // variable for the duration of sound wave travel
int distance; // variable for the distance measurement


/*
   NOTES
   bHigh True means loosening (open)
   bHigh False means tightening (close)
    
   During early stages of testing, motorspeeds > 55 could not open a 
   fully tightened grip, without physical assistance.
   Why? High Speed and Low Torque
   Lower speed produces higher torque to overcome the 'holding torque'
   OR it happens because it shuts too tightly?
   I tested again with low speed and it still didn't open without assisstance
   Probably because it shuts too tightly. Maybe. Motor jammed.

*/



// Setup function (runs when Arduino is powered on or reset
void setup()
{
  pinMode(pwm, OUTPUT) ;    //we have to set PWM pin as output
  pinMode(dir, OUTPUT) ;    // Direction pin pins are also set as output
  pinMode(ledPin, OUTPUT);  // LED pin is an output

  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT
  Serial.begin(9600); 
}

// Loop function runs repeatedly after the setup function
void loop()
{

  // ULTRASONIC SENSOR CODE ------------------------
  
  // Clears the trigPin condition
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
  // Displays the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // END OF USSC ----------------------------------

  
  // Read voltage from output of force sensor
  forceValue = analogRead(forcePin);

  if (forceValue > 60)
  { // There is a high force applied to force sensor
    // Loosen grip
    if (bHigh) {
      // we were just loosening
      bChangeDir = false;
    }
    else {
      // we were just tightening and need to change direction
      bChangeDir = true;
    }
    bHigh = true;
  } else
  { // There is a low force applied to force sensor
    // Tighten grip
    if (bHigh) {
      // we were just loosening and need to change direction
      bChangeDir = true;
    }
    else {
      // we were just tightening
      bChangeDir = false;
    }
    bHigh = false;
  }

  // Stop motor for a brief delay because we need to change directions
  if (bChangeDir)
  {
    analogWrite(pwm, 0);
    delay(250); // delay is in milliseconds
  }

  // Set motor direction
  if (bHigh)
  {
    // Loosen the grip and turn on LED
    digitalWrite(ledPin, HIGH);
    digitalWrite(dir, LOW);
  } else
  {
    // Tighten the grip and turn off LED
    digitalWrite(ledPin, LOW);
    digitalWrite(dir, HIGH);
  }

  // Resume motor motion
  analogWrite(pwm, motorSpeed);
}
