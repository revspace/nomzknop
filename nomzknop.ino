#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>

#define PIN_LEDS D5
#define PIN_KNOP D6

#define DELAY_DEBOUNCE 20
#define DELAY_FLASH_FAST 25
#define DELAY_FLASH_SLOW 500

#define WIFI_SSID "revspace-pub-2.4ghz"
#define WIFI_PASS ""

#define MQTT_HOST "mosquitto.space.revspace.nl"
#define MQTT_TOPIC "revspace/button/testknop"

Adafruit_NeoPixel strip = Adafruit_NeoPixel(14, PIN_LEDS, NEO_GRBW + NEO_KHZ800);
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  Serial.println("Setup...");

  strip.setBrightness(255);
  strip.begin();
  strip.setPixelColor(0, 255, 0, 0);
  strip.setPixelColor(1, 255, 0, 0);
  strip.setPixelColor(2, 255, 0, 0);
  strip.setPixelColor(3, 255, 0, 0);
  strip.setPixelColor(4, 0, 0, 0);
  strip.show();

  pinMode(PIN_KNOP, INPUT_PULLUP);
  
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
      delay(250);
      Serial.print(".");
  }
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  client.setServer(MQTT_HOST, 1883);

  reconnect();
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    // Attempt to connect
    if (client.connect("nomzknop")) {
      Serial.println("Connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 2 seconds");
      // Wait 2 seconds before retrying
      delay(2000);
    }
  }
}

void loop() {
  uint32_t wheelVal;
  int redVal, greenVal, blueVal;
  for(int j=0; j<256; j++) { // 5 cycles of all colors on wheel
    for(int i=0; i< strip.numPixels(); i++) {
      wheelVal = Wheel(((i * 256 / strip.numPixels()) + j) & 255);
      strip.setPixelColor(i, strip.Color(red(wheelVal), green(wheelVal), blue(wheelVal)));
    }
    strip.show();
    delay(6);
    
    if (!client.connected()) {
      Serial.println("Connection lost, reconnecting...");
      reconnect();
    }
    client.loop();
    
    if(digitalRead(PIN_KNOP) == LOW) {
      Serial.println("NOMZ! Posting on mqtt...");
      String mqtt_text = "lekker! ";
      mqtt_text += millis();
      char mqtt_char[mqtt_text.length()+1];
      mqtt_text.toCharArray(mqtt_char, mqtt_text.length()+1);
      client.publish(MQTT_TOPIC, mqtt_char);
      for(int i=15; i>0; i--) {
        for(int n=5; n>0; n--) {
          strip.setPixelColor(0, 0, 0, 255);
          strip.setPixelColor(1, 0, 0, 255);
          strip.setPixelColor(2, 0, 0, 0);
          strip.setPixelColor(3, 0, 0, 0);
          strip.show();
          delay(DELAY_FLASH_FAST);
          strip.setPixelColor(0, 0, 0, 0);
          strip.setPixelColor(1, 0, 0, 0);
          strip.setPixelColor(2, 0, 0, 0);
          strip.setPixelColor(3, 0, 0, 0);
          strip.show();
          delay(DELAY_FLASH_FAST);
          strip.setPixelColor(0, 0, 0, 0);
          strip.setPixelColor(1, 0, 0, 0);
          strip.setPixelColor(2, 0, 0, 255);
          strip.setPixelColor(3, 0, 0, 255);
          strip.show();
          delay(DELAY_FLASH_FAST);
          strip.setPixelColor(0, 0, 0, 0);
          strip.setPixelColor(1, 0, 0, 0);
          strip.setPixelColor(2, 0, 0, 0);
          strip.setPixelColor(3, 0, 0, 0);
          strip.show();
          delay(DELAY_FLASH_FAST);
        }
        delay(DELAY_FLASH_SLOW);
        for(int n=5; n>0; n--) {
          strip.setPixelColor(0, 255, 128, 0);
          strip.setPixelColor(1, 255, 128, 0);
          strip.setPixelColor(2, 0, 0, 0);
          strip.setPixelColor(3, 0, 0, 0);
          strip.show();
          delay(DELAY_FLASH_FAST);
          strip.setPixelColor(0, 0, 0, 0);
          strip.setPixelColor(1, 0, 0, 0);
          strip.setPixelColor(2, 0, 0, 0);
          strip.setPixelColor(3, 0, 0, 0);
          strip.show();
          delay(DELAY_FLASH_FAST);
          strip.setPixelColor(0, 0, 0, 0);
          strip.setPixelColor(1, 0, 0, 0);
          strip.setPixelColor(2, 255, 128, 0);
          strip.setPixelColor(3, 255, 128, 0);
          strip.show();
          delay(DELAY_FLASH_FAST);
          strip.setPixelColor(0, 0, 0, 0);
          strip.setPixelColor(1, 0, 0, 0);
          strip.setPixelColor(2, 0, 0, 0);
          strip.setPixelColor(3, 0, 0, 0);
          strip.show();
          delay(DELAY_FLASH_FAST);
        }
        delay(DELAY_FLASH_SLOW);
      }
      Serial.println("Restarting");
      ESP.restart();
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3,0);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3,0);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0,0);
}

uint8_t red(uint32_t c) {
  return (c >> 16);
}
uint8_t green(uint32_t c) {
  return (c >> 8);
}
uint8_t blue(uint32_t c) {
  return (c);
}

