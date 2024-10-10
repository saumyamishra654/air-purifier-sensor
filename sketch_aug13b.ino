#include <Wire.h>                                                  
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#include <WiFiMulti.h>
WiFiMulti wifiMulti;
#define DEVICE "ESP32"

#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

#define WIFI_SSID "Saumya"                                                                                        //Network Name
#define WIFI_PASSWORD "pardalala"                                                                                //Network Password
#define INFLUXDB_URL "https://us-east-1-1.aws.cloud2.influxdata.com"
#define INFLUXDB_TOKEN "IpEYLMmqDlBV3v6N4fu3vXqiAWSTfkFMAUcA5qjSHiov8pWcftMXz4f091egrKoAmKqGwLbNoiDbt-e_plItdw=="
#define INFLUXDB_ORG "f9c2eb3c53661d1a"
#define INFLUXDB_BUCKET "Weather"                                                                             //InfluxDB v2 bucket name (Use: InfluxDB UI ->  Data -> Buckets)
#define TZ_INFO "UTC5.5"                                                                                               //InfluxDB v2 timezone

Adafruit_BME680 bme; 

InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);  // InfluxDB client instance with preconfigured InfluxCloud certificate

Point sensor("weather");  

void setup() 
{
  Serial.begin(115200);                                           
  if (!bme.begin()) {                                              
    Serial.println("Could not find a valid BME680 sensor, check wiring!");
    while (1);
  }

  bme.setTemperatureOversampling(BME680_OS_8X);                
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150);

  WiFi.mode(WIFI_STA);  
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to wifi"); 
  while (wifiMulti.run() != WL_CONNECTED) 
  {
    Serial.print(".");
    delay(100);
  }
  Serial.println();

  sensor.addTag("device", DEVICE);  
  sensor.addTag("SSID", WIFI_SSID);

  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov"); 

  if (client.validateConnection())
  {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
  } 
  else 
  {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }
}

void loop() 
{
  if (!bme.performReading()) {
    Serial.println("Failed to perform reading :(");
    return;
  }

  float temp = bme.temperature;                  
  float humid = bme.humidity;                    
  float pressure = bme.pressure / 100.0;         

  sensor.clearFields(); 

  sensor.addField("temperature", temp);  
  sensor.addField("humidity", humid);    
  sensor.addField("pressure", pressure); 

  if (wifiMulti.run() != WL_CONNECTED) 
    Serial.println("Wifi connection lost");

  if (!client.writePoint(sensor)) 
  {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());
  }

  Serial.print("Temp: ");         
  Serial.println(temp);
  Serial.print("Humidity: ");
  Serial.println(humid);
  Serial.print("Pressure: ");
  Serial.println(pressure);
  delay(60000); 
}

