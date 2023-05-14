#include <WiFi.h>
#include <LittleFS.h>
#include <FastLED.h>
#include "WebServer.h"
#include "Lighting.h"
#include "NTPTime.h"
#include "Backlight.h"
#include <ESPAsyncWebServer.h>
#include <sstream> //included for a new version that works with homebridge work in progress
#include <string> ////included for a new version that works with homebridge work in progress


#define HTTP_GET     0b00000001 //defined as without this would not compile

AsyncWebServer serverNew(80);


void setupServer(){
  //Runs when site is loaded
  serverNew.on("/getsettings", HTTP_GET, [](AsyncWebServerRequest *request) {
    String value = parseSettings();
    Serial.println("Sending: " + value);
    request->send(200, "text/plain", value);
  });

  serverNew.on("/power", HTTP_GET, [](AsyncWebServerRequest *request) {
     if(request->hasArg("value")) {
    power = (request->arg("value")=="true") ? 1 : 0;
    lightingChanges.power = true;
    lastUpdate = 0;
    updateSettings = true;
    request->send(200, "text/plain", "1");
  } else {
    request->send(400, "text/plain", "Missing argument"); // Send error if "value" argument is not provided
  }
});

serverNew.on("/power/status", HTTP_GET, [](AsyncWebServerRequest *request) {
  request->send(200, "text/plain", power ? "1" : "0"); // Send the current power status as "1" or "0"
});

 serverNew.on("/transparency", HTTP_GET, [](AsyncWebServerRequest *request) {
  if(request->hasArg("value")) {
    setForegroundTransparency((byte)request->arg("value").toInt()%256);
    lightingChanges.foregroundTransparency = true;
    lastUpdate = 0;
    updateSettings = true;
    request->send(200, "text/plain", "1");
  } else {
    request->send(400, "text/plain", "Missing argument"); // Send error if "value" argument is not provided
  }
});

serverNew.on("/autobrightness", HTTP_GET, [](AsyncWebServerRequest *request) {
  if(request->hasArg("value")) {
    autobrightness = (request->arg("value")=="true")%256;
    lightingChanges.autobrightness = true;
    lastUpdate = 0;
    updateSettings = true;
    request->send(200, "text/plain", "1");
  } else {
    request->send(400, "text/plain", "Missing argument"); // Send error if "value" argument is not provided
  }
});

serverNew.on("/brightness", HTTP_GET, [](AsyncWebServerRequest *request) {
  if(request->hasArg("value")) {
    setSegmentBrightness((byte)request->arg("value").toInt()%256);
    lightingChanges.segmentBrightness = true;
    lastUpdate = 0;
    updateSettings = true;
    request->send(200, "text/plain", "1");
  } else {
    request->send(400, "text/plain", "Missing argument"); // Send error if "value" argument is not provided
  }
});

serverNew.on("/spotlightbrightness", HTTP_GET, [](AsyncWebServerRequest *request) {
  if(request->hasArg("value")) {
    setSpotlightBrightness((byte)request->arg("value").toInt()%256);
    lightingChanges.spotlightBrightness = true;
    lastUpdate = 0;
    updateSettings = true;
    request->send(200, "text/plain", "1");
  } else {
    request->send(400, "text/plain", "Missing argument"); // Send error if "value" argument is not provided
  }
});

serverNew.on("/backgroundbrightness", HTTP_GET, [](AsyncWebServerRequest *request) {
  if(request->hasArg("value")) {
    setBackgroundBrightness((byte)request->arg("value").toInt()%256);
    lightingChanges.backgroundBrightness = true;
    lastUpdate = 0;
    updateSettings = true;
    request->send(200, "text/plain", "1");
  } else {
    request->send(400, "text/plain", "Missing argument"); // Send error if "value" argument is not provided
  }
});

serverNew.on("/foregroundTransparency", HTTP_GET, [](AsyncWebServerRequest *request) {
  if(request->hasArg("value")) {
    setForegroundTransparency((byte)request->arg("value").toInt()%256);
    lightingChanges.foregroundTransparency = true;
    lastUpdate = 0;
    updateSettings = true;
    request->send(200, "text/plain", "1");
  } else {
    request->send(400, "text/plain", "Missing argument"); // Send error if "value" argument is not provided
  }
});

serverNew.on("/bgcolor", HTTP_GET, [](AsyncWebServerRequest *request) {
  if(request->hasArg("red") && request->hasArg("green") && request->hasArg("blue")) {
    CRGB color = CRGB::Black;
    color.red = request->arg("red").toInt()%256;
    color.green = request->arg("green").toInt()%256;
    color.blue = request->arg("blue").toInt()%256;
    bg = color;
    lightingChanges.bg = true;
    lastUpdate = 0;
    autoEffect = 0; //turn off scheduled lighting effects and toggle back on regular effects
    updateSettings = true;
    request->send(200, "text/plain", "1");
  } else {
    request->send(400, "text/plain", "Missing color argument"); // Send error if any color argument is not provided
  }
});

  serverNew.on("/bg2color", HTTP_GET, [](AsyncWebServerRequest *request) {
  if(request->hasArg("red") && request->hasArg("green") && request->hasArg("blue")) {
    CRGB color = CRGB::Black;
    color.red = request->arg("red").toInt()%256;
    color.green = request->arg("green").toInt()%256;
    color.blue = request->arg("blue").toInt()%256;
    bg2 = color;
    lightingChanges.bg2 = true;
    lastUpdate = 0;
    autoEffect = 0; //turn off scheduled lighting effects and toggle back on regular effects
    updateSettings = true;
    request->send(200, "text/plain", "1");
  } else {
    request->send(400, "text/plain", "Missing color argument"); // Send error if any color argument is not provided
  }
});

serverNew.on("/h1color", HTTP_GET, [](AsyncWebServerRequest *request) {
  if(request->hasArg("red") && request->hasArg("green") && request->hasArg("blue")) {
    CRGB color = CRGB::Black;
    color.red = request->arg("red").toInt()%256;
    color.green = request->arg("green").toInt()%256;
    color.blue = request->arg("blue").toInt()%256;
    h_ten_color = color;
    lightingChanges.h_ten_color = true;
    lastUpdate = 0;
    autoEffect = 0; //turn off scheduled lighting effects and toggle back on regular effects
    updateSettings = true;
    request->send(200, "text/plain", "1");
  } else {
    request->send(400, "text/plain", "Missing color argument"); // Send error if any color argument is not provided
  }
});

serverNew.on("/h2color", HTTP_GET, [](AsyncWebServerRequest *request) {
  if(request->hasArg("red") && request->hasArg("green") && request->hasArg("blue")) {
    CRGB color = CRGB::Black;
    color.red = request->arg("red").toInt()%256;
    color.green = request->arg("green").toInt()%256;
    color.blue = request->arg("blue").toInt()%256;
    h_one_color = color;
    lightingChanges.h_one_color = true;
    lastUpdate = 0;
    autoEffect = 0; //turn off scheduled lighting effects and toggle back on regular effects
    updateSettings = true;
    request->send(200, "text/plain", "1");
  } else {
    request->send(400, "text/plain", "Missing color argument"); // Send error if any color argument is not provided
  }
});

  
  serverNew.on("/m1color", HTTP_GET, [](AsyncWebServerRequest *request) {
  if (request->hasArg("red") && request->hasArg("green") && request->hasArg("blue")) {
    CRGB color = CRGB::Black;
    color.red = request->arg("red").toInt() % 256;
    color.green = request->arg("green").toInt() % 256;
    color.blue = request->arg("blue").toInt() % 256;
    m_ten_color = color;
    lightingChanges.m_ten_color = true;
    lastUpdate = 0;
    autoEffect = 0; // turn off scheduled lighting effects and toggle back on regular effects
    updateSettings = true;
    request->send(200, "text/plain", "1");
  } else {
    request->send(400, "text/plain", "Missing color argument"); // Send error if any color argument is not provided
  }
});

serverNew.on("/m2color", HTTP_GET, [](AsyncWebServerRequest *request) {
  if (request->hasArg("red") && request->hasArg("green") && request->hasArg("blue")) {
    CRGB color = CRGB::Black;
    color.red = request->arg("red").toInt() % 256;
    color.green = request->arg("green").toInt() % 256;
    color.blue = request->arg("blue").toInt() % 256;
    m_one_color = color;
    lightingChanges.m_one_color = true;
    lastUpdate = 0;
    autoEffect = 0; // turn off scheduled lighting effects and toggle back on regular effects
    updateSettings = true;
    request->send(200, "text/plain", "1");
  } else {
    request->send(400, "text/plain", "Missing color argument"); // Send error if any color argument is not provided
  }
});
  
  serverNew.on("/spotcolor", HTTP_GET, [](AsyncWebServerRequest *request) {
  if (request->hasArg("red") && request->hasArg("green") && request->hasArg("blue") && request->hasArg("value")) {
    CRGB color = CRGB::Black;
    int index = request->arg("value").toInt();
    color.red = request->arg("red").toInt() % 256;
    color.green = request->arg("green").toInt() % 256;
    color.blue = request->arg("blue").toInt() % 256;
    if (index >= WIDTH * HEIGHT) {
      request->send(400, "text/plain", String(index) + " not supported");
      return;
    }
    setSpotlightColor(index - 1, color);
    if (index == 1)
      setSpotlight1(color);
    else if (index == 2)
      setSpotlight2(color);

    lightingChanges.spotlights[index - 1] = true;
    lastUpdate = 0;
    autoEffect = 0; // turn off scheduled lighting effects and toggle back on regular effects
    updateSettings = true;
    request->send(200, "text/plain", "1");
  } else {
    request->send(400, "text/plain", "Missing argument"); // Send error if any argument is missing
  }
});

serverNew.on("api/spotcolor/set", HTTP_GET, [](AsyncWebServerRequest *request) {
  if (request->hasArg("color") && request->hasArg("value")) {
    std::string hexColor = request->arg("color").c_str();
    int index = request->arg("value").toInt();

    if (hexColor.front() == '#') {
        hexColor.erase(0, 1);
    }

    if(hexColor.length() != 6) {
        request->send(400, "text/plain", "Invalid hex color code."); // Send error if the color argument is not a valid hex color code
        return;
    }

    CRGB color = CRGB::Black;
    std::istringstream(hexColor.substr(0,2)) >> std::hex >> color.red;
    std::istringstream(hexColor.substr(2,2)) >> std::hex >> color.green;
    std::istringstream(hexColor.substr(4,2)) >> std::hex >> color.blue;

    if (index >= WIDTH * HEIGHT) {
      request->send(400, "text/plain", String(index) + " not supported");
      return;
    }

    setSpotlightColor(index - 1, color);
    if (index == 1)
      setSpotlight1(color);
    else if (index == 2)
      setSpotlight2(color);

    lightingChanges.spotlights[index - 1] = true;
    lastUpdate = 0;
    autoEffect = 0; //turn off scheduled lighting effects and toggle back on regular effects
    updateSettings = true;
    request->send(200, "text/plain", "1");
  } else {
    request->send(400, "text/plain", "Missing argument"); // Send error if any argument is missing
  }
});


  serverNew.on("/effectfg", HTTP_GET, [](AsyncWebServerRequest *request) {
  if (request->hasArg("value")) {
    String pattern = request->arg("value");
    if (pattern == "off")
      foregroundPattern = 0;
    else if (pattern == "solid")
      foregroundPattern = 1;
    else if (pattern == "rainbow")
      foregroundPattern = 2;
    else if (pattern == "gradient")
      foregroundPattern = 3;
    else {
      request->send(400, "text/plain", pattern + " not supported");
      return;
    }
    lightingChanges.foregroundPattern = true;
    lastUpdate = 0;
    autoEffect = 0; // turn off scheduled lighting effects and toggle back on regular effects
    updateSettings = true;
    request->send(200, "text/plain", "1");
  } else {
    request->send(400, "text/plain", "Missing value argument"); // Send error if value argument is missing
  }
});

serverNew.on("/effectbg", HTTP_GET, [](AsyncWebServerRequest *request) {
  if (request->hasArg("value")) {
    String pattern = request->arg("value");
    if (pattern == "off")
      backgroundPattern = 0;
    else if (pattern == "solid")
      backgroundPattern = 1;
    else if (pattern == "rainbow")
      backgroundPattern = 2;
    else if (pattern == "gradient")
      backgroundPattern = 3;
    else if (pattern == "rain")
      backgroundPattern = 4;
    else if (pattern == "sparkle")
      backgroundPattern = 5;
    else if (pattern == "fire")
      backgroundPattern = 6;
    else if (pattern == "loop")
      backgroundPattern = 255;
    else {
      request->send(400, "text/plain", pattern + " not supported");
      return;
    }
    lightingChanges.backgroundPattern = true;
    clearLightingCache();
    lastUpdate = 0;
    autoEffect = 0; // turn off scheduled lighting effects and toggle back on regular effects
    updateSettings = true;
    request->send(200, "text/plain", "1");
  } else {
    request->send(400, "text/plain", "Missing value argument"); // Send error if value argument is missing
  }
});
  
serverNew.on("/effectsl", HTTP_GET, [](AsyncWebServerRequest *request) {
  if (request->hasParam("value")) {
    String pattern = request->getParam("value")->value();
    if (pattern == "off")
      spotlightPattern = 0;
    else if (pattern == "solid")
      spotlightPattern = 1;
    else if (pattern == "rainbow")
      spotlightPattern = 2;
    else if (pattern == "gradient")
      spotlightPattern = 3;
    else if (pattern == "rain")
      spotlightPattern = 4;
    else if (pattern == "sparkle")
      spotlightPattern = 5;
    else if (pattern == "fire")
      spotlightPattern = 6;
    else {
      request->send(400, "text/plain", pattern + " not supported");
      return;
    }
    lightingChanges.spotlightPattern = true;
    clearLightingCache();
    lastUpdate = 0;
    autoEffect = 0; // turn off scheduled lighting effects and toggle back on regular effects
    updateSettings = true;
    request->send(200, "text/plain", "1");
  } else {
    request->send(400, "text/plain", "Missing value argument"); // Send error if value argument is missing
  }
});

  serverNew.on("/utcoffset", HTTP_GET, [](AsyncWebServerRequest *request) {
  if (request->hasParam("value")) {
    String value = request->getParam("value")->value();
    storeUtcOffset(value.toDouble());
    setNewOffset();
    request->send(200, "text/plain", "1");
  } else {
    request->send(400, "text/plain", "Missing value argument"); // Send error if value argument is missing
  }
});

   
  serverNew.on("/cmd", HTTP_GET, [](AsyncWebServerRequest *request) {
  String command = request->getParam("c")->value();
  String val = request->getParam("v")->value();
  command.toLowerCase();
  val.toLowerCase();
    if(command=="help" || command=="?"){
      String resp = String("Available commands:<br> settings - Gets the current settings<br>" + String("") +  //Doesnt like it when I just do *char[] + *char[] here, so I need to do this
                                        "utcoffset ## - Set UTF offset in hours for clock [current offset: " + String(getOffset()) + "]<br>" +
                                        "rainbowrate ## - Set how rainbowy the rainbow is [current rate: " + String(rainbowRate) + "]<br>" +
                                        "fps ## - Sets frames per second [current rate: " + String(FRAMES_PER_SECOND) + "]<br>" +
                                        "effecttimer ## - Turns on or off scheduled time configurations. [current value: " + String(autoEffect) + "]<br>" +
                                        "hyphen ## - Sets the length of the hyphen seperator. 0 Disables hyphen. [current length: " + String(hyphenLength) + "]<br>" +
                                        "hyphencolor HEX - Sets the hyphen color (format should be RRGGBB in hex, like FFA400). [current color: " + String(crgbToCss(hyphenColor)) + "]<br>" +
                                        "reset - Reset all settings<br>" +
                                        "resetprofile - Reset lighting settings<br>" +
                                        "save - Saves all current settings<br>" +
                                        "reboot - Reboots the device after 3 seconds");
      request->send(200, "text/plain", resp);
    }else if(command=="settings"){
      request->send(200, "text/plain", getCurrentSettings("<br>"));
    }else if(command=="utcoffset"){
      utcOffset = val.toDouble();
      setNewOffset();
      updateTime();
      storeUtcOffset(utcOffset);
      request->send(200, "text/plain", "Set UTC offset to " + String(getUtcOffset()));
    }else if(command=="rainbowrate"){
      rainbowRate = (double)val.toInt();
      lightingChanges.rainbowRate = true;
      lastUpdate = EEPROM_UPDATE_DELAY*FRAMES_PER_SECOND;
      updateSettings = true;
      request->send(200, "text/plain", "Set rainbow rate to " + String(rainbowRate));
    }else if(command=="fps"){
      FRAMES_PER_SECOND = max((byte)1,(byte)val.toInt()); //Setting to 0 sets off the watchdog since it never refreshes
      lightingChanges.fps = true;
      lastUpdate = EEPROM_UPDATE_DELAY*FRAMES_PER_SECOND;
      updateSettings = true;
      request->send(200, "text/plain", "Set fps to " + String(FRAMES_PER_SECOND));
    }else if(command=="reset"){
      defaultSettings();
      saveAllSettings();
      request->send(200, "text/plain", "Reset Settings");
    }else if(command=="resetprofile"){
      deleteSettings();
      request->send(200, "text/plain", "Profile will reset to default on reboot");
    }else if(command=="hyphen"){
      hyphenLength = max(min((byte)val.toInt(),(byte)LEDS_PER_LINE),(byte)0);
      lightingChanges.hyphenLength = true;
      lastUpdate = EEPROM_UPDATE_DELAY*FRAMES_PER_SECOND;
      updateSettings = true;
      request->send(200, "text/plain", "Set hyphen length to " + String(hyphenLength));
    }else if(command=="hyphencolor"){
      Serial.println("Color: #" + val.substring(0,2) + val.substring(2,4) + val.substring(4,6));
      Serial.println("Value: " + String(hexToByte(val.substring(0,2))) + " " + String(hexToByte(val.substring(2,4))) + " " + String(hexToByte(val.substring(4,6))));
      hyphenColor = CRGB(hexToByte(val.substring(0,2)),hexToByte(val.substring(2,4)),hexToByte(val.substring(4,6)));
      lightingChanges.hyphenColor = true;
      lastUpdate = EEPROM_UPDATE_DELAY*FRAMES_PER_SECOND;
      updateSettings = true;
      request->send(200, "text/plain", "Set hyphen color to " + String(crgbToCss(hyphenColor)));
    }else if(command=="save"){
      saveAllSettings();
      request->send(200, "text/plain", "Saved Settings");
    }else if(command=="reboot" || command=="restart"){
      //Wont actually send
      //request->send(200, "text/plain", "Rebooting in 3 seconds...");
      
       ESP.restart(); // Turn on watchdog then let program hang. Im not sure if the 15ms is actually implemented in the ESP source code
      for(;;) {} //Make watchdog reset the device
    }else if(command == "effecttimer"){
      autoEffect = (val.toInt() != 0); //force bool cast
      request->send(200, "text/plain", "Set effecttimer to " + String(autoEffect));
    }else{
      request->send(200, "text/plain", "Command not found");
    }
  });

  serverNew.serveStatic("/", LittleFS, "/");  // Serve static files from the "spiffs" partition

  serverNew.begin();
  Serial.println("HTTP web server initialized");
}

byte hexToByte(String hex){
  return ((hexCharToNum(hex.charAt(0))<<4) + (hexCharToNum(hex.charAt(1)))); //bitshift <<4 doesnt work
}
byte hexCharToNum(char letter){
  switch(letter){ //could do better with an ascii representation
    case '0': return 0;
    case '1': return 1;
    case '2': return 2;
    case '3': return 3;
    case '4': return 4;
    case '5': return 5;
    case '6': return 6;
    case '7': return 7;
    case '8': return 8;
    case '9': return 9;
    case 'a': case 'A': return 10;
    case 'b': case 'B': return 11;
    case 'c': case 'C': return 12;
    case 'd': case 'D': return 13;
    case 'e': case 'E': return 14;
    case 'f': case 'F': return 15;
    default:
      Serial.println("Unknown value: " + String(letter)); 
      return 0;
  }
}
