/*********************************************************************************************************************************************************
 * SMART BUILDING PROJECT : EXPERIMENTAL SENSOR NODE
 * 
 * This code contains functions from the BME680 library created by Adafruit and from the WiFi library.
 * These modules and functions will be replaced in the future by low level code.
 * But for the moment, this code allow me to present a prototype of the final solution.
 * 
 * The principle of this code is to get data on temperature, humidity, pressure and gas for example, and send them on a WiFi server which is a FiPy module.
 * Then this last device send data on a cloud thanks to LoRa network
 * 
 * HAVARD Nicolas, student at Polytech Lille, France
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
#include <WiFi.h>

/* Modules to use the BME680 sensor */
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"


/* Node ID */
const char ID[] = "living_room" ;


/* Variables used to communicate with server */

const char ssid[] = "fipy-wlan-f514" ;               // Name of the access point created by the FiPy module
const char password[] =  "www.pycom.io" ;            // Password of this access point

IPAddress host(192,168,4,1) ;                        // IP address of the FiPy module on the local network
const uint16_t port = 15555 ;                        // Port of the socket of the FiPy, which allow data transfer


/* Variables used to manage BME680 sensor */

#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME680 bme ; // I2C
//Adafruit_BME680 bme(BME_CS) ; // hardware SPI
//Adafruit_BME680 bme(BME_CS, BME_MOSI, BME_MISO,  BME_SCK) ;


#define led 17




/* INITIALISATION FUNCTION */


void setup() 
{
  /* Serial initilisation */
  Serial.begin(115200) ;
  while (!Serial) ;

  /* Test of BME680 */
  Serial.println(F("BME680 test")) ;
  if (!bme.begin()) 
  {
    Serial.println("Could not find a valid BME680 sensor, check wiring!") ;
    while (1) ;
  }

  // Set up oversampling and filter initialization for BME680
  bme.setTemperatureOversampling(BME680_OS_8X) ;
  bme.setHumidityOversampling(BME680_OS_2X) ;
  bme.setPressureOversampling(BME680_OS_4X) ;
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3) ;
  bme.setGasHeater(320, 150) ; // 320*C for 150 ms
  

  /* WiFi initilisation */
  Serial.print("Connecting to ") ;
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
  
  /* set led as output to control led on-off */
  pinMode(led, OUTPUT) ;
  boolean led_state = 0 ;
  for(int compteur = 0 ; compteur < 6 ; compteur++)
  {
    led_state = (compteur+1)%2 ;
    digitalWrite(led, led_state) ;
    delay(500) ;
    /*digitalWrite(led,LOW) ;
    delay(500) ;*/
  }
}



/* MAIN FUNCTION */

void loop() 
{

  /* data sent on serial port to compare with received data by FiPy module */  
  if (! bme.performReading()) {
    Serial.println("Failed to perform reading :(") ;
    return ;
  }
  
  float temperature = bme.temperature ;
  float pressure = bme.pressure/100.0 ;
  float humidity = bme.humidity ;
  float gas_resistance = bme.gas_resistance/1000.0 ;
  
  Serial.print("Temperature = ") ;
  Serial.print(temperature) ;
  Serial.println(" *C") ;

  Serial.print("Pressure = ") ;
  Serial.print(pressure) ;
  Serial.println(" hPa") ;

  Serial.print("Humidity = ") ;
  Serial.print(humidity) ;
  Serial.println(" %") ;

  Serial.print("Gas = ") ;
  Serial.print(gas_resistance) ;
  Serial.println(" KOhms") ;

/* not used for the moment */
/*Serial.print("Approx. Altitude = ") ;
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA)) ;
  Serial.println(" m") ;
*/

  Serial.println() ;


  /* sending of data through WiFi */
  Serial.println("Connecting ...") ;
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
    client.printf("%s : %s | %s : %f | %s : %f | %s : %f | %s : %f", "ID", ID, "temperature", temperature, "pressure", pressure, "humidity", humidity, "gas resistance", gas_resistance) ;
    delay(250);
    Serial.println("Disconnecting...") ;
  
    client.stop() ;                                   // deconnection 
    digitalWrite(led, LOW);
  }

  delay(2000) ;
}
