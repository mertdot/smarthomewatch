#include <TinyMPU6050.h>
#include "ESPAsyncWebServer.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

const int scl=5;
const int sda=4;

float pos_offset = 10;
float neg_offset = -10;
long angle_x, angle_y, angle_z, offset_x, offset_y, offset_z;

AsyncWebServer server(80);
MPU6050 mpu (Wire);

const char* ssid = "test";
const char* password = "8163264128";

String state="0";

String mp6050(){
   for(int i=0; i<5;i++){
  mpu.Execute();
  angle_x = mpu.GetAngX();
  angle_y = mpu.GetAngY();
  angle_z = mpu.GetAngZ();
  //delay(200);
 }
 
  if ( pos_offset < angle_x - offset_x || neg_offset > angle_x - offset_x || pos_offset < angle_y - offset_y || neg_offset > angle_y - offset_y || pos_offset < angle_z - offset_z || neg_offset > angle_z - offset_z){
for(int i=0; i<50; i++){
  digitalWrite(sda,HIGH);
  digitalWrite(scl,LOW);
  delay(100);
  digitalWrite(sda,LOW);
  digitalWrite(scl,LOW);
  delay(100);
  }   
  Serial.println("Deprem");
  state="1";
  delay(5000);  
  mpu.Execute();
  offset_x = mpu.GetAngX();
  offset_y = mpu.GetAngY();
  offset_z = mpu.GetAngZ();
 }
 return state;
  }


void setup() {
    Serial.begin(9600);
  // Connect to Wi-Fi
      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid, password);

        while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi..");
      }
      // Print ESP32 Local IP Address
      Serial.println(WiFi.localIP());

  // Initialization
  mpu.Initialize();

  // Calibration

  Serial.println("=====================================");
  Serial.println("Starting calibration...");
  Serial.println("Calibration complete!");
  Serial.println("Offsets:");
  Serial.print("AccX Offset = ");
  Serial.println(mpu.GetAccXOffset());
  Serial.print("AccY Offset = ");
  Serial.println(mpu.GetAccYOffset());
  Serial.print("AccZ Offset = ");
  Serial.println(mpu.GetAccZOffset());
  Serial.print("GyroX Offset = ");
  Serial.println(mpu.GetGyroXOffset());
  Serial.print("GyroY Offset = ");
  Serial.println(mpu.GetGyroYOffset());
  Serial.print("GyroZ Offset = ");
  Serial.println(mpu.GetGyroZOffset());
  pinMode(sda,OUTPUT);
  pinMode(scl, OUTPUT);
  digitalWrite(sda, LOW);
  digitalWrite(scl, LOW);

  delay(2000);
  for(int i=0; i<200;i++){
   mpu.Execute();
  offset_x = mpu.GetAngX();
  offset_y = mpu.GetAngY();
  offset_z = mpu.GetAngZ();
  delay(200);
  }
  Serial.print("offset_x = ");
  Serial.print(offset_x);
  Serial.print("  /  offsetY = ");
  Serial.print(offset_y);
  Serial.print("  /  offsetZ = ");
  Serial.println(offset_z);
  
}

void loop() {
state="0";

  
  mp6050();
  Serial.print("AngX = ");
  Serial.print(angle_x - offset_x);
  Serial.print("  /  AngY = ");
  Serial.print(angle_y - offset_y);
  Serial.print("  /  AngZ = ");
  Serial.println(angle_z - offset_z);

server.on("/earthquake", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", state.c_str());
  });
 server.begin();
 
}
