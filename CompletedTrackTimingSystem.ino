#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Pin definitions
const int irSensorPin = A0;  // IR sensor connected to analog pin A0
const int buttonPin = 9;     // Button connected to digital pin 9
const int servoPin1 = 2;     // Servo 1 connected to digital pin 2
const int servoPin2 = 4;     // Servo 2 connected to digital pin 4

// Servo objects
Servo servo1;
Servo servo2;

// LCD setup (I2C address may vary, typically 0x27 or 0x3F)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Timing variables
unsigned long startTime;
unsigned long endTime;
unsigned long displayMillis;  // To manage display updates
float elapsedTime;            // Variable to store the elapsed time
bool raceStarted = false;
bool raceFinished = false;

// IR sensor threshold
int irThreshold = 100;  // Adjust this based on testing

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);

  // Initialize button pin
  pinMode(buttonPin, INPUT_PULLUP);  // Using internal pull-up resistor

  // Initialize servos
  servo1.attach(servoPin1);
  servo2.attach(servoPin2);
  resetServos();

  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Initializing...");

  delay(2000); // Simulate initialization time

  // Ready to race state
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ready to Race!");
}

void loop() {
  // Print IR sensor value for debugging
  int irValue = analogRead(irSensorPin);
  Serial.print("IR Sensor Value: ");
  Serial.println(irValue);

  // Check for button press to start race
  if (digitalRead(buttonPin) == LOW && !raceStarted) {
    delay(50); // Simple debounce delay
    if (digitalRead(buttonPin) == LOW) { // Check again after delay to confirm
      startRace();
    }
  }

  // Check for finish line crossing
  if (raceStarted && !raceFinished && irValue < irThreshold) {
    endRace();
  }
}

void startRace() {
  raceStarted = true;
  raceFinished = false;

  // Display race start
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Race Started!");

  // Record start time
  startTime = millis();

  // Release the cars by moving servos to the correct angles
  servo1.write(180);  // Move to 180 degrees to release car 1
  servo2.write(0);    // Move to 0 degrees to release car 2

  delay(1000); // Wait for servos to release cars

  // Reset servos to starting position after releasing
  resetServos();
}

void endRace() {
  raceFinished = true;
  endTime = millis();
  unsigned long raceTime = endTime - startTime;

  // Convert race time to seconds
  float raceTimeSeconds = raceTime / 1000.0;

  // Display race time in seconds
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Time: ");
  lcd.print(raceTimeSeconds, 3); // Display with 3 decimal places
  lcd.print(" s");

  // Debugging output
  Serial.print("Race Time: ");
  Serial.print(raceTimeSeconds);
  Serial.println(" s");

  // Wait for 5 seconds before resetting
  delay(5000);

  resetSystem();
}

void resetSystem() {
  raceStarted = false;
  raceFinished = false;

  // Clear the LCD and show "Ready to Race!"
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ready to Race!");

  // Reset servos to initial positions
  resetServos();
}

void resetServos() {
  servo1.write(90); // Return to starting position
  servo2.write(90); // Return to starting position
}

int thresholdValue() {
  // This function returns the threshold to detect the IR beam break.
  // Adjust the threshold based on your IR sensor's output during testing.
  return irThreshold; // Return the current threshold value
}
