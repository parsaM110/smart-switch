#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoWebsockets.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include "DHT.h"
#include <EEPROM.h>

const char* websockets_server_host = "http://spadwebsocket.runflare.run/ws/chat/lobby_room/"; //Enter server adress


using namespace websockets;

unsigned long previousMillis = 0;  //will store last time LED was blinked
const long period = 6000;

WiFiUDP Udp;

//--------------------------------------------
#define WiFi_rst 0                  //WiFi credential reset pin (Flash button on ESP8266)
unsigned long rst_millis;
//--------------------------------------------
String ssid;                        //string variable to store ssid
String pss;                         //string variable to store password
//-----------------------------------------

#define BUTTON_PIN1 5  // ESP32 pin GIOP14, which connected to button
#define BUTTON_PIN2 4  // ESP32 pin GIOP15, which connected to button

#define LED_PIN1    14  // ESP32 pin GIOP14, which connected to led
#define LED_PIN2    16  // ESP32 pin GIOP18, which connected to led
#define LED_PIN3    13  // ESP32 pin GIOP22, which connected to led
#define LED_PIN4    12  // ESP32 pin GIOP23, which connected to led


// The below are variables, which can be changed
int led_state1 = HIGH;    // the current state of LED
int led_state2 = HIGH;    // the current state of LED
int led_state3 = LOW;    // the current state of LED
int led_state4 = LOW;    // the current state of LED

int button_state1;       // the current state of button
int button_state2;       // the current state of button

int last_button_state1;  // the previous state of button
int last_button_state2;  // the previous state of button

WebsocketsClient client;

#define DHTPIN 2     // Digital pin connected to the DHT sensor

#define DHTTYPE DHT11   // DHT 11


DHT dht(DHTPIN, DHTTYPE);
 
void setup() {

 Serial.begin(115200);               // initialize serial

  //-----------------------------------------
  pinMode(WiFi_rst, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  EEPROM.begin(200);
 
//-----------------------------------------

  pinMode(BUTTON_PIN1, INPUT); // set ESP32 pin to input pull-up mode
  pinMode(LED_PIN1, OUTPUT);          // set ESP32 pin to output mode
  pinMode(LED_PIN3, OUTPUT);          // set ESP32 pin to output mode

  pinMode(BUTTON_PIN2, INPUT); // set ESP32 pin to input pull-up mode
  pinMode(LED_PIN2, OUTPUT);          // set ESP32 pin to output mode
  pinMode(LED_PIN4, OUTPUT);          // set ESP32 pin to output mode

  button_state1 = digitalRead(BUTTON_PIN1);
  button_state2 = digitalRead(BUTTON_PIN2);
  
    digitalWrite(LED_PIN1, HIGH);
    digitalWrite(LED_PIN2, HIGH);
    digitalWrite(LED_PIN3, LOW);
    digitalWrite(LED_PIN4, LOW);
  


   dht.begin();



   Serial.println("Reading EEPROM ssid");
 
    
    for (int i = 0; i < 32; ++i)
    {
      ssid += char(EEPROM.read(i));
    }
    Serial.print("SSID = ");
    Serial.println(ssid);
    Serial.println("Reading EEPROM pass");
 
    for (int i = 32; i < 96; ++i)
    {
      pss += char(EEPROM.read(i));
    }
    Serial.print("pass = ");
    Serial.println(pss);
  
//-----------------------------------------
  
    WiFi.begin(ssid.c_str(), pss.c_str());

    delay(5000);


    //-----------------------------------------

  if (WiFi.status() != WL_CONNECTED) // if WiFi is not connected
  {
  
    WiFi.mode(WIFI_STA);
    WiFi.beginSmartConfig();
//------------------------------------------
  Serial.println("waiting for smartConfig");
  while(!WiFi.smartConfigDone()) {
    delay(500);
    Serial.print(".");
  }
//------------------------------------------
   Serial.println("");
   Serial.println("SmartConfig Successsssss");
  

    //WiFi.printDiag(Serial);
  ssid = WiFi.SSID();
  pss = WiFi.psk();
  Serial.print("SSID:");
  Serial.println(ssid);
  Serial.print("PSS:");
  Serial.println(pss);
  Serial.println("writing eeprom ssid:");
        for (int i = 0; i < ssid.length(); ++i)
        {
          EEPROM.write(i, ssid[i]);
          Serial.print("Wrote: ");
          Serial.println(ssid[i]);
        }
        EEPROM.write( ssid.length(), '\0');
        Serial.println("writing eeprom pass:");
        for (int i = 0; i < pss.length(); ++i)
        {
          EEPROM.write(32 + i, pss[i]);
          Serial.print("Wrote: ");
          Serial.println(pss[i]);
        }
        EEPROM.write(32 + pss.length(), '\0');
        EEPROM.commit();
 








  
  
 
}

void loop() {

  
   // save the last time you blinked the LED
   if(client.available()) {
                   int humid = dht.readHumidity(); 
                   int temp = dht.readTemperature();
                   unsigned long currentMillis = millis(); // store the current time
                    if (currentMillis - previousMillis >= period) { // check if 1000ms passed
                     previousMillis = currentMillis;
                     client.send("temp :" + String(temp));
                     client.send("humid :" + String(humid));
                    }   
                     

                      client.poll();
                   }
  
         esptouch();
         Udp.parsePacket();



  rst_millis = millis();
  while (digitalRead(WiFi_rst) == LOW){
  
   }
  // check the button press time if it is greater than 3sec clear wifi cred and restart ESP 
  if (millis() - rst_millis >= 3000){
  digitalWrite(LED_BUILTIN, LOW);
  for (int i = 0; i < 96; ++i)
        {
          EEPROM.write(i, 0);
        }
  EEPROM.commit();
  delay(5000);
  ESP.restart();
  }
        



          

 

  last_button_state1 = button_state1;      // save the last state
  button_state1 = digitalRead(BUTTON_PIN1); // read new state

  last_button_state2 = button_state2;      // save the last state
  button_state2 = digitalRead(BUTTON_PIN2); // read new state

  if (last_button_state1 == HIGH && button_state1 == LOW) {
    Serial.println("The button1 is pressed");

    // toggle state of LED
    led_state1 = !led_state1;
    led_state3 = !led_state1;
    // control LED arccoding to the toggled state
    digitalWrite(LED_PIN1, led_state1);
    digitalWrite(LED_PIN3, led_state3);


      if(led_state1 == HIGH){
      client.send("1");
    }
    else{
      client.send("2");
      }

    
  }
  if (last_button_state2 == HIGH && button_state2 == LOW) {
    Serial.println("The button2 is pressed");

    // toggle state of LED
    led_state2 = !led_state2;
    led_state4 = !led_state2;

    // control LED arccoding to the toggled state
    digitalWrite(LED_PIN2, led_state2);
    digitalWrite(LED_PIN4, led_state4);

    if(led_state2 == HIGH){
      client.send("3");
    }
    else{
      client.send("4");
      }
    }
  }


void esptouch(){

     if(WiFi.status() != WL_CONNECTED) {
    
       WiFi.beginSmartConfig();
       
          
           if(WiFi.smartConfigDone()){
             Serial.println("SmartConfig Success");
             WiFi.printDiag(Serial);
             
       
  
             // Start the server
                 Udp.begin(49999);
                 websocketfunction();
                
                 
             }
       }
       
  }







  void websocketfunction(){
    bool connected = client.connect(websockets_server_host);
    if(connected) {
        Serial.println("Connecetd!");
       client.send("Hello Server");
    } else {
        //Serial.println("Not Connected!");
    }
    
    // run callback when messages are received
    client.onMessage([&](WebsocketsMessage message) {
        Serial.print("Got Message: ");
        Serial.println(message.data());
        if((message.data()== "{\"message\": \"1\"}") ){
           Serial.println("1");
           digitalWrite(LED_PIN1, LOW);
           digitalWrite(LED_PIN3, HIGH);
           
      }
      if(message.data()=="{\"message\": \"2\"}"){
           Serial.println("2");
           digitalWrite(LED_PIN1, HIGH);
           digitalWrite(LED_PIN3, LOW);
      } 
      if((message.data()=="{\"message\": \"3\"}") ){
           Serial.println("3");
           digitalWrite(LED_PIN2, LOW);
           digitalWrite(LED_PIN4, HIGH);
      } 
      if(message.data()=="{\"message\": \"4\"}"){
           Serial.println("4");
           digitalWrite(LED_PIN2, HIGH);
           digitalWrite(LED_PIN4, LOW);
      } 
    
    });
  
    }


    
