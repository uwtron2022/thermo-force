/*************************************************************
FYDP-Group12
-Main program for ThermoForce temperature/weight control
  -Mot dir pin LOW = CW when facing motor shaft, voltage +ve
  -Mot dir pin HIGH = CCW when facing motor shaft, voltage -ve
*************************************************************/
// Includes
#include <Adafruit_BluefruitLE_UART.h>
#include <Adafruit_ATParser.h>
#include <Adafruit_BLEGatt.h>
#include <Adafruit_BLE.h>
#include <ADS1115_lite.h>
#include <HX711_ADC.h>
#include "BluefruitConfig.h"

// Constants
const int16_t BAUDRATE = 9600;
const bool DEBUG_PRINT = true;
// BLE Const
const uint8_t BLE_FAIL_MAX = 10;
const String BLE_EMPTY_RX = "nodata";
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
const uint8_t TEMP_MAX = 35;
const uint8_t TEMP_MIN = 10;
const uint16_t TEMP_PERIOD = 1000; // Milliseconds
// Weight Control Const
const float GRAM_GRAVITY = 0.981; // N/g
const float SPOOL_RAD = 0.5*3/100;
const float TORQ_CONST = 1.20124389328973*1000; // mA/Nm
const uint16_t MASS_MAX = 20000; // Grams
const uint16_t MASS_MIN = 500; // Grams

// Globals
// BLE
bool bleConnected = false;
bool  bleHeartbeat = false;
uint8_t bleCommFails = 0;
// Loadcell
float ldclCalVal = -60.31;
float ldclGrams = 0;
volatile boolean ldclMeasRdy = false;
// Motor
uint8_t motPWM = 0;
// Shunt ADC
uint16_t adcSSCnts = 0;
// Temp Feedback
bool htrOn = false;
uint8_t tempOnTimes[6] = {0, 0, 0, 0, 0, 0};
uint16_t currTime = 0;
uint32_t timeElapsed = 0;
float refTemp = 0;
// Weight Feedback
float currErr = 0;
float refMass = 0;
// Feedback Scaling
float maxTemp = 0;
float minTemp = 0;
float maxMass = 0;
float minMass = 0;

// Constructors
Adafruit_BluefruitLE_UART bleUART(BLUEFRUIT_HWSERIAL_NAME, BLUEFRUIT_UART_MODE_PIN);
HX711_ADC ldclADC(LDCL_DOUT, LDCL_SCK);
ADS1115_lite shuntADC(ADS1115_DEFAULT_ADDRESS);

// BLE Initalize
bool setupBLE () { // Serial2 used to communicate w/ BLE module
  // Initialize peripheral
  Serial.println("##########Initialize BLE##########");
  if (!bleUART.begin()) {
    Serial.println("Failed to bgein BLE Module\r\n");
    return false;
  }

  // Setup peripheral
  Serial.println("BLE begin ok, disable verbose, disable echo, request info");
  while (!bleUART.echo(false) == "OK") {} // Wait for response
  bleUART.verbose(VERBOSE_MODE);
  bleUART.info();
  Serial.println();

  // Connect, set connected flag
  while (!bleUART.isConnected()) {
    Serial.println("Waiting for BLE connection");
    delay(1000);
  }
  Serial.println("BLE connected");
  Serial.println("BLE initialized\r\n");
  bleConnected = true;
  return true;
}

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

// Heater/Cooler Initialization
bool setupHeatCool() {
  // Start
  Serial.println("##########Initialize Heaters/Cooler##########");

  // Set pinMode
  pinMode(HTR_THMB, OUTPUT);
  pinMode(HTR_INDX, OUTPUT);
  pinMode(HTR_MIDL, OUTPUT);
  pinMode(HTR_PNKY, OUTPUT);
  pinMode(HTR_PALM, OUTPUT);
  pinMode(PELT_CLR, OUTPUT);

  // Success
  Serial.println("Heaters/Cooler initialized\r\n");
  return true;
}

// Thermistor Initialization
bool setupThermistors() {
  // Start
  Serial.println("##########Initialize Thermistors##########");

  // Set pinMode
  pinMode(THERM_THMB, INPUT);
  pinMode(THERM_INDX, INPUT);
  pinMode(THERM_MIDL1, INPUT);
  pinMode(THERM_MIDL2, INPUT);
  pinMode(THERM_PNKY, INPUT);
  pinMode(THERM_PALM, INPUT);
  pinMode(THERM_CLR, INPUT);
  pinMode(THERM_AMB, INPUT);

  // Success
  Serial.println("Thermistors initialized\r\n");
  return true;
}

// Failsafe Function
void failSafe() {
// Turn off heaters/coolers
  digitalWrite(HTR_THMB, LOW);
  digitalWrite(HTR_INDX, LOW);
  digitalWrite(HTR_MIDL, LOW);
  digitalWrite(HTR_PNKY, LOW);
  digitalWrite(HTR_PALM, LOW);
  digitalWrite(PELT_CLR, LOW);

  // Turn off motor
  digitalWrite(MOT_PWM, LOW);
  digitalWrite(MOT_BRK, HIGH);

  // Wait
  Serial.println("##########FAILSAFE TRIGGERED##########");
  Serial.println("##########PROGRAM STOPPED##########");
  while(true) {};
}

// BLE Rx
String bleRX() {
  // Check for incoming char
  String recvMsg;
  bleUART.println("AT+BLEUARTRX");
  bleUART.readline();
  if (strcmp(bleUART.buffer, "OK") == 0) // No Data
    return BLE_EMPTY_RX;
  else {
    // Read from buffer
    recvMsg = String(bleUART.buffer);
    bleUART.waitForOK();
    return recvMsg;
  }
}

// BLE Tx
bool bleTX(String msg) {
  // Check connection
  if (!bleConnected)
    return false;

  // Check msg length
  char printArr[msg.length() + 1];
  if (msg.length() > BUFSIZE)
    return false;
    
  // Parse to char array
  msg.toCharArray(printArr, msg.length() + 1);
  
  // Send char to BLE
  bleUART.print("AT+BLEUARTTX=");
  bleUART.println(printArr);

  // Check response status
  if (!bleUART.waitForOK())
    return false;
  
  // Return success
  return true;
}

// 5Pt Current Measurement
float currMeasSmoothed() {
  // Read and average current counts
  uint8_t totRuns = 0;
  uint16_t countsAvg = 0;
  for (uint8_t i=0; i<5; i++) {
      // Read measurement
      shuntADC.triggerConversion();
      int16_t rawCounts = shuntADC.getConversion();
      if (rawCounts >= 0)
        countsAvg += rawCounts-adcSSCnts;
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
  return (digitalRead(MOT_DIR) == HIGH) ? -(countsAvg/5)*SHUNT_MULT*ADC_VPER_CNT : -(countsAvg/5)*SHUNT_MULT*ADC_VPER_CNT;
}

void ldclReadyISR() {
  // Set flag for ldcl ready
  if(ldclADC.update())
    ldclMeasRdy = true;
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
  bool bleSuccess = setupBLE();
  bool ldclSuccess = setupLDCL();
  bool motSuccess = setupMOTShield();
  bool shuntSuccess = setupShuntADC();
  bool heatCoolSuccess = setupHeatCool();
  bool thermistorSuccess = setupThermistors();
  if (!(bleSuccess && ldclSuccess && shuntSuccess && heatCoolSuccess && thermistorSuccess)) {
    if(!bleSuccess)
       Serial.println("***BLE INITIALIZATION FAIL***\r\n");
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
  Serial.flush();
  delay(1000);

  // Get start time
  currTime = millis();
  timeElapsed = millis();
}

// Main
void loop() { 
  // Read BLE
  String cmdLine = bleRX();
  if (DEBUG_PRINT)
    Serial.println("***Bluetooth Values***");
  if (cmdLine != BLE_EMPTY_RX) {
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
      // Debug print
      if (DEBUG_PRINT)
        Serial.println(currToken);

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
            maxTemp = String(currToken).toFloat();
            setMax = false;
            setTemp = false;
          } 
          else if (setMin) {
            minTemp = String(currToken).toFloat();
            setMin = false;
            setTemp = false;
          }
          else {
            refTemp = (String(currToken).toFloat()-minTemp)*(TEMP_MAX-TEMP_MIN)/(maxTemp-minTemp) + TEMP_MIN;
            setTemp = false;
          }
        }
        if (setMass) {
          if (setMax) {
            minMass = 1000*String(currToken).toFloat();
            setMax = false;
            setMass = false;
          }
          else if (setMin) {
            maxMass = 1000*String(currToken).toFloat();
            setMax = false;
            setMass = false;
          }
          else {
            refMass = (1000*String(currToken).toFloat()-minMass)*(MASS_MAX-MASS_MIN)/(maxMass-minMass) + MASS_MIN; // Mass recv in Kg, convert to grams
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
  bleCommFails = (bleTX(String(bleHeartbeat)) == false) ? (bleCommFails + 1) : 0;
  bleHeartbeat = !bleHeartbeat;
  if (bleCommFails >= BLE_FAIL_MAX)
    failSafe();
  
  // Read temperature sensors
  float handTemps[5];
  handTemps[0] = voltToTemp(THERM_THMB);
  handTemps[1] = voltToTemp(THERM_INDX);
  handTemps[2] = 0.5*(voltToTemp(THERM_MIDL1)+voltToTemp(THERM_MIDL2));
  handTemps[3] = voltToTemp(THERM_PNKY);
  handTemps[4] = voltToTemp(THERM_PALM);
  float clrTemp = voltToTemp(THERM_CLR);
  float ambTemp = voltToTemp(THERM_AMB); // UNUSED
  float avgTemp = (1.0/6.0)*(handTemps[0]+handTemps[1]+handTemps[2]+handTemps[3]+handTemps[4]+clrTemp);
  if (DEBUG_PRINT) {
    Serial.println("***Thermistor Values***");
    Serial.println("Thumb(degC): " + String(handTemps[0]));
    Serial.println("Index(degC): " + String(handTemps[1]));
    Serial.println("Middle 1(degC): " + String(voltToTemp(THERM_MIDL1)));
    Serial.println("Middle 2(degC): " + String(voltToTemp(THERM_MIDL2)));
    Serial.println("Pinky(degC): " + String(handTemps[3]));
    Serial.println("Palm(degC): " + String(handTemps[4]));
    Serial.println("Cooler(degC): " + String(clrTemp));
    Serial.println("Ambient(degC): " + String(ambTemp));
    Serial.println("Average(degC): " + String(avgTemp) + "\r\n");
  }
  
  // Read weight feedback sensors
  ldclGrams = ldclMeasRdy ? ldclADC.getData()  : ldclGrams;
  float currMilliAmps = currMeasSmoothed();
  if (DEBUG_PRINT) {
    Serial.println("***Weight Feedback Values***");
    Serial.println("Loadcell(g): " + String(ldclGrams));
    Serial.println("Current(mA): " + String(currMilliAmps) + "\r\n");
  }
  
  // Calculate temperature control signals
  htrOn = (refTemp >= avgTemp) ? true : false; // Heating or cooling
  if (htrOn) { // Calculate heater control signals
    float tempErr[5];
    for (int i=0; i < 5; i++) {
      tempErr[i] = refTemp - handTemps[i];
      if (tempErr[i] >= 0)
        tempOnTimes[i] += 10*tempErr[i];
      else if (tempErr[i] < 0)
        tempOnTimes[i] += 50*tempErr[i];
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
  
  // Calculate weight control signals
  float refCurr = refMass*GRAM_GRAVITY*SPOOL_RAD/TORQ_CONST;
  currErr += refCurr - currMilliAmps;
  int16_t vRef = voltToPWM(2.5*currErr/1000);
  if (DEBUG_PRINT) {
    Serial.println("***Weight Feedback Signals***");
    Serial.println("Ref Current(mA): " + String(refCurr));
    Serial.println("Current Error: " + String(currErr));
    Serial.println("PWM Signal: " + String(vRef) + "\r\n");
  }
  
  // Write temperature output
  if (htrOn) {
    if (currTime < tempOnTimes[0]) // Thumb
      digitalWrite(HTR_THMB, HIGH);
    else
      digitalWrite(HTR_THMB, LOW);
    if (currTime < tempOnTimes[1]) // Index
      digitalWrite(HTR_INDX, HIGH);
    else
      digitalWrite(HTR_INDX, LOW);
    if (currTime < tempOnTimes[2]) // Middles
      digitalWrite(HTR_MIDL, HIGH);
    else
      digitalWrite(HTR_MIDL, LOW);
    if (currTime < tempOnTimes[3]) // Pinky
      digitalWrite(HTR_PNKY, HIGH);
    else
      digitalWrite(HTR_PNKY, LOW);
    if (currTime < tempOnTimes[4]) // Palm
      digitalWrite(HTR_PALM, HIGH);
    else
      digitalWrite(HTR_PALM, LOW);
  }
  else if (!htrOn) {
    if (currTime < tempOnTimes[5]) // Peltier
      digitalWrite(PELT_CLR, HIGH);
    else
      digitalWrite(PELT_CLR, LOW);
  }
  currTime += millis() - timeElapsed;
  currTime = currTime%TEMP_PERIOD;
  timeElapsed = millis();
  if (DEBUG_PRINT) {
    Serial.println("***Temp Control Output***");
    Serial.println("Current Time: " + String(tempOnTimes[0]) + "\r\n");
  }
  
  // Write weight output
  if (vRef >= 0) {
    digitalWrite(MOT_DIR, LOW);
    digitalWrite(MOT_PWM, vRef);
  }
  else if (vRef < 0) {
    digitalWrite(MOT_DIR, HIGH);
    digitalWrite(MOT_PWM, abs(vRef));
  }
}
