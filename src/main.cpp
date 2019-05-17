/**
 * @file main.cpp
 * @brief Demo of speech recognition using Maix board - K210 MCU
 * @copyright 2019 - Andri Yadi, DycodeX
 */

#include <Arduino.h>
#include "Maix_Speech_Recognition.h"

#include "voice_model.h"
#include <Sipeed_ST7789.h>

#define RECORD_MODE         0 // Set this 1 to start collecting your Voice Model

#define LABEL_TEXT_SIZE     2 // 2x font size
#define BG_COLOR            COLOR_RED

// The awesome recognizer
SpeechRecognizer rec;

// LCD
SPIClass spi_(SPI0); // MUST be SPI0 for Maix series on board LCD
Sipeed_ST7789 lcd(320, 240, spi_, SIPEED_ST7789_DCX_PIN, SIPEED_ST7789_RST_PIN, DMAC_CHANNEL2);

#if RECORD_MODE
void recordModel() {
  Serial.println("Start recording...");
  if( rec.record(0, 0) == 0) //keyword_num, model_num 
  {    
    rec.print_model(0, 0);
  }
  else 
      Serial.println("Recording failed! Reset the board.");
}
#endif

void printCenterOnLCD(Sipeed_ST7789 &lcd_, const char *msg, uint8_t textSize = LABEL_TEXT_SIZE) 
{
    lcd_.setCursor((lcd_.width() - (6 * textSize * strlen(msg))) / 2, (lcd_.height() - (8*textSize)) / 2);
    lcd_.print(msg);
}

void setup()
{ 
  rec.begin();
  delay(1000);
  Serial.begin(115200);
  
#if RECORD_MODE
  recordModel();
  while(1);
#endif

  if (!lcd.begin(15000000, BG_COLOR)) {
    Serial.println("who wants to live forever....");
    while(1); //who wants to live forever....
  }
    
  lcd.setTextSize(LABEL_TEXT_SIZE);
  lcd.setTextColor(COLOR_WHITE);

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);

  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_BLUE, HIGH);

  Serial.println("Initializing model...");
  printCenterOnLCD(lcd, "Initializing...");

  rec.addVoiceModel(0, 0, hey_friday_0, fram_num_hey_friday_0); 
  rec.addVoiceModel(0, 1, hey_friday_1, fram_num_hey_friday_1); 
  rec.addVoiceModel(0, 2, hey_friday_2, fram_num_hey_friday_2); 
  rec.addVoiceModel(0, 3, hey_friday_3, fram_num_hey_friday_3); 
  rec.addVoiceModel(1, 0, hey_jarvis_0, fram_num_hey_jarvis_0);     
  rec.addVoiceModel(1, 1, hey_jarvis_1, fram_num_hey_jarvis_1);     
  rec.addVoiceModel(1, 2, hey_jarvis_2, fram_num_hey_jarvis_2);     
  rec.addVoiceModel(1, 3, hey_jarvis_3, fram_num_hey_jarvis_3); 

  Serial.println("Model init OK!");
  lcd.fillScreen(BG_COLOR);
  printCenterOnLCD(lcd, "Friday & Jarvis are ready!");
  delay(1000);
}

void loop()
{
  int res;
  lcd.fillScreen(BG_COLOR);
  printCenterOnLCD(lcd, "Waiting for Wake Word..");
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_BLUE, HIGH);

  res = rec.recognize();
  Serial.printf("Result: %d --> ", res);
  lcd.fillScreen(BG_COLOR);

  if (res > 0){
    switch (res)
    {
    case 1:
        digitalWrite(LED_RED, LOW); //power on red led
        digitalWrite(LED_BLUE, HIGH);
        Serial.println("means red LED");
        printCenterOnLCD(lcd, "Hey Tony, Friday is ready");
        break;
    case 2:
        digitalWrite(LED_BLUE, LOW); //power on blue led
        digitalWrite(LED_RED, HIGH);
        Serial.println("means blue LED");
        printCenterOnLCD(lcd, "Hey Tony, Jarvis is ready");
        break;
    case 3:
        digitalWrite(LED_BLUE, LOW);
        digitalWrite(LED_RED, LOW);
        Serial.println("-> failed!");
        printCenterOnLCD(lcd, "Sorry Tony, I don't know");
    default:
        break;
    }
  }else
  {
      Serial.println("Recognizing failed.");
      digitalWrite(LED_BLUE, LOW);
      digitalWrite(LED_RED, LOW);
      Serial.println("-> failed!");
      printCenterOnLCD(lcd, "Sorry Tony, I don't know");
  }
  delay(3000);
}