#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <NewPing.h>

// OLED size
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// OLED object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Ultrasonic pins
#define TRIG_PIN 5     // ESP32 GPIO for Trig
#define ECHO_PIN 18    // ESP32 GPIO for Echo 
#define MAX_DISTANCE 400 // sensor max distance

// Create ultrasonic object
NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE);

// Buzzer + LED
const int buzzer = 23;  // ESP32 GPIO
const int ledPin = 2;  // ESP32 GPIO

// Safety distance
int safetyDistance = 50; // cm

void setup() {
  Serial.begin(115200);

  pinMode(buzzer, OUTPUT);
  pinMode(ledPin, OUTPUT);

  // OLED init
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
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
  // Ping the sensor
  delay(100); // small gap between pings
  unsigned int uS = sonar.ping();

  int distance;
  if (uS == 0) {
    distance = -1; // No echo
  } else {
    distance = uS / US_ROUNDTRIP_CM;
  }

  // Show distance
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print("Dist:");

  if (distance == -1) {
    display.println(" ---");
    Serial.println("No Echo");
  } else {
    display.print(distance);
    display.println("cm");
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
  }

  // Check safety
  if (distance != -1 && distance <= safetyDistance) {
    display.setTextSize(2);
    display.setCursor(0, 30);
    display.println("ALERT!");
    display.display();

    digitalWrite(buzzer, HIGH);
    digitalWrite(ledPin, HIGH);
    delay(5000); // Alert ON for 5s
    digitalWrite(buzzer, LOW);
    digitalWrite(ledPin, LOW);
  } else {
    display.setTextSize(2);
    display.setCursor(0, 30);
    display.println("Safe");
    display.display();
  }
}
