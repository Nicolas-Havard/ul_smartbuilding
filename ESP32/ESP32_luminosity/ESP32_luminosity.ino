/*********************************************************************************************************************************************************
   SMART BUILDING PROJECT : EXPERIMENTAL SENSOR NODE

   This code contains functions from the BME680 library created by Adafruit and from the WiFi library.
   These modules and functions will be replaced in the future by low level code.
   But for the moment, this code allow me to present a prototype of the final solution.

   The principle of this code is to get data on temperature, humidity, pressure and gas for example, and send them on a WiFi server which is a FiPy module.
   Then this last device send data on a cloud thanks to LoRa network

   HAVARD Nicolas, student at Polytech Lille, France
 ********************************************************************************************************************************************************/




/************************ CREDITS ***********************************/

/***************************************************************************
  This is a library for the BME680 gas, humidity, temperature & pressure sensor
  Designed specifically to work with the Adafruit BME680 Breakout
  ----> http://www.adafruit.com/products/3660

  These sensors use I2C or SPI to communicate, 2 or 4 pins are required
  to interface.
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing products
  from Adafruit!

  Written by Limor Fried & Kevin Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/






/************************** CODE ***************************/



/* Module to connect the ESP32 to the FiPy device by WiFi */
//#include <WiFi.h>

//#include <Adafruit_Sensor.h>
//#include <DHT.h>
//#include <DHT_U.h>
#include "cactus_io_DHT22.h"


/* Node ID */
const char ID[] = "bedroom_1" ;


/* Variables used to communicate with server */

const char ssid[] = "fipy-wlan-f514" ;               // Name of the access point created by the FiPy module
const char password[] =  "www.pycom.io" ;            // Password of this access point

IPAddress host(192, 168, 4, 1) ;                     // IP address of the FiPy module on the local network
const uint16_t port = 15555 ;                        // Port of the socket of the FiPy, which allow data transfer


#define DHT22_PIN 25         // Pin which is connected to the DHT22 sensor.
#define led 17
#define photo 14



// Uncomment the type of sensor in use:
//#define DHTTYPE           DHT11     // DHT 11
//#define DHTTYPE           DHT22     // DHT 22 (AM2302)
//#define DHTTYPE           DHT21     // DHT 21 (AM2301)

//DHT_Unified dht(DHTPIN, DHTTYPE);
DHT22 dht(DHT22_PIN);


/* INITIALISATION FUNCTION */


void setup()
{
  /* Serial initilisation */
  Serial.begin(115200) ;

  // Initialize device.
  dht.begin();
  Serial.println("DHTxx Unified Sensor Example");
  // Print temperature sensor details.
  //sensor_t sensor;
  //dht.temperature().getSensor(&sensor);
  Serial.println("------------------------------------");
  //Serial.println("Temperature");
  //Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  //Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  //Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  //Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" *C");
  //Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" *C");
  //Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" *C");
  //Serial.println("------------------------------------");
  // Print humidity sensor details.
  //dht.humidity().getSensor(&sensor);
  Serial.println("------------------------------------");
  //Serial.println("Humidity");
  //Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  //Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  //Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  //Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println("%");
  //Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println("%");
  //Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println("%");
  Serial.println("------------------------------------");
  // Set delay between sensor readings based on sensor details.
  //float delayMS = sensor.min_delay / 1000;


  /* WiFi initilisation */
  /*  Serial.print("Connecting to ") ;
    Serial.println(ssid) ;
    WiFi.begin(ssid, password) ;
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500) ;
      Serial.print(".") ;
    }

    Serial.println("") ;
    Serial.println("WiFi connected") ;
    Serial.print("IP address : ") ;
    Serial.println(WiFi.localIP()) ;
  */
  /* set led as output to control led on-off */
  pinMode(led, OUTPUT) ;
  boolean led_state = 0 ;
  for (int compteur = 0 ; compteur < 6 ; compteur++)
  {
    led_state = (compteur + 1) % 2 ;
    digitalWrite(led, led_state) ;
    delay(500) ;
    /*digitalWrite(led,LOW) ;
      delay(500) ;*/
  }
}



/* MAIN FUNCTION */

void loop()
{
  // Delay between measurements.
  delay(2000);
  dht.readTemperature();
  dht.readHumidity();

  // Check if any reads failed and exit early (to try again).
  if (isnan(dht.humidity) || isnan(dht.temperature_C)) 
  {
    Serial.println("DHT sensor read failure!");
    return;
  }

  Serial.print(dht.humidity); Serial.print(" %\t\t");
  Serial.print(dht.temperature_C); Serial.print(" *C\t");
  Serial.print(dht.temperature_F); Serial.print(" *F\t");
  Serial.print(dht.computeHeatIndex_C()); Serial.print(" *C\t");
  Serial.print(dht.computeHeatIndex_F()); Serial.println(" *F");


/*
  // Get temperature event and print its value.
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println("Error reading temperature!");
  }
  else {
    Serial.print("Temperature: ");
    Serial.print(event.temperature);
    Serial.println(" *C");
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println("Error reading humidity!");
  }
  else {
    Serial.print("Humidity: ");
    Serial.print(event.relative_humidity);
    Serial.println("%");
  }



  /* data sent on serial port to compare with received data by FiPy module */

  float luminosity;
  luminosity = analogRead(photo);

  Serial.print("luminosity = ") ;
  Serial.print(luminosity) ;
  Serial.println(" ") ;

  /* not used for the moment */
  /*Serial.print("Approx. Altitude = ") ;
    Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA)) ;
    Serial.println(" m") ;
  */

  Serial.println() ;


  /* sending of data through WiFi */
  /*  Serial.println("Connecting ...") ;
    WiFiClient client ;
    if (!client.connect(host, port))                  // connection test
    {
      Serial.println("Connection to host failed") ;
      delay(1000) ;
      return ;
    }
    else
    {
      digitalWrite(led, HIGH);
      Serial.println("Message sent to FiPy :") ;
      client.printf("%s : %s | %s : %f", "ID", ID, "luminosity", luminosity) ;
      delay(250);
      Serial.println("Disconnecting...") ;

      client.stop() ;                                   // deconnection
      digitalWrite(led, LOW);
    }*/

  delay(2000) ;
}
