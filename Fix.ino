#include <LiquidCrystal_I2C.h>
#include <DS3231.h>
#include <Wire.h> 
#include <OneWire.h>
#include <DallasTemperature.h>
#include "GravityTDS.h"
#define TdsSensorPin A1
#define echoPin 12 
#define trigPin 11 

int maximumRange = 200; 
int minimumRange = 00;
long duration, distance; 
const int pendingin = 4; //pin4
const int air = 5; //pin5
const int pengaduk = 3; //pin3
const int nutrisi = 7; //pin7
int relayON = LOW; //relay nyala
int relayOFF = HIGH; //relay mati
int a = 0;

GravityTDS gravityTds;
float temperature = 25,tdsValue = 0;

OneWire pin_DS18B20(2);
DallasTemperature DS18B20(&pin_DS18B20);

LiquidCrystal_I2C lcd(0x27, 20, 4);
DS3231  rtc(SDA, SCL); 

Time  t;

int Hor;
int Min;
int Sec;
int hari=1;
int ppm=0;
int tambah=0;
int kurang=0;
int potong=0;
int potongg=0;

void setup()
{
  Serial.begin(9600); //set komunikasi baut serial monitor pada 115200
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  lcd.begin();
  gravityTds.setPin(TdsSensorPin);
  gravityTds.setAref(5.0); 
  gravityTds.setAdcRange(1024);  
  gravityTds.begin();
  rtc.begin();
  lcd.backlight();
  DS18B20.begin();
  lcd.clear();
  pinMode(pendingin, OUTPUT);
  pinMode(air, OUTPUT);
  pinMode(pengaduk, OUTPUT);
  pinMode(nutrisi, OUTPUT);
  digitalWrite(pendingin, relayOFF);
  digitalWrite(air, relayOFF);
  digitalWrite(pengaduk, relayOFF);
  digitalWrite(nutrisi, relayOFF);
}

void TDS(){
  gravityTds.setTemperature(temperature);  
  gravityTds.update();  
  tdsValue = gravityTds.getTdsValue();
  
  int tds = tdsValue;
  float tdss;
  tdss = 1*tds;
  int tdsss;
  tdsss = tds+tdss;
  
  lcd.setCursor(0,2);
  lcd.print("PPM:");
  lcd.print(tdsValue,0); //tdsss
}

void Air(){
  int persen;
  lcd.setCursor(0,1);
  persen=map(distance, 20, 6, 0, 100);
  lcd.print("Air:"); 
  lcd.print(persen);
  lcd.print("%");
}

void Suhu(){
  lcd.setCursor(9,1);
  lcd.print("Temp:");
  lcd.print(DS18B20.getTempCByIndex(0));
  lcd.print("C");
}
void sistemsuhu(){
  float jml;
  lcd.setCursor(0,3);
  lcd.print("PA:0");
  lcd.setCursor(7,3);
  lcd.print("Pab+:0");
  lcd.setCursor(16,3);
  lcd.print("PP:0");
  if(DS18B20.getTempCByIndex(0) > 32){
    digitalWrite(pendingin, relayON);
    jml =(9259.9*DS18B20.getTempCByIndex(0)) - 161904;
    lcd.setCursor(10,2);
    lcd.print("KPS:1");
    lcd.setCursor(16,2);
    lcd.print("PN:1");
    
    delay(jml);
  }else if(DS18B20.getTempCByIndex(0) < 32){
    digitalWrite(pendingin, relayOFF);
    lcd.setCursor(10,2);
    lcd.print("KPS:0");
    lcd.setCursor(16,2);
    lcd.print("PN:0");
  }  
}

void loop()
{
  float jml;
  int persen;
  t = rtc.getTime();
  Hor = t.hour;
  Min = t.min;
  Sec = t.sec;
  DS18B20.requestTemperatures();

  digitalWrite(trigPin, LOW);delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);

  distance = duration/58.2;
  
  //Serial.print(rtc.getDOWStr()); //prosedur pembacaan hari
  //Serial.print(" ");
  
  //Serial.print(rtc.getDateStr()); //prosedur pembacaan tanggal
  //Serial.print(" -- ");
 
  //Serial.println(rtc.getTimeStr()); //prosedur pembacaan waktu
  
  lcd.setCursor(0,0);
  lcd.print(rtc.getDateStr());
  lcd.setCursor(12,0);
  lcd.print(rtc.getTimeStr());

  Suhu();
  TDS();
  Air();
  delay (1000);
  lcd.setCursor(0,0);
  lcd.print(rtc.getDateStr());
  lcd.setCursor(12,0);
  lcd.print(rtc.getTimeStr());

  Suhu();
  TDS();
  Air();
  
  if(Hor == 12 && Min < 16){
    Suhu();
    TDS();
    Air();
    while(persen < 80){
      Suhu();
      TDS();
      Air();
      digitalWrite(air, relayON);
      digitalWrite(pengaduk, relayON);
      lcd.setCursor(0,3);
      lcd.print("PA:1");
      lcd.setCursor(16,3);
      lcd.print("PP:1");

      lcd.setCursor(10,2);
      lcd.print("KPS:0");
      lcd.setCursor(16,2);
      lcd.print("PN:0");
    }
      Suhu();
      TDS();
      Air();
      digitalWrite(air, relayOFF);
      digitalWrite(pengaduk, relayOFF);
      lcd.setCursor(0,3);
      lcd.print("PA:0");
      lcd.setCursor(16,3);
      lcd.print("PP:0");
      
      lcd.setCursor(10,2);
      lcd.print("KPS:0");
      lcd.setCursor(16,2);
      lcd.print("PN:0");
    
  }else if(Hor == 12 && Min > 15 && Min < 30){
    Suhu();
    TDS();
    Air();
    lcd.setCursor(0,3);
    lcd.print("PA:0");
    if(tdsValue < 1100){
      Suhu();
      TDS();
      Air();
      int tds = tdsValue;
      digitalWrite(nutrisi, relayON);
      digitalWrite(pengaduk, relayON);
      jml =(-1.3298*tds) + 1608;
      lcd.setCursor(7,3);
      lcd.print("Pab+:1");
      lcd.setCursor(16,3);
      lcd.print("PP:1");
      delay(jml);
      digitalWrite(nutrisi, relayOFF);
      lcd.setCursor(7,3);
      lcd.print("Pab+:0");
      delay(5000);
      lcd.setCursor(16,3);
      lcd.print("PP:0");
      digitalWrite(pengaduk, relayOFF);
      lcd.setCursor(0,3);
      lcd.print("PA:0");
      
      lcd.setCursor(10,2);
      lcd.print("KPS:0");
      lcd.setCursor(16,2);
      lcd.print("PN:0");
      
    }else if(tdsValue > 1100){
      Suhu();
      TDS();
      Air();
      digitalWrite(nutrisi, relayOFF);
      digitalWrite(pengaduk, relayOFF);
      lcd.setCursor(7,3);
      lcd.print("Pab+:0");
      lcd.setCursor(16,3);
      lcd.print("PP:0");

      lcd.setCursor(0,3);
      lcd.print("PA:0");
      
      lcd.setCursor(10,2);
      lcd.print("KPS:0");
      lcd.setCursor(16,2);
      lcd.print("PN:0");
    }
  
  }else if (Hor < 12 || Hor > 12){
    Suhu();
    sistemsuhu();
  }else if (Hor == 12 && Min > 29 ){
    Suhu();
    sistemsuhu();
  }
  
  if(persen < 0){
    persen = 0;
  }else if(persen < 100){
    persen = 100;
  }
  
  delay (10); //waktu tunda 1 detik per cycle
  
  lcd.setCursor(4,2);
  lcd.print("      ");
  lcd.setCursor(4,1);
  lcd.print("     ");
}
