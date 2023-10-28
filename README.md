# AM2320 Single Bus
Basic Arduino library for temperature &amp; humidity sensor AM2320 using single bus (one wire).

I've created this simple library in desperation that the sensor does not work on I2C when connected on a long cable and I could not find any other existing library for AM2320 for one wire communication. 
Luckily sensor supports also single bus communication to retrieve data, unfortunately not up to OneWire standard. 

I've successfully tested the sensor on 6m long UTP cat5 cable. Having 4k7 resistor right by the sensor between VCC (5V) and SDA pins. SCL connected to GND. 

Just recently I've discovered there is also more advanced DHT library which apparently supports AM2320 but I haven't personally tested: https://github.com/RobTillaart/DHTNew


```cpp
#include <AM2320SB.h>

// Create senso object with PIN number
AM2320SB sensor(4);

void setup() {
  Serial.begin(115200);
  // Give sensor time to wake up
  delay(3000);
  // sensor.debug = true; // if you wish to examine data from sensor
  // sensor.skipParityCheck = true; // for debugging purposes only
}

void loop() {
  // Call read() method to receive data from sensor.
  int sensorStatus = sensor.read();
  Serial.print("Sensor status: ");
  Serial.println(sensorStatus);

  // Print received values. 
  Serial.print("Temp: ");
  Serial.print(sensor.getTemperature());
  Serial.print("°C, Humidity: ");
  Serial.println(sensor.getHumidity());

  delay(3000);
}

```
