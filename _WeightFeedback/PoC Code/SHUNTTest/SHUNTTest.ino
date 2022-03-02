/*************************************************************
FYDP-Group12
-Read mot current with shunt resistor
-Motor winding resistance is 2.6 Ohms
*************************************************************/
// Shunt ADC Includes
#include <ADS1115_lite.h>

// Constants
const int BAUDRATE = 9600;
const int MOT_BRK = 8;
const int MOT_PWM = 11;
const int MOT_DIR = 13;
const int SHUNT_MULT = 20;
const float V_PER_CNT = 0.0078125; // In mV

// Globals
int pwmCmd = 0;
ADS1115_lite shuntADC(ADS1115_DEFAULT_ADDRESS);

// Motor Shield Initalize
void setupMotorShield() {
  // Setup Motor Channel A
  pinMode(MOT_BRK, OUTPUT);
  pinMode(MOT_DIR, OUTPUT);
  pinMode(MOT_PWM, OUTPUT);
  digitalWrite(MOT_BRK, LOW);
  digitalWrite(MOT_DIR, LOW);
  analogWrite(MOT_PWM, 50);
  Serial.println("Motor Initialized");

  // Start Motor
  digitalWrite(MOT_BRK, LOW);
  digitalWrite(MOT_DIR, LOW);
  analogWrite(MOT_PWM, pwmCmd);
  Serial.println("Running Motor at PWM: " + String(pwmCmd));
}

// Shunt ADC Initialize
void setupShuntADC() {
  // Setup ADC, Set Gain
  if (!shuntADC.testConnection()) {
      Serial.println("ADS1115 Connection Failed");
      while (true) {}
  }
  shuntADC.setGain(ADS1115_REG_CONFIG_PGA_0_256V); // 1Bit=0.0078125mV
  shuntADC.setSampleRate(ADS1115_REG_CONFIG_DR_128SPS); // 8 Samples/Sec
  shuntADC.setMux(ADS1115_REG_CONFIG_MUX_DIFF_0_1);
  Serial.println("Shunt ADC Initialized");
}

// Main
void setup() {
  Serial.begin(BAUDRATE);
  while (!Serial) {} // Wait for Initilization
  setupMotorShield();
  setupShuntADC();
  Serial.flush();
  delay(100);
}

void loop() {
  // Plotting
  shuntADC.setMux(ADS1115_REG_CONFIG_MUX_DIFF_0_1);
  shuntADC.triggerConversion();
  int16_t rawCounts = shuntADC.getConversion();
  if (rawCounts > 0)
    Serial.println(rawCounts*SHUNT_MULT*V_PER_CNT);
}
