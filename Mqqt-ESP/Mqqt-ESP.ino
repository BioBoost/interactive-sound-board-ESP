#include <WiFi.h>
#include "PubSubClient.h" 
#include "Freenove_WS2812_Lib_for_ESP32.h"
#include <ArduinoJson.h>

const char * WIFI_SSID = "xxxxxxxxxxxxxx";
const char * WIFI_PASSWORD = "xxxxxxxxxxxxxxx";
const char * MQTT_BROKER_IP = "xxxxxxxxxxxxxx";
const unsigned int MQTT_BROKER_PORT = 1883;
const String MQTT_BASE_TOPIC = "test/sonic/modules";

// HS SR04 Ultrasonic Sensor Pins
#define TRIGGER_PIN 0
#define ECHO_PIN    1

// Communications
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient); 

String mqttClientId = "";



#define LEDS_COUNT  1
#define LEDS_PIN  8
#define CHANNEL   0

// RGB LED lib
Freenove_ESP32_WS2812 strip = Freenove_ESP32_WS2812(LEDS_COUNT, LEDS_PIN, CHANNEL, TYPE_GRB);


// SENSOR
float duration_us, distance_cm;
float distance_save; 
float * ptr_distance_save;

// MQTT 
const char * devices_topic = "test/devices/";


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


void status_no_communications(void) {
  strip.setLedColorData(0,255,0,0);
  strip.show();
}

void status_only_wifi(void) {
  strip.setLedColorData(0, 255, 255, 0);
  strip.show();
}

void status_wifi_and_mqtt(void) {
  strip.setLedColorData(0, 0, 255, 0);
  strip.show();
}

bool connect_to_wifi(void) {
  Serial.print("Connecting to WiFi ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Not yet connected ...");
  }

  Serial.println("Connected to WiFi");
  return true;
}

String generate_client_id(void) {
  static long randomId = random(1000,9999);
  String mqttClientId = WiFi.macAddress().substring(9);
  mqttClientId.replace(":", "");
  mqttClientId.toLowerCase();
  mqttClientId += "-esp32-sonic-" + String(randomId);
  return mqttClientId;
}

bool connect_to_mqtt_broker(void) {
  mqttClient.setServer(MQTT_BROKER_IP, MQTT_BROKER_PORT);

  if (mqttClient.connect(generate_client_id().c_str())) {
    Serial.print("Connected to MQTT Broker as ");
    Serial.println(generate_client_id());
  }
  else {
    Serial.println("Connection to MQTT Broker failed ..");
  }

  return mqttClient.connected();
}

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

void publish_hello_package(void) {
  String helloTopic = MQTT_BASE_TOPIC + "/hello";

  char output[128];
  StaticJsonDocument<128> message;

  message["ip"] = WiFi.localIP();
  message["id"] = generate_client_id();

  serializeJson(message, output);

  Serial.println("Publishing hello message");
  mqttClient.publish(helloTopic.c_str(), output);
}

void setup() {
  Serial.begin(115200);

  // if analog input pin 0 is unconnected, random analog
  // noise will cause the call to randomSeed() to generate
  // different seed numbers each time the sketch runs.
  // randomSeed() will then shuffle the random function.
  randomSeed(analogRead(0));

  // Configure RGB Led
  strip.begin();
  strip.setBrightness(10);
  status_no_communications();

  // Configure HS SR04 Ultrasonic Sensor Module pins
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  Serial.println("Starting firmware of ESP32-C3 Sonic Module ...");
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());

  connect_to_wifi();
  connect_to_mqtt_broker();
  publish_hello_package();

  // mqttClient.setCallback(callback);
}

void loop() {
//   //checks connections to mqtt broker and WIFI
//   connections();
//  // IF STATUS is true we start sending
//   if(Status == true){
//         // generate 10-microsecond pulse to TRIG pin
//       digitalWrite(TRIG_PIN, HIGH);
//       delayMicroseconds(10);
//       digitalWrite(TRIG_PIN, LOW);
    
//       // measure duration of pulse from ECHO pin
//       duration_us = pulseIn(ECHO_PIN, HIGH);
       
//       // calculate the distance
//       // calculate the distance limit between 0 an 1 
//       distance_cm = (0.017 * duration_us) * 0.01; // Beperken tussen 0 en 1
      
//       if (distance_cm < 1) {
//         mqttClient.publish(sensor_topic, String(distance_cm).c_str());
//         ptr_distance_save = &distance_cm;
//         //Serial.println(sensor_topic);
//       }
//       else{
//         mqttClient.publish(sensor_topic, String(1).c_str());
//         //Serial.println(sensor_topic);
//       }
//    }

// mqttClient.loop();
}
