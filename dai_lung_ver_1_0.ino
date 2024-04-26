#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include <string.h>
#include "addons/RTDBHelper.h"
#include <SimpleKalmanFilter.h>
SimpleKalmanFilter bo_loc(0.1, 0.1, 0.001);
#define WIFI_SSID "Phong 1"
#define WIFI_PASSWORD "annhuthung1"  
#define API_KEY "AIzaSyCWbf12yW-0lmc_rpE6JQYnIXdiV5BnVzk"
#define DATABASE_URL "https://iotproject-f9067-default-rtdb.firebaseio.com/"

#define sensorPin A0
#define vibrationPin 16 //pin D0
#define onlinePin 12
#define offlinePin 13

int x = 915;
int y = 930;
const int threshold = 50;
int count = 0;
int stt;

bool mode_rung = false;
int time_laymau = 0;
int time_warning = 0;
int counter = 0;
bool working = false;

unsigned long realtime = 0;
unsigned long timecount = 0;
float sensorValue;
int i = 0;
bool onlineWork;

void setup() {
  Serial.begin(115200);
  if (digitalRead(onlinePin)) {
    wifi_init();
    firebase_init();
    getDatasetup();
  }

  pinMode(A0,INPUT);
  pinMode(onlinePin,INPUT);
  pinMode(offlinePin,INPUT);
  pinMode(vibrationPin,OUTPUT);
  autoTurning();
  
  Serial.println("RESET");
  onlineWork = digitalRead(onlinePin);
}

void loop() {
  while (!digitalRead(onlinePin)){ //offlinework
    sensorValue = getSensorvalue();
    Serial.print("offline work: ");
    Serial.println(sensorValue);
    if (sensorValue >= threshold) {
      count++;
    } else {
      count = 0;
    }
    time_laymau = 1000;
    time_warning = 3000;
    if (count == 3){ // nếu biến đếm = 3 có nghĩa là 3s liên tiếp thì bật cảm biến rung
      Serial.println("CANH BAO!!!!!");
      timecount = millis();
      digitalWrite(vibrationPin,HIGH);
      count = 0;
    }
    if ((timecount + time_warning) < millis()) digitalWrite(vibrationPin,LOW);
    delay(1000);
  }

  while (!working && onlineWork){
    autoTurning();
    getDatasetup();
    digitalWrite(vibrationPin,LOW);
    i = counter;
  }

  while (working && onlineWork){
    realtime = millis();
    sensorValue = getSensorvalue();
    Serial.print("value is: ");
    Serial.println(sensorValue);
    
    if (sensorValue >= threshold) {
      count++;
    } else {
      count = 0;
    }
    time_laymau = 1000;
    if ((count == 3) && (mode_rung)){ // nếu biến đếm = 3 có nghĩa là 3s liên tiếp thì bật cảm biến rung
      Serial.println("CANH BAO!!!!!");
      timecount = millis();
      digitalWrite(vibrationPin,HIGH);
      count = 0;
    }
    while((realtime + time_laymau) > millis()){
      if ((timecount + time_warning) < millis()) digitalWrite(vibrationPin,LOW);
      Serial.print(".");
      delay(100);
    }
    Serial.println(".");
    sentCountertofirebase(i);
    sentDatatofirebase(i++, sensorValue);
    working = getData("/status/working");
    delay(100);
    onlineWork = digitalRead(onlinePin);
  }

  Serial.println("STOPPED");
  delay(10);
}


void getDatasetup() //lay cac thong so ve may
{
  working        =     getData("/status/working");
  mode_rung      =     getData("/status/mode_rung");
  time_laymau    =     getData("/status/timer_laymau");
  time_warning   =     getData("/status/timer_warning");
  counter        =     getData("/status/counter");
}

int getSensorvalue(){
    sensorValue = (map((int)analogRead(sensorPin),x,y,100,0));  
    sensorValue = (sensorValue > 0) ? sensorValue : 0;
    sensorValue = (sensorValue < 100) ? sensorValue : 100;
    //return bo_loc.updateEstimate(sensorValue);
    return sensorValue;
}

void autoTurning(){
  y = analogRead(sensorPin);
  x = y - 20;
}
