#include <Arduino.h> //Delete or comment out if you're going to use Arduino IDE, leave if you want you use platform.io
#include <LittleFS.h>
#include "Config.h"
#include "NTPTime.h"
#include "Lighting.h"
#include "WebServer.h" 
#include "Backlight.h"

byte FRAMES_PER_SECOND = 30; //will be overwritten later on by EEPROM or default settings
unsigned long frameStart; //For fps counter

void setup(){
  Serial.begin(115200);
  pinMode(LIGHT_SENSOR,INPUT);
  Serial.println("\n\n\n\n\n"); //get rid of the jiberish from boot
  random16_add_entropy((uint16_t)random16());
  for(int i=0;i<10;i++) random16_add_entropy(random(65535));
  
  //Wipe EEPROM if specified
  #ifdef RESET_EEPROM
  resetEEPROM();
  #endif
  
  //FastLED Setup
  fastLEDInit();
  FastLED.setDither(false);
  FastLED.setBrightness(255);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, MILLI_AMPS);
  solidSegments(CRGB::Black);
  
  Serial.println("Initializing data structures for lighting effects");
  lightingInit();
  #ifdef BACKLIGHT
  initBacklight();
  #endif

if (!LittleFS.begin()) {
    Serial.println("Mounting LittleFS failed");
    return;
  }


Serial.println("Listing directory: /");

File root = LittleFS.open("/");
  if(!root){
    Serial.println("Failed to open directory");
    return;
  }
  if(!root.isDirectory()){
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while(file) {
    if(file.isDirectory()){
        Serial.print("  DIR : ");
        Serial.println(file.name());
    } else {
        Serial.print("  FILE: ");
        Serial.print(file.name());
        Serial.print("  SIZE: ");
        Serial.println(file.size());
    }
    file = root.openNextFile();
  }

  Serial.printf("\n");
  //WiFi Setup
  Serial.println("Setting up Wi-Fi and NTP Connection");
  setupWiFi();
  Serial.println("Showing lights");
  showLightingEffects();
  Serial.println("Getting webserver started...");
  setupServer();
}

uint16_t counter = 0;
void loop() {
  //Track FPS
  if (counter==FRAMES_PER_SECOND*5) frameStart = micros();

  //Pretty much the only 2 lines that get called aside from the last 2 FastLED calls
  showLightingEffects();
  #ifdef BACKLIGHT
  showBacklight();
  #endif

  //Track FPS
  if(counter>=FRAMES_PER_SECOND*5){
    unsigned long microsecondsPerFrame = micros()-frameStart;
    char buff[60];
    sprintf(buff, "Maximum FPS: %.1f     Milliseconds per frame: %.2f",1000000.0/microsecondsPerFrame,microsecondsPerFrame/1000.0);
    Serial.println(buff);
    counter = 0;
  }
  counter++;

  //Track EEPROM updates
  lastUpdate++;
  if(lastUpdate >= EEPROM_UPDATE_DELAY*FRAMES_PER_SECOND && updateSettings){
    lastUpdate = 0;
    updateSettings = false;
    storeEEPROM();
    #ifdef BACKLIGHT
    saveBLEEPROM();
    #endif
  }
  
  // insert a delay to maintain framerate.
  FastLED.show();
  FastLED.delay(1000 / FRAMES_PER_SECOND);
}
