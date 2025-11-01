/*
  Tilt-or-Treat: The Motion-Activated Halloween Candy Game
  --------------------------------------------------------
  Author: Raoul Mendanha
  Date: October 2025

  Description:
  This Halloween-themed Arduino project detects tilt using an ADXL345 accelerometer.
  When you tilt left or right, the system randomly decides whether it’s a “Trick” or “Treat”:
  - Treat: Plays a happy tone and blinks LEDs.
  - Trick: Spins the motor with a mounted spider and flashes the LEDs chaotically!

  GitHub Repository: https://github.com/raoulmendanha/TiltorTreat
*/

#include <Wire.h>
#include <Servo.h>
#include <LiquidCrystal.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

// --- Pin Definitions ---
const int LCD_RS = 12;
const int LCD_E  = 11;
const int LCD_D4 = 5;
const int LCD_D5 = 4;
const int LCD_D6 = 3;
const int LCD_D7 = 2;

const int SERVO_PIN = 9;
const int BUZZER_PIN = 8;

// LEDs: main + 3 extras
const int LED_PIN_MAIN = 10;
const int LED_PIN_2 = 6;
const int LED_PIN_3 = 7;
const int LED_PIN_4 = 13;

// --- Global Objects ---
LiquidCrystal lcd(LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
Servo myServo;
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

// --- Global Variables ---
bool isArmed = false;
const float TILT_THRESHOLD = 5.0;

// ===================================================================
// SETUP
// ===================================================================
void setup() {
  pinMode(LED_PIN_MAIN, OUTPUT);
  pinMode(LED_PIN_2, OUTPUT);
  pinMode(LED_PIN_3, OUTPUT);
  pinMode(LED_PIN_4, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  myServo.attach(SERVO_PIN);
  lcd.begin(16, 2);

  if(!accel.begin()) {
    lcd.print("Sensor Error!");
    while(1);
  }
  accel.setRange(ADXL345_RANGE_4_G);

  randomSeed(analogRead(A0)); // Seed randomness
  resetGame();
}

// ===================================================================
// LOOP
// ===================================================================
void loop() {
  if (isArmed) {
    sensors_event_t event;
    accel.getEvent(&event);
    float x = event.acceleration.x;

    if (x > TILT_THRESHOLD || x < -TILT_THRESHOLD) {
      isArmed = false; // Disarm
      int choice = random(2); // 0 = Trick, 1 = Treat

      if (choice == 0) {
        runTrickSequence();
      } else {
        runTreatSequence();
      }
    }
  }
}

// ===================================================================
// RESET FUNCTION
// ===================================================================
void resetGame() {
  isArmed = true;
  myServo.write(90); // Stop servo
  noTone(BUZZER_PIN);
  digitalWrite(LED_PIN_MAIN, LOW);
  digitalWrite(LED_PIN_2, LOW);
  digitalWrite(LED_PIN_3, LOW);
  digitalWrite(LED_PIN_4, LOW);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Tilt to Choose!");
  lcd.setCursor(0, 1);
  lcd.print("Left or Right?");
}

// ===================================================================
// TREAT SEQUENCE
// ===================================================================
void runTreatSequence() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Treat:");
  lcd.setCursor(0, 1);
  lcd.print("Take one treat...");
  delay(1500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("before ghosts");
  lcd.setCursor(0, 1);
  lcd.print("take you!");

  // --- Happy tone ---
  tone(BUZZER_PIN, 523, 150);
  delay(150);
  tone(BUZZER_PIN, 659, 150);
  delay(150);
  tone(BUZZER_PIN, 784, 150);
  delay(200);
  noTone(BUZZER_PIN);

  // --- Gentle random LED blinking ---
  long start = millis();
  while (millis() - start < 4000) {
    digitalWrite(LED_PIN_MAIN, random(2));
    digitalWrite(LED_PIN_2, random(2));
    digitalWrite(LED_PIN_3, random(2));
    digitalWrite(LED_PIN_4, random(2));
    delay(random(150, 400));
  }

  // turn off LEDs
  digitalWrite(LED_PIN_MAIN, LOW);
  digitalWrite(LED_PIN_2, LOW);
  digitalWrite(LED_PIN_3, LOW);
  digitalWrite(LED_PIN_4, LOW);

  delay(1000);
  resetGame();
}

// ===================================================================
// TRICK SEQUENCE
// ===================================================================
void runTrickSequence() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Trick:");
  lcd.setCursor(0, 1);
  lcd.print("Oh no...");
  delay(1500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("the cursed");
  lcd.setCursor(0, 1);
  lcd.print("spider awakens!");

  // Random motor direction for chaos
  int direction = random(2);
  if (direction == 0) {
    myServo.write(0);   // CW
  } else {
    myServo.write(180); // CCW
  }

  // --- Chaotic LED flashing & siren ---
  long startTime = millis();
  while (millis() - startTime < 4000) {
    // all LEDs flash rapidly
    digitalWrite(LED_PIN_MAIN, HIGH);
    digitalWrite(LED_PIN_2, HIGH);
    digitalWrite(LED_PIN_3, HIGH);
    digitalWrite(LED_PIN_4, HIGH);

    tone(BUZZER_PIN, 1000);
    delay(100);

    digitalWrite(LED_PIN_MAIN, LOW);
    digitalWrite(LED_PIN_2, LOW);
    digitalWrite(LED_PIN_3, LOW);
    digitalWrite(LED_PIN_4, LOW);

    tone(BUZZER_PIN, 400);
    delay(100);
  }

  // stop everything
  myServo.write(90);
  noTone(BUZZER_PIN);
  digitalWrite(LED_PIN_MAIN, LOW);
  digitalWrite(LED_PIN_2, LOW);
  digitalWrite(LED_PIN_3, LOW);
  digitalWrite(LED_PIN_4, LOW);

  delay(1000);
  resetGame();
}