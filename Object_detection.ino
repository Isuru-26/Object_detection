#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED display size
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Create OLED display object (I2C address 0x3C is common)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Pin numbers
const int trigPin = 9;
const int echoPin = 10;
const int buzzer = 11;
const int ledPin = 13;

// Variables
long duration;
int distance;
int safetyDistance = 20; // cm

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(ledPin, OUTPUT);

  // Serial Monitor
  Serial.begin(9600);

  // OLED init
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Stop if OLED not found
  }
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 25);
  display.println("Starting...");
  display.display();
  delay(2000);
  display.clearDisplay();
}

void loop() {
  // Trigger ultrasonic
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Measure echo with timeout (30ms ≈ 5 meters)
  duration = pulseIn(echoPin, HIGH, 30000);

  // Convert to distance
  if (duration == 0) {
    distance = -1; // no echo
  } else {
    distance = duration * 0.034 / 2; // cm
  }

  // Clear OLED
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);

  if (distance > 0 && distance < 400) {
    // Print distance
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    display.print("Dist:");
    display.print(distance);
    display.println("cm");

    // Safety check
    if (distance <= safetyDistance) {
      display.setCursor(0, 30);
      display.println("ALERT!");
      display.display();

      digitalWrite(buzzer, HIGH);
      digitalWrite(ledPin, HIGH);
      delay(5000); // buzzer + LED on for 5s
      digitalWrite(buzzer, LOW);
      digitalWrite(ledPin, LOW);
    } else {
      display.setCursor(0, 30);
      display.println("Safe");
      display.display();
    }
  } else {
    // Out of range or no echo
    Serial.println("No echo / Out of range");
    display.print("No Echo");
    display.display();
  }

  delay(300); // wait before next measurement
}
