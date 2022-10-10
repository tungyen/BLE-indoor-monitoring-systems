#include "DHT.h"
#include <SPI.h>
#include <BLEPeripheral.h>

#define DHTPIN 15
#define DHTTYPE DHT11


DHT dht(DHTPIN, DHTTYPE);

BLEPeripheral blePeripheral = BLEPeripheral();

BLEService tempService = BLEService("CCC0");
BLEIntCharacteristic tempCharacteristic = BLEIntCharacteristic("CCC0",BLERead | BLENotify);
BLEDescriptor tempDescriptor = BLEDescriptor("2901","Temp Celsius");

BLEService humidityService = BLEService("DDD0");
BLEIntCharacteristic humidityCharacteristic = BLEIntCharacteristic("DDD0",BLERead | BLENotify);
BLEDescriptor humidityDescriptor = BLEDescriptor("2901","Humidity Percent");

volatile bool readFromSensor = false;

float lastTempReading = dht.readTemperature();
float lastHumidityReading = dht.readHumidity();

long previousMillis = 0;
long interval = 2000;

void setup() 
{
  Serial.begin(9600);

  blePeripheral.setLocalName("Temperature");

  
  
  blePeripheral.addAttribute(tempService);
  blePeripheral.setAdvertisedServiceUuid(tempService.uuid());
  blePeripheral.addAttribute(tempCharacteristic);
  blePeripheral.addAttribute(tempDescriptor);

  
  blePeripheral.addAttribute(humidityService);
  blePeripheral.setAdvertisedServiceUuid(humidityService.uuid());
  blePeripheral.addAttribute(humidityCharacteristic);
  blePeripheral.addAttribute(humidityDescriptor);

  //blePeripheral.setEventHandler(BLEConnected,blePeripheralConnectHandler);
  //blePeripheral.setEventHandler(BLEDisconnected,blePeripheralDisconnectHandler);

  blePeripheral.begin();

  //Serial.println(F("BLE Temperature Sensor Peripheral"));
}

void loop(){
  blePeripheral.poll();
  //delay(2000);
  /*float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);
  if (isnan(h) || isnan(t) || isnan(f)) {
    //Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }*/
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > interval){
    previousMillis = currentMillis;
    setTempCharacteristicValue();
    setHumidityCharacteristicValue();
    
  }
}

void setTempCharacteristicValue()
{
  int reading = dht.readTemperature();
  double num = 10.2;
  tempCharacteristic.setValue(reading);
  lastTempReading = reading;
  /*Serial.print(F("Temperature: "));
  Serial.print(reading);
  Serial.println(F("C"));*/
  /*if(!isnan(reading)&&significantChange(lastTempReading, reading,0.01)){
    tempCharacteristic.setValue(reading);
    lastTempReading = reading;
  }*/
}

void setHumidityCharacteristicValue()
{
  double num = 20.2;
  int reading = dht.readHumidity();
  humidityCharacteristic.setValue(reading);
  lastHumidityReading = reading;
  /*Serial.print(F("Humidity: "));
  Serial.print(reading);
  Serial.println(F("%"));
  if(!isnan(reading)&&significantChange(lastHumidityReading, reading,0.01)){
    humidityCharacteristic.setValue(reading);
    lastHumidityReading = reading;
  }*/
}

boolean significantChange(float val1, float val2, float threshold){
  return (abs(val1-val2)>=threshold);
}

void blePeripheralConnectHandler(BLECentral& central){
    Serial.print(F("Connected event,central: "));
    Serial.println(central.address());
}

void blePeripheralDisconnectHandler(BLECentral& central){
    Serial.print(F("Disconnected event,central: "));
    Serial.println(central.address());
}
