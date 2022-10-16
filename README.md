# Interactive-sound-board-ESP
Arduino code of all the ESP.
We are using the model: ESP32-C3-DevKitM-1

## Execute
Require to install needed libraries:
Installing ESP32 Add-on in Arduino IDE

We need to add on preferances Additional Board Manager URLs:
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

Then in board manager we can type: ESP32
We install the ESP32 by Espressif Systems
Than we chose ur board: In tools/board/esp32

## Important parts of the code

### We take as example the Code of EXP1:

The needed ssid and password to connect to access point

```CPP
const char* ssid      = "devbit-verhuis";
const char* password  = "Dr@@dloos!";
```

We connect to the broker mqtt.devbit.be, we publisch to
test/soundboard/esp1. We declare an client ID soo the socket knows what unical client it is.

```CPP
const char* mqtt_server = "mqtt.devbit.be";
const char* sensor_topic = "test/soundboard/esp1"; 
const char* mqtt_username = "test_esp1"; 
const char* mqtt_password = "test_esp1"; 
const char* clientID = "client_soundboard_esp1"; 
```

```CPP
const char* mqtt_server = "mqtt.devbit.be";
const char* sensor_topic = "test/soundboard/esp1"; 
const char* mqtt_username = "test_esp1"; 
const char* mqtt_password = "test_esp1"; 
const char* clientID = "client_soundboard_esp1"; 
```

Inside **void Setup** we publish ur value to the socket. We convert the float in a type string.

```CPP
 if (client.publish(sensor_topic, String(distance_cm).c_str())) {
    Serial.println("distance sent!");
  }
```

We have tree modules and every module publish a value to an topic.

![alt text](./img/Basic-mqtt.png "label")
