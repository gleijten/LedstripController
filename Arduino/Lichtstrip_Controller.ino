// ========================================
// 2 channel RGBWW ledstrip wifi controller for HomeAssistant
// designed for Arduino GIGA R1 (M4)Board
// by G.L. 2024
// ========================================

//check hardware
#ifndef ARDUINO_GIGA   // load only if correct hardware board is selected in "Tools - Board manager"
#error Wrong board. Please choose "Arduino Giga R1 board"
#endif

#include <my_secrets.h>     // passwords for network
#include <WiFi.h>           // onboard wifi library
#include <ArduinoHA.h>      // homeassistant library

//activate for debugging. Activate/deactivate various debug options elsewhere in programming
// #define DEBUG_network
// #define DEBUG_variables


#define TESTLED_PIN      50 // led for testing connection
#define WIFIon_PIN       51 // indicate Wifi connected
//define all pins for RGBWW ledstrip.channel1
#define RED_PIN1         2
#define GREEN_PIN1       3
#define BLUE_PIN1        4
#define WARMWHITE_PIN1   6
#define COLDWHITE_PIN1   7
//define all pins for RGBWW ledstrip.channel2
#define RED_PIN2         8
#define GREEN_PIN2       9
#define BLUE_PIN2        10
#define WARMWHITE_PIN2   12
#define COLDWHITE_PIN2   13

const char* ssid = SECRET_SSID;
const char* password = SECRET_PASSWORD;
const char* mqtt_server = SECRET_MQTT_SERVER;  // I use the HomeAssistant MQTT server
const char* mqtt_user = SECRET_MQTT_USER;
const char* mqtt_passw = SECRET_MQTT_PASSWORD;

boolean lightON1 = true;    // for switching on/off CH1
boolean lightON2 = true;    // for switching on/off CH2
boolean useInvert = false;   // when using driver transistors in hardware
boolean syncON = false;     // false when CH2 is not in sync with CH1 (default)

// variables to remember start of color adjustment
int redStart1 = 0;
int greenStart1 = 255;     // show green color on ledstrip after startup
int blueStart1 = 0;
int wwhiteStart1 = 0;
int cwhiteStart1 =0;
int brightnessStart1 = 20;

int redStart2 = 0;
int greenStart2 = 255;     // show green color on ledstrip after startup
int blueStart2 = 0;
int wwhiteStart2 = 0;
int cwhiteStart2 =0;
int brightnessStart2 = 20;

//variables for executing
int redVal1 = redStart1;
int greenVal1 = greenStart1;
int blueVal1 = blueStart1;
int wwhiteVal1 = wwhiteStart1;
int cwhiteVal1 = cwhiteStart1;
int brightnessVal1 = brightnessStart1;

int redVal2 = redStart2;
int greenVal2 = greenStart2;
int blueVal2 = blueStart2;
int wwhiteVal2 = wwhiteStart2;
int cwhiteVal2 = cwhiteStart2;
int brightnessVal2 = brightnessStart2;

WiFiClient client;
HADevice device("LedStripController1");   // MQTT unique devicename for registering in HomeAssistant
HAMqtt mqtt(client, device);

// "led" is unique ID of the switch. You can define your own ID.
HASwitch led("testled");    // MQTT device childname for testled switch
HASwitch sync("syncchannel");    // MQTT device childname for CHANNEL sync switch

// HALight::BrightnessFeature enables support for setting brightness of the light.
// HALight::ColorTemperatureFeature enables support for setting color temperature of the light.
// Both features are optional and you can remove them if they're not needed.
// "light1" and "light2" are unique ID's of the lights. You can define your own ID's.
HALight light1("ledStrip_CH1", HALight::BrightnessFeature | HALight::ColorTemperatureFeature | HALight::RGBFeature);
HALight light2("ledStrip_CH2", HALight::BrightnessFeature | HALight::ColorTemperatureFeature | HALight::RGBFeature);

//====================================================================== DEBUG
// void printWifiData() is only necessary for debugging
#ifdef DEBUG_network
  void printWifiData() {
    // print the SSID of the network you're attached to:
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

    // print your WiFi shield's IP address:
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);

    // print your MAC address:
    byte mac[6];
    WiFi.macAddress(mac);
    Serial.print("MAC address: ");
    Serial.print(mac[5], HEX);
    Serial.print(":");
    Serial.print(mac[4], HEX);
    Serial.print(":");
    Serial.print(mac[3], HEX);
    Serial.print(":");
    Serial.print(mac[2], HEX);
    Serial.print(":");
    Serial.print(mac[1], HEX);
    Serial.print(":");
    Serial.println(mac[0], HEX);
  }
#endif

// void printRGBWWColors() is only necessary for debugging
#ifdef DEBUG_variables
  void printRGBWWColors(int r,int g,int b,int ww,int cw){
  // Print color values for debugging
      Serial.print("Red");
      Serial.print("\t");
      Serial.print("Green");
      Serial.print("\t");
      Serial.print("Blue");
      Serial.print("\t");
      Serial.print("W-white");
      Serial.print("\t");
      Serial.println("C-white");

      Serial.print(r);
      Serial.print("\t");
      Serial.print(g);
      Serial.print("\t");
      Serial.print(b);
      Serial.print("\t");
      Serial.print(ww);
      Serial.print("\t");
      Serial.println(cw);
  }
#endif
//====================================================================== END DEBUG

void onStateCommand1(bool state, HALight* sender) {
    Serial.print("State1: ");
    Serial.println(state);
    if (state == 1){
      lightON1 = true;
    }
    else{ 
      lightON1 = false;
    }
    sender->setState(state); // report state back to the Home Assistant
    sender->setBrightness(brightnessVal1);
}
void onStateCommand2(bool state, HALight* sender) {
    Serial.print("State2: ");
    Serial.println(state);
    if (state == 1){
      lightON2 = true;
    }
    else{ 
      lightON2 = false;
    }
    sender->setState(state); // report state back to the Home Assistant
    sender->setBrightness(brightnessVal2);
}

void onBrightnessCommand1(uint8_t brightness1, HALight* sender) {
    // temp variables for color brightness adjustment
    float redTemp;
    float greenTemp;
    float blueTemp;
    float brightnessTemp;

    // 
    redTemp = redStart1;
    greenTemp = greenStart1;
    blueTemp = blueStart1;
    brightnessTemp = brightness1;
    
    redVal1 = round(redTemp*(brightnessTemp/255));
    greenVal1 = round(greenTemp*(brightnessTemp/255));
    blueVal1 = round(blueTemp*(brightnessTemp/255));
    brightnessVal1 = brightnessTemp;

    #ifdef DEBUG_variables    // Print brightness values for debugging
      Serial.print("Brightness: ");
      Serial.println(round((brightnessTemp/255)*100));

      Serial.println("Start values RGB CH1:");
      printRGBWWColors(redTemp,greenTemp,blueTemp,NULL,NULL);
      Serial.println("Color values CH1:");
      printRGBWWColors(redVal1,greenVal1,blueVal1,wwhiteVal1,cwhiteVal1);
    #endif
    
    sender->setBrightness(brightness1); // report brightness back to the Home Assistant
}

void onBrightnessCommand2(uint8_t brightness2, HALight* sender) {
    // temp variables for color brightness adjustment
    float redTemp;
    float greenTemp;
    float blueTemp;
    float brightnessTemp;

    // 
    redTemp = redStart2;
    greenTemp = greenStart2;
    blueTemp = blueStart2;
    brightnessTemp = brightness2;
    
    redVal2 = round(redTemp*(brightnessTemp/255));
    greenVal2 = round(greenTemp*(brightnessTemp/255));
    blueVal2 = round(blueTemp*(brightnessTemp/255));
    brightnessVal2 = brightnessTemp;

    #ifdef DEBUG_variables    // Print brightness values for debugging
      Serial.print("Brightness: ");
      Serial.println(round((brightnessTemp/255)*100));

      Serial.println("Start values RGB CH2:");
      printRGBWWColors(redTemp,greenTemp,blueTemp,NULL,NULL);
      Serial.println("Color values CH2:");
      printRGBWWColors(redVal2,greenVal2,blueVal2,wwhiteVal2,cwhiteVal2);
    #endif
    
    sender->setBrightness(brightness2); // report brightness back to the Home Assistant
}

void onColorTemperatureCommand1(uint16_t temperature, HALight* sender) {
      //temperature value = between 153-500
      wwhiteVal1 =  map(temperature, 153, 500, 0, 255);
      cwhiteVal1 =  map(temperature, 500, 153, 0, 255);

      redVal1 = 0;
      greenVal1 = 0;
      blueVal1 = 0;
    
    #ifdef DEBUG_variables  // Print temperature values for debugging
      Serial.print("Color temperature CH1: ");
      Serial.println(temperature);
      Serial.println("Color values CH1:");
      printRGBWWColors(redVal1,greenVal1,blueVal1,wwhiteVal1,cwhiteVal1);
    #endif

    sender->setColorTemperature(temperature); // report color temperature back to the Home Assistant
    sender->setBrightness(brightnessVal1);
}

void onColorTemperatureCommand2(uint16_t temperature, HALight* sender) {
      //temperature value = between 153-500
      wwhiteVal2 =  map(temperature, 153, 500, 0, 255);
      cwhiteVal2 =  map(temperature, 500, 153, 0, 255);

      redVal2 = 0;
      greenVal2 = 0;
      blueVal2 = 0;
    
    #ifdef DEBUG_variables  // Print temperature values for debugging
      Serial.print("Color temperature CH2: ");
      Serial.println(temperature);
      Serial.println("Color values CH2:");
      printRGBWWColors(redVal2,greenVal2,blueVal2,wwhiteVal2,cwhiteVal2);
    #endif

    sender->setColorTemperature(temperature); // report color temperature back to the Home Assistant
    sender->setBrightness(brightnessVal2);
}

void onRGBColorCommand1(HALight::RGBColor color, HALight* sender) {

    float redTemp;
    float greenTemp;
    float blueTemp;
    float brightnessTemp;
  
    redTemp = color.red;
    greenTemp = color.green;
    blueTemp = color.blue;
    brightnessTemp = brightnessVal1;

    //remember original color settings
    redStart1 = redTemp;
    greenStart1 = greenTemp;
    blueStart1 = blueTemp;

    //
    redVal1 = round(redTemp*(brightnessTemp/255));
    greenVal1 = round(greenTemp*(brightnessTemp/255));
    blueVal1 = round(blueTemp*(brightnessTemp/255));
    wwhiteVal1 =  0;
    cwhiteVal1 =  0;
    
    #ifdef DEBUG_variables  // Print color values for debugging
      Serial.println("new Colors RGB CH1: ");
      printRGBWWColors(color.red,color.green,color.blue,NULL,NULL);
    
      Serial.println("Color values CH1:");
      printRGBWWColors(redVal1,greenVal1,blueVal1,wwhiteVal1,cwhiteVal1);
  
      Serial.println("Start values CH1:");
      printRGBWWColors(redStart1,greenStart1,blueStart1,wwhiteVal1,cwhiteVal1);
      
    #endif

    sender->setRGBColor(color); // report color back to the Home Assistant
}

void onRGBColorCommand2(HALight::RGBColor color, HALight* sender) {

    float redTemp;
    float greenTemp;
    float blueTemp;
    float brightnessTemp;
  
    redTemp = color.red;
    greenTemp = color.green;
    blueTemp = color.blue;
    brightnessTemp = brightnessVal1;

    //remember original color settings
    redStart2 = redTemp;
    greenStart2 = greenTemp;
    blueStart2 = blueTemp;

    //
    redVal2 = round(redTemp*(brightnessTemp/255));
    greenVal2 = round(greenTemp*(brightnessTemp/255));
    blueVal2 = round(blueTemp*(brightnessTemp/255));
    wwhiteVal2 =  0;
    cwhiteVal2 =  0;
    
    #ifdef DEBUG_variables  // Print color values for debugging
      Serial.println("new Colors RGB CH2: ");
      printRGBWWColors(color.red,color.green,color.blue,NULL,NULL);
    
      Serial.println("Color values CH2:");
      printRGBWWColors(redVal2,greenVal2,blueVal2,wwhiteVal2,cwhiteVal2);
  
      Serial.println("Start values CH2:");
      printRGBWWColors(redStart2,greenStart2,blueStart2,wwhiteVal2,cwhiteVal2);
      
    #endif

    sender->setRGBColor(color); // report color back to the Home Assistant
}

void reconnect2Wifi(){
    // Serial.print("Attempting to reconnect to SSID: ");
    // Serial.println(ssid);
    WiFi.begin(ssid, password);
    // wait 10 seconds for connection:
    delay(10000);
}


void onSwitchCommand(bool state, HASwitch* sender)
{
    digitalWrite(TESTLED_PIN, (state ? HIGH : LOW));
    #ifdef DEBUG_variables  // Print values for debugging
      Serial.print("Testled: ");
      Serial.println(state);
    #endif
    sender->setState(state); // report state back to the Home Assistant
}

// Function to set sync on CH1&CH2 controlled by CH1
void onSyncCommand(bool state, HASwitch* sender)
{
    syncON = !syncON;
    #ifdef DEBUG_variables  // Print values for debugging
      Serial.print("Sync State: ");
      Serial.println(state);
    #endif
    sender->setState(state); // report state back to the Home Assistant
}

// Function to invert pin output when using driver transistor in hardware.
int invert(int val)
{
    int newVal;
    if(useInvert)
    {
      newVal = map(val,0,255,255,0);
    }
    else
    {
      newVal = val;
    }
    return(newVal);
}

void setup() {
   
    Serial.begin(115200);
    Serial.println("Starting...");

    // set up LED for Wificontrol en test
    pinMode(TESTLED_PIN, OUTPUT);
    digitalWrite(TESTLED_PIN, LOW);
    pinMode(WIFIon_PIN, OUTPUT);
    digitalWrite(WIFIon_PIN, LOW);

    // set up LEDs channel1
    pinMode(RED_PIN1, OUTPUT);
    pinMode(GREEN_PIN1, OUTPUT);
    pinMode(BLUE_PIN1, OUTPUT);
    pinMode(WARMWHITE_PIN1, OUTPUT);
    pinMode(COLDWHITE_PIN1, OUTPUT);
    // set up LEDs channel2
    pinMode(RED_PIN2, OUTPUT);
    pinMode(GREEN_PIN2, OUTPUT);
    pinMode(BLUE_PIN2, OUTPUT);
    pinMode(WARMWHITE_PIN2, OUTPUT);
    pinMode(COLDWHITE_PIN2, OUTPUT);

    // set MQTT device's details (optional)
    device.setName("Ledstrip_Gang");        // devicename in HA
    device.setSoftwareVersion("1.0.0");     // device info
    device.setManufacturer("Arduino");
    device.setModel("GigaR1");

    device.enableSharedAvailability();      // to track online/offline states of devices and device types
    device.enableLastWill();                // the Last Will and Testament feature offers a solution to respond effectively to ungraceful disconnects.

    sync.setName("SyncChannels");                    // switch name in HA
    sync.onCommand(onSyncCommand);           // handle switch state for Sync
    led.setName("TestLED");                 // switch name in HA
    led.onCommand(onSwitchCommand);         // handle switch state for TestLED

    light1.setName("Lichtkanaal_1");        // light name in HA
    light1.onStateCommand(onStateCommand1);
    light1.onBrightnessCommand(onBrightnessCommand1);
    light1.onColorTemperatureCommand(onColorTemperatureCommand1);
    light1.onRGBColorCommand(onRGBColorCommand1);

    light2.setName("Lichtkanaal_2");         // light name in HA
    light2.onStateCommand(onStateCommand2);
    light2.onBrightnessCommand(onBrightnessCommand2);
    light2.onColorTemperatureCommand(onColorTemperatureCommand2);
    light2.onRGBColorCommand(onRGBColorCommand2);

    // connect to wifi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      #ifdef DEBUG_network  // debug network
        Serial.print(".");
      #endif
      delay(500); // waiting for the connection
    }

    #ifdef DEBUG_network     // debug network
      Serial.println();
      Serial.println("Connected to the network");
      printWifiData();
    #endif

    delay (3000);
    // connect to MQTT server after 3 sec delay
    mqtt.begin(mqtt_server, mqtt_user, mqtt_passw);

}
    
void loop() {
    
    while (WiFi.status() != WL_CONNECTED){
      digitalWrite(WIFIon_PIN, LOW);
      reconnect2Wifi();
    }

    if (WiFi.status() == WL_CONNECTED) digitalWrite(WIFIon_PIN, HIGH);  // light up led is Wifi is OK
    
    mqtt.loop();

    if (syncON){        // both channels together controlled via CH1
      if (lightON1) {
        analogWrite(RED_PIN1, invert(redVal1));
        analogWrite(GREEN_PIN1, invert(greenVal1));
        analogWrite(BLUE_PIN1, invert(blueVal1));
        analogWrite(WARMWHITE_PIN1, invert(wwhiteVal1));
        analogWrite(COLDWHITE_PIN1, invert(cwhiteVal1));
        
        analogWrite(RED_PIN2, invert(redVal1));
        analogWrite(GREEN_PIN2, invert(greenVal1));
        analogWrite(BLUE_PIN2, invert(blueVal1));
        analogWrite(WARMWHITE_PIN2, invert(wwhiteVal1));
        analogWrite(COLDWHITE_PIN2, invert(cwhiteVal1));
      }
      else{
        //turn off all leds
        analogWrite(RED_PIN1, invert(0));
        analogWrite(GREEN_PIN1, invert(0));
        analogWrite(BLUE_PIN1, invert(0));
        analogWrite(WARMWHITE_PIN1, invert(0));
        analogWrite(COLDWHITE_PIN1, invert(0));

        analogWrite(RED_PIN2, invert(0));
        analogWrite(GREEN_PIN2, invert(0));
        analogWrite(BLUE_PIN2, invert(0));
        analogWrite(WARMWHITE_PIN2, invert(0));
        analogWrite(COLDWHITE_PIN2, invert(0));
      }
    }
   
    if (!syncON){
      if (lightON1) {
        analogWrite(RED_PIN1, invert(redVal1));
        analogWrite(GREEN_PIN1, invert(greenVal1));
        analogWrite(BLUE_PIN1, invert(blueVal1));
        analogWrite(WARMWHITE_PIN1, invert(wwhiteVal1));
        analogWrite(COLDWHITE_PIN1, invert(cwhiteVal1));
      }
      else{
        //turn off all leds
        analogWrite(RED_PIN1, invert(0));
        analogWrite(GREEN_PIN1, invert(0));
        analogWrite(BLUE_PIN1, invert(0));
        analogWrite(WARMWHITE_PIN1, invert(0));
        analogWrite(COLDWHITE_PIN1, invert(0));
      }

      if (lightON2 && !syncON) {
        analogWrite(RED_PIN2, invert(redVal2));
        analogWrite(GREEN_PIN2, invert(greenVal2));
        analogWrite(BLUE_PIN2, invert(blueVal2));
        analogWrite(WARMWHITE_PIN2, invert(wwhiteVal2));
        analogWrite(COLDWHITE_PIN2, invert(cwhiteVal2));
      }
      else{
        //turn off all leds
        analogWrite(RED_PIN2, invert(0));
        analogWrite(GREEN_PIN2, invert(0));
        analogWrite(BLUE_PIN2, invert(0));
        analogWrite(WARMWHITE_PIN2, invert(0));
        analogWrite(COLDWHITE_PIN2, invert(0));
      }
    }
}
