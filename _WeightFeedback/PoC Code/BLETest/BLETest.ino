/*************************************************************
FYDP-Group12
-Blueooth Low Energy (BLE) PoC
*************************************************************/
// Bluetooth Includes
#include <Adafruit_ATParser.h>
#include <Adafruit_BLE.h>
#include <Adafruit_BLEGatt.h>
#include <Adafruit_BluefruitLE_UART.h>
#include "BluefruitConfig.h"

// Constants
const int BAUDRATE = 9600;
const int COMM_FAIL_MAX = 5;
const int BLE_ERR_LED_MODE = 5;
const String EMPTY_RX = "nodata";

// Globals
int loopCount = 0;
int commFails = 0;
bool bleConnected = false;
Adafruit_BluefruitLE_UART bleUART(BLUEFRUIT_HWSERIAL_NAME, BLUEFRUIT_UART_MODE_PIN);

// BLE Init
bool setupBLE (int baudRate) {
    // Setup Serial
    bool bleInitSuccess = false;
    Serial.begin(baudRate);
    while (!Serial) {} // Wait for initilization
    Serial.println("Serial Initialized");

    // Setup BLE
    Serial.println("Initalize BLE");
    if (!bleUART.begin())
      Serial.println("Failed to Find BLE Module");
    Serial.println("BLE begin OK, disable verbose, disable echo, request info");
    while (!bleUART.echo(false) == "OK") {} // Wait for response
    bleUART.verbose(VERBOSE_MODE);
    bleUART.info();

    // Connect
    while (!bleUART.isConnected()) {
      Serial.println("Waiting for BLE connection");
      delay(1000);
    }

    // Set BLE flag
    Serial.println("Device connected");
    bleConnected = true;
}

// BLE Error
void bleErr() {
  bleUART.sendCommandCheckOK("AT+HWModeLED=" "MANUAL");
}

// BLE Tx
bool bleTX(String msg) {
  // Check connection
  if (bleConnected)
    msg += "\\r\\n"; // Append newline char
  else {
    Serial.println("BLE not connected");
    return false;
  }

  // Check msg length
  char printArr[msg.length() + 1];
  if (msg.length() > BUFSIZE) {
     Serial.println("Msg to long for BLE send");
    return false;
  }
  else {  
    // Parse to char array
    msg.toCharArray(printArr, msg.length() + 1);
    
    // Send char to BLE
    bleUART.print("AT+BLEUARTTX=");
    bleUART.println(printArr);

    // Check Response Status
    if (!bleUART.waitForOK()) {
      Serial.println("Failed to Send Over BLE");
      return false;
    }
  }

  // Return success
  return true;
}

// BLE Rx
String bleRX() {
  // Check for incoming char
  String recvMsg;
  bleUART.println("AT+BLEUARTRX");
  bleUART.readline();
  if (strcmp(bleUART.buffer, "OK") == 0) // No Data
    return EMPTY_RX;
  else {
    // Data Found, Read from Buffer
    recvMsg = String(bleUART.buffer);
    Serial.println("BLE received data: " + recvMsg);
    bleUART.waitForOK();
    return recvMsg;
  }
}

// Main
void setup() {
  setupBLE(BAUDRATE);
}

void loop() {
  // Receive data
  String rxData = bleRX();

  // Transmit data
  String txData = "Currently on loop: " + String(loopCount);
  commFails += (bleTX(txData) == false) ? 1 : 0;
  if (rxData != EMPTY_RX)
    commFails += (bleTX("Received data: " + rxData) == false) ? 1 : 0;

  // Check comm status
  if (commFails >= COMM_FAIL_MAX) {
    // Blink board
    
    // Disconnect
    bleUART.disconnect();
    
    // Alert
    Serial.println("BLE max fail count met, BLE connection terminated, please restart");
  }
  
  // Delay
  loopCount += 1;
  delay(1000);
}
