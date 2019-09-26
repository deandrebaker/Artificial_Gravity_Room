// Includes the following libraries
#include <Servo.h>
#include <Wire.h>
#include "ADXL345lib.h"

//Creates an instance of the servo object and acc object
Servo servo;
Accelerometer acc;

// Angle away from vertical perpendicular to the floor
int angleMax = 170;
int angleMin = 75;
int angle = angleMax;

// Global variables
int servoPin = 9;
int motorPin = 11;
int potPin = 0;
int potVal = 0;
int motorSpeed = 0;
bool fail;

// Variables to store acceleration values
double x, y, z;
double g;

void setup()
{
  // Begins serial communication
  Serial.begin(9600);

  // Connects servo motor to arduino and adjusts angle
  servo.attach(servoPin);
  servo.write(angleMax);

  // Sets motorPin to output
  pinMode(motorPin, OUTPUT);

  // Stops program if accelerometer is not detected
  if (acc.begin(OSEPP_ACC_SW_ON) != 0)
  {
    Serial.println("Error connecting to accelerometer");
    fail = true;
    return;
  }
  // Sets accelerometer's sensitivity to 16 g
  acc.setSensitivity(ADXL345_RANGE_PM16G);
}

void loop()
{
  // Stops running code if accelerometer is not detected
  if (fail)
    return;

  // Inputs analog reading from potentiometer
  potVal = analogRead(potPin);

  // Maps the raw data reading to a scale from 0 to 255
  motorSpeed = map(potVal, 0, 1023, 0, 255);

  // Outputs a pwm signal to the motor based on the reading from the potentiometer
  analogWrite(motorPin, motorSpeed);

  // Reads the acceleration values and returns them in gs
  if (acc.readGs(&x, &y, &z) != 0)
  {
    Serial.println("Failed to read accelerometer");
    return;
  }

  // Calculates net acceleration using the 3 dimensional pythagorean theorem
  g = sqrt(x * x + y * y + z * z);

  // Display x, y, z, and g acceleration values
  Serial.print("X: ");
  Serial.print(x);
  Serial.print(" Y: ");
  Serial.print(y);
  Serial.print(" Z: ");
  Serial.print(z);
  Serial.print(" G: ");
  Serial.print(g);
  // Display angle of elevation
  Serial.print(" Angle: ");
  Serial.print(165 - angle);
  // Display motor speed percent
  Serial.print(" Motor Speed: ");
  Serial.print(motorSpeed / 2.55);
  Serial.print(" %\n");

  // Increases angle when the net acceleration on the x-axis is outward
  if ((x > 0.05) && (angle != angleMax))
  {
    servo.write(++angle);
  }

  // Decreases angle when the net acceleration on the x-axis is inward
  else if ((x < -0.05) && (angle != angleMin))
  {
    servo.write(--angle);
  }

  delay(10);
}
