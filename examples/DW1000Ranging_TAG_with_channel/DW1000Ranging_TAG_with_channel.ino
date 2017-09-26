/**
 * 
 * @todo
 *  - move strings to flash (less RAM consumption)
 *  - fix deprecated convertation form string to char* startAsTag
 *  - give example description
 */
#include <SPI.h>
#include "DW1000Ranging.h"

// connection pins ARDUINO UNO
const uint8_t PIN_RST = 9; // reset pin
const uint8_t PIN_IRQ = 2; // irq pin
const uint8_t PIN_SS = SS; // spi select pin

// connection pins ESP8266 NODEMCU LUA
//const uint8_t PIN_SCK = D5;
//const uint8_t PIN_MOSI = D6;
//const uint8_t PIN_MISO = D7;
//const uint8_t PIN_SS = D8;
//const uint8_t PIN_RST = D1;
//const uint8_t PIN_IRQ = D2;


// переменные для калмана
float varVolt = 0.0308;  // среднее отклонение
float varProcess = 0.002; // скорость реакции на изменение (подбирается вручную)
float Pc = 0.0;
float G = 0.0;
float P = 1.0;
float Xp = 0.0;
float Zp = 0.0;
float Xe = 0.0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  //init the configuration
  DW1000Ranging.initCommunication(PIN_RST, PIN_SS, PIN_IRQ); //Reset, CS, IRQ pin
  //define the sketch as anchor. It will be great to dynamically change the type of module
  DW1000Ranging.attachNewRange(newRange);
  DW1000Ranging.attachNewDevice(newDevice);
  DW1000Ranging.attachInactiveDevice(inactiveDevice);
//  Enable the filter to smooth the distance
//  DW1000Ranging.useRangeFilter(true);
  
  //we start the module as a tag
  DW1000Ranging.startAsTag("E1:00:22:EA:82:60:3B:9C", DW1000.CHANNEL_4, DW1000.MODE_SHORTDATA_FAST_ACCURACY, false);
  DW1000.setAntennaDelay(16400);
  Serial.println(DW1000.getAntennaDelay());
}

void loop() {
  DW1000Ranging.loop();
}

float ranges[300];
int j=0;

float filter(float val) {  //функция фильтрации
  Pc = P + varProcess;
  G = Pc/(Pc + varVolt);
  P = (1-G)*Pc;
  Xp = Xe;
  Zp = Xp;
  Xe = G*(val-Zp)+Xp; // "фильтрованное" значение
  return(Xe);
}

void newRange() {
  float range = filter(DW1000Ranging.getDistantDevice()->getRange());
//  ranges[j] = range;
//  j++;
//  if(j == 399) {
//    float a, sigma;
//    for(int i=0; i < 400; i++) {
//      a += ranges[i];
//    }
//    a /= 400;
//
//    for(int i=0; i < 400; i++) {
//      sigma += pow((ranges[i] - a),2);
//    }
//    sigma /= 400;
//    sigma = sqrt(sigma);
//    varVolt = sigma; // среднее отклонение
//    Serial.print("\t FR: "); Serial.print(filter(range)); Serial.print(" m"); 
//    j=0;
//  }
  Serial.print("by: "); Serial.print(DW1000Ranging.getDistantDevice()->getShortAddress(), HEX);
  Serial.print("\t R: ");  Serial.print(range);  Serial.print(" m");
//  Serial.print("\t FR: "); Serial.print(filter(range)); Serial.print(" m"); 
  Serial.print("\t RX_P: "); Serial.print(DW1000Ranging.getDistantDevice()->getRXPower()); Serial.println(" dBm");
}

void newDevice(DW1000Device* device) {
  Serial.print("ranging init; 1 device added ! -> ");
  Serial.print(" short:");
  Serial.println(device->getShortAddress(), HEX);
}

void inactiveDevice(DW1000Device* device) {
  Serial.print("delete inactive device: ");
  Serial.println(device->getShortAddress(), HEX);
}

