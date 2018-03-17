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

  put_value("Humidity", humi);
  put_value("Temperature", temp);
  put_value("CO2", co2);
  put_value("O2", o2);
  put_value("Water_Level", waterlevel);

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
  delay(2000);
  
}
