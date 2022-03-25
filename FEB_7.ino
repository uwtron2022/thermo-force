float TEMP_REF_POT = 20.0;
int IT_0; 
/////////////////// TEMP CODE ABOVE 


// MAX and MIN Temps for SAFETY
float MAX_TEMP = 35.0; 
float MIN_TEMP = 10.0; 
int PERIOD = 100;


/*
Thermistors:
------------
Thumb:1
Index finger:2
Middle finger:3
Ring finger:4
Pinky:5
Palm:6
Peltier plate:7
*/
int Therm_1 = 8;
int Therm_2 = 9;
int Therm_3 = 10;
int Therm_4 = 11;
int Therm_5 = 12;
int Therm_6 = 13;
int Therm_7 = 14;
int Therm_8 = 15;

int IT_1; 
int IT_2; 
int IT_3; 
int IT_4; 
int IT_5; 
int IT_6; 
int IT_7; 
int IT_8; 

// This is the Temperature Values from the Thermistors 
float T1, T2, T3, T4, T5, T6, T7, T8, T_AVG;
// This is the Reference Temperature (Numbers match the thermistor value 
float RT1, RT2, RT3, RT4, RT5, RT6, RT_AVG;

/*
Heating pads:
-------------
Thumb:G
Index finger:F
Middle finger:E
Ring finger:B
Pinky:A
Palm:D
*/
int CoolerPin_C = 22; 
int HeaterPin_A = 26; 
int HeaterPin_BE = 30;//SAME AS A_TEMP 
int HeaterPin_D = 34; 
//int HeaterPin_E = 34; //SAME AS _TEMP 
int HeaterPin_F = 38; 
int HeaterPin_G = 42; 

int ON_TIME = 0; 
int OFF_TIME = PERIOD; 
int HTR_ON_TIME_A = 0; 
int HTR_OFF_TIME_A = PERIOD; 
int HTR_ON_TIME_BE = 0; // NOT CURRENTLY USED 
int HTR_ON_TIME_D = 0; 
//int HTR_ON_TIME_E = 0; 
int HTR_ON_TIME_F = 0; 
int HTR_ON_TIME_G = 0; 

int LOOPER = 0; 

int DEBUG_TEMP = 1; 

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
pinMode(CoolerPin_C, OUTPUT);    // sets the digital pin as output
pinMode(HeaterPin_A, OUTPUT);    // sets the digital pin as output
pinMode(HeaterPin_BE, OUTPUT);    // sets the digital pin as output
pinMode(HeaterPin_D, OUTPUT);    // sets the digital pin as output
//pinMode(HeaterPin_E, OUTPUT);    // sets the digital pin as output
pinMode(HeaterPin_F, OUTPUT);    // sets the digital pin as output
pinMode(HeaterPin_G, OUTPUT);    // sets the digital pin as output

}

void loop() {
  IT_0 = analogRead(0);
  TEMP_REF_POT = ((IT_0/51.6)+15); // (0-1022)/500+18 
  /////////////////////// TEMP CODE ABOVE 
  // put your main code here, to run repeatedly:
  // Note, High means (heater/cooler) = OFF, Low means (heater/cooler) = ON 

  // Start of program: Read Temperatures 
  IT_1 = analogRead(Therm_1);
  IT_2 = analogRead(Therm_2);
  IT_3 = analogRead(Therm_3);
  IT_4 = analogRead(Therm_4);
  IT_5 = analogRead(Therm_5);
  IT_6 = analogRead(Therm_6);
  IT_7 = analogRead(Therm_7);
  IT_8 = analogRead(Therm_8);

if(DEBUG_TEMP == 0){
  Serial.print("TEMP_REF: "); 
  Serial.print(IT_0);
  Serial.print(" 1: "); 
  Serial.print(IT_1);
  Serial.print(" 2: "); 
  Serial.print(IT_2);
  Serial.print(" 3: "); 
  Serial.print(IT_3);
  Serial.print(" 4: "); 
  Serial.print(IT_4);
  Serial.print(" 5: "); 
  Serial.print(IT_5);
  Serial.print(" 6: "); 
  Serial.print(IT_6);
  Serial.print(" 7: "); 
  Serial.print(IT_7);
  Serial.print(" 8: "); 
  Serial.print(IT_8);
  Serial.println(""); 
}

// This is used to convert from voltage values to Temperature values in *C 
  T1 = (0.1319*IT_1 -40);
  T2 = (0.1319*IT_2 -40);
  T3 = (0.1319*IT_3 -40);
  T4 = (0.1319*IT_4 -40);
  T5 = (0.1319*IT_5 -40);
  T6 = (0.1319*IT_6 -40);
  T7 = (0.1319*IT_7 -40);
  T8 = (0.1319*IT_8 -40);
  
if(DEBUG_TEMP == 1){
  Serial.print("REF_POT: "); 
  Serial.print(TEMP_REF_POT);
  Serial.print(" 1: "); 
  Serial.print(T1);
  Serial.print(" 2: "); 
  Serial.print(T2);
  Serial.print(" 3: "); 
  Serial.print(T3);
  Serial.print(" 4: "); 
  Serial.print(T4);
  Serial.print(" 5: "); 
  Serial.print(T5);
  Serial.print(" 6: "); 
  Serial.print(T6);
  Serial.print(" 7: "); 
  Serial.print(T7);
  Serial.print(" 8: "); 
  Serial.print(T8);
  Serial.println(""); 
}


  /////////////////////// TEMP CODE BELOW 
  RT1 = TEMP_REF_POT;
  RT2 = TEMP_REF_POT;
  RT3 = TEMP_REF_POT;
  RT4 = TEMP_REF_POT;
  RT5 = TEMP_REF_POT;
  RT6 = TEMP_REF_POT;
  /////////////////////// TEMP CODE ABOVE 

////////////////////////////////////////////////////////////////////
// This is used as a safety feature, ensure that max temperature limits 
if((MAX_TEMP < RT1) || (MIN_TEMP > RT1)){
  RT1 = (MAX_TEMP + MIN_TEMP)/2.0; // Set to Average 
}
if((MAX_TEMP < RT2) || (MIN_TEMP > RT2)){
  RT2 = (MAX_TEMP + MIN_TEMP)/2.0; // Set to Average 
}
if((MAX_TEMP < RT3) || (MIN_TEMP > RT3)){
  RT3 = (MAX_TEMP + MIN_TEMP)/2.0; // Set to Average 
}
if((MAX_TEMP < RT4) || (MIN_TEMP > RT4)){
  RT4 = (MAX_TEMP + MIN_TEMP)/2.0; // Set to Average 
}
if((MAX_TEMP < RT5) || (MIN_TEMP > RT5)){
  RT5 = (MAX_TEMP + MIN_TEMP)/2.0; // Set to Average 
}
if((MAX_TEMP < RT6) || (MIN_TEMP > RT6)){
  RT6 = (MAX_TEMP + MIN_TEMP)/2.0; // Set to Average 
}

// This is the reference temperature average 
RT_AVG = ((RT1 + RT2 + RT3 + RT4 + RT5 + RT6)/6.0); 
T_AVG = ((T1 + T2 + T3 + T4 + T5 + T6 + T7 + T8)/8.0); 
////////////////////////////////////////////////////////////////////
  // PALM - T7(Plate) 
  // Temp Controller Colder 
  if (RT_AVG > T7) {
    ON_TIME = ON_TIME - 25;
  } else 
  if ((RT_AVG < T7) || (RT_AVG < T_AVG)) {
    ON_TIME = ON_TIME + 10;
  }

  OFF_TIME = (PERIOD - ON_TIME);

  // Saturator Limit
  if (ON_TIME >= PERIOD) {
    ON_TIME = PERIOD;
  } else if (ON_TIME <= 0) {
    ON_TIME = 0;
  } 
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// PINKY(A) - T5
// Temp Controller - HOTER A 
  if (RT5 > T5) {
    HTR_ON_TIME_A = HTR_ON_TIME_A + 10;
  } else 
  if (RT5 < T5) {
    HTR_ON_TIME_A = HTR_ON_TIME_A - 50;
  }

  // Saturator Limit
  if (HTR_ON_TIME_A >= PERIOD) {
    HTR_ON_TIME_A = PERIOD;
  } else if (HTR_ON_TIME_A <= 0) {
    HTR_ON_TIME_A = 0;
  } 
////////////////////////////////////////////////////////////////////
// THUMB(G) - T1
// Temp Controller - HOTER G 
  if (RT1 > T1) {
    HTR_ON_TIME_G = HTR_ON_TIME_G + 10;
  } else 
  if (RT1 < T1) {
    HTR_ON_TIME_G = HTR_ON_TIME_G - 50;
  }

  // Saturator Limit
  if (HTR_ON_TIME_G >= PERIOD) {
    HTR_ON_TIME_G = PERIOD;
  } else if (HTR_ON_TIME_G <= 0) {
    HTR_ON_TIME_G = 0;
  } 
////////////////////////////////////////////////////////////////////
// INDEX(F) - T2
// Temp Controller - HOTER F
  if (RT2 > T2) {
    HTR_ON_TIME_F = HTR_ON_TIME_F + 10;
  } else 
  if (RT2 < T2) {
    HTR_ON_TIME_F = HTR_ON_TIME_F - 50;
  }

  // Saturator Limit
  if (HTR_ON_TIME_F >= PERIOD) {
    HTR_ON_TIME_F = PERIOD;
  } else if (HTR_ON_TIME_F <= 0) {
    HTR_ON_TIME_F = 0;
  } 
////////////////////////////////////////////////////////////////////
// PALM(D) - T6
// Temp Controller - HOTER d
  if (RT6 > T6) {
    HTR_ON_TIME_D = HTR_ON_TIME_D + 10;
  } else 
  if (RT6 < T6) {
    HTR_ON_TIME_D = HTR_ON_TIME_D - 50;
  }

  // Saturator Limit
  if (HTR_ON_TIME_D >= PERIOD) {
    HTR_ON_TIME_D = PERIOD;
  } else if (HTR_ON_TIME_D <= 0) {
    HTR_ON_TIME_D = 0;
  } 
////////////////////////////////////////////////////////////////////
// RING+MIDDLE(BE) - T3+T4
// Temp Controller - HOTER BE
  if ((RT3 > T3) || (RT4 > T4)) {
    HTR_ON_TIME_BE = HTR_ON_TIME_BE + 10;
  } else 
  if ((RT3 < T3) || (RT4 < T4)) {
    HTR_ON_TIME_BE = HTR_ON_TIME_BE - 50;
  }

  // Saturator Limit
  if (HTR_ON_TIME_BE >= PERIOD) {
    HTR_ON_TIME_BE = PERIOD;
  } else if (HTR_ON_TIME_BE <= 0) {
    HTR_ON_TIME_BE = 0;
  } 
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// HEATER OR COOLER ONLY _ NOT BOTH 
  if ( RT_AVG > T_AVG) {
    ON_TIME = 0;
  }else {
    HTR_ON_TIME_A = 0; 
    HTR_ON_TIME_BE = 0; 
    HTR_ON_TIME_D = 0; 
    //HTR_ON_TIME_E = 0; 
    HTR_ON_TIME_F = 0; 
    HTR_ON_TIME_G = 0; 
    
  }

  ////////////////////////////////////////////////////////////////////


// This is to turn on or off each of the specific elements at a various level 
if(LOOPER < ON_TIME){
  digitalWrite(CoolerPin_C, LOW); // COOLER ON
  }
else {
  digitalWrite(CoolerPin_C, HIGH); // COOLER OFF
  }

if(LOOPER < HTR_ON_TIME_A){
  digitalWrite(HeaterPin_A, LOW); // HEATER A ON 
  }
else {
  digitalWrite(HeaterPin_A, HIGH); // HEATER A OFF
  }
if(LOOPER < HTR_ON_TIME_A){
  digitalWrite(HeaterPin_BE, LOW); // HEATER A ON 
  }
else {
  digitalWrite(HeaterPin_BE, HIGH); // HEATER A OFF
  }
if(LOOPER < HTR_ON_TIME_D){
  digitalWrite(HeaterPin_D, LOW); // HEATER A ON 
  }
else {
  digitalWrite(HeaterPin_D, HIGH); // HEATER A OFF
  }
///if(LOOPER < HTR_ON_TIME_E){
///  digitalWrite(HeaterPin_E, LOW); // HEATER A ON 
///  }
///else {
///  digitalWrite(HeaterPin_E, HIGH); // HEATER A OFF
///  }
if(LOOPER < HTR_ON_TIME_F){
  digitalWrite(HeaterPin_F, LOW); // HEATER A ON 
  }
else {
  digitalWrite(HeaterPin_F, HIGH); // HEATER A OFF
  }
if(LOOPER < HTR_ON_TIME_G){
  digitalWrite(HeaterPin_G, LOW); // HEATER A ON 
  }
else {
  digitalWrite(HeaterPin_G, HIGH); // HEATER A OFF
  }


  /////////////////////// TEMP CODE BELOW
// TO PRVENT FROM TURNING ON, WILL NEED TO DELETE 
//  digitalWrite(CoolerPin_C, HIGH); 
//  digitalWrite(HeaterPin_A, HIGH); 
//  digitalWrite(HeaterPin_BE, HIGH);  
//  digitalWrite(HeaterPin_D, HIGH); 
//  ////// //digitalWrite(HeaterPin_E, HIGH); // NOT CURRENTLY USED 
//  digitalWrite(HeaterPin_F, HIGH); 
//  digitalWrite(HeaterPin_G, HIGH); 

    /////////////////////// TEMP CODE ABOVE 

  // To Determine Counts 
  LOOPER = (LOOPER + 1);
  if(LOOPER > (PERIOD)){
  LOOPER = 0; 
  }
  delay(2);
}
