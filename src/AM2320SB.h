#ifndef AM2320SB_h
#define AM2320SB_h

class AM2320SB {
private:
  int pin;
  float temperature;
  float humidity;
  const int AM2320_INIT_LOW_MS = 850;  // Pulse lenght to activate sensor

public:
  AM2320SB::AM2320SB(int pin);
  int read();
  float getTemperature();
  float getHumidity();
  bool debug = false;
  bool skipParityCheck = false;
  static const int STATUS_READ_OK = 1;
  static const int STATUS_NOT_RESPONDING = 9;
  static const int STATUS_READ_ERROR = 10;
  static const int STATUS_PARITY_CHECK_FAILED = 11;
};

#endif