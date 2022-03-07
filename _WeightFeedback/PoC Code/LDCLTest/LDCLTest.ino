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
volatile boolean ldclMeasRdy;
float calibrationValue = 95;  // Cal Val from LDCLCal.ino
HX711_ADC ldclADC(HX711_DOUT, HX711_SCK);

// Loadcell ADC Init
void setupADC() {
  Serial.println("Loadcell Initialization");
  ldclADC.begin();
  unsigned long stabilTime = 2000;
  bool adcTare = true; 

  // Start & setup
  ldclADC.start(stabilTime, adcTare);
  if (ldclADC.getTareTimeoutFlag()) {
    Serial.println("Timeout, check wiring and pin designations");
    while (1) {};
  }
  else {
    ldclADC.setCalFactor(calibrationValue); // user set calibration value
    Serial.println("Current calibration factor is: " + String(ldclADC.getCalFactor()));
    Serial.println("Initalization complete");
  }

  // Tare loadcell
  attachInterrupt(digitalPinToInterrupt(HX711_DOUT), ldclReadyISR, FALLING);
  ldclADC.tareNoDelay();
  delay(1000);
}

// Loadcell ISR Function
void ldclReadyISR() {
  // Set flag for ldcl ready
  if(ldclADC.update())
    ldclMeasRdy = true;
}

// Main
void setup() {
  Serial.begin(BAUDRATE);
  while (!Serial) {} // Wait for initilization
  setupADC();
}

void loop() {
  // Take measurement
  if (ldclMeasRdy) {
    float ldclForce = ldclADC.getData();
    ldclMeasRdy = 0;
    Serial.println("Loadcell Force(g): " + String(ldclForce));
  }
}
