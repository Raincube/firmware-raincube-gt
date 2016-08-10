#include "ESP8266.h"
#include <NewPing.h>
#include <SoftwareSerial.h>


#define PILA_PIN 8
#define CUBE_PIN 9
#define MAX_DISTANCE 200

#define SSID        "Mordor"
#define PASSWORD    "Gollum4408"
#define HOST_NAME   "54.86.114.164"
#define HOST_PORT   (3150)

NewPing pilaSonar(PILA_PIN, PILA_PIN, MAX_DISTANCE);
NewPing cubeSonar(CUBE_PIN, CUBE_PIN, MAX_DISTANCE);
SoftwareSerial mySerial(4, 5);
ESP8266 wifi(mySerial);

unsigned long previousMillis = 0; 
const long interval = 10000;

void setup() {
  Serial.begin(9600);
  pinMode(6, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);

  valveSwitchOFF();
    
  Serial.print("---------------------------------\r\n");
  Serial.print("RAINCUBE GUATEMALA PROTOTYPE v1.0\r\n");
  Serial.print("---------------------------------\r\n");
  Serial.print("\r\n");

  if (wifi.joinAP(SSID, PASSWORD)) {
    Serial.print("**********************\r\n");
    Serial.print("- JOIN AP SUCCESS!\r\n");
  } else {
    Serial.print("**********************\r\n");
    Serial.print("- JOIN AP FAILURE!\r\n");
  }

  if (wifi.disableMUX()) {
    Serial.print("- SINGLE CONNECTION OK\r\n");
  } else {
    Serial.print("- SINGLE CONNECTION ER\r\n");
  }

  Serial.print("- SETUP END\r\n");
  Serial.print("**********************\r\n");
  Serial.print("\r\n");

  disconnectTCPServer();
  connectTCPServer();

  digitalWrite(13, LOW);  
  digitalWrite(11, HIGH);
  
}

void loop() {

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    sendData();
  }

  uint8_t buffer[128] = {0};
  
  uint32_t len = wifi.recv(buffer, sizeof(buffer), 10000);
  
  if (len > 0) {
    Serial.print("- RECEIVED: ");
    for (uint32_t i = 0; i < len; i++) {
      Serial.print((char)buffer[i]);
    }

    Serial.print("\r\n");

    if ((char)buffer[0] == 'H') {
      valveSwitchON();
      Serial.println("- VALVE ON");
    } else if ((char)buffer[0] == 'F') {
      valveSwitchOFF();
      Serial.println("- VALVE OFF");
    }

    Serial.print("\r\n");
  }  
}


void connectTCPServer() {

  Serial.print("*********************\r\n");
  Serial.print("TCP SERVER CONNECTION\r\n");
  Serial.print("*********************\r\n");

  if (wifi.createTCP(HOST_NAME, HOST_PORT)) {
    Serial.print("- CREATE TCP OK!\r\n");
    delay(500);
  } else {
    Serial.print("- CREATE TCP ERROR!\r\n");
    delay(500);
  }
  
  Serial.print("*********************\r\n");
  Serial.print("\r\n");
  
}

void disconnectTCPServer() {
  
  Serial.print("************************\r\n");
  Serial.print("TCP SERVER DISCONNECTION\r\n");
  Serial.print("************************\r\n");

  if (wifi.releaseTCP()) {
    Serial.print("- RELEASE TCP OK!\r\n");
    delay(500);
  } else {
    Serial.print("- RELEASE TCP ER!\r\n");
    delay(500);
  }

  Serial.print("************************\r\n");
  Serial.print("\r\n");

}

void sendData() {
  
  String temp1 = String(getRaincubeLevel());
  String temp2 = String(getPilaLevel());
  
  String temp3 = "id=001&r=" + temp1 + "&p=" + temp2 + "&";
  int str_len = temp3.length();
  Serial.print("- ");
  Serial.println(temp3);
  char charBuf[50];
  temp3.toCharArray(charBuf, str_len);

  char *dataStream = charBuf;

  wifi.send((const uint8_t*)dataStream, strlen(dataStream));
  //delay(500);
  
}

unsigned int getRaincubeLevel() {
  unsigned int uS = cubeSonar.ping();
  return (uS/US_ROUNDTRIP_CM);
}
unsigned int getPilaLevel() {
  unsigned int uS = pilaSonar.ping();
  return (uS/US_ROUNDTRIP_CM);
}

void valveSwitchON() {
  digitalWrite(6, HIGH);
  digitalWrite(13, HIGH);
  digitalWrite(11, LOW);     
}

void valveSwitchOFF() {
  digitalWrite(6, LOW);
  digitalWrite(13, LOW);
  digitalWrite(11, HIGH);     
}


