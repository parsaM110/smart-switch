#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoWebsockets.h>


const char* websockets_server_host = "http://spadwebsocket.runflare.run/ws/chat/lobby_room/"; //Enter server adress


using namespace websockets;


WiFiUDP Udp;

#define BUTTON_PIN1 4  // ESP32 pin GIOP14, which connected to button
#define BUTTON_PIN2 5  // ESP32 pin GIOP15, which connected to button

#define LED_PIN1    14  // ESP32 pin GIOP14, which connected to led
#define LED_PIN2    16  // ESP32 pin GIOP18, which connected to led
#define LED_PIN3    12  // ESP32 pin GIOP22, which connected to led
#define LED_PIN4    13  // ESP32 pin GIOP23, which connected to led


// The below are variables, which can be changed
int led_state1 = LOW;    // the current state of LED
int led_state2 = LOW;    // the current state of LED
int led_state3 = LOW;    // the current state of LED
int led_state4 = LOW;    // the current state of LED

int button_state1;       // the current state of button
int button_state2;       // the current state of button

int last_button_state1;  // the previous state of button
int last_button_state2;  // the previous state of button

WebsocketsClient client;

void setup() {

 Serial.begin(115200);               // initialize serial
  pinMode(BUTTON_PIN1, INPUT_PULLUP); // set ESP32 pin to input pull-up mode
  pinMode(LED_PIN1, OUTPUT);          // set ESP32 pin to output mode
  pinMode(LED_PIN3, OUTPUT);          // set ESP32 pin to output mode

  pinMode(BUTTON_PIN2, INPUT_PULLUP); // set ESP32 pin to input pull-up mode
  pinMode(LED_PIN2, OUTPUT);          // set ESP32 pin to output mode
  pinMode(LED_PIN4, OUTPUT);          // set ESP32 pin to output mode

  button_state1 = digitalRead(BUTTON_PIN1);
  button_state2 = digitalRead(BUTTON_PIN2);
  


 
  
  WiFi.mode(WIFI_STA);








  
  
 
}

void loop() {
  
        esptouch();
         Udp.parsePacket();
         if(client.available()) {
                      client.poll();
                      client.send("test");
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
  }
  if (last_button_state2 == HIGH && button_state2 == LOW) {
    Serial.println("The button2 is pressed");

    // toggle state of LED
    led_state2 = !led_state2;
    led_state4 = !led_state2;

    // control LED arccoding to the toggled state
    digitalWrite(LED_PIN2, led_state2);
    digitalWrite(LED_PIN4, led_state4);
  }
}

void esptouch(){

     if(WiFi.status() != WL_CONNECTED) {
    
       WiFi.beginSmartConfig();
       
           delay(1000);
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
        Serial.println("Not Connected!");
    }
    
    // run callback when messages are received
    client.onMessage([&](WebsocketsMessage message) {
        Serial.print("Got Message: ");
        Serial.println(message.data());
        if((message.data()== "{\"message\": \"1\"}") ){
           Serial.println("1");
           digitalWrite(LED_PIN1, HIGH);
           digitalWrite(LED_PIN3, LOW);
      }
      if(message.data()=="{\"message\": \"2\"}"){
           Serial.println("2");
           digitalWrite(LED_PIN1, LOW);
           digitalWrite(LED_PIN3, HIGH);
      } 
      if((message.data()=="{\"message\": \"3\"}") ){
           Serial.println("3");
           digitalWrite(LED_PIN2, HIGH);
           digitalWrite(LED_PIN4, LOW);
      } 
      if(message.data()=="{\"message\": \"4\"}"){
           Serial.println("4");
           digitalWrite(LED_PIN2, LOW);
           digitalWrite(LED_PIN4, HIGH);
      } 
    
    });
  
    }



  
