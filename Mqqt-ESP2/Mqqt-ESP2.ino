include <WiFi.h>
#include "PubSubClient.h" // Connect and publish to the MQTT broker

#define TRIG_PIN 0 // ESP32 pin GIOP23 connected to Ultrasonic Sensor's TRIG pin
#define ECHO_PIN 1 // ESP32 pin GIOP22 connected to Ultrasonic Sensor's ECHO pin

// Code ESP2 


// SENSOR
float duration_us, distance_cm;

// WIFI netwerk
// Network on school  
//const char* ssid      = "devbit-verhuis";
//const char* password  = "Dr@@dloos!";

// Network home
const char* ssid      = "WiFi-2.4-FA30";
const char* password  = "wzsd7cyh5e76a";



// MQTT 
const char* mqtt_server = "mqtt.devbit.be";  // Broker we connect to 
const char* sensor_topic = "test/soundboard/esp2"; // Topic we send to
const char* mqtt_username = "test_esp2"; // MQTT username
const char* mqtt_password = "test_esp2"; // MQTT password
const char* clientID = "client_soundboard_esp2"; // MQTT client ID


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
  if (client.connect(clientID, mqtt_username, mqtt_password)) {
    Serial.println("Connected to MQTT Broker!");
  }
  else {
    Serial.println("Connection to MQTT Broker failed...");
  }
}



void setup() {
  // begin serial port
  Serial.begin (9600);
  // configure the trigger pin to output mode
  pinMode(TRIG_PIN, OUTPUT);
  // configure the echo pin to input mode
  pinMode(ECHO_PIN, INPUT);

  connect_MQTT();
}

void loop() {
  // generate 10-microsecond pulse to TRIG pin
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // measure duration of pulse from ECHO pin
  duration_us = pulseIn(ECHO_PIN, HIGH);

   // To test home without using the sensor 
   float max = 100; 
   float min = 100;
   random(max, min);

   
  // calculate the distance
  //distance_cm = 0.017 * duration_us;

  
  // For testing from home 
  distance_cm = 10 +rand()%100;
  // We publich the data to the topic
  if (client.publish(sensor_topic, String(distance_cm).c_str())) {
    Serial.println("distance sent!");
  }
  // Again, client.publish will return a boolean value depending on whether it succeded or not.
  // If the message failed to send, we will try again, as the connection may have broken.



  // print the value to Serial Monitor
  Serial.print("distance: ");


 
  Serial.print(distance_cm);

}
