#include <Arduino.h>
#include <Wire.h>
#include <MQUnifiedsensor.h>

#define Board           "ESP-32"
#define Voltage_Resolution 3.3
#define pin AOUT        34 // GPIO pin where the sensor is connected
#define type            "MQ-135"
#define ADC_Bit_Resolution 12
#define RatioMQ135CleanAir  3.6

MQUnifiedsensor MQ135(Board, Voltage_Resolution, ADC_Bit_Resolution, pin, type);

void setup() {
  Serial.begin(115200);
  MQ135.setRegressionMethod(1); 
  MQ135.setA(110.47); // Constants for NOx
  MQ135.setB(-2.862); // Constants for NOx

  MQ135.init();

  // Calibrate the sensor (assume it is in clean air at startup)
  Serial.print("Calibrating please wait.");
  float calcR0 = 0;
  for (int i = 1; i <= 10; i++) {
    MQ135.update();
    calcR0 += MQ135.calibrate(RatioMQ135CleanAir);
    Serial.print(".");
  }
  MQ135.setR0(calcR0 / 10);
  Serial.println(" done!.");

  if (isinf(calcR0)) {
    Serial.println("Warning: Connection issue detected.");
    while (1);
  }

  if (calcR0 == 0) {
    Serial.println("Warning: Connection issue detected.");
    while (1);
  }

  Serial.println("Sensor calibrated successfully.");
}

void loop() {
  MQ135.update();
  float NOx = MQ135.readSensor();
  Serial.print("NOx Concentration: ");
  Serial.print(NOx);
  Serial.println(" ppm");

  delay(2000); // Delay 2 seconds between readings
}
