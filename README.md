# Interactive-sound-board-ESP
Arduino code for the ESP.
We are using the model: ESP32-C3-DevKitM-1

made by:
- [<img src="https://github.com/TristanDeLil.png" width="25">Tristan De Lil](https://github.com/TristanDeLil)
- [<img src="https://github.com/r0755466.png" width="25">Rayan Azzi](https://github.com/r0755466)

## importent note
This was all programmed on the Arduino IDE version 1.

## Libraries

Required libraries:
- Installing ESP32 Add-on in Arduino IDE
- PubSubClient for esp 32: [MQTT library link](https://github.com/plapointe6/EspMQTTClient)
- Freenova WS2812: [RGB led link](https://github.com/Freenove/Freenove_WS2812_Lib_for_ESP32)
- WIFI for arduino: [WIFI link](https://www.arduino.cc/reference/en/libraries/wifi/)

### use esp32 with arduino IDEv1

We need to add on preferances Additional Board Manager URLs:

File -> Preferences -> add url to Addittional Boards Manager URLs: https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

Then in board manager we can type: ESP32
tools -> board manager -> esp32
We install the ESP32 by Espressif Systems

Than we chose ur board: In tools -> board -> ESP32 arduino -> ESP32c3 Dev module.

### how to install

Tools -> manage libraries -> search for the libraries

or go to the folder libs copy them to Documents\Arduino\libraries and unzip them there.

## code 

include alle nodige libraries:
- wifi 
    Om de esp te kunnen verbinden met wifi netwerken.
- PubSubClient
    Om te kunnen communiceren via MQTT.
- Freenove WS2812
    De RGB led op de ESP makelijker te kunnen gebruiken en aanpassen.

```cpp
#include <WiFi.h>
#include "PubSubClient.h" 
#include "Freenove_WS2812_Lib_for_ESP32.h"
```

```cpp
#define LEDS_COUNT  1
#define LEDS_PIN  8
#define CHANNEL   0

// RGB LED lib
Freenove_ESP32_WS2812 strip = Freenove_ESP32_WS2812(LEDS_COUNT, LEDS_PIN, CHANNEL, TYPE_GRB);
```

Globale variabelen voor de Ultrasone sensor.

```cpp
#define TRIG_PIN 0 // ESP32 pin GIOP23 connected to Ultrasonic Sensor's TRIG pin
#define ECHO_PIN 1 // ESP32 pin GIOP22 connected to Ultrasonic Sensor's ECHO pin
// SENSOR
float duration_us, distance_cm;
float distance_save; 
float * ptr_distance_save;
```

Aanmaken van de variable om de esp te verbinden met WIFI.

```cpp
// WIFI netwerk  
const char* ssid      = "<wifi name>";
const char* password  = "<password>";
```

Aanmaken van de variable om de esp te verbinden met MQTT.

```cpp
// MQTT 
const char* mqtt_server = "<ip/hostname of MQTT broker>";  // Broker we connect to s
const char * devices_topic = "test/devices/";
```

Initialisatie van de Wifi en mqtt libraries. 
De esp verbind de mqtt broker via poort 1883.

```cpp
// Initialise the WiFi and MQTT Client objects
WiFiClient wifiClient;
// 1883 is the listener port for the Broker
PubSubClient client(mqtt_server, 1883, wifiClient); 
```

Aanmaken van variabelen
- GlobalClientID: om het unieke clientID later in op te slaan.
- Status: sensor aan of uit;
- Sensor_topic: topic om waar het mac address wordt in verwerkt om later de sensor waren naar door te sturen;
- Sensor_topic_status: topic om mac address naar te publishen.

```cpp
//globale string variable ! niet aanpassen.
String globalClientID;
// Status 0 or 1
bool Status = false;
// Attributes 
const char * value_topic = "test/";
String value_s(value_topic);
String samen = String(value_s + WiFi.macAddress() + "/sensor");
  
// Char convert from string 
const char * sensor_topic = samen.c_str();
  
//*************************** 
const char * devices_status_topic = "test/status/";

// Need to listen to the status of this ESP (mac)
// test/mac/sensor
const char * status_ = "test/";
String status_s(status_);
String samen_ = String(status_s + WiFi.macAddress() + "/status");
  
// Char convert from string 
const char * sensor_topic_status = samen_.c_str();
```

connect_WIFI_MQTT functie verbindt de esp met de wifi en daarna met de MQTT broker.
In deze functie wordt ook een unieke clientID genereerd voor de MQTT broker en opgeslagen 
en in globale variable om later nog te gebruiken.

```cpp
void connect_WIFI_MQTT(){
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Connect to the WiFi
  WiFi.begin(ssid, password);

  // Wait until the connection has been confirmed before continuing
  while (WiFi.status() != WL_CONNECTED) {
    // to test 1s , otherwise 500ms
    delay(1000);
    Serial.print(".");
  }
  
  String clientID = WiFi.macAddress().substring(15) + "_esp_sound_" + String(random(1000,9999));
  globalClientID = clientID;
  if (client.connect(clientID.c_str())) {
    Serial.println("Connected to MQTT Broker!");
    Serial.println(clientID);
    strip.setLedColorData(0, 255, 0, 0);
    strip.show();
    
  }
  else {
    Serial.println("Connection to MQTT Broker failed...");
    strip.setLedColorData(0,0, 0, 255);
    strip.show();
  }
}
```

Callback is waar de Mqtt messages worden ontvangen en verwerkt.
Deze kijkt of de binnen gekomen waarden een '1' ASCII 49 of een andere waarde is.
Dit zal de variabele status veranderen naar TRUE/FALSE.

```cpp
// ********* callback 
void callback(char *topic, byte *payload, unsigned int length) {
 Serial.print("Message arrived in topic: ");
 Serial.println(topic);
 Serial.print("Message:");
 for (int i = 0; i < length; i++) {
     Serial.print((char) payload[i]);
     String recv_payload = String(( char *) payload);
    
      if (payload[0] == 49) {
    // the strings arequal
           Status = true; 
           Serial.print("recived one  ");
           Serial.print(payload[0]);
      }
      else{
      Status = false; 
      Serial.print("recived zero  ");
      Serial.print(payload[0]);
      }
 } 
 Serial.println();
 Serial.println("-----------------------");
}
```

Functie die de connecties met de WIFI en MQTT controleren en als nodig proberen te herstellen.
- Rood = WIFI verbinding verbroken.
- BLAUW = MQTT verbinding verbroken.
- GROEN = alle verbinding operationeel.

```cpp
void connections(){
  if(WiFi.status() != WL_CONNECTED){
    strip.setBrightness(0);
    strip.setLedColorData(0, 255, 0, 0);
    Serial.println("connection lost to WIFI");
    reconnect_WIFI();
  }
  if (!client.connected()) {
    strip.setBrightness(10);
    strip.setLedColorData(0, 0, 0, 255);
    reconnect_MQTT();
  }
  else{
    strip.setLedColorData(0, 0, 255, 0);
  }
  strip.show();
}
```

Reconnect functie voor de verbinding te herstellen met de MQTT broker.
Deze functie heeft de globaleClientID nodig om de verbinding te herstellen.
Als het reconnecten succesvol was, her subscribed hij op de topics.
In de tegengestelde scenario print het de state van de client uit en probeert hij na 5 seconden opnieuw.

```cpp
void reconnect_MQTT() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection... ");
    // Attempt to connect
    Serial.println(globalClientID.c_str());
    if (client.connect(globalClientID.c_str())) {
      Serial.println("connected");
      // Subscribe
      client.subscribe(sensor_topic_status);
      Serial.println(sensor_topic_status);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
```

Functie dat probeert de verbinding opnieuw op te stellen met de WIFI elke seconde.

```cpp
void reconnect_WIFI(){
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  // Wait until the connection has been confirmed before continuing
  while (WiFi.status() != WL_CONNECTED) {
    // to test 1s , otherwise 500ms
    delay(1000);
    Serial.print(".");
  }
}
```

De setup functie wordt éénmalig uitgevoert als de esp opgestart wordt:
- De led activeren , de brightness aanpassen en standaard op rood zetten.
- Zal seriele communicatie met een baudrate van 9600 initializeren.
- De pinnen van de Ultrasone sensor input en output definiëren.
- Verbinding maken WIFI en MQTT broker.
- De MQTT loop aanmaken aan de hand van de callback functie.
- Zijn MAC address publishen naar test/devices.
- Subscriben op test/{MAC}/status.

```cpp
void setup() {
  strip.begin();
  strip.setBrightness(10);
  strip.setLedColorData(0,255,0,0);
  strip.show();
  // begin serial port
  Serial.begin (9600);
  // configure the trigger pin to output mode
  pinMode(TRIG_PIN, OUTPUT);
  // configure the echo pin to input mode
  pinMode(ECHO_PIN, INPUT);
  connect_WIFI_MQTT();
  client.setCallback(callback);
  // Status and mac 
  const char * mac = WiFi.macAddress().c_str();
  client.publish(devices_topic, mac);
  //Callback for Subscribing 
  
  client.subscribe(sensor_topic_status);
  Serial.println(sensor_topic_status);
  //Serial.println(clientID);
}

```

The loop check eerst of the WIFI & MQTT nog in orde zijn.
Daarna als de status true is zal hij waarden meten via de ultrasone en door sturen naar de sensor_topic.
Deze blijft draaien tot de esp wordt uitgetrokken of de code er langer overdoet dan 15 seconden om aan client.loop() te geraken.
client.loop() moet elke keer aangeroepen worden of de esp wordt automatisch van de broker gedisconnect.

```cpp
void loop() {
  //checks connections to mqtt broker and WIFI
  connections();
 // IF STATUS is true we start sending
  if(Status == true){
        // generate 10-microsecond pulse to TRIG pin
      digitalWrite(TRIG_PIN, HIGH);
      delayMicroseconds(10);
      digitalWrite(TRIG_PIN, LOW);
    
      // measure duration of pulse from ECHO pin
      duration_us = pulseIn(ECHO_PIN, HIGH);
       
      // calculate the distance
      // calculate the distance limit between 0 an 1 
      distance_cm = (0.017 * duration_us) * 0.01; // Beperken tussen 0 en 1
      
      if (distance_cm < 1) {
        client.publish(sensor_topic, String(distance_cm).c_str());
        ptr_distance_save = &distance_cm;
        //Serial.println(sensor_topic);
      }
      else{
        client.publish(sensor_topic, String(1).c_str());
        //Serial.println(sensor_topic);
      }
   }

client.loop();
}
```
