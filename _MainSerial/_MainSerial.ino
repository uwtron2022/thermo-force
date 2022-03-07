/*************************************************************
FYDP-Group12
-Main program for ThermoForce temperature/weight control
  -With MOT_DIR HIGH: CW-Anti Gravity, -ve Voltage Req. -ve Current Read
  -With MOT_DIR LOW: CCW-Gravity, +ve Voltage Req, +ve Current Read
  -Shunt accurate to about +/-10% flat
  -Loadcell accurate to about +/-15%, or +/-100g, whichever is larger
*************************************************************/
// Includes
#include <HX711_ADC.h>
#include <ADS1115_lite.h>

// Constants
const bool DEBUG_PRINT = true;
const int16_t BAUDRATE = 9600;
// Serial Const
const uint8_t SERIAL_FAIL_MAX = 10;
const String SERIAL_EMPTY_RX = "nodata";
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
const uint8_t TEMP_MIN = 20;
const uint16_t TEMP_PERIOD = 250; // Milliseconds
// Loadcell Const
const uint8_t LDCL_DOUT = 18;
const uint8_t LDCL_SCK = 9;
const float LDCL_CAL = 95;
// Motor Const
const uint8_t MOT_BRK = 8;
const uint8_t MOT_PWM = 11;
const uint8_t MOT_DIR = 13;
const uint8_t MOT_STICTION = 1.55;
// Shunt/ADC Const
const uint8_t SHUNT_MULT = 20; // 1/R
const float ADC_VPER_CNT = 0.0078125; // mV
// Weight Control Const
const int16_t FUZZY_CURR = 100;
const int16_t FUZZY_MASS = 200;
const uint8_t FUZZY_MASS_OFFSET = 100;
const uint16_t MASS_MAX = 10000; // Grams
const uint16_t MASS_MIN = 0; // Grams
const float GRAM_GRAVITY = 0.981; // N/g
const float SPOOL_RAD = 2.5; // cm
const float TORQ_CONST = 1.2012439*10; // mA/Ncm
const float FUZZY_DEADBAND = 0.1;

// Globals
// Serial
bool serialConnected = false;
bool  serialHeartbeat = false;
uint8_t serialCommFails = 0;
// Temp Feedback
bool htrOn = false;
uint16_t tempOnTimes[6] = {0, 0, 0, 0, 0, 0};
uint16_t currTime = 0;
uint32_t timeElapsed = 0;
float refTemp = 0;
// Loadcell
volatile boolean ldclMeasRdy = false;
int16_t ldclGrams = 0;
int16_t ldclGrams1 = 0;
// Motor
bool motDirection = LOW;
uint8_t motPWM = 0;
int16_t motCurr = 0;
int16_t motCurr1 = 0;
// Shunt ADC
uint16_t adcSSCnts = 0;
// Weight Feedback
bool firstLoop = true;
int16_t massDelta = 0;
int16_t massDelta1 = 0;
int16_t currDelta = 0;
int16_t currDelta1 = 0;
int32_t currIntegralErr = 0;
float uI1 = 0;
float uL1 = 0;
float refMass = 0;
// Feedback Scaling
int8_t minTemp = 0;
int8_t maxTemp = 0;
uint16_t minMass = 0;
uint16_t maxMass = 0;

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
  ldclADC.setCalFactor(LDCL_CAL);
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
  Serial.println("Avg current consumption (mA): " + String(adcSSCnts*ADC_VPER_CNT*SHUNT_MULT) + "\r\n");
  return true;
}

// Heater/Cooler Initialization
bool setupHeatCool() {
  // Start
  Serial.println("##########Initialize Heaters/Cooler##########");

  // Set pinMode
//  pinMode(HTR_THMB, OUTPUT);
//  pinMode(HTR_INDX, OUTPUT);
//  pinMode(HTR_MIDL, OUTPUT);
//  pinMode(HTR_PNKY, OUTPUT);
//  pinMode(HTR_PALM, OUTPUT);
//  pinMode(PELT_CLR, OUTPUT);
  
  // Turn off
//  digitalWrite(HTR_THMB, HIGH);
//  digitalWrite(HTR_INDX, HIGH);
//  digitalWrite(HTR_MIDL, HIGH);
//  digitalWrite(HTR_PNKY, HIGH);
//  digitalWrite(HTR_PALM, HIGH);
//  digitalWrite(PELT_CLR, HIGH);

  // Success
  Serial.println("Heaters/Cooler initialized\r\n");
  return true;
}

// Thermistor Initialization
bool setupThermistors() {
  // Start
  Serial.println("##########Initialize Thermistors##########");

  // Set pinMode
//  pinMode(THERM_THMB, INPUT);
//  pinMode(THERM_INDX, INPUT);
//  pinMode(THERM_MIDL1, INPUT);
//  pinMode(THERM_MIDL2, INPUT);
//  pinMode(THERM_PNKY, INPUT);
//  pinMode(THERM_PALM, INPUT);
//  pinMode(THERM_CLR, INPUT);
//  pinMode(THERM_AMB, INPUT);

  // Success
  Serial.println("Thermistors initialized\r\n");
  return true;
}

// Failsafe Function
void failSafe() {
// Turn off heaters/coolers
  digitalWrite(HTR_THMB, HIGH);
  digitalWrite(HTR_INDX, HIGH);
  digitalWrite(HTR_MIDL, HIGH);
  digitalWrite(HTR_PNKY, HIGH);
  digitalWrite(HTR_PALM, HIGH);
  digitalWrite(PELT_CLR, HIGH);

  // Turn off motor
  digitalWrite(MOT_PWM, LOW);
  digitalWrite(MOT_BRK, HIGH);

  // Wait
  Serial.println("##########FAILSAFE TRIGGERED##########");
  Serial.println("##########PROGRAM STOPPED##########");
  while(true) {};
}

// Serial Rx
String serialRX() {
  // Check for incoming char
  if(Serial.available()){
    String recvMsg;
    recvMsg = Serial.readString();
    if (recvMsg == "") // No Data
      return SERIAL_EMPTY_RX;
    else {
      // Read from buffer
      return recvMsg;
    }
  }
  return SERIAL_EMPTY_RX;
}

// Serial Tx
bool serialTX(String msg) {
  // Check connection
  if(Serial) {
    size_t numBytes = Serial.write(msg.c_str());
    if (numBytes == 0)
      return false;
    // Return success
    return true;
  }
  return false;
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

// Loadcell ISR Function
void ldclReadyISR() {
  // Set flag for ldcl ready
  if(ldclADC.update())
    ldclMeasRdy = true;
}

// Linear Interpolation
float linInterp(int x1, int x3, int y1, int y3, float x) {
  float y2 = (1.0*y1/(x3 - x1))*(1.0*(x3 - x)) + (1.0*y3/(x1-x3))*(1.0*(x1 - x));
  if (y2 > y3)
    return y3;
  else if (y2 < y1)
    return y1;
  else
    return y2;
}

// Volt to Temp
float voltToTemp(uint8_t pinNum) {
  // Read pin, convert, return
  return 0.1319*analogRead(pinNum)-40;
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

// Setup 
void setup() {
  // Initialize serial
  Serial.begin(BAUDRATE);
  while (!Serial) {} // Wait for initilization
  Serial.println("##########Initialize Serial##########");
  Serial.println("Serial Initialized\r\n");

  // Initialize peripherals
  bool ldclSuccess = setupLDCL();
  bool motSuccess = setupMOTShield();
  bool shuntSuccess = setupShuntADC();
  bool heatCoolSuccess = setupHeatCool();
  bool thermistorSuccess = setupThermistors();
  if (!(ldclSuccess && shuntSuccess && heatCoolSuccess && thermistorSuccess)) {
    if(!ldclSuccess)
       Serial.println("***LDCL INITIALIZATION FAIL***\r\n");
    if(!shuntSuccess)
       Serial.println("***SHUNT ADC INITIALIZATION FAIL***\r\n");
    if(!heatCoolSuccess)
       Serial.println("***HEATERS/COOLER INITIALIZATION FAIL***\r\n");
    if(!thermistorSuccess)
       Serial.println("***THERMISTOR INITIALIZATION FAIL***\r\n");
    while(true) {};
  }

  // Flush
  Serial.println("***INITIALIZATION COMPLETE: STARTING PROGRAM***\r\n");
  Serial.flush();
  delay(1000);

  // Get start time
  currTime = millis();
  timeElapsed = millis();
}

// Main
void loop() {
  /* **********BLUETOOTH********** */
  // Read Serial
  String cmdLine = serialRX();
  //String cmdLine = "-min -t -10 -max -t 50 -min -w 0 -max -w 10000 -t 50 -w 1000";
  if (DEBUG_PRINT)
  Serial.println("***Serial Values***");
  if (cmdLine != SERIAL_EMPTY_RX) {
    // Debug prints
    if (DEBUG_PRINT)
      Serial.println("Received Command: " + cmdLine);

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
    while (currToken != NULL) {
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
    Serial.println("Min Temp: " + String(minTemp));
    Serial.println("Max Temp: " + String(maxTemp));
    Serial.println("Ref Temp: " + String(refTemp));
    Serial.println("Min Mass: " + String(minMass));
    Serial.println("Max Mass: " + String(maxMass));
    Serial.println("Ref Mass: " + String(refMass) + "\r\n");
  }
  
  // Send heartbeat
  serialCommFails = (serialTX(String(serialHeartbeat) + "\r\n") == false) ? (serialCommFails + 1) : 0;
  serialHeartbeat = !serialHeartbeat;
  if (serialCommFails >= SERIAL_FAIL_MAX)
    failSafe();

  /* **********TEMPERATURE CONTROL********** */
  // Read temperature sensors
  int8_t handTemps[6];
  int8_t midl1Temp = voltToTemp(THERM_MIDL1);
  int8_t midl2Temp = voltToTemp(THERM_MIDL2);
  handTemps[0] = voltToTemp(THERM_THMB);
  handTemps[1] = voltToTemp(THERM_INDX);
  handTemps[2] = 0.5*(midl1Temp+midl2Temp);
  handTemps[3] = voltToTemp(THERM_PNKY);
  handTemps[4] = voltToTemp(THERM_PALM);
  int8_t clrTemp = voltToTemp(THERM_CLR);
  int8_t ambTemp = voltToTemp(THERM_AMB); // UNUSED
  int8_t avgTemp = (1.0/6.0)*(handTemps[0]+handTemps[1]+handTemps[2]+handTemps[3]+handTemps[4]+clrTemp);
  if (DEBUG_PRINT) {
    Serial.println("***Thermistor Values***");
    Serial.println("Thumb(degC): " + String(handTemps[0]));
    Serial.println("Index(degC): " + String(handTemps[1]));
    Serial.println("Middle 1(degC): " + String(midl1Temp));
    Serial.println("Middle 2(degC): " + String(midl2Temp));
    Serial.println("Pinky(degC): " + String(handTemps[3]));
    Serial.println("Palm(degC): " + String(handTemps[4]));
    Serial.println("Cooler(degC): " + String(clrTemp));
    Serial.println("Ambient(degC): " + String(ambTemp));
    Serial.println("Average(degC): " + String(avgTemp) + "\r\n");
  }

  // Calculate temperature control signals
  htrOn = (refTemp >= avgTemp) ? true : false; // Heating or cooling
  if (htrOn) { // Calculate heater control signals
    float tempErr[5];
    for (int i=0; i < 5; i++) {
      tempErr[i] = refTemp - handTemps[i];
      if (tempErr[i] >= 0)
        tempOnTimes[i] += 3*tempErr[i];
      else if (tempErr[i] < 0)
        tempOnTimes[i] -= 8*tempErr[i];
      if (tempOnTimes[i] > TEMP_PERIOD)
        tempOnTimes[i] = TEMP_PERIOD;
    }
    
    // Turn cooler off
    tempOnTimes[5] = 0;
  }
  else if (!htrOn) { // Calculate cooler control signal
    float tempErr = refTemp - avgTemp;
    if (tempErr <= 0)
      tempOnTimes[5] -= 10*tempErr; 
    else if (tempErr > 0)
      tempOnTimes[5] -= 25*tempErr;
    if (tempOnTimes[5] > TEMP_PERIOD)
      tempOnTimes[5] = TEMP_PERIOD;
      
    // Turn heaters off
    tempOnTimes[0] = 0;
    tempOnTimes[1] = 0;
    tempOnTimes[2] = 0;
    tempOnTimes[3] = 0;
    tempOnTimes[4] = 0;
  }
  if (DEBUG_PRINT) {
    Serial.println("***Temp On Times(mS)***");
    Serial.println("Heater On: " + String(htrOn));
    Serial.println("Thumb: " + String(tempOnTimes[0]));
    Serial.println("Index: " + String(tempOnTimes[1]));
    Serial.println("Middles: " + String(tempOnTimes[2]));
    Serial.println("Pinky: " + String(tempOnTimes[3]));
    Serial.println("Palm: " + String(tempOnTimes[4]));
    Serial.println("Cooler: " + String(tempOnTimes[5]) + "\r\n");
  }

  // Write temperature output
  if (htrOn) {
    if (currTime < tempOnTimes[0] && handTemps[0] < TEMP_MAX) // Thumb
      digitalWrite(HTR_THMB, LOW);
    else
      digitalWrite(HTR_THMB, HIGH);
    if (currTime < tempOnTimes[1] && handTemps[1] < TEMP_MAX) // Index
      digitalWrite(HTR_INDX, LOW);
    else
      digitalWrite(HTR_INDX, HIGH);
    if (currTime < tempOnTimes[2] && midl1Temp < TEMP_MAX && midl2Temp < TEMP_MAX) // Middles
      digitalWrite(HTR_MIDL, LOW);
    else
      digitalWrite(HTR_MIDL, HIGH && handTemps[1] < TEMP_MAX);
    if (currTime < tempOnTimes[3]) // Pinky
      digitalWrite(HTR_PNKY, LOW);
    else
      digitalWrite(HTR_PNKY, HIGH);
    if (currTime < tempOnTimes[4] && handTemps[1] < TEMP_MAX) // Palm
      digitalWrite(HTR_PALM, LOW);
    else
      digitalWrite(HTR_PALM, HIGH);
    digitalWrite(PELT_CLR, HIGH);
  }
  else if (!htrOn) {
    if (currTime < tempOnTimes[5] && clrTemp > TEMP_MIN) // Peltier
      digitalWrite(PELT_CLR, LOW);
    else
      digitalWrite(PELT_CLR, HIGH);
    digitalWrite(HTR_THMB, HIGH);
    digitalWrite(HTR_INDX, HIGH);
    digitalWrite(HTR_MIDL, HIGH);
    digitalWrite(HTR_PNKY, HIGH);
    digitalWrite(HTR_PALM, HIGH);
  }
  currTime += millis() - timeElapsed;
  currTime = currTime%TEMP_PERIOD;
  timeElapsed = millis();
  if (DEBUG_PRINT) {
    Serial.println("***Temp Control Time***");
    Serial.println("Current Time: " + String(currTime) + "\r\n");
  }

  /* **********WEIGHT CONTROL********** */
  // Read weight feedback sensors
  motCurr = (motDirection == LOW) ? currMeasSmoothed() : -currMeasSmoothed();
  currDelta = (firstLoop == false) ? motCurr - motCurr1 : 0;
  if (ldclMeasRdy) {
    ldclGrams1 = ldclGrams;
    ldclGrams = ldclADC.getData();
    ldclGrams = (ldclGrams < 0) ? -ldclGrams : ldclGrams;
    massDelta = (firstLoop == false) ? ldclGrams1 - ldclGrams : 0;
    ldclMeasRdy = 0;
  }
  if (DEBUG_PRINT) {
    Serial.println("***Weight Feedback Values***");
    Serial.println("Current(mA): " + String(motCurr));
    Serial.println("Current Past(mA): " + String(motCurr1));
    Serial.println("Current Diff: " + String(currDelta));
    Serial.println("Loadcell(g): " + String(ldclGrams));
    Serial.println("Loadcell Past(g): " + String(ldclGrams1));
    Serial.println("LDCL Diff: " + String(massDelta) + "\r\n");
  }

  // Fuse weight measurements w/ fuzzy logic
  bool currSNve = false, ldclSNve = false;
  float currR = 0, currS = 0, currD = 0;
  float ldclR = 0, ldclS = 0, ldclD = 0;
  float uI = uI1, uL = uI1;
  if (currDelta <= -FUZZY_CURR) {
    currR = 1;
    currS = 0;
    currD = 0;
  }
  else if ((-FUZZY_CURR < currDelta) && (currDelta  < 0)) {
    currR = -1.0*currDelta/FUZZY_CURR;
    currS = -1.0*(currDelta+FUZZY_CURR)/FUZZY_CURR;
    currD = 0;
    currSNve = true;
  }
  else if ((0 <= currDelta) && (currDelta < FUZZY_CURR)) {
    currR = 0;
    currS = 1.0*(FUZZY_CURR-currDelta)/FUZZY_CURR;
    currD = 1.0*currDelta/FUZZY_CURR;
  }
  else { // currDelta >= FUZZY_CURR
    currR = 0;
    currS = 0;
    currD = 1;
  }
  if (massDelta <= -2*FUZZY_MASS) {
    ldclR = 1;
    ldclS = 0;
    ldclD = 0;
  }
  else if ((-2*FUZZY_MASS < massDelta) && (massDelta < 0)) {
    ldclR = -1.0*massDelta/(2*FUZZY_MASS);
    ldclS = -1.0*(massDelta+2*FUZZY_MASS)/(2*FUZZY_MASS);
    ldclD = 0;
    ldclSNve = true;
  }
  else if ((0 <= massDelta) && (massDelta < FUZZY_MASS)) {
    ldclR = 0;
    ldclS = 1.0*(FUZZY_MASS-massDelta)/FUZZY_MASS;
    ldclD = 1.0*massDelta/FUZZY_MASS;
  }
  else { // massDelta >= FUZZY_MASS
    ldclR = 0;
    ldclS = 0;
    ldclD = 1;
  }
  if ((currR > currS) && (currR > currD))
    uI = -currR;
  else if ((currS > currR) && (currS > currD))
    uI = (currSNve == true) ? -currS : currS;
  else // (currD > currR) && currD > currS
    uI = currD;
  if ((ldclR > ldclS) && (ldclR > ldclD))
    uL = -ldclR;
  else if ((ldclS > ldclR) && (ldclS > ldclD))
    uL = (ldclSNve == true) ? -ldclS : ldclS;
  else // (ldclD > ldclR) && ldclD > ldclS
    uL = ldclD;
  float trueDir = 0.2*uI1 + 0.4*uI + 0.1*uL1 + 0.3*uL;
  if (trueDir >= FUZZY_DEADBAND) {
    digitalWrite(MOT_DIR, LOW); // Stationary or Lowering
    motDirection = LOW;
  }
  else if (trueDir <= -FUZZY_DEADBAND) {
    digitalWrite(MOT_DIR, HIGH); // Raising
    motDirection = HIGH;
  } 
  if (DEBUG_PRINT) {
    Serial.println("***Weight Feedback Fuzzy Logic***");
    Serial.println("Current Raise: " + String(-currR));
    Serial.println("Current Stationary: " + String(currS));
    Serial.println("Current Drop: " + String(currD));
    Serial.println("LDCL Raise: " + String(-ldclR));
    Serial.println("LDCL Stationary: " + String(ldclS));
    Serial.println("LDCL Drop: " + String(ldclD));
    Serial.println("Fuzzy Current Previous: " + String(uI1));
    Serial.println("Fuzzy Current: " + String(uI));
    Serial.println("Fuzzy LDCL Previous: " + String(uL1));
    Serial.println("Fuzzy LDCL: " + String(uL));
    Serial.println("Fuzzy Deadband: " + String(FUZZY_DEADBAND));
    Serial.println("Fuzzy Sel Direction: " + String(trueDir));
    Serial.println("Selected Motor Dir (High-CW, Low-CCW): " + String(motDirection) + "\r\n");
  }
      
  // Calculate weight control signals
  int16_t currSetpoint = (motDirection == LOW) ? refMass*GRAM_GRAVITY*SPOOL_RAD/TORQ_CONST : 0;
  currIntegralErr += (motDirection == LOW) ? (currSetpoint-motCurr) : (currSetpoint+motCurr);
  float vCtrl = (motDirection == LOW) ? 10*(currIntegralErr/1000) : 1.3;
  if ((vCtrl < 0) && (vCtrl > -MOT_STICTION))
    vCtrl -= 0.1;
  else if ((vCtrl > 0) && (vCtrl < MOT_STICTION))
    vCtrl += 0.1;
  int16_t vRef = voltToPWM(vCtrl);
  if (DEBUG_PRINT) {
    Serial.println("***Weight Feedback Signals***");
    Serial.println("Current Setpoint: " + String(currSetpoint));
    Serial.println("Current Integral Error: " + String(currIntegralErr));
    Serial.println("Control Voltage: " + String(vCtrl));
    Serial.println("PWM Signal: " + String(vRef) + "\r\n");
  }
  
  // Write weight output
  if (vRef >= 0)
    analogWrite(MOT_PWM, vRef);
  else if (vRef < 0)
    analogWrite(MOT_PWM, abs(vRef));
  
  // Save values for next loop
  firstLoop = false;
  massDelta1 = massDelta;
  motCurr1 = motCurr;
  uI1 = uI;
  uL1 = uL;
}
