#include <SPI.h>
#include <EEPROM.h>
#include <RH_RF95.h>

#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 2


#define TIMER 8000UL
unsigned long startTime;




/////////////////////////////////

#define RF95_FREQ 433.0
#define LMP_CNTRL1 4
#define LMP_CNTRL2 5
#define LMP_CNTRL3 6
#define LMP_CNTRL4 7
#define LED_DATA 17

String ID_Device = "LA001";
int Status_lampu_1;  //save parameter
int Status_lampu_2;
int Status_lampu_3;
int Status_lampu_4;

/////////////////////////////////

//RH_RF95 rf95;
RH_RF95 rf95(RFM95_CS, RFM95_INT);

void load_parameter(){
  Serial.println("load Parameter");

  if (EEPROM.read(2) == '#') {
        Status_lampu_1=EEPROM.read(1);
    } 
    else {
     EEPROM.write(1, 0); 
     EEPROM.write(2, '#');
  }   

  if (EEPROM.read(4) == '#') {
        Status_lampu_2=EEPROM.read(3);
    } 
  else {
     EEPROM.write(3, 0); 
     EEPROM.write(4, '#');
  }

  if (EEPROM.read(6) == '#') {
        Status_lampu_3=EEPROM.read(5);
    } 
  else {
     EEPROM.write(5, 0); 
     EEPROM.write(6, '#');
  }

  if (EEPROM.read(8) == '#') {
        Status_lampu_4=EEPROM.read(7);
    } 
  else {
     EEPROM.write(7, 0); 
     EEPROM.write(8, '#');
  }

Serial.print("status lampu 1 : ");Serial.println(Status_lampu_1);
Serial.print("status lampu 2 : ");Serial.println(Status_lampu_2);
Serial.print("status lampu 3 : ");Serial.println(Status_lampu_3);
Serial.print("status lampu 4 : ");Serial.println(Status_lampu_4);
  //Default state OFF

    if(Status_lampu_1==1){
      digitalWrite(LMP_CNTRL1,HIGH);
    }else{
      digitalWrite(LMP_CNTRL1,LOW);
    }

    if(Status_lampu_2==1){
      digitalWrite(LMP_CNTRL2,HIGH);
    }else{
      digitalWrite(LMP_CNTRL2,LOW);
    }

    if(Status_lampu_3==1){
      digitalWrite(LMP_CNTRL3,HIGH);
    }else{
      digitalWrite(LMP_CNTRL3,LOW);
    }

    if(Status_lampu_4==1){
      digitalWrite(LMP_CNTRL4,HIGH);
    }else{
      digitalWrite(LMP_CNTRL4,LOW);
    }
}

void setup() 
{
  pinMode(RFM95_RST, OUTPUT);
  pinMode(LMP_CNTRL1, OUTPUT);
  pinMode(LMP_CNTRL2, OUTPUT);
  pinMode(LMP_CNTRL3, OUTPUT);
  pinMode(LMP_CNTRL4, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);


 
  Serial.begin(9600);
  delay(100);
  
  Serial.println("NODE VAN MILL V2.0");
  load_parameter();

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }

  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }

  rf95.setSpreadingFactor(7);
  rf95.setSignalBandwidth(125000);
  rf95.setCodingRate4(5);
  rf95.setTxPower(18);

  
  Serial.println("START");
  startTime = millis();
}

int8_t send_ack=0; // flag var

void Send_current_status(){
    digitalWrite(LED_DATA, HIGH);
    Serial.println("Send Current Status");
    String DatatoSendStr;
    char DatatoSend[100];
    DatatoSendStr =String("DAT#")+ ID_Device + String("#") + String(Status_lampu_1) +String("#") + String(Status_lampu_2) + String("#") + String(Status_lampu_3) +String("#") + String(Status_lampu_4);
    DatatoSendStr.toCharArray(DatatoSend, 100);
    Serial.println(DatatoSendStr);
    rf95.send((uint8_t *)DatatoSend, 100);
    delay(20);
    digitalWrite(LED_DATA, LOW);
}

void loop()
{


  if(rf95.available()){
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];    
    uint8_t len = sizeof(buf);
    if(rf95.recv(buf, &len)){
       digitalWrite(LED_DATA, HIGH);
       Serial.print("Receive: ");
       Serial.println((char*)buf);
       Serial.print("RSSI: ");
       Serial.println(rf95.lastRssi(), DEC);
       digitalWrite(LED_DATA, LOW);

        //Pharse Input  //LA001#1,2,3,4,A,#ON/OFF
        String id_in, target_lampu ,state_lampu;
        id_in = getValue((char*) buf, '#', 0);
        target_lampu = getValue((char*) buf, '#', 1);
        state_lampu = getValue((char*) buf, '#', 2);

        if(id_in == ID_Device){
            if(target_lampu=="1"){
                if (state_lampu == "OFF"){
                     digitalWrite(LMP_CNTRL1,LOW);
                     Status_lampu_1=0;
                     Serial.println("Turn OFF lampu 1");
                     EEPROM.write(1, 0);
                   }
                   else if (state_lampu == "ON"){
                     digitalWrite(LMP_CNTRL1,HIGH);
                     Serial.println("Turn ON lampu 1");
                     Status_lampu_1=1;
                     EEPROM.write(1, 1);
                   }
            }
            else if(target_lampu=="2"){
                 if (state_lampu == "OFF"){
                     digitalWrite(LMP_CNTRL2,LOW);
                     Status_lampu_2=0;
                     Serial.println("Turn OFF lampu 2");
                     EEPROM.write(3, 0);
                   }
                   else if (state_lampu == "ON"){
                     digitalWrite(LMP_CNTRL2,HIGH);
                     Serial.println("Turn ON lampu 2");
                     Status_lampu_2=1;
                     EEPROM.write(3, 1);
                   }
            }
            else if(target_lampu=="3"){
                 if (state_lampu == "OFF"){
                     digitalWrite(LMP_CNTRL3,LOW);
                     Status_lampu_3=0;
                     Serial.println("Turn OFF lampu 3");
                     EEPROM.write(5, 0);
                   }
                   else if (state_lampu == "ON"){
                     digitalWrite(LMP_CNTRL3,HIGH);
                     Serial.println("Turn ON lampu 3");
                     Status_lampu_3=1;
                     EEPROM.write(5, 1);
                   }
            }
            else if(target_lampu=="4"){
                 if (state_lampu == "OFF"){
                     digitalWrite(LMP_CNTRL4,LOW);
                     Status_lampu_4=0;
                     Serial.println("Turn OFF lampu 4");
                     EEPROM.write(7, 0);
                   }
                   else if (state_lampu == "ON"){
                     digitalWrite(LMP_CNTRL4,HIGH);
                     Serial.println("Turn ON lampu 4");
                     Status_lampu_4=1;
                     EEPROM.write(7, 1);
                   }
              
            }
            else if(target_lampu=="A"){
                if (state_lampu == "OFF"){
                     digitalWrite(LMP_CNTRL1,LOW);
                     Status_lampu_1=0;
                     EEPROM.write(1, 0);

                     digitalWrite(LMP_CNTRL2,LOW);
                     Status_lampu_2=0;
                     EEPROM.write(3, 0);

                     digitalWrite(LMP_CNTRL3,LOW);
                     Status_lampu_3=0;
                     EEPROM.write(5, 0);
                     
                     digitalWrite(LMP_CNTRL4,LOW);
                     Status_lampu_4=0;
                     Serial.println("Turn OFF ALL");
                     EEPROM.write(7, 0);
                   }
                   else if (state_lampu == "ON"){

                     digitalWrite(LMP_CNTRL1,HIGH);
                     Status_lampu_1=1;
                     EEPROM.write(1, 1);

                     digitalWrite(LMP_CNTRL2,HIGH);
                     Status_lampu_2=1;
                     EEPROM.write(3, 1);

                     digitalWrite(LMP_CNTRL3,HIGH);
                     Status_lampu_3=1;
                     EEPROM.write(5, 1);
                     
                     digitalWrite(LMP_CNTRL4,HIGH);
                     Serial.println("Turn ON ALL");
                     Status_lampu_4=1;
                     EEPROM.write(7, 1);
                   }
            }
        

          Send_current_status();
        }
        else{
          Serial.println("Command tidak di olah");
        }
          
    }
    else{
      Serial.println("Receive failed");
    }
  }
  else{
    //Serial.println("blank");
    delay(250);
  }


  if (millis() - startTime > TIMER)
  {
    Send_current_status();
    startTime = millis();
  }
      
  
}

String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
