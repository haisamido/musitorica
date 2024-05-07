/**
 * @file musitorica.ino
 * @author Haisam Ido (haisam.ido@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-04-23
 * https://wokwi.com/projects/395360264870700033
 * source: https://github.com/haisamido/musitorica
 * @copyright Copyright (c) 2024
 * 
 */

#include <LiquidCrystal_I2C.h>
#include <Adafruit_NeoPixel.h>
#include <Keypad.h>
#include "icons.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);  // Replace 0x27 with your LCD I2C address if different

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
const uint8_t pitch_class_set[] = {0,1,3,5,7,8,10};

// derived
int number_of_pitches_in_pitch_class = sizeof(pitch_class_set);

// defaults
const float frequency_reference = 330.0; // reference frequency, i.e. 0
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

// global
String pitch_class_set_string = "{" + join(pitch_class_set, ', ', number_of_pitches_in_pitch_class) + "}";

void pitch_modes(){

    float cents;
    int pitch_class_step;

    for ( int i = 1; i < number_of_pitches_in_pitch_class; i++) {
      cents            = i*100.0;
      pitch_class_step = pitch_class_set[i];
      Serial.print(pitch_class_step);
      Serial.print("\t");
    }

}

void showStartupMessage() {
  lcd.setCursor(0, 0);
  String message = " ... musitorica ...";
  
  for (byte i = 0; i < message.length(); i++) {
    lcd.print(message[i]);
    delay(100);
  }
  
  delay(2000);
  lcd.clear();
  delay(500);
  
}

void pitch_bracelet_startup() {
  pitch_bracelet.begin();
  pitch_bracelet.show();
}

void setup() {
  Serial.begin(9600); // open the serial port at 9600 bps:

  lcd.begin(0, 0);
  lcd.init();
  lcd.backlight();  // Optional: turn on backlight\

  delay(10);
  
  lcd.setCursor(0, 0);

  init_icons(lcd);

  pinMode(SPEAKER_PIN, OUTPUT);

  pitch_bracelet_startup();
  showStartupMessage();

  float cents;
  int pitch_class_step;
  int delta;

  int number_of_modes = number_of_pitches_in_pitch_class;

    for ( int i = 0; i < number_of_pitches_in_pitch_class; i++) {

      for ( int j = 0; j < number_of_pitches_in_pitch_class; j++) {
        pitch_class_step = pitch_class_set[j] - pitch_class_set[i];

        if( pitch_class_step < 0 ) {
          pitch_class_step=pitch_class_step+12;
        }
        Serial.print("(");
        Serial.print(i);
        Serial.print(",");
        Serial.print(j);
        Serial.print(")");
        Serial.print(": ");
        Serial.print(pitch_class_step);
        Serial.print("\t");
      }
    Serial.println();
    }
    Serial.println();
  }

//}

/* source: https://github.com/haisamido/maqamat/blob/main/maqamat.py#L62 */ 
float frequency_from_cents(float f1 , float cents, float cents_per_octave){
  return f1*pow(2,cents/cents_per_octave);
}

void loop2() {

}

void loop() {

  float frequency;
  float cents;
  
  int pitch_class_step;
  int mode=1;
  int row;

  String pitch_class_set_mode;

  for( int mode = 1; mode <= 3; mode++ ) {

    row = mode - 1;

    int red   =255;
    int green =255;
    int blue  =255;
    
    for ( int i = 0; i < number_of_pitches_in_pitch_class; i++) {
      cents            = i*100.0;
      pitch_class_step = pitch_class_set[i];

      pitch_class_set_mode = pitch_class_set_string;

      lcd.setCursor(0, row); 
      lcd.print(mode);
      lcd.setCursor(2, row);
      lcd.print(pitch_class_set_mode);

      // turn on LED on neopixel for the pitch_class_step in question
      pitch_bracelet.setBrightness(255);
      pitch_bracelet.show();
      pitch_bracelet.setPixelColor(pitch_class_step, red, green, blue);
      pitch_bracelet.show();

      Serial.print(mode);
      Serial.print("\t");
      Serial.print(pitch_class_step);
      Serial.print("\t");
      Serial.print(pitch_class_set_mode);
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
      pitch_bracelet.setBrightness(0);
      pitch_bracelet.show();
      delay(1000);
  }

  Serial.println();
}
