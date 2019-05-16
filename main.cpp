#include <Arduino.h>
#include <Adafruit_MQTT.h>            
#include <Adafruit_MQTT_Client.h>     
#include <WiFi.h>      
#include <Servo.h>


#define relayPin 23                   //declaring pin for relay 
#define LED 25
#define WLAN_SSID "ric"          //assign to your WiFi name in place of SSID 
#define WLAN_PASS "********"          //Enter your WiFi Password here

#define AIO_SERVER       "io.adafruit.com" //IOT Server
#define AIO_SERVERPORT   1883               // use 8883 is used for non SSL
#define AIO_USERNAME     "khushboo145"       //Username given bu Adafruit IO
#define AIO_KEY          "25b8297579ae4a699c018e76b1f7a360"       // Secret key given by adafruit IO

Servo myservo;  // create servo object to control a servo       
WiFiClient client;                                                                                  // Create an ESP8266 WiFiClient class to connect to the MQTT server.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);              // Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/onoff");  // Setup a feed called 'onoff' for subscribing to changes.
int pos = 0;    // variable to store the servo position

void MQTT_connect();
void setup() 
{
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);  
  Serial.begin(9600);
  delay(10);
  Serial.println(); 
  Serial.println();
    myservo.attach(13);  // attaches the servo on pin 13 to the servo object

  Serial.print("Connecting to ");       // Connect to WiFi access point.
  Serial.println(WLAN_SSID);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: "); 
  Serial.println(WiFi.localIP());
  mqtt.subscribe(&onoffbutton);        // Setup MQTT subscription for onoff feed.
}

uint32_t x=0;
void loop() 
{
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();
  // this is our 'wait for incoming subscription packets' busy subloop
  // try to spend your time here
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) 
  {
    if (subscription == &onoffbutton) {
      Serial.print(F("Got: "));
      Serial.println((char *)onoffbutton.lastread);
      String response = (char*)onoffbutton.lastread; //converts the received 1 or 0 to string to compare in the if-else statement
      if (response == "1")
      {
        digitalWrite(relayPin, HIGH);
  }
      }
      else 
      {
        digitalWrite(relayPin, LOW);
      }
    }
  }
                                            

void MQTT_connect() 
{
  int8_t ret;
  if (mqtt.connected())     // Stop if already connected.
  {
    return;
  }
  Serial.print("Connecting to MQTT... ");
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) 
  { 
       Serial.println(mqtt.connectErrorString(ret));                  // connect will return 0 for connected
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);              // wait 5 seconds
       retries--;
       if (retries == 0) 
       {
         while (1);              // basically die and wait for WDT to reset me
       }
  }
  Serial.println("MQTT Connected!");
}