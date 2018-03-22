#include <Adafruit_AM2320.h>

#include <Bridge.h>
#include "T6603.h"
#include "DHT.h"

#define DHT_Pin 9
#define co2_TX_Pin 10
#define co2_RX_Pin 11
#define o2_Pin A0
#define water_Pin A1
#define VRefer 5

char putvaletmp[100] = {'\0'};

T6603 co2_sensor;
DHT dht(DHT_Pin, DHT11); 

double get_o2(){
   int sum = 0;
   for(int i=0; i<32; i++) sum += analogRead(o2_Pin);
   sum >>= 5;

   double MeasuredVout = sum * (VRefer / 1023.0)*21/2.0;
   return MeasuredVout;
}

double get_waterlevel(){
  return (double)analogRead(water_Pin);
}

void get_value(char *dataname, int pinNum, int mode = 0){
  char num[5] = {'\0'};
  Bridge.get(dataname, num, 5);
  num[4] = '\0';
  int val = atoi(num);
  if (mode == 0){
    if (val > 0) digitalWrite(pinNum, HIGH);
    else digitalWrite(pinNum, LOW);
  }
  else if(mode == 1){
    if (val > 1023) analogWrite(pinNum, 1023);
    else if (val < 0) analogWrite(pinNum, 0);
    else analogWrite(pinNum, val);
  }
}

void put_value(char *dataname, double data){
  dtostrf(data, 6, 6, putvaletmp);
  Bridge.put(dataname, putvaletmp);
}

void setup() {
  Bridge.begin();
  Serial.begin(115200);
  Serial.println("Setup");
  co2_sensor.begin(co2_TX_Pin, co2_RX_Pin);
}

void loop() {
  double humi = dht.readHumidity();
  double temp = dht.readTemperature();
  double co2 = (double)co2_sensor.get_co2();
  double o2 = get_o2();
  double waterlevel = get_waterlevel();

  put_value("Humidity Plantbox", humi);
  put_value("Temperature Plantbox", temp);
  put_value("CO2 Plantbox", co2);
  put_value("O2 Plantbox", o2);
  put_value("Water Level Plantbox", waterlevel);

  get_value("LED_test", 13);

  char buf[10];
  Bridge.get("CO2", buf, 10);
  
  Serial.print("CO2 = ");
  buf[6] = '\0';
  Serial.print(buf);
  Serial.print("PPM\tO2 = ");
  Bridge.get("O2", buf, 10);
  buf[6] = '\0';
  Serial.print(buf);
  Serial.print("% Water_Level = ");
  Bridge.get("Water_Level", buf, 10);
  buf[6] = '\0';
  Serial.print(buf);
  Serial.print(" Temp = ");
  Bridge.get("Temperature", buf, 10);
  buf[6] = '\0';
  Serial.print(buf);
  Serial.print(" Humidity = ");
  Bridge.get("Humidity", buf, 10);
  buf[6] = '\0';
  Serial.print(buf);
  buf[6] = '\0';
  Serial.println("\n");
  Bridge.get("LED_test", buf, 10);
  buf[6] = '\0';
  Serial.println(buf);
  delay(2000);
  
}
