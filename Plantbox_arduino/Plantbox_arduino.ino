#include <Adafruit_AM2320.h>

#include <Bridge.h>
#include "T6603.h"
#include "DHT.h"

#define DELAYTIME 2000

#define DHT_Pin 9
#define co2_TX_Pin 10
#define co2_RX_Pin 11
#define o2_Pin A0
#define water_Pin A1
#define VRefer 5

unsigned short watching_dog;
int outcoming [13];

char putvaletmp[100] = {'\0'};

T6603 co2_sensor;
DHT dht(DHT_Pin, DHT11); 

double get_o2(){
   int sum = 0;
   for(int i=0; i<8; i++) sum += analogRead(o2_Pin);
   sum >>= 3;

   double MeasuredVout = sum * (VRefer / 1024.0)*17.5;
   return MeasuredVout;
}

double get_waterlevel(){
  return (double)analogRead(water_Pin);
}

void get_value(char *dataname, int pinNum, int mode = 0){
  char num[5] = {'\0'};
  Bridge.get(dataname, num, 5);
  num[4] = '\0';
  //Serial.println(dataname);
  //Serial.println(num);
  if (num[0] == '\0'){
    //Serial.println("NAN");
    return ;
  }
  int val = atoi(num);
  if (mode == 0){
    if (val > 0){
      digitalWrite(pinNum, HIGH);
      watching_dog |= (1 << pinNum);
    } 
    else digitalWrite(pinNum, LOW);
  }
  else if(mode == 1){
    if (val > 1023) analogWrite(pinNum, 1023);
    else if (val < 0) analogWrite(pinNum, 0);
    else analogWrite(pinNum, val);
  }
}

void put_value(char *dataname, double data, int index){
  dtostrf(data, 6, 6, putvaletmp);
  Bridge.put(dataname, putvaletmp);

  char outcome[60] = "outcomming_";
  strcat(outcome, dataname);
  outcoming[index] = outcoming[index]^1;
  char outcomedata[5];
  itoa(outcoming[index], outcomedata, 10);
  //Serial.print(outcome);
  //Serial.println(outcomedata);
  Bridge.put(outcome, outcomedata);
  
}

void setup() {
  for (int i=0;i<13;i++){
    outcoming[i] = 0;
  }

  for (int i=2;i<8;i++){
    pinMode(i, OUTPUT);
  }
  for (int i=2;i<8;i++){
    digitalWrite(i, LOW);
  }

  watching_dog = 0;
  Bridge.begin();
  Serial.begin(9600);
  Serial.println("Setup");
  co2_sensor.begin(co2_TX_Pin, co2_RX_Pin);
  dht.begin();
}

void loop() {
  
  //co2_sensor.begin(co2_TX_Pin, co2_RX_Pin);
  double humi = dht.readHumidity();
  double temp = dht.readTemperature();
  double co2 = (double)co2_sensor.get_co2();
  double o2 = get_o2();
  double waterlevel = get_waterlevel();

  put_value("Humidity Plantbox", humi, 0);
  put_value("Temperature Plantbox", temp, 1);
  put_value("CO2 Plantbox", co2, 2);
  put_value("O2 Plantbox", o2, 3);
  put_value("Water Level Plantbox", waterlevel, 4);
  put_value("Watching Dog", watching_dog, 5);


  watching_dog = 0;
  get_value("Moter1", 2);
  get_value("Moter2", 3);
  get_value("Moter3", 4);
  get_value("LED1", 5);
  get_value("LED2", 6);
  get_value("LED3", 7);
  watching_dog >>= 2;
  //Serial.println(co2);
  /*
  for (int i=2;i<8;i++){
    digitalWrite(i, HIGH);
  }
  delay(2000);
  for (int i=2;i<8;i++){
    digitalWrite(i, LOW);
  }
  Serial.println("123");
  */
  /*
  char buf[10];
  Bridge.get("CO2 Plantbox", buf, 10);
  
  Serial.print("CO2 = ");
  buf[6] = '\0';
  Serial.print(buf);
  Serial.print("PPM\tO2 = ");
  Bridge.get("O2 Plantbox", buf, 10);
  buf[6] = '\0';
  Serial.print(buf);
  Serial.print("% Water_Level = ");
  Bridge.get("Water Level Plantbox", buf, 10);
  buf[6] = '\0';
  Serial.print(buf);
  Serial.print(" Temp = ");
  Bridge.get("Temperature Plantbox", buf, 10);
  buf[6] = '\0';
  Serial.print(buf);
  Serial.print(" Humidity = ");
  Bridge.get("Humidity Plantbox", buf, 10);
  buf[6] = '\0';
  Serial.print(buf);
  buf[6] = '\0';
  Serial.println("\n");
  Bridge.get("LED1 Plantbox", buf, 10);
  buf[6] = '\0';
  Serial.println(buf);
  for (int i=0;i<13;i++){
    Serial.print(outcoming[i]);
  }
  Serial.print("\n");*/
  
  delay(DELAYTIME);
  //watching_dog++;
}
