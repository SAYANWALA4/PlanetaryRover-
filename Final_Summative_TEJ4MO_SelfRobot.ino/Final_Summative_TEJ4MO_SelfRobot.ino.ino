#include <Servo.h>

Servo servo1;
Servo servo2;

int servo1Pin = 9;  // Connect servo 1 to digital pin 9
int servo2Pin = 10; // Connect servo 2 to digital pin 10

const int dataPin = 7;    // Connect the data pin of the sensor to digital pin 4
const int dataRight = 8;


const int FRONT_TRIGGER_PIN = 12;
const int FRONT_ECHO_PIN = 11;

// lowest and highest flame sensor readings:
const int sensorMin = 0;     //  sensor minimum
const int sensorMax = 1024;  // sensor maximum

const int ledPin = A1;
const int ledPin2 = A2;
const int ledPin3 = A3;
const int ldrPin = A4; 

const int motor1Pin1 = 2;
const int motor1Pin2 = 3;
const int motor2Pin1 = 4;
const int motor2Pin2 = 5;

void setup() {
  pinMode(FRONT_TRIGGER_PIN, OUTPUT);
  pinMode(FRONT_ECHO_PIN, INPUT);

  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);

  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);

  servo1.attach(servo1Pin);
  servo2.attach(servo2Pin);

  pinMode(dataPin, INPUT);
  pinMode(ledPin, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  long frontDistance = measureDistance(FRONT_TRIGGER_PIN, FRONT_ECHO_PIN); // Measure distance in front of the car
  

  // Check for obstacles and take appropriate actions
  if (frontDistance >= 50 || frontDistance == 0) {
    // Obstacle detected in front
    long leftSide =  detectObstacle();
    long rightSide = detectObstacleR();
     if (leftSide == 1){
       turnRight();
       delay(100);
       stopMotors();
       delay(17);
       moveForward();
       delay(200);
     }

    else if (rightSide == 1){
       turnLeft();
       delay(100);
       stopMotors();
       delay(17);
       moveForward();
       delay(200);
     }

    else {
      moveForward();
    }
  } 

  else {
    // obstacle or ledge detected, Mission:AVOID
    lightDetect();
    avoidObstacle(frontDistance);
  }

}

int detectObstacle() {
  int obstacleDetected = digitalRead(dataPin);

  if (obstacleDetected == LOW) {
    // Obstacle detected
    Serial.println("Obstacle detected!");
    return 1;
  } 
  else {
    // No obstacle
    Serial.println("No obstacle");
    return 2;
  }
}

int detectObstacleR() {
  int obstacleDetected = digitalRead(dataRight);

  if (obstacleDetected == LOW) {
    // Obstacle detected
    Serial.println("Obstacle detected!");
    return 1;
  } 
  else {
    // No obstacle
    Serial.println("No obstacle");
    return 2;
  }
}


long measureDistance(int triggerPin, int echoPin) {
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);

  unsigned long duration = pulseIn(echoPin, HIGH);

  // Check if the duration is within a reasonable range
  if (duration > 0 && duration < 30000) {
    long distance = duration * 0.034 / 2;
    Serial.print("Distance is: ");
    Serial.println(distance);
    return distance;
  } else {
    Serial.println("Invalid distance reading, retrying...");
    return 51;
  }
}


void lightDetect(){
  int ldrValue = analogRead(ldrPin);
  int threshold = 500;

  if (ldrValue < threshold) {
    // It's dark, turn on the LED
    digitalWrite(ledPin3, HIGH);
    Serial.println("It is dark");
  } else {
    // It's daylight, turn off the LED
    digitalWrite(ledPin3, LOW);
  }
}

void flameDetect(){
  // read the sensor on analog A0:
	int sensorReading  = analogRead(A0);
  // map the sensor range (four options):
  // ex: 'long  int map(long int, long int, long int, long int, long int)'
	int range = map(sensorReading,  sensorMin, sensorMax, 0, 3);
  
  // range value:
  switch (range) {
  case 0:    // A fire closer than 1.5 feet away.
    Serial.println("** Close  Fire **");
    digitalWrite(ledPin, HIGH);
    digitalWrite(ledPin2, LOW); 
    break;
  case 1:    // A fire between 1-3 feet away.
    Serial.println("**  Distant Fire **");
    digitalWrite(ledPin, HIGH);
    digitalWrite(ledPin2, LOW); 
    break;
  case 2:    // No fire detected.
    Serial.println("No  Fire");
    digitalWrite(ledPin2, HIGH); 
    digitalWrite(ledPin, LOW); 
    break;
  }
  delay(1);  // delay between reads
}

void headServo(){
    // Move the servo to position 0 degrees
  servo1.write(0);
  delay(1000);

  // Move the servo to position 90 degrees
  servo1.write(90);
  delay(1000);

  // Move the servo to position 180 degrees
  servo1.write(180);
  delay(1000);

  servo1.write(90);
  delay(1000);
}

void handServo(){
      // Move the servo to position 0 degrees
  servo2.write(0);
  delay(1000);

  // Move the servo to position 90 degrees
  servo2.write(90);
  flameDetect();
  delay(1000);

  // Move the servo to position 180 degrees
  servo2.write(0);
  delay(1000);
}

void moveForward() {
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW);
  digitalWrite(motor2Pin1, HIGH);
  digitalWrite(motor2Pin2, LOW);
}

void avoidObstacle(long frontDistance) {
  stopMotors();
  delay(1000);
  handServo();
  delay(300);
  reverse();
  delay(200);
  turnRight();
  delay(150);
  stopMotors();
  headServo();
  stopMotors();
  frontDistance = measureDistance(FRONT_TRIGGER_PIN, FRONT_ECHO_PIN);
  delay(100);

   if (frontDistance<=49){
      avoidObstacle(frontDistance);
    }
}


void reverse() {
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, HIGH);
  digitalWrite(motor2Pin1, LOW);
  digitalWrite(motor2Pin2, HIGH);
}

void turnLeft() {
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, HIGH);
  digitalWrite(motor2Pin1, HIGH);
  digitalWrite(motor2Pin2, LOW);
}

void turnRight() {
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW);
  digitalWrite(motor2Pin1, LOW);
  digitalWrite(motor2Pin2, HIGH);
}

void stopMotors() {
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, LOW);
  digitalWrite(motor2Pin1, LOW);
  digitalWrite(motor2Pin2, LOW);
}