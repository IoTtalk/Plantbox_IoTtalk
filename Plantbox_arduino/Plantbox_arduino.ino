#include <Bridge.h>
#include <Adafruit_AM2320.h>
#include <FastLED.h>

#define NUM_LEDS 60
#define LED_PIN1 5
#define LED_PIN2 6

#define DHT_Pin 9
#define co2_TX_Pin 10
#define co2_RX_Pin 11
#define o2_Pin A0
#define water_Pin A1
#define VRefer 5

#include "T6603.h"
#include "DHT.h"

#define DELAYTIME 200
#define WATERLEVEL_MIN 40
#define WATERLEVEL_PEAK 10

CRGB leds1[NUM_LEDS];
CRGB leds2[NUM_LEDS];

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
  int value = analogRead(water_Pin);
  if (value < 200) return WATERLEVEL_MIN;
  if (value < 400) return WATERLEVEL_MIN+WATERLEVEL_PEAK*1;
  if (value < 600) return WATERLEVEL_MIN+WATERLEVEL_PEAK*2;
  if (value < 850) return WATERLEVEL_MIN+WATERLEVEL_PEAK*3;
  return WATERLEVEL_MIN+WATERLEVEL_PEAK*4;
}

void get_value(char *dataname, int pinNum, int mode = 0){
  char num[5] = {'\0'};
  Bridge.get(dataname, num, 5);
  num[4] = '\0';
  if (num[0] == '\0'){
    return ;
  }
  int val = atoi(num);
  if (mode == 0){
    if (val > 0){
      digitalWrite(pinNum, HIGH);
    } 
    else digitalWrite(pinNum, LOW);
  }
  else if(mode == 1){
    if (val > 1023) analogWrite(pinNum, 1023);
    else if (val < 0) analogWrite(pinNum, 0);
    else analogWrite(pinNum, val);
  }
}

void set_LED(){
  char LEDR[6] = {'\0'};
  char LEDG[6] = {'\0'};
  char LEDB[6] = {'\0'};
  Bridge.get("LED-R-O", LEDR, 6);
  Bridge.get("LED-G-O", LEDG, 6);
  Bridge.get("LED-B-O", LEDB, 6);
  LEDR[5] = '\0';
  LEDG[5] = '\0';
  LEDB[5] = '\0';
  
  int valR = (LEDR[0] == '\0') ? 0 : atoi(LEDR);
  int valG = (LEDG[0] == '\0') ? 0 : atoi(LEDG);
  int valB = (LEDB[0] == '\0') ? 0 : atoi(LEDB);
  valR = (valR > 255) ? 255 : (valR < 0) ? 0 : valR;
  valG = (valG > 255) ? 255 : (valG < 0) ? 0 : valG;
  valB = (valB > 255) ? 255 : (valB < 0) ? 0 : valB;
  
  for (int i=0; i<NUM_LEDS; i++) {
    leds1[i] = CRGB(valG, valR, valB);
    leds2[i] = CRGB(valG, valR, valB);
  }
  FastLED.show();
}

void put_value(char *dataname, double data, int index){
  dtostrf(data, 6, 6, putvaletmp);
  Bridge.put(dataname, putvaletmp);

  char outcome[60] = "outcomming_";
  strcat(outcome, dataname);
  outcoming[index] = outcoming[index]^1;
  char outcomedata[5];
  itoa(outcoming[index], outcomedata, 10);
  Bridge.put(outcome, outcomedata);
}

void setup() {
  for (int i=0;i<13;i++){
    outcoming[i] = 0;
  }

  //Digital output init.
  for (int i=2;i<8;i++){
    pinMode(i, OUTPUT);
  }
  for (int i=2;i<8;i++){
    digitalWrite(i, LOW);
  }
  
  //LED init.
  FastLED.addLeds<WS2813, LED_PIN1, RGB>(leds1, NUM_LEDS);
  FastLED.addLeds<WS2813, LED_PIN2, RGB>(leds2, NUM_LEDS);
  for (int i=0; i<NUM_LEDS; i++) {
    leds1[i] = CRGB(0, 0, 0);
    leds2[i] = CRGB(0, 0, 0);
  }
  FastLED.show();
  
  //Sensor init.
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
  
  //Get sensor value and put it to bridge
  put_value("Humidity-I", humi, 0);
  put_value("Temp-I", temp, 1);
  put_value("CO2-I", co2, 2);
  put_value("O2-I", o2, 3);
  put_value("WaterLvl-I", waterlevel, 4);
  
  //Get signal from brigde.
  get_value("PumpWater-O", 2);
  get_value("Spray-O", 3);
  get_value("AddWater-O", 4);
  set_LED();
  
  delay(DELAYTIME);
}
