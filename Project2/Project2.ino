#include <Arduino.h>
#include <DHT.h>
#include "NewPing.h"
#include <ESP32Servo.h>
#include "pitches.h"

// ----------- Define Pins -----------
#define DHT_PIN   35
#define DHT_TYPE DHT11
DHT dht11(DHT_PIN, DHT_TYPE);

// Ultrasonic
#define TRIG_PIN 4
#define ECHO_PIN 5
#define MAX_DISTANCE 400
NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE);

// Other devices
#define LED_PIN 1
#define BUZZER_PIN 15
#define SERVO_PIN 7
Servo servo;

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // initialize the DHT11 sensor
  dht11.begin();

  // attach servo pin
  servo.attach(SERVO_PIN);

  // config pin modes
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
}

void loop() {

  // read humidity & temperature
  float humi  = dht11.readHumidity();
  float tempC = dht11.readTemperature();

  // read distance from ultrasonic
  int distance = sonar.ping_cm();

  // check whether the reading is successful
  if (isnan(tempC) || isnan(humi)) {
    Serial.println("Failed to read from DHT11 sensor!");
    return;
  }

  // -------- SAFE CONDITION --------
  if (tempC <= 30) {

    // servo motor stop
    servo.write(90);
    //tone(BUZZER_PIN, NOTE_G5, 500);
    // Serial Monitor show Safe
    Serial.println("SAFE");
    Serial.println(tempC);
    Serial.println(humi);
    // buzzer no sound
    noTone(BUZZER_PIN);

    digitalWrite(LED_PIN, LOW);
  }

  // -------- CAUTION CONDITION --------
  else if (humi >= 30) {

    // servo motor stop
    servo.write(90);

    // Serial Monitor show Caution
    Serial.println("CAUTION");
    Serial.println(tempC);
    Serial.println(humi);

    // buzzer continuously alarm
    tone(BUZZER_PIN,NOTE_G5, 1000);
    delay(1000);
    noTone(BUZZER_PIN);
    // report heat source distance
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    digitalWrite(LED_PIN, HIGH);
  }

  // -------- DANGEROUS CONDITION --------
  else if (humi < 30) {

    // servo motor run
    servo.write(0);
    delay(500);
    servo.write(180);
    delay(500);

    // Serial Monitor show Dangerous
    Serial.println("DANGEROUS");
    Serial.println(tempC);
    Serial.println(humi);
    // buzzer beep
    tone(BUZZER_PIN,NOTE_E5, 1000);
    delay(1000);
    noTone(BUZZER_PIN);
    digitalWrite(LED_PIN, HIGH);
  }

  // wait 2 seconds between readings
  delay(2000);
}
