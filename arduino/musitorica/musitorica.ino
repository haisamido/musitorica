/**
 * @file musitorica.ino
 * @author Haisam Ido (haisam.ido@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-04-23
 * https://wokwi.com/projects/395360264870700033
 * @copyright Copyright (c) 2024
 * 
 */

#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);  // Replace 0x27 with your LCD I2C address if different

#include <Adafruit_NeoPixel.h>

#include <Keypad.h>
#include "icons.h"

/* Locking mechanism definitions */
#define SERVO_PIN        6
#define SERVO_LOCK_POS   20
#define SERVO_UNLOCK_POS 90

/* Keypad setup */
const byte KEYPAD_ROWS    = 4;
const byte KEYPAD_COLS    = 4;
byte rowPins[KEYPAD_ROWS] = {5, 4, 3, 2};
byte colPins[KEYPAD_COLS] = {A3, A2, A1, A0};
char keys[KEYPAD_ROWS][KEYPAD_COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, KEYPAD_ROWS, KEYPAD_COLS);

#define SPEAKER_PIN 13

// input
const uint8_t pitch_class_set[] = {0,1,3,5};

int number_of_pitches_in_pitch_class = sizeof(pitch_class_set);

// defaults
const float frequency_reference = 330.; // reference frequency, i.e. 0
const int number_of_pitches     = 12;   // equally tempered (e.g. 12 equal temperament)

// NeoPixel : Bracelet
#define PIXEL_COUNT number_of_pitches
#define PIXEL_PIN 6
Adafruit_NeoPixel pitch_bracelet(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

/* source: https://gist.github.com/abachman/5257b1700cc77a6ebabdff1aeefb3376 */
String join(uint8_t vals[], char sep, int items) {
  String out = "";
  
  for (int i=0; i<items; i++) {
    out = out + String(vals[i]);
    if ((i + 1) < items) {
      out = out + sep;
    }
  }
  return out;
}

void showStartupMessage() {
  lcd.setCursor(0, 0);
  String message = "   ... MAQAMAT ...";
  
  for (byte i = 0; i < message.length(); i++) {
    lcd.print(message[i]);
    delay(100);
  }
  
  delay(1000);

  lcd.setCursor(0, 1);
  lcd.print("Mode: 1,");
  lcd.setCursor(0, 2);

  String pitch_class_set_string = "{ " + join(pitch_class_set, ', ', number_of_pitches_in_pitch_class) + " }";
  lcd.print(pitch_class_set_string);

  lcd.setCursor(0, 3);

  delay(500);
}

void pitch_bracelet_startup() {
  pitch_bracelet.begin();
  pitch_bracelet.show();
}

void setup() {
  Serial.begin(9600); // open the serial port at 9600 bps:

  lcd.begin(16, 2);

  lcd.init();
  lcd.backlight();  // Optional: turn on backlight\

  delay(10);
  lcd.setCursor(4, 0);

  init_icons(lcd);

  pinMode(SPEAKER_PIN, OUTPUT);

  pitch_bracelet_startup();
  showStartupMessage();

}

/* source: https://github.com/haisamido/maqamat/blob/main/maqamat.py#L62 */ 
float frequency_from_cents(float f1 , float cents, float cents_per_octave){
  return f1*pow(2,cents/cents_per_octave);
}

void loop() {

  float frequency;
  float cents;
  
  int pitch_class_step;

  for ( int i = 0; i < number_of_pitches_in_pitch_class; i++) {

    cents            = i*100.0;
    pitch_class_step = pitch_class_set[i];

    // turn on LED on neopixel for the pitch_class_step in question
    pitch_bracelet.setPixelColor(pitch_class_step, 0, 255, 0);
    pitch_bracelet.show();

    Serial.print(pitch_class_step);
    Serial.print("\t");

    /* source: https://github.com/haisamido/maqamat/blob/main/maqamat.py#L62 */
    frequency = frequency_from_cents(frequency_reference, cents, 1200);

//    lcd.print(frequency);
    Serial.print(frequency);
    Serial.println();

    tone(SPEAKER_PIN, frequency);
    delay(500);
    noTone(SPEAKER_PIN);

  }

  Serial.println();
}
