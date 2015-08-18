
/* 
 * Smart Home Demonstration Project
 * By Salena Dreadon 
 * July 2015 
 * 
 * Copyright (c) Salena Dreadon 2015
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
 
#include <PCD8544.h>

//libraries for temperature sensors 
#include <OneWire.h>
#include <DallasTemperature.h>

// define Digital Output Pins
// lcd uses Arduino pins D3-SCLK,D4-MOSI,D5-D/C,D6-RST,D7-SCE
enum { MOTION_SENSOR  = 2, FAN_EXHAUST = 8, LCD_BACKLIGHT  = 9,  FAN_CEILING  = 10, 
       ALARM_SPEAKER = 11, ONE_WIRE_BUS = 12,
       CEILING_LIGHT = 13, SWITCH_ALARM = 14, SWITCH_LIGHT = 15, HEATER_CONTROL = 16};    
enum { CEILING_FAN_COUNT = 10};       
enum { TEMP_ROOM_INDEX = 0, TEMP_ROOF_INDEX = 1}; 
const float HEATER_ON_TEMP = 17.0, LOWER_COMFY = 20.0, UPPER_COMFY = 22.0;

// Setup a oneWire instance to communicate with any OneWire devices 
static OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
static DallasTemperature DSSensors(&oneWire);

//create driver for lcd 
static PCD8544 lcd;


//int brightness = 0;    // how bright the LED is
//int fadeAmount = 5;    // how many points to fade the LED by
bool motionDetected, alarmSWState ,lightSWState;
int ledState;
int fanTimerCount;
//bool alarmFlip;
float roomTemperature, roofTemperature;

void setup() {

//  Serial.begin(9600);
//  Serial.println("Automation Demo");
   
  lcd.begin(84, 48);  // PCD8544-compatible displays may have a different resolution...
  //lcd uses Arduino pins D3-SCLK,D4-MOSI,D5-D/C,D6-RST,D7-SCE
  
  // declare DATA pins
  pinMode (LCD_BACKLIGHT, OUTPUT); 
  pinMode (FAN_EXHAUST, OUTPUT); 
  pinMode (FAN_CEILING, OUTPUT);
  pinMode (ALARM_SPEAKER, OUTPUT);
  pinMode (CEILING_LIGHT, OUTPUT); 
  pinMode (SWITCH_ALARM, OUTPUT); 
  pinMode (SWITCH_LIGHT, OUTPUT); 
  pinMode (HEATER_CONTROL, OUTPUT);
  
  pinMode (MOTION_SENSOR, INPUT);
  pinMode (SWITCH_ALARM, INPUT); 
  pinMode (SWITCH_LIGHT, INPUT); 

  // Write to screen
  lcd.setCursor(12, 0);
  lcd.print("Smart Home");
  
  lcd.setCursor(0,1);
  lcd.print(" LS:");
  lcd.setCursor(40,1);
  lcd.print(" AS:");
   lcd.setCursor(0,2);
  lcd.print(" Motion:");
   
  lcd.setCursor(0,3);
  lcd.print(" Alarm:");

  lcd.setCursor(0,4);   lcd.print(" Room:");
  lcd.setCursor(0,5);   lcd.print(" Roof:");
  
  lcd.setContrast(65);
 
  //DSSensor uses ardiuno pin D12
  DSSensors.begin();
  // Serial.print("found "); 
  // Serial.print(DSSensors.getDeviceCount(), DEC);
  // Serial.println(" devices.");
}

void loop() {
  if (fanTimerCount) {fanTimerCount--;}; //timer for fan & alarm 
  
  motionDetected = digitalRead (MOTION_SENSOR);
  alarmSWState = digitalRead (SWITCH_ALARM); 
  lightSWState = digitalRead (SWITCH_LIGHT);

  lcd.setCursor(25,1);   lcd.print(lightSWState?"ON ":"OFF ");
  lcd.setCursor(65,1);   lcd.print(alarmSWState?"ON ":"OFF ");
  lcd.setCursor(60,2);   lcd.print(motionDetected?"YES":"NO "); 
  
  BackLight(motionDetected);  //flash LCD screen 
  CeilingLight(lightSWState && fanTimerCount); // Light switch ON and room occupied

  DSSensors.requestTemperatures();            // Send the command to get temperatures
  roomTemperature = GetRoomTemperature();
  roofTemperature = GetRoofTemperature();
  lcd.setCursor(40,4); lcd.print(roomTemperature); lcd.print("'C ");
  lcd.setCursor(40,5); lcd.print(roofTemperature); lcd.print("'C ");
 
  // Serial.print("Motion Sensor: "); Serial.println (motionDetected);
  // Serial.print("Alarm Switch: "); Serial.println (alarmSWState);
  // Serial.print("Light Switch: "); Serial.println (lightSWState);  
 
  if (motionDetected){fanTimerCount = CEILING_FAN_COUNT;}; //Set a timer 
    
  lcd.setCursor(60,3);
  if (fanTimerCount) {lcd.print(fanTimerCount); lcd.print("  ");} else {lcd.print("OFF"); };
  
  if (alarmSWState) {     //ALARM is set ON
    if (fanTimerCount) { 
      tone (ALARM_SPEAKER, (fanTimerCount%2)?2000:2500);  //siren sound
      CeilingLight (!(fanTimerCount%2));}                  //flashing light
    else {
      noTone (ALARM_SPEAKER);};
  } 
  else {                       // ALARM is set OFF = normal operation
    noTone (ALARM_SPEAKER);     //make sure alarm is off
  };

  // if (roomTemperature > 22.0) {digitalWrite (FAN_EXHAUST, HIGH);} else {digitalWrite (FAN_EXHAUST, LOW);}; 
  ExhaustFan ( roomTemperature > UPPER_COMFY);   //turn fan ON to cool room 
  Heater ( (!alarmSWState) && ( roomTemperature < HEATER_ON_TEMP)); //heat the room if it is VERY cold, but not if the alarm is on (no one home)  
  CeilingFanControl ();   

  delay(1000);
};

void ExhaustFan (bool ST) { digitalWrite(FAN_EXHAUST, ST);};

void CeilingLight (bool ST) { digitalWrite(CEILING_LIGHT, ST);};

void BackLight (bool ST) { digitalWrite(LCD_BACKLIGHT, ST);};

void Heater (bool ST) { digitalWrite( HEATER_CONTROL, !ST);};  // invert because LED is tied high

float GetRoomTemperature () {return DSSensors.getTempCByIndex(TEMP_ROOM_INDEX); };
 
float GetRoofTemperature () {return DSSensors.getTempCByIndex(TEMP_ROOF_INDEX); };

void CeilingFanControl () { 
  if (IsRoomHot ()) {CoolRoom();}
  else if (IsRoomCold ()) { HeatRoom ();} 
  else // IsRoomComfy () 
    { ComfyRoom ();};
};

bool IsRoomHot () {return roomTemperature > UPPER_COMFY;};  // Comfy is 20.0 to 22.0 -the goal temperature
bool IsRoomCold () {return roomTemperature < LOWER_COMFY;}; 

void CoolRoom () {digitalWrite(FAN_CEILING, roofTemperature < roomTemperature);};  //This will be expanded when I get a humidity sensor  
void HeatRoom () {digitalWrite(FAN_CEILING, roofTemperature > roomTemperature);};  //  ""
void ComfyRoom () {digitalWrite(FAN_CEILING, LOW);};                               //  ""




