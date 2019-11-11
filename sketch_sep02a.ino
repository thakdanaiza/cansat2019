#include <SFE_BMP180.h>
#include <Wire.h>
#include <SPI.h>
#include <LoRa.h>
#include <LSM303.h>
#include<Arduino.h>
#include "SSD1306.h"
#include "Adafruit_L3GD20.h"

SSD1306  display(0x3c, 4, 15);
SFE_BMP180 pressure;
LSM303 compass;

#define SS      18
#define RST     14
#define DI0     26
#define BAND    433E6  //915E6 
#define ALTITUDE 1655.0

Adafruit_L3GD20 gyro;
char report[80];
int counter = 0;

void setup()
{
  double T,P,p0,a;
  Serial.begin(115200);
  Serial.println("REBOOT");
  compass.init();
  compass.enableDefault();
  LoRa.beginPacket();
  if (pressure.begin())
  {
    //LoRa.print("BMP180 init success");
    Serial.println("BMP180 init success");
  }
  else
  {
    //LoRa.print("BMP180 init fail\n\n");
    Serial.println("BMP180 init fail\n\n");
    while(1); // Pause forever.
  }
  
  if (!gyro.begin(gyro.L3DS20_RANGE_250DPS))
  {
    //LoRa.print("Oops ... unable to initialize the L3GD20. Check your wiring!");
    Serial.println("Oops ... unable to initialize the L3GD20. Check your wiring!");
    while (1);
  }
  else
  {
    //LoRa.print("L3DS20 success");
    Serial.println("L3DS20 success");
  }
  
  SPI.begin(5,19,27,18);
  LoRa.setPins(SS,RST,DI0);
  Serial.println("LoRa Sender");
  if (!LoRa.begin(BAND)) {
    //LoRa.print("Starting LoRa failed!");
    Serial.println("Starting LoRa failed!");
    while (1);
  }
 // LoRa.print("LoRa Initial OK!");
  Serial.println("LoRa Initial OK!");
}

void loop()
{
  char status;
  double T,P,p0,a;
  gyro.read();
  LoRa.beginPacket();
  LoRa.print(counter);
  LoRa.print("/");
  LoRa.print(ALTITUDE,0);
  LoRa.print("/");
  status = pressure.startTemperature();
  status = pressure.getTemperature(T);
  LoRa.print(T,2);
  LoRa.print("/");
  status = pressure.startPressure(3);
  status = pressure.getPressure(P,T);
  LoRa.print(P,2);
  LoRa.print("/");
  p0 = pressure.sealevel(P,ALTITUDE);
  a = pressure.altitude(P,p0);
  LoRa.print(a,0);
  LoRa.print("/");
  LoRa.print((int)gyro.data.x);
  LoRa.print("/");
  LoRa.print((int)gyro.data.y);
  LoRa.print("/");
  LoRa.print((int)gyro.data.z);
  LoRa.endPacket();
  counter++;
  Serial.println(counter);
  delay(1000);
}
