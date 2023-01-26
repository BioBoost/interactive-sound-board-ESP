# ESP32-C3 Sonic

Ultrasonic measurement device that can be controlled via MQTT.

Make sure to copy the `config.example.h` file as `config.h` and provide the needed configuration.

## How to

When device starts it will publish a message to `sonic/modules/hello` with some device information in JSON format. Example:

```json
{ "ip":"xxx.xxx.xxx.xxx","id":"esp32-sonic-xxxxxx","version":"v0.1" }
```

Each device has a unique ID based on it's MAC address.

Once this message has been published, the device will start to listen on the topic `sonic/modules/:id/commands` where `:id` is it's own device id.

To trigger the device into measuring the distance in cm, one just has to publish the following command message on that topic:

```json
{ "cmd":"measure" }
```

After which the device will measure the distance and report it's finding on the topic `sonic/modules/measurements` in the following format:

```json
{"id":"esp32-sonic-xxxxxx","unit":"cm","distance":58}
```

## Hardware

Arduino code for the ESP.
We are using the model: ESP32-C3-DevKitM-1

made by:
- [<img src="https://github.com/TristanDeLil.png" width="25">Tristan De Lil](https://github.com/TristanDeLil)
- [<img src="https://github.com/r0755466.png" width="25">Rayan Azzi](https://github.com/r0755466)
- [<img src="https://github.com/aaronD14.png" width="25">Aaron Delplancq](https://github.com/aaronD14)

## importent note
This was all programmed on the Arduino IDE version 1.

## Libraries

Required libraries:
- Installing ESP32 Add-on in Arduino IDE
- PubSubClient for esp 32: [MQTT library link](https://github.com/plapointe6/EspMQTTClient)
- Freenova WS2812: [RGB led link](https://github.com/Freenove/Freenove_WS2812_Lib_for_ESP32)
- WIFI for arduino: [WIFI link](https://www.arduino.cc/reference/en/libraries/wifi/)
- ArduinoJSON

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

## PCB

<table>
  <tr>
    <td>Schematic</td>
     <td>KICAD schematic</td>
     <td>KICAD PCB editor</td>
  </tr>
  <tr>
    <td><img src="https://user-images.githubusercontent.com/71698092/209342793-e7d6afee-f2bd-4c9f-a332-e23ef9c96636.png" width="300" height="350"></td>
    <td><img src="https://user-images.githubusercontent.com/71698092/209343866-022a32b0-8deb-48b5-8e29-e62e0acbca16.jpg" width="300" height="350"></td>
    <td><img src="https://user-images.githubusercontent.com/71698092/209344002-be443e50-0a6f-45fd-8db9-eca0af2c7371.jpg" width="300" height="350"></td>
  </tr>
</table>

