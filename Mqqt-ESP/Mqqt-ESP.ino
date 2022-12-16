#include <WiFi.h>
#include "PubSubClient.h" // Connect and publish to the MQTT broker
#include "Freenove_WS2812_Lib_for_ESP32.h"

#define TRIG_PIN 0 // ESP32 pin GIOP23 connected to Ultrasonic Sensor's TRIG pin
#define ECHO_PIN 1 // ESP32 pin GIOP22 connected to Ultrasonic Sensor's ECHO pin

#define LEDS_COUNT  1
#define LEDS_PIN  8
#define CHANNEL   0

// LED
u8 m_color[5][3] = { {255, 0, 0}, {0, 255, 0}, {0, 0, 255}, {255, 255, 255}, {0, 0, 0} };
Freenove_ESP32_WS2812 strip = Freenove_ESP32_WS2812(LEDS_COUNT, LEDS_PIN, CHANNEL, TYPE_GRB);

// SENSOR
float duration_us, distance_cm;

// WIFI netwerk
// Network on school  
const char* ssid      = "devbit-verhuis";
const char* password  = "Dr@@dloos!";

// MQTT 
const char* mqtt_server = "172.16.240.44";  // Broker we connect to s
const char * devices_topic = "test/devices/";

// Status 0 or 1
bool Status = false;

// We add Mac user 
//const char* mqtt_username = WiFi.macAddress().c_str(); // MQTT username
//const char* mqtt_password = WiFi.macAddress().c_str(); // MQTT password
String tempclient = WiFi.macAddress();
//const char* clientID = "espusb";//WiFi.macAddress().c_str(); // MQTT client ID

// Initialise the WiFi and MQTT Client objects
WiFiClient wifiClient;

// 1883 is the listener port for the Broker
PubSubClient client(mqtt_server, 1883, wifiClient); 

void connect_MQTT(){
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
  // Connect to MQTT Broker
  // client.connect returns a boolean value to let us know if the connection was successful.
  // If the connection is failing, make sure you are using the correct MQTT Username and Password (Setup Earlier in the Instructable)
  /*
  if (client.connect(clientID, mqtt_username, mqtt_password)) {
    Serial.println("Connected to MQTT Broker!");
  }
  */
  String clientID = WiFi.macAddress().substring(15) + "_esp_sound_" + String(random(1000,9999));
  if (client.connect(clientID.c_str())) {
    Serial.println("Connected to MQTT Broker!");
    Serial.println(clientID);
    strip.setLedColorData(0, m_color[1][0], m_color[1][1], m_color[1][2]);
    strip.show();
    
  }
  else {
    Serial.println("Connection to MQTT Broker failed...");
    strip.setLedColorData(0, m_color[0][0], m_color[0][1], m_color[0][2]);
    strip.show();
  }
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


void connections(){
  if(WiFi.status() != WL_CONNECTED){
    strip.setBrightness(0);
    strip.setLedColorData(0, m_color[0][0], m_color[0][1], m_color[0][0]);
    Serial.println("connection lost");
  }
  else{
    strip.setLedColorData(0, m_color[1][0], m_color[1][1], m_color[1][2]);
  }
  strip.show();
}

// Need to listen to the status of this ESP (mac)
// test/mac/sensor
const char * status_ = "test/";
String status_s(status_);
String samen_ = String(status_s + WiFi.macAddress() + "/status");
  
// Char convert from string 
const char * sensor_topic_status = samen_.c_str();

void setup() {
  strip.begin();
  strip.setBrightness(10);
  strip.setLedColorData(0, m_color[0][0], m_color[0][1], m_color[0][2]);
  strip.show();
  // begin serial port
  Serial.begin (9600);
  // configure the trigger pin to output mode
  pinMode(TRIG_PIN, OUTPUT);
  // configure the echo pin to input mode
  pinMode(ECHO_PIN, INPUT);
  connect_MQTT();
  client.setCallback(callback);
  // Status and mac 
  const char * mac = WiFi.macAddress().c_str();
  client.publish(devices_topic, mac);
  //Callback for Subscribing 
  
  client.subscribe(sensor_topic_status);
  Serial.println(sensor_topic_status);
  //Serial.println(clientID);
}

/*void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(clientID, mqtt_username, mqtt_password)) {
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
*/

void loop() {
  // Attributes 
  const char * value_topic = "test/";
  String value_s(value_topic);
  String samen = String(value_s + WiFi.macAddress() + "/sensor");
  
  // Char convert from string 
  const char * sensor_topic = samen.c_str();
  
  //*************************** 
  const char * devices_status_topic = "test/status/";
  
  float distance_save; 
  float * ptr_distance_save;

  if (!client.connected()) {
    strip.setBrightness(10);
    strip.setLedColorData(0, m_color[0][0], m_color[0][1], m_color[0][2]);
    //reconnect();
    strip.show();
  }
  //connections();
  // generate 10-microsecond pulse to TRIG pin
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // measure duration of pulse from ECHO pin
  duration_us = pulseIn(ECHO_PIN, HIGH);
   
  // calculate the distance
  // calculate the distance limit between 0 an 1 
  distance_cm = (0.017 * duration_us) * 0.01; // Beperken tussen 0 en 1
  
 // IF STATUS is true we start sending
  if(Status == true){
      if (distance_cm < 1) {
        client.publish(sensor_topic, String(distance_cm).c_str());
        ptr_distance_save = &distance_cm;
        Serial.println(sensor_topic);
      }
      else{
        client.publish(sensor_topic, String(1).c_str());
        Serial.println(sensor_topic);
      }
   }

  // terug aanpassen later 
  // print the value to Serial Monitor
   // Serial.print("distance: ");
  //Serial.print(distance_cm);
  //Serial.print(samen);
Serial.println(tempclient);
//Added for subcribing
client.loop();
Serial.println("client loop");
}
