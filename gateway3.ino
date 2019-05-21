#define BAUDRATE 115200

#include <Console.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <PubSubClient.h>
#include "YunClient.h"
YunClient clientint;

// Singleton instance of the radio driver
RH_RF95 rf95;

int led = A2;
int led_Data = A0;  //Input dari MQTT
int led_Internet = A1;
int led_RF_in = A3; 
int led_RF_out = A4;

float frequency = 433.0;
#define TIMER 3000UL
unsigned long startTime;

//================================================


IPAddress server(156, 67, 214, 77);
char Conection_name[20] = "testMQTT";
char Username_conn[20] = "rahman";
char Password_conn[20] = "rahman"; 
char Subscribe[10] = "Command";
char Publish[10] = "RFINPUT";

//=================================================


////// MQTT
void callback(char* topic, byte* payload, unsigned int length) {
    payload[length] = '\0';
     String strTopic = String((char*)topic);
     String strSubs = String((char*)Subscribe);

    if (strTopic == strSubs) {
       analogWrite(led_Data,255);
      analogWrite(led_RF_out,255);
      Console.println((char *)payload);

        rf95.send((uint8_t *)payload, 100);
        rf95.waitPacketSent();
        Console.println("Send Command");
      
      analogWrite(led_RF_out,0);
       analogWrite(led_Data,0);
        
    }
}
 ////

PubSubClient client(server, 1883, callback, clientint);

void setup() 
{

  pinMode(led, OUTPUT);
  pinMode(led_Data, OUTPUT);
  pinMode(led_Internet, OUTPUT);
  pinMode(led_RF_in, OUTPUT);
  pinMode(led_RF_out, OUTPUT);

  analogWrite(led_Data,255);
  analogWrite(led_Internet,255);
  analogWrite(led_RF_in,255);
  analogWrite(led_RF_out,255);
 
 //Sign program active blink 5 times last still ON untill program ready       
 digitalWrite(led, HIGH);
     delay(200);
      digitalWrite(led, LOW);
      delay(200);
 digitalWrite(led, HIGH);
     delay(200);
      digitalWrite(led, LOW);
      delay(200);
 digitalWrite(led, HIGH);
     delay(200);
      digitalWrite(led, LOW);
      delay(200);
 digitalWrite(led, HIGH);
     delay(200);
      digitalWrite(led, LOW);
      delay(200);
       digitalWrite(led, HIGH);
     delay(200);
     
  digitalWrite(led, HIGH);
  Bridge.begin(BAUDRATE);
  if (!rf95.init()){
  }
  else{
    rf95.setFrequency(frequency);
    rf95.setTxPower(18);
    rf95.setSpreadingFactor(7);
    rf95.setSignalBandwidth(125000);
    rf95.setCodingRate4(5);
    analogWrite(led_RF_in,0);
    analogWrite(led_RF_out,0);
     analogWrite(led_Data,0);
 }

digitalWrite(led, LOW);
}

void loop()
{

 if (client.connect(Conection_name, Username_conn, Password_conn)) {
    client.subscribe(Subscribe);
       analogWrite(led_Internet,0);

      uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];    
      uint8_t len = sizeof(buf);

    if (rf95.available()){    
        if (rf95.recv(buf, &len)){
          analogWrite(led_RF_in,255);
          digitalWrite(led, HIGH);
          digitalWrite(led, LOW);
          client.publish(Publish,(char*)buf);
          delay(200);
          analogWrite(led_RF_in,0);
         
        }
        else
        {
            digitalWrite(led, HIGH);
        }
    }

 //MQTT Loop
        client.loop();
       

  }
  else {
      analogWrite(led_Internet,255);
  }

    
}
