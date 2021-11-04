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
uint8_t motorSpeed = 30;  // 8 bit unsigned integer (0-255) defining motor speed
bool bHigh = false;       // boolean tracking direction
bool bChangeDir = false;  // boolean tracking whether we need to switch directions

const int trigPin = 11;
const int echoPin = 12;

long duration; // variable for the duration of sound wave travel
int distance; // variable for the distance measurement

const int sBuffer = 16;
int sByte;
int sCount;
char command[sBuffer] = "wait"; //Starting command, also to initialize command string
char sString[sBuffer]; //Starting command, also to initialize command string

bool isCalibrated = false;

const int minPos = 0;
int maxPos = 0;
//Just initial value. Based on motorSpeed 40 (overestimate; > 1600). Should be adjustable in UI

int posCounter = 0;


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
   
   Time delays from ultrasonic sensors interfere with calibration timing (12ms*f(mainloop,calibrationWhileLoops))

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
  //  digitalWrite(trigPin, LOW);
  //  delayMicroseconds(2);
  //  digitalWrite(trigPin, HIGH);       // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  //  delayMicroseconds(10);
  //  digitalWrite(trigPin, LOW);
  //  duration = pulseIn(echoPin, HIGH); // Reads echoPin, returns sound wave travel time(microseconds)
  //  distance = duration * 0.034 / 2;   // Speed of sound wave divided by 2 (go and back)
  //  Serial.print("Distance: ");
  //  Serial.print(distance);
  //  Serial.println(" cm");


  // SERIAL STUFF ---------------------------------

  setSerialCommand();


  // MOTOR CONTROL CODE -------------------------------

  // Read voltage from output of force sensor
  forceValue = analogRead(forcePin);

  if (forceValue > 60 )
  {
    if (strcmp(command, "grip") == 0) {
      grip();
    }
    else if (strcmp(command, "ungrip") == 0) {
      ungrip();
    }
    else if (strcmp(command, "wait") == 0) {
      wait();
    }
    else if (strcmp(command, "calibrate") == 0) {
      calibrate();
    }
    else {
      Serial.println("Command unrecognized");
    }
  }
  else if (strcmp(command, "calibrate") == 0) {
    calibrate();
  }
  else {
    analogWrite(pwm, 0);
  }

  // END OF MCC --------------------------------------

}





int grip() {
  //  Serial.println("GRIPPING.....");
  if (bHigh) {
    // we were just loosening and need to change direction
    bChangeDir = true;
  }
  else {
    // we were just tightening
    bChangeDir = false;
  }
  bHigh = false;

  if (bChangeDir)
  {
    analogWrite(pwm, 0);
    delay(250); // delay is in milliseconds
  }

  digitalWrite(ledPin, LOW);
  digitalWrite(dir, HIGH);

  if (isCalibrated && posCounter <= minPos) {
    return -1;
  }
  posCounter--;
  Serial.println(posCounter);
  analogWrite(pwm, motorSpeed);
  return 1;
}


int ungrip() {
  //  Serial.println("UNGRIPPING.....");
  if (bHigh) {
    // we were just loosening
    bChangeDir = false;
  }
  else {
    // we were just tightening and need to change direction
    bChangeDir = true;
  }
  bHigh = true;

  if (bChangeDir)
  {
    analogWrite(pwm, 0);
    delay(250); // delay is in milliseconds
  }

  digitalWrite(ledPin, HIGH);
  digitalWrite(dir, LOW);

  if (isCalibrated && posCounter >= maxPos) {
    return -1;
  }

  posCounter++;
  Serial.println(posCounter);
  analogWrite(pwm, motorSpeed);
  return 1;
}


void wait() {
  Serial.println("---------- Waiting For Command ----------");
  analogWrite(pwm, 0);
}



void setSerialCommand() {
  while (Serial.available() > 0) {
    sByte = Serial.read();

    if (sByte == '\n') {
      command[0] = '\0';

      for (int i = 0; i <= sCount; i++) {
        command[i] = sString[i];
      }
      for (int i = sCount; i < sBuffer; i++) {
        command[i] = '\0';
      }

      Serial.print("Command: ");
      Serial.println(command);

      sCount = 0;
      sString[0] = '\0';
      break;
    }

    sString[sCount] = sByte;
    sCount++;

  }
}



void calibrate() {
  isCalibrated = false;
  Serial.println("Calibrating ---- Type 'start' to continue");
  while (strcmp(command, "start") != 0) {
    setSerialCommand();
  }

  Serial.println("Move hand to START POSITION using commands 'grip' and 'ungrip' THEN type 'next'");

  while (strcmp(command, "next") != 0) {
    setSerialCommand();
    forceValue = analogRead(forcePin);
    if (forceValue > 60 )
    {
      if (strcmp(command, "grip") == 0) {
        grip();
      }
      else if (strcmp(command, "ungrip") == 0) {
        ungrip();
      }
      else {
        Serial.println("Move hand to START POSITION using commands 'grip' and 'ungrip' THEN type 'next'");
      }
    } else {
      analogWrite(pwm, 0);
    }
  }

  posCounter = 0;

  Serial.println("Move hand to END POSITION using commands 'grip' and 'ungrip' THEN type 'finish'");

  while (strcmp(command, "finish") != 0) {
    setSerialCommand();
    forceValue = analogRead(forcePin);
    if (forceValue > 60 )
    {
      if (strcmp(command, "grip") == 0) {
        grip();
      }
      else if (strcmp(command, "ungrip") == 0) {
        ungrip();
      }
      else {
        Serial.println("Move hand to END POSITION using commands 'grip' and 'ungrip' THEN type 'finish'");
      }
    } else {
      analogWrite(pwm, 0);
    }
  }

  maxPos = posCounter;
  isCalibrated = true;

  Serial.println("********* FINISHED CALIBRATING *************'");

  Serial.println("Minimum: 0");
  Serial.print("Maximum: ");
  Serial.println(maxPos);



  command[0] = '\0';
  char s[] = "wait";

  for (int i = 0; i < sizeof(s); i++) {
    command[i] = s[i];
  }
  for (int i = sizeof(s); i < sBuffer; i++) {
    command[i] = '\0';
  }

}


//
//void copystr(char*){
//
//  }
//
//
