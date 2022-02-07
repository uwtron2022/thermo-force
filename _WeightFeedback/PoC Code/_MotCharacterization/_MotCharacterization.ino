/*************************************************************
FYDP-Group12
-Program to find motor parameters
  -Run through 0 to 12 volts
  -Turn volt into PWM signal
  -Find stall voltage (IE minimum voltage to move motor)
    -Need to do this for CW, CCW
  -Find equation relating voltage to no load current
  -Find equation relating voltage to stall current, force
-DIR_PIN LOW = CW
-DIR_PIN HIGH = CCW

-Findings
  -CW Stiction: 1.6V
  -CCW Stiction: 1.65V

-W/ 12V supply, at 255PWM actual Vmot = 8.15
  -So range is: 0->8.15 becomes 0->12 becomes 0->255
*************************************************************/
// Includes
#include <ADS1115_lite.h>
#include <HX711_ADC.h>

// Constants
const int16_t BAUDRATE = 9600;
// Loadcell Const
const uint8_t LDCL_DOUT = 18;
const uint8_t LDCL_SCK = 9;
// Motor Const
const uint8_t MOT_BRK = 8;
const uint8_t MOT_PWM = 11;
const uint8_t MOT_DIR = 13;
// Shunt/ADC Const
const uint8_t SHUNT_MULT = 20; // 1/R
const float ADC_VPER_CNT = 0.0078125; // mV

// Loadcell
float ldclCalVal = -60.31;
// Motor
uint8_t motPWM = 0;
// Shunt ADC
int16_t adcCnts = 0;
int16_t adcCnts1 = 0;
int16_t adcCnts2 = 0;
int16_t adcSSCnts = 0;

// Constructors
HX711_ADC ldclADC(LDCL_DOUT, LDCL_SCK);
ADS1115_lite shuntADC(ADS1115_DEFAULT_ADDRESS);

// LDCL Initialize
bool setupLDCL () {
  // Initialize peripheral
  Serial.println("##########Initialize Loadcell##########");
  pinMode(LDCL_DOUT, INPUT);
  pinMode(LDCL_SCK, INPUT);
  int ldclStabilTime = 5000;
  bool ldclTare = true;
  ldclADC.begin();
 
  // Setup peripheral
  ldclADC.start(ldclStabilTime, ldclTare);
  if (ldclADC.getTareTimeoutFlag()) {
    Serial.println("Failed to setup loadcell/r/n");
    return false;
  }
  ldclADC.setCalFactor(ldclCalVal);
  Serial.println("Current calibration factor is: " + String(ldclADC.getCalFactor()));

  // Tare and return
  ldclADC.tareNoDelay();
  delay(1000);
  Serial.println("Loadcell initialized\r\n");
  return true;
}

// MOT Initialize
bool setupMOTShield () {
  // Setup Motor Channel A
  Serial.println("##########Initialize Motor Shield##########");
  pinMode(MOT_BRK, OUTPUT);
  pinMode(MOT_DIR, OUTPUT);
  pinMode(MOT_PWM, OUTPUT);
  digitalWrite(MOT_BRK, LOW);
  digitalWrite(MOT_DIR, LOW);

  // Setup Non-Audible Fast PWM Frequency 31372.55 Hz
  //  // Bit7,5,3=1 & Bit6,4,2=0 for Non-Inverting PWM, Bit1=0 & Bit0=1 for Fast PWM Mode 
  //  TCCR1A = 0;
  //  TCCR1A = 0b10101000 | 0b00000001;
  //  // Bit4=0 & Bit3=1 for Fast PWM Mode, Bit1=1 for No Prescaling
  //  TCCR1B = 0;
  //  TCCR1B = 0b00001000 | 0b00000010;
  Serial.println("Motor initialized\r\n");
  return true;
}

// Shunt ADC Initialize
bool setupShuntADC () {
  // Initialize peripheral
  Serial.println("##########Initialize Shunt ADC##########");
  if (!shuntADC.testConnection()) {
      Serial.println("Shunt ADC connection failed\r\n");
      return false;
  }

  // Setup peripheral
  shuntADC.setGain(ADS1115_REG_CONFIG_PGA_0_256V); // 1Bit=0.0078125mV
  shuntADC.setSampleRate(ADS1115_REG_CONFIG_DR_128SPS); // 8 Samples/Sec
  shuntADC.setMux(ADS1115_REG_CONFIG_MUX_DIFF_0_1);

  // Calc steady-state mot shield current consumption
  uint8_t avgCnts = 10;
  int32_t adcCounts = 0;
  int i=0;
//  while(i<avgCnts) {
//    shuntADC.triggerConversion();
//    int16_t rawCounts = shuntADC.getConversion();
//    if (rawCounts >= 0) {
//      adcCounts += rawCounts;
//      i++;
//    }
//    else
//      i--;
//
//    if(i<0)
//      i = 0;
//  }
  adcSSCnts = adcCounts/avgCnts;
  Serial.println("Avg current consumption: " + String(adcSSCnts*ADC_VPER_CNT*SHUNT_MULT) + "mA");
  Serial.println("Shunt ADC initialized");
  return true;
}

// Setup
void setup() {
  // Initialize Serial
  Serial.begin(BAUDRATE);
  while (!Serial) {} // Wait for initilization
  Serial.println("##########Initialize Serial##########");
  Serial.println("Serial Initialized\r\n");

  // Initialize Peripherals
  bool ldclSucess = setupLDCL();
  bool motSucess = setupMOTShield();
  bool shuntSucess = setupShuntADC();

  // Flush
  Serial.println();
  Serial.flush();
  delay(1000);
}

// Volt to PWM
int16_t voltToPWM(float voltage) {
  // 0V-12V, 0-255PWM
  if (voltage == 0)
    return 0;
  else if (voltage < 0)
    return (256*voltage/12)+1;
  else
    return (256*voltage/12)-1;
}

void loop() {
  // Variables
  float voltage = 0;
  uint8_t avgPeriods = 10;
  uint32_t countsAvg = 0;
  
  // Armature Resistance Measurement
  for (uint8_t i=0; i<5; i++) {
    // Stall Motor Running @ 2, 4, 6, 8, 10 V
    // Measure Current, Calc R
    Serial.println("Enter 'm' to start");
    while (!Serial.available() > 0) {}
    Serial.readString();
    delay(100);

    // Measure
    countsAvg = 0;
    for (uint8_t k=0; k<avgPeriods; k++) {
      shuntADC.triggerConversion();
      int16_t rawCounts = shuntADC.getConversion();
      if (rawCounts >= 0)
        countsAvg += rawCounts-adcSSCnts;
      else
        k--;
      if(k<0)
        k = 0;
    }

    // Ouput
    Serial.println("Avg ADC Counts: " + String(countsAvg/10));
    Serial.flush();
    delay(100);
  }
}
