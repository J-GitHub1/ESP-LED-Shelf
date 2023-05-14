#include "Config.h"
#include "NTPTime.h"
#include "Lighting.h"
#include "Secrets.h"
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <WiFi.h>
#include <WebServer.h> //updated to ESP32 Library
#include <ESPmDNS.h> //updated to ESP32 Library

double utcOffset;

const String deviceName = "ledshelf";

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

//IP config
IPAddress ip(192,168,1,99);     //Device IP
IPAddress gateway(192,168,1,254); //IP of router
IPAddress subnet(255,255,255,0);
IPAddress primaryDNS(192,168,1,2); // I use PIHOLE default is 8,8,8,8
IPAddress secondaryDNS(8,8,4,4);

void setupWiFi(){
  if (!WiFi.config(ip, gateway, subnet, primaryDNS, secondaryDNS)) {Serial.println("STA Failed to configure");}  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status()!=WL_CONNECTED){
    //Show pattern while trying to connect to Wi-Fi
    for(int i=0 ; i<60 ; i++){
      loadingEffect(CRGB::White);
      delay(17);
      FastLED.show();
    }
    Serial.print(".");
  }
  Serial.println();
     // Initialize MDNS Updated to ESP32
  if (!MDNS.begin("esp32")) {
    Serial.println("Error setting up MDNS responder!");
    while(1) {
      delay(1000);
    }
  }

  solidSegments(CRGB::Black);
  solidSpotlights(CRGB::Black);
  WiFi.setSleep(false);
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  
  timeClient.begin();
  utcOffset = getUtcOffset();
  setNewOffset();
  updateTime();
  Serial.println("Time updated");
}
void updateTime(){timeClient.update();}
void setNewOffset(){timeClient.setTimeOffset((int)(utcOffset*3600));}
double getOffset(){return utcOffset;}
int getHour12(){return ((timeClient.getHours()%12 + 12*(timeClient.getHours()%12==0)));} //branchess formula to get 12 to display, but cycle back to 1 in a 12hr format instead of displaying 0 when it is 12am/pm
int getHour24(){return (timeClient.getHours());}
int getMinute(){return (timeClient.getMinutes());}
int getSecond(){return (timeClient.getSeconds());}
