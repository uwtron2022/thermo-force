/*************************************************************
FYDP-Group12
-Blueooth Low Energy (BLE) Parser
*************************************************************/

// Variables
const uint16_t BAUDRATE = 9600;
const bool DEBUG_PRINT = true;
const String BLE_EMPTY_RX = "nodata";
const uint8_t TEMP_MAX = 35;
const uint8_t TEMP_MIN = 10;
const uint16_t MASS_MAX = 20000; // Grams
const uint16_t MASS_MIN = 500; // Grams
float maxTemp = 0;
float minTemp = 0;
float maxMass = 0;
float minMass = 0;
float refMass = 0;
float refTemp = 0;

// Main
void setup() {
  // Initialize serial
  Serial.begin(BAUDRATE);
  while (!Serial) {} // Wait for initilization
  Serial.println("##########Initialize Serial##########");
  Serial.println("Serial Initialized\r\n");
  Serial.flush();
  delay(1000);
}

void loop() {
  // Read BLE
  String cmdLine = "-min -t -10 -max -t 50 -min -w 2 -min -w 10";
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
    char *splitToken = " ";
    char *currToken;

    // Flags
    bool setMax = false;
    bool setMin = false;
    bool setTemp = false;
    bool setMass = false;
    
    // Parse
    char cmdTemp[cmdLine.length() + 1];
    cmdLine.toCharArray(cmdTemp, cmdLine.length() + 1);
    currToken = strtok(cmdTemp, splitToken);
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
            maxMass = 1000*String(currToken).toFloat();
            setMax = false;
            setMass = false;
          }
          else if (setMin) {
            minMass = 1000*String(currToken).toFloat();
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
      currToken = strtok(NULL, splitToken);
      // Delay
      delay(1000);
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

  // Delay
  delay(1000);
}
