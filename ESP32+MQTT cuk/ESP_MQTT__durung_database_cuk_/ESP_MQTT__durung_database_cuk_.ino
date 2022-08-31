
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <TinyGPSPlus.h>

TinyGPSPlus gps;

const char* ssid="Bismillah";
const char* pass="enggatau";
const char* brokerUser = "madda";
const char* brokerPass = "kagawa585261";
const char* broker = "47.250.47.28";
const char* outTopic ="Displays/espnext01/out";
const char* inTopic ="Displays/espnext01/in";

static const int RXPin = 16, TXPin = 17;

WiFiClient espClient;
PubSubClient client(espClient);

char messages[50];

void setupWifi(){
  delay(100);
  Serial.print("\nConnecting to");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  
  while(WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
}

void reconnect(){
  while(!client.connected()){
    Serial.print("\nConncting to ");
    Serial.println(broker);
    if(client.connect("espnext01", brokerUser, brokerPass)){
      if(client.connect("espnext01")){
        Serial.print("\nConnected to ");
        Serial.println(broker);
        client.subscribe(inTopic);
      } else {
        Serial.println("\n Trying to reconnect");
        delay(1000);
      }
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length){
  Serial.print("Nachricht empfangen: ");
  Serial.println(topic);
  for(int i=0; i<length; i++){
    Serial.print((char) payload[i]);
  }
  Serial.println();
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial1.begin(9600,SERIAL_8N1,RXPin,TXPin);
  setupWifi();
  client.setServer(broker, 1883);
  client.setCallback(callback);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!client.connected()){
    reconnect();
  }
  client.loop();

  while (Serial1.available() > 0){
    if (gps.encode(Serial1.read()))
    {
      Serial.print(F("Location: ")); 
      if (gps.location.isValid())
      {
        snprintf(messages, 75, "1,%f,%f",gps.location.lat(),gps.location.lng());
        Serial.print(gps.location.lat(), 6);
        Serial.print(F(","));
        Serial.print(gps.location.lng(), 6);
        client.publish(outTopic, messages);
      }
      else
      {
        Serial.print(F("INVALID"));
      }

      Serial.println("");
      delay(1000);
    }
  }
}
