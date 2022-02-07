/*************************************************************
FYDP-Group12
-Motor Testing
-With DIR HIGH: CW
-With DIR LOW: CCW
*************************************************************/
// Constants
const int BAUDRATE = 9600;
const int MOT_BRK = 8;
const int MOT_PWM = 11;
const int MOT_DIR = 13;

// Globals
int pwmCmd = 0;

// Motor Shield Initalize
void setupMotorShield() {
  // Setup Motor Channel A
  pinMode(MOT_BRK, OUTPUT);
  pinMode(MOT_DIR, OUTPUT);
  pinMode(MOT_PWM, OUTPUT);
  digitalWrite(MOT_BRK, LOW);
  digitalWrite(MOT_DIR, LOW);

  // Setup Non-Audiable Fast PWM Frequency 31372.55 Hz
//  // Bit7,5,3=1 & Bit6,4,2=0 for Non-Inverting PWM, Bit1=0 & Bit0=1 for Fast PWM Mode 
//  TCCR1A = 0;
//  TCCR1A = 0b10101000 | 0b00000001;
//  // Bit4=0 & Bit3=1 for Fast PWM Mode, Bit1=1 for No Prescaling
//  TCCR1B = 0;
//  TCCR1B = 0b00001000 | 0b00000010;
  Serial.println("Motor Initialized");
}

// Main
void setup() {
  Serial.begin(BAUDRATE);
  while (!Serial) {} // Wait for Initilization
  setupMotorShield();
}

void loop() {
  // Test Motor
  digitalWrite(MOT_BRK, LOW);
  digitalWrite(MOT_DIR, LOW);

  // Run Motor
  Serial.println("Running Motor at PWM: " + String(pwmCmd));
  analogWrite(MOT_PWM, pwmCmd);

  // Wait
  delay(3000);

  // Increment
  if (pwmCmd == 255) {
    analogWrite(MOT_PWM, 0);
    while(true) {}
  }
  else
    pwmCmd += 1;
}
