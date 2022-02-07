/*************************************************************
FYDP-Group12
-Main program for ThermoForce temperature/weight control
  -Mot dir pin LOW = CW when facing motor shaft
  -Mot dir pin HIGH = CCW when facing motor shaft
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

// Globals
// BLE
bool bleConnected = false;
bool  bleHeartbeat = false;
uint8_t bleCommFails = 0;
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
Adafruit_BluefruitLE_UART bleUART(BLUEFRUIT_HWSERIAL_NAME, BLUEFRUIT_UART_MODE_PIN);
HX711_ADC ldclADC(LDCL_DOUT, LDCL_SCK);
ADS1115_lite shuntADC(ADS1115_DEFAULT_ADDRESS);

// BLE Initalize
bool setupBLE () { // Serial2 used to communicate w/ BLE module
  // Initialize peripheral
  Serial.println("Initalize BLE\r\n");
  if (!bleUART.begin()) {
    Serial.println("Failed to setup BLE Module\r\n");
    return false;
  }

  // Setup peripheral
  Serial.println("BLE begin ok, disable verbose, disable echo, request info\r\n");
  while (!bleUART.echo(false) == "OK") {} // Wait for response
  bleUART.verbose(VERBOSE_MODE);
  bleUART.info();
  Serial.println();

  // Connect, set connected flag
  while (!bleUART.isConnected()) {
    Serial.println("Waiting for BLE connection\r\n");
    delay(1000);
  }
  Serial.println("BLE connected\r\n");
  bleConnected = true;
  return true;
}

// LDCL Initialize
bool setupLDCL () {
  // Initialize peripheral
  Serial.println("Initialize LDCL\r\n");
  pinMode(LDCL_DOUT, INPUT);
  pinMode(LDCL_SCK, INPUT);
  int ldclStabilTime = 5000;
  bool ldclTare = true;
  ldclADC.begin();
 
  // Setup peripheral
  ldclADC.start(ldclStabilTime, ldclTare);
  if (ldclADC.getTareTimeoutFlag()) {
    Serial.println("Failed to setup loadcell\r\n");
    return false;
  }
  ldclADC.setCalFactor(ldclCalVal);
  Serial.println("Current calibration factor is: " + String(ldclADC.getCalFactor()) + "\r\n");
  Serial.println("Loadcell initialized\r\n");

  // Tare and return
  ldclADC.tareNoDelay();
  delay(1000);
  return true;
}

// MOT Initialize
bool setupMOTShield () {
  // Setup Motor Channel A
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
  if (!shuntADC.testConnection()) {
      Serial.println("Shunt ADC connection failed\r\n");
      return false;
  }

  // Setup peripheral
  shuntADC.setGain(ADS1115_REG_CONFIG_PGA_0_256V); // 1Bit=0.0078125mV
  shuntADC.setSampleRate(ADS1115_REG_CONFIG_DR_128SPS); // 8 Samples/Sec
  shuntADC.setMux(ADS1115_REG_CONFIG_MUX_DIFF_0_1);
  Serial.println("Shunt ADC initialized\r\n");

  // Calc steady-state mot shield current consumption
  uint8_t avgCnts = 10;
  int32_t adcCounts = 0;
  for (uint8_t i=0; i++; i<avgCnts) {
    shuntADC.triggerConversion();
    int16_t rawCounts = shuntADC.getConversion();
    if (rawCounts >= 0)
      adcCounts += rawCounts;
    else
      i--;
  }
  adcSSCnts = adcCounts/avgCnts;
  Serial.println("Avg current consumption: " + String(adcSSCnts*ADC_VPER_CNT*SHUNT_MULT) + "\r\n");
  return true;
}

// Setup 
void setup() {
  // Initialize Serial
  Serial.begin(BAUDRATE);
  while (!Serial) {} // Wait for initilization
  Serial.println("Serial Initialized\r\n");

  // Initialize Peripherals
  Serial.println("##########Initialize Peripherals##########\r\n");
  Serial.println("##########Initialize BLE##########\r\n");
  bool bleSucess = setupBLE();
  Serial.println("##########Initialize Loadcell##########\r\n");
  bool ldclSucess = setupLDCL();
  Serial.println("##########Initialize Motor Shield##########\r\n");
  bool motSucess = setupMOTShield();
  Serial.println("##########Initialize Shunt ADC##########\r\n");
  bool shuntSucess = setupShuntADC();

  // Flush
  Serial.flush();
  delay(1000);
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
    Serial.println(recvMsg);
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

// Main
void loop() { 
 // Read BLE

 // Sent heartbeat

 // Update sensors

 // Calculate control signals

 // Write output
}
