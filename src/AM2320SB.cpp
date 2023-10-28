#include <Arduino.h>
#include "AM2320SB.h"

AM2320SB::AM2320SB(int pin) {
  this->pin = pin;
}

int AM2320SB::read() {
  uint8_t bytes[5];

  // Read the 40 individual bits
  uint8_t bits[40];
  int bitTimes[40];

  long timerStart = 0;
  long timerStop = 0;

  if (this->debug) Serial.println("Initiating communication.");
  // Send long low signal to wake sensor up
  pinMode(this->pin, OUTPUT);
  digitalWrite(this->pin, LOW);
  delayMicroseconds(AM2320SB::AM2320_INIT_LOW_MS);
  // Release bus for listening
  pinMode(this->pin, INPUT);

  // Wait for the start bit from the AM2320 sensor
  

  timerStart = micros();
  while (digitalRead(this->pin) == HIGH) {
    // Wait for the start bit to go low
    if ((micros() - timerStart) > 1000) {
      if(this->debug) Serial.println("Error: Listening for response LOW signal too long. Sensor not responding.");
      return AM2320SB::STATUS_NOT_RESPONDING;
    }
  }
  //if (this->debug) Serial.print("HIGH ended. ");

  timerStart = micros();


  while (digitalRead(this->pin) == LOW) {
    // Wait for the start bit to go high
    if ((micros() - timerStart) > 1000) {
      if(this->debug) Serial.println("Error: received LOW signal too long.");
      return AM2320SB::STATUS_READ_ERROR;
    }
  }
  timerStop = micros();
  // LOW ended but wait a little before start reading the bits otherwise the first bump of HIGH will be read as bit.
  // Do not use any serial print during reading process, ohterwise it slows down the process and results in read error.
  delayMicroseconds(80);  
  for (int i = 0; i < 40; i++) {

    bool bit = false;
    long bitLenght = 0;
    timerStart = micros();
    while (digitalRead(this->pin) == LOW) {
      // Wait for the data bit to start
      if ((micros() - timerStart) > 1000) {
        if(this->debug) Serial.println("Error: received LOW signal too long.");
        return AM2320SB::STATUS_READ_ERROR;
      }
    }
    timerStart = micros();
    while (digitalRead(this->pin) == HIGH) {
      // Wait for the data bit to start
      if ((micros() - timerStart) > 1000) {
        if(this->debug) Serial.println("Error: received HIGH signal too long.");
        return AM2320SB::STATUS_READ_ERROR;
      }
    }

    bitLenght = micros() - timerStart;

    if (bitLenght > 50) {
      bit = true;
    }
    bits[i] = bit;
    bitTimes[i] = bitLenght;
  }

  if (this->debug) {
    Serial.print("Bits received: ");
    for (int i = 0; i < 40; i++) {
      Serial.print(bits[i]);
    }
    Serial.println(" ");
    Serial.println("Bits HIGH timing from sensor: ");

    for (int i = 0; i < 40; i++) {
      Serial.print(bitTimes[i]);
      Serial.print(" ");
    }
    Serial.println(" ");
  }

  // Set first temperature bit to 1 to test negative temp, only for testing purposes
  // bits[16] = 1;

  // Convert the bits array to the bytes array.
  // Iterate over the bits array and convert each group of 8 bits to a byte.
  for (int i = 0; i < 5; i++) {
    uint8_t byte = 0;
    for (int j = 0; j < 8; j++) {
      byte |= (bits[i * 8 + j] << (7 - j));
    }

    // Store the converted byte in the output array.
    bytes[i] = byte;
  }

  
  if (this->debug) {  // Print bytes
  Serial.print("Bits converted to bytes (HEX): ");
    for (int i = 0; i < 5; i++) {
      Serial.print(bytes[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  }

  // Byte parity check
  uint8_t expectedParity = bytes[0] + bytes[1] + bytes[2] + bytes[3];

  // Check for negative temperature is bytes[2] starts with 1 i.e. 1000000 (128)
  bool freezing = false;
  if (bytes[2] >= 128) {
    // Remove the "flag" bit from the byte
    bytes[2] &= 0x7F;
    if (this->debug) Serial.println("Removing first bit in the temperature and marking freezing.");
    freezing = true;
  }

  if (expectedParity == bytes[4] || this->skipParityCheck) {
    if (this->debug) {
      if (this->skipParityCheck) {
        Serial.print("Byte parity check skipped. Invalid check might mean incorrect readings. ");
      }
      Serial.print("Parity check: ");
      Serial.println(expectedParity == bytes[4] ? "OK" : "FAILED");
    }
    long humidity = ((bytes[0] << 8) | bytes[1]);
    long temperature = (bytes[2] << 8) | bytes[3];

    this->temperature = freezing ? temperature / -10.0 : temperature / 10.0;
    this->humidity = humidity / 10.0;

    if (this->debug) {
      Serial.print("Temp: ");
      Serial.print(this->temperature);
      Serial.print("°C, Humidity: ");
      Serial.println(this->humidity);
    }
  } else {
    return AM2320SB::STATUS_PARITY_CHECK_FAILED;
  }
  return AM2320SB::STATUS_READ_OK;
}

float AM2320SB::getHumidity() {
  return this->humidity;
}

float AM2320SB::getTemperature() {
  return this->temperature;
}