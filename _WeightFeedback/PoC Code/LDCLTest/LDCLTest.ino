/*************************************************************
FYDP-Group12
-Loadcell Read PoC
*************************************************************/
// ADC includes
#include <HX711_ADC.h>
#if defined(ESP8266)|| defined(ESP32) || defined(AVR)
  #include <EEPROM.h>
#endif

// Constants
const int BAUDRATE = 9600;
const int HX711_DOUT = 18;
const int HX711_SCK = 9;

// Globals
float calibrationValue = -60.31;  // Cal Val from LDCLCal.ino
HX711_ADC forceADC(HX711_DOUT, HX711_SCK);

// Loadcell ADC Init
void setupADC() {
  Serial.println("Loadcell Initialization");
  forceADC.begin();
  unsigned long stabilTime = 2000;
  bool adcTare = true; 

  // Start & setup
  forceADC.start(stabilTime, adcTare);
  if (forceADC.getTareTimeoutFlag()) {
    Serial.println("Timeout, check wiring and pin designations");
    while (1) {};
  }
  else {
    forceADC.setCalFactor(calibrationValue); // user set calibration value
    Serial.println("Current calibration factor is: " + String(forceADC.getCalFactor()));
    Serial.println("Initalization complete");
  }

  // Tare loadcell
  forceADC.tareNoDelay();
  delay(1000);
}

// Main
void setup() {
  Serial.begin(BAUDRATE);
  while (!Serial) {} // Wait for initilization
  setupADC();
}

void loop() {
  // Take measurement
  while (!forceADC.update()) {}
  float ldclForce = forceADC.getData();
  Serial.println("Loadcell Force(g): " + String(ldclForce));
}
