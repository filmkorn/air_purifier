#include "esphome.h"

class UartReadLineSensor : public Component, public UARTDevice, public Sensor {

 protected:
//  uint8_t max_samples = 30;
//  float data[max_samples];
//  uint8_t dataindex;

 public:
  UartReadLineSensor(UARTComponent *parent) : UARTDevice(parent) {}

  void setup() override {
    // nothing to do here
    check_uart_settings(9600);
  }

  int readline(int readch, float *result)
  {
    static int pos = 0;

   // ESP_LOGD("custom_uart.h", "readline(): %d", readch);
   switch (readch) {
    case 255: // Start bit
      pos = 0;
      break;
    default:
      pos++;
   }
   switch (pos) {
    case 3:
      // Whole numbers.
      *result = static_cast<float>(readch);
      break;
    case 4:
      // Decimals.
      *result += (static_cast<float>(readch) / 100.0);
      return 1;
   }
    // No end of line has been found, so return -1.
    return -1;
  }

  void loop() override {
    // Numbers of collected samples to average before publishing the average.
    float result = 0;
    float *resultPt = &result;
    int readch;

    while (available()) {
       readch = read();
       if(readline(readch, resultPt) > 0) {
          // TODO: Find a better conversion to parts per billion.
          // This conversion is a bit arbitrary.
          // The range is 0.5% to 50% according to the data sheet.
          publish_state((result - 0.5) * 100.0);
       }
     }
   }
};
