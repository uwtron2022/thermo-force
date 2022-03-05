/*************************************************************
FYDP-Group12
-Motor Testing
-With MOT_DIR HIGH: CW-Anti Gravity, -ve Voltage Req. -ve Current Read
-With MOT_DIR LOW: CCW-Gravity, +ve Voltage Req, +ve Current Read
-Loadcell accurate to about +/-10%, or +/-100g @ 0
*************************************************************/
// Incudes
#include <ADS1115_lite.h>
#include <HX711_ADC.h>

// Constants
const int16_t BAUDRATE = 9600;
const bool DEBUG_PRINT = true;
// BLE Const
const uint8_t BLE_FAIL_MAX = 10;
const String BLE_EMPTY_RX = "nodata";
// Loadcell Const
const uint8_t LDCL_DOUT = 18; // Interrupt pin
const uint8_t LDCL_SCK = 9;
// Motor Const
const uint8_t MOT_BRK = 8;
const uint8_t MOT_PWM = 11;
const uint8_t MOT_DIR = 13;
const float MOT_STICTION = 1.55;
// Shunt/ADC Const
const uint8_t SHUNT_MULT = 20; // 1/R
const float ADC_VPER_CNT = 0.0078125; // mV
// Resistive Heaters & Peltier Plate Const
const uint8_t HTR_THMB = 42;
const uint8_t HTR_INDX = 38;
const uint8_t HTR_MIDL = 30; // Middle two fingers
const uint8_t HTR_PNKY = 26;
const uint8_t HTR_PALM = 34;
const uint8_t PELT_CLR = 22;
// Thermistor Const
const uint8_t THERM_THMB = 8;
const uint8_t THERM_INDX = 9;
const uint8_t THERM_MIDL1 = 10;
const uint8_t THERM_MIDL2 = 11;
const uint8_t THERM_PNKY = 12;
const uint8_t THERM_PALM = 13;
const uint8_t THERM_CLR = 14;
const uint8_t THERM_AMB = 15;
// Temperature Const
const uint8_t TEMP_MAX = 40;
const uint8_t TEMP_MIN = 10;
const uint16_t TEMP_PERIOD = 250; // Milliseconds
// Weight Control Const
const uint8_t FUZZY_CUR = 100;
const uint8_t FUZZY_MASS = 200;
const uint16_t MASS_MAX = 12500; // Grams
const uint16_t MASS_MIN = 0; // Grams
const float GRAM_GRAVITY = 0.981;
const float SPOOL_RAD = 2.5; // cm
const float TORQ_CONST = 1.2012439*10; // mA/Ncm

// Globals
// BLE
bool bleConnected = false;
bool  bleHeartbeat = false;
uint8_t bleCommFails = 0;
// Loadcell
volatile bool ldclMeasRdy = false;
int16_t ldclGrams1 = 0;
int16_t ldclGrams = 0;
float ldclCalVal = 0.73;
// Motor
uint8_t motPWM = 0;
// Shunt ADC
uint16_t adcSSCnts = 0;
// Temp Feedback
bool htrOn = false;
uint16_t tempOnTimes[6] = {0, 0, 0, 0, 0, 0};
uint16_t currTime = 0;
uint32_t timeElapsed = 0;
float refTemp = 0;
// Weight Feedback
bool firstLoop = true;
bool motDir = false;
int16_t massDelta = 0;
int16_t currErr = 0;
int16_t currIntegralErr = 0;
float uI1 = 0;
float uL1 = 0;
float refMass = 0;
// Feedback Scaling
int8_t maxTemp = 0;
int8_t minTemp = 0;
uint16_t maxMass = 0;
uint16_t minMass = 0;

// Constructors
HX711_ADC ldclADC(LDCL_DOUT, LDCL_SCK);
ADS1115_lite shuntADC(ADS1115_DEFAULT_ADDRESS);

// LDCL Initialize
bool setupLDCL () {
  // Initialize peripheral
  Serial.println("##########Initialize Loadcell##########");
  pinMode(LDCL_DOUT, INPUT);
  pinMode(LDCL_SCK, INPUT);
  uint16_t ldclStabilTime = 5000;
  bool ldclTare = true;
  ldclADC.begin();
 
  // Setup peripheral
  ldclADC.start(ldclStabilTime, ldclTare);
  if (ldclADC.getTareTimeoutFlag()) {
    Serial.println("Failed to setup loadcell\r\n");
    return false;
  }
  ldclADC.setCalFactor(ldclCalVal);
  attachInterrupt(digitalPinToInterrupt(LDCL_DOUT), ldclReadyISR, FALLING);
  Serial.println("Current calibration factor is: " + String(ldclADC.getCalFactor()));
  Serial.println("Loadcell initialized\r\n");

  // Tare and return
  ldclADC.tareNoDelay();
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
  analogWrite(MOT_PWM, 0);

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
  Serial.println("Shunt ADC initialized");

  // Calc steady-state mot shield current consumption
  uint8_t totRuns = 0;
  uint32_t adcCounts = 0;
  for (uint8_t i=0; i<10; i++) {
    shuntADC.triggerConversion();
    int16_t rawCounts = shuntADC.getConversion();
    if (rawCounts >= 0)
      adcCounts += rawCounts;
    else if (i >= 1)
      i--;
    else
      i = 0;

    // Prevent infinite loop
    if (totRuns >= 50)
      return false;
    totRuns += 1;
  }

  // Return
  adcSSCnts = adcCounts/10;
  Serial.println("Avg current consumption: " + String(adcSSCnts*ADC_VPER_CNT*SHUNT_MULT) + "\r\n");
  return true;
}

// 5Pt Current Measurement
float currMeasSmoothed() {
  // Read and average current counts
  uint8_t totRuns = 0;
  uint32_t countsAvg = 0;
  for (uint8_t i=0; i<5; i++) {
      // Read measurement
      shuntADC.triggerConversion();
      int16_t rawCounts = shuntADC.getConversion();
      if (rawCounts >= 0 && rawCounts > adcSSCnts)
        countsAvg += rawCounts-adcSSCnts;
      else if (rawCounts >= 0 && rawCounts < adcSSCnts)
        countsAvg += 0;
      else if (i > 1)
        i--;
      else
        i = 0;

      // Prevent infinite loop
      if (totRuns >= 10)
        break;
      totRuns += 1;
    }

  // Return mA's, change sign according to direction
  return (countsAvg/5)*SHUNT_MULT*ADC_VPER_CNT;
}

// Loadcell ISR Funttion
void ldclReadyISR() {
  // Set flag for ldcl ready
  if(ldclADC.update())
    ldclMeasRdy = true;
}

// Volt to PWM  
int16_t voltToPWM(float voltage) {
  // 0V-12V, 0-255PWM
  if (voltage > 12)
    voltage = 12;
  else if (voltage < -12)
    voltage = -12;
  if (voltage == 0)
    return 0;
  else if (voltage < 0)
    return (256*voltage/12)+1;
  else
    return (256*voltage/12)-1;
}

// Linear Interpolation
float linInterp(int x1, int x3, int y1, int y3, float x) {
  float y2 = (y1/(x3 - x1))*(x3 - x) + (y3/(x1-x3))*(x1 - x);
  if (y2 > y3)
    return y3;
  else if (y2 < y1)
    return y1;
  else
    return y2;
}

// Setup 
void setup() {
  // Initialize serial
  Serial.begin(BAUDRATE);
  while (!Serial) {} // Wait for initilization
  Serial.println("##########Initialize Serial##########");
  Serial.println("Serial Initialized\r\n");

  // Initialize peripherals
  //bool bleSuccess = setupBLE();
  bool ldclSuccess = setupLDCL();
  bool motSuccess = setupMOTShield();
  bool shuntSuccess = setupShuntADC();
  //bool heatCoolSuccess = setupHeatCool();
  //bool thermistorSuccess = setupThermistors();
//  if (!(bleSuccess && ldclSuccess && shuntSuccess && heatCoolSuccess && thermistorSuccess)) {
//    if(!bleSuccess)
//       Serial.println("***BLE INITIALIZATION FAIL***\r\n");
//    if(!ldclSuccess)
//       Serial.println("***LDCL INITIALIZATION FAIL***\r\n");
//    if(!shuntSuccess)
//       Serial.println("***SHUNT ADC INITIALIZATION FAIL***\r\n");
//    if(!heatCoolSuccess)
//       Serial.println("***HEATERS/COOLER INITIALIZATION FAIL***\r\n");
//    if(!thermistorSuccess)
//       Serial.println("***THERMISTOR INITIALIZATION FAIL***\r\n");
//    while(true) {};
//  }

  // Flush
  Serial.println();
  Serial.flush();
  delay(1000);

  // Get start time
  currTime = millis();
  timeElapsed = millis();
}

// Main
void loop() { 
  // Read BLE
//  String cmdLine = bleRX();
  String cmdLine = "-min -t -10 -max -t 50 -min -w 0 -max -w 12500 -t 25 -w 1000";
  if (cmdLine != BLE_EMPTY_RX) { 
    // Command switches
    String maxSwitch = "-max";
    String minSwitch = "-min";
    String tempSwitch = "-t";
    String massSwitch = "-w";
    char *splitcurrToken = " ";
    char *currToken;

    // Flags
    bool setMax = false;
    bool setMin = false;
    bool setTemp = false;
    bool setMass = false;
    
    // Parse
    char cmdTemp[cmdLine.length() + 1];
    cmdLine.toCharArray(cmdTemp, cmdLine.length() + 1);
    currToken = strtok(cmdTemp, splitcurrToken);

    // Debug print
    if (DEBUG_PRINT)
      Serial.println("***Bluetooth Values***");

    // Update
    while (currToken != NULL) {
      // Debug print
//      if (DEBUG_PRINT)
//        Serial.println("Bluetooth Token: " + String(currToken));

            // Set flags and values
      if (String(currToken) == maxSwitch)
        setMax = true;
      else if (String(currToken) == minSwitch)
        setMin = true;
      else if (String(currToken) == tempSwitch)
        setTemp = true;
      else if (String(currToken) == massSwitch)
        setMass = true;
      else {
        if (setTemp) {
          if (setMax) {
            maxTemp = String(currToken).toInt();
            setMax = false;
            setTemp = false;
          } 
          else if (setMin) {
            minTemp = String(currToken).toInt();
            setMin = false;
            setTemp = false;
          }
          else {
            refTemp = linInterp(minTemp, maxTemp, TEMP_MIN, TEMP_MAX, String(currToken).toFloat());
            setTemp = false;
          }
        }
        if (setMass) {
          if (setMax) {
            maxMass = String(currToken).toInt();
            setMax = false;
            setMass = false;
          }
          else if (setMin) {
            minMass = String(currToken).toInt();
            setMin = false;
            setMass = false;
          }
          else {
            if (String(currToken).toInt() == 0)
              refMass = 0;
            else
              refMass = linInterp(minMass, maxMass, MASS_MIN, MASS_MAX, String(currToken).toFloat());
            setMass = false;           
          }
        }
      }

      // Read new currToken
      currToken = strtok(NULL, splitcurrToken);
    }
  }
  if (DEBUG_PRINT) {
    Serial.println("Ref Temp: " + String(refTemp));
    Serial.println("Ref Mass: " + String(refMass) + "\r\n");
  }
  
  // Read weight feedback sensors
  float currMilliAmps = (motDir == LOW) ? currMeasSmoothed() : -currMeasSmoothed();
  currErr = refMass*GRAM_GRAVITY*SPOOL_RAD/TORQ_CONST - currMilliAmps;
  if (ldclMeasRdy) {
    ldclGrams1 = ldclGrams;
    ldclGrams = ldclADC.getData();
    if (ldclGrams < 0)
      ldclGrams = 0;
    massDelta = (firstLoop == false) ? ldclGrams1 - ldclGrams : 0;
    ldclMeasRdy = false;
  }
  if (DEBUG_PRINT) {
    Serial.println("***Weight Feedback Values***");
    Serial.println("Current(mA): " + String(currMilliAmps));
    Serial.println("Current Error: " + String(currErr));
    Serial.println("Loadcell(g): " + String(ldclGrams));
    Serial.println("Loadcell Past(g): " + String(ldclGrams1));
    Serial.println("LDCL Error: " + String(massDelta)+ "\r\n");
  }

  // Fuse weight measurements w/ fuzzy logic
  float currR = 0, currS = 0, currD = 0;
  float ldclR = 0, ldclS = 0, ldclD = 0;
  float uI = uI1, uL = uI1, uIBar = uI1;
  if (currErr <= -FUZZY_CUR) {
    currR = 1;
    currS = 0;
    currD = 0;
  }
  else if ((-FUZZY_CUR < currErr) && (currErr  <= 0)) {
    currR = -1.0*currErr/FUZZY_CUR;
    currS = 1.0*(currErr+FUZZY_CUR)/FUZZY_CUR;
    currD = 0;
  }
  else if ((0 < currErr) && (currErr < FUZZY_CUR)) {
    currR = 0;
    currS = 1.0*(FUZZY_CUR-currErr)/FUZZY_CUR;
    currD = 1.0*currErr/FUZZY_CUR;
  }
  else {  // currIntegralErr >= FUZZY_CUR
    currR = 0;
    currS = 0;
    currD = 1;
  }
  if (massDelta <= -2*FUZZY_MASS) {
    ldclR = 1;
    ldclS = 0;
    ldclD = 0;
  }
  else if ((-2*FUZZY_MASS < massDelta) && (massDelta <= 0)) {
    ldclR = -1.0*massDelta/(2*FUZZY_MASS);
    ldclS = 1.0*(massDelta+2*FUZZY_MASS)/(2*FUZZY_MASS);
    ldclD = 0;
  }
  else if ((0 < massDelta) && (massDelta < FUZZY_MASS)) {
    ldclR = 0;
    ldclS = 1.0*(FUZZY_MASS-massDelta)/FUZZY_MASS;
    ldclD = 1.0*massDelta/FUZZY_MASS;
  }
  else { // massDelta >= FUZZY_MASS
    ldclR = 0;
    ldclS = 0;
    ldclD = 1;
  }
  if ((currR < currS) && (currR > currD))
    uI = -currR;
  else if ((currD > currR) && (currD > currS))
    uI = currS;
  else // (currD > currR) && currD > currS
    uI = currD;
  if ((ldclR > ldclS) && (ldclR > ldclD))
    uL = -ldclR;
  else if ((ldclS > ldclR) && (ldclS > ldclD))
    uL = ldclS;
  else // (ldclD > ldclR) && ldclD > ldclS
    uL = ldclD;
  float trueDir = 0.1*uI1 + 0.3*uI + 0.2*uL1 + 0.4*uL;
  if (trueDir < 0)
    digitalWrite(MOT_DIR, HIGH); // Raising
  else if (trueDir > 0)
    digitalWrite(MOT_DIR, LOW); // Stationary or Lowering
  if (DEBUG_PRINT) {
    Serial.println("***Weight Feedback Fuzzy Logic***");
    Serial.println("Current Raise: " + String(currR));
    Serial.println("Current Stationary: " + String(currS));
    Serial.println("Current Drop: " + String(currD));
    Serial.println("LDCL Raise: " + String(ldclR));
    Serial.println("LDCL Stationary: " + String(ldclS));
    Serial.println("LDCL Drop: " + String(ldclD));
    Serial.println("Fuzzy Current Previous: " + String(uI1));
    Serial.println("Fuzzy Current: " + String(uI));
    Serial.println("Fuzzy Current Avg: " + String(uIBar));
    Serial.println("Fuzzy LDCL: " + String(uL));
    Serial.println("Fuzzy Sel Direction: " + String(trueDir));
    Serial.println("Selected Motor Dir (High-CW, Low-CCW): " + String(digitalRead(MOT_DIR)) + "\r\n");
  }
      
  // Calculate weight control signals
  bool motDirCurrent = digitalRead(MOT_DIR);
  int16_t vRef = 0;
  currIntegralErr += currErr;
//  if (motDirCurrent != motDir) // If switching dir reset integral error
//    currIntegralErr = (motDirCurrent == LOW) ? currErr : -currErr;
  if (motDirCurrent == 0) { // counter-clockwise
    float vCtrl = 2.5*currIntegralErr/1000;
    if ((vCtrl < 0) && (vCtrl > -MOT_STICTION))
      vCtrl -= 0.5;
    else if ((vCtrl > 0) && (vCtrl < MOT_STICTION))
      vCtrl += 0.5;
    if (vCtrl > 12)
      vCtrl = 12;
    else if (vCtrl < -12)
      vCtrl = -12;
    vRef = voltToPWM(vCtrl);
    if (DEBUG_PRINT) {
      Serial.println("***Weight Feedback Signals***");
      Serial.println("Current Integrated Error: " + String(currIntegralErr));
      Serial.println("Requested Voltage: " + String(vCtrl));
      Serial.println("PWM Signal: " + String(vRef) + "\r\n");
    }
  }
  else { // clockwise - MAKE CONTROLLER FOR THIS CASE 
    float vCtrl = 2;
    if ((vCtrl < 0) && (vCtrl > -MOT_STICTION))
      vCtrl -= 0.25;
    else if ((vCtrl > 0) && (vCtrl < MOT_STICTION))
      vCtrl += 0.25;
    if (vCtrl > 12)
      vCtrl = 12;
    else if (vCtrl < -12)
      vCtrl = -12;
    vRef = voltToPWM(vCtrl);
    if (DEBUG_PRINT) {
      Serial.println("***Weight Feedback Signals***");
      Serial.println("Requested Voltage: " + String(vCtrl));
      Serial.println("PWM Signal: " + String(vRef) + "\r\n");
    }
  }
  
  // Write weight output
  if (vRef >= 0)
    analogWrite(MOT_PWM, vRef);
  else if (vRef < 0)
    analogWrite(MOT_PWM, abs(vRef));

  // Save values for next loop
  firstLoop = false;
  uI1 = uI;
  uL1 = uL;
  motDir = motDirCurrent;

  // TESTING
//  analogWrite(MOT_PWM, 0);
//  delay(10000);
}
