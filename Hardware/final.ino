/*
  Semi automated irrigation system 
*/
#include<SoftwareSerial.h>

#include "DHT.h"

#define DHTPIN 2 //connect out pin of DHT in pin 2
    
#define DHTTYPE DHT22  //select sensor model for DHT library

SoftwareSerial mySerial(9,10); //9, 10 are used as tx and rx pin of gsm

DHT dht(DHTPIN, DHTTYPE);

int rainPin = A0; //connect A0 to arduino, Y69 sensors

void setup() 
{
  pinMode(11, OUTPUT);  //relay
  Serial.begin(19200); //Software Serail start.
  mySerial.begin(19200); // Gsm Start.
  dht.begin();  //DHT22 start
  pinMode(rainPin, INPUT); //analog
  digitalWrite(11, HIGH);  
  delay(5000); //delay 5 seconds for Gsm Start.
  initGPRS();
  
}

void loop() 
{
  double humidity,temperature;
  int moisture;
  char n = toSerial();
  Serial.print("value of n");
  Serial.println(n);
  if(n=='1')
  {
    digitalWrite(11, LOW); //motor on
  }
  else
  {
    digitalWrite(11, HIGH);//motor off 
  }
   temperature=getTemp("c");
   moisture=analogRead(rainPin);
   humidity=getTemp("h");
   serverdata(moisture,temperature,humidity);
   delay(60000);
  
}

void initGPRS()
{
  mySerial.println("AT");
  delay(200);
  mySerial.println("AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r\n");
  delay(500);
  mySerial.println("AT+SAPBR=3,1,\"APN\",\"internet\"\r\n");
  delay(500);
  mySerial.println("AT+SAPBR =1,1");
  delay(3500);
  mySerial.println("AT+HTTPINIT");
  delay(500);
  mySerial.println("AT+HTTPPARA=\"CID\",1");
  delay(500);

}

float serverdata(double moisture, double temperature, int humidity)
{
  mySerial.print("AT+HTTPPARA=\"URL\",\"http://autoirri.000webhostapp.com/update_status.php?id=1&moisture=");
  mySerial.print(moisture);
  mySerial.print("&temperature=");
  mySerial.print(temperature);
  mySerial.print("&humidity=");
  mySerial.print(humidity);
  mySerial.println("\"");
  delay(200);
  mySerial.println(" AT+HTTPACTION=0");
  delay(600);
}

float getTemp(String req)
{

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  // Compute heat index in Kelvin 
  float k = t + 273.15;
  if(req =="c"){
    return t;//return Cilsus
  }else if(req =="f"){
    return f;// return Fahrenheit
  }else if(req =="h"){
    return h;// return humidity
  }else if(req =="hif"){
    return hif;// return heat index in Fahrenheit
  }else if(req =="hic"){
    return hic;// return heat index in Cilsus
  }else if(req =="k"){
    return k;// return temprature in Kelvin
  }else{
    return 0.000;// if no reqest found, retun 0.000
  }
 
}

char  toSerial() 
{
  mySerial.println("AT+HTTPREAD");
  char a[100];
  int i=0;
    while(mySerial.available() > 0) {
   //char c = mySerial.read();
   a[i]=mySerial.read();
   Serial.println(a[i]);
   i=i+1;     
  }
 return  a[88];
 
}

