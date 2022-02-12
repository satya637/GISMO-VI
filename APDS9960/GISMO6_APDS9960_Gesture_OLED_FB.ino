/****************************************************************
Tests the gesture sensing abilities of the APDS-9960. Configures
APDS-9960 over I2C and waits for gesture events. Calculates the
direction of the swipe (up, down, left, right) and displays it
on a serial console. 

To perform a NEAR gesture, hold your hand
far above the sensor and move it close to the sensor (within 2
inches). Hold your hand there for at least 1 second and move it
away.

To perform a FAR gesture, hold your hand within 2 inches of the
sensor for at least 1 second and then move it above (out of
range) of the sensor.

Hardware Connections:

IMPORTANT: The APDS-9960 can only accept 3.3V!
 
Resources:
Include Wire.h and SparkFun_APDS-9960.h

Development environment specifics:
Written in Arduino 1.0.5
Tested with SparkFun Arduino Pro Mini 3.3V

****************************************************************/
#include "Credentials.h"
#include <Wire.h>
#include <SparkFun_APDS9960.h>

String tag = "IOTLAB/GestureRecognition/Gesture";

// Pins
#define APDS9960_INT 23 // Needs to be an interrupt pin

// Constants

// Global Variables
SparkFun_APDS9960 apds = SparkFun_APDS9960();
int isr_flag = 0;

void OLEDInit();
void OLEDUpdate();
void WiFiinit();
void FirebaseInit();
void FirebaseWrite();

String gesture = "NONE";

void setup() {

  // Set interrupt pin as input
  pinMode(APDS9960_INT, INPUT);

  // Initialize Serial port
  Serial.begin(115200);
  Serial.println();
  Serial.println(F("--------------------------------"));
  Serial.println(F("SparkFun APDS-9960 - GestureTest"));
  Serial.println(F("--------------------------------"));
  
  // Initialize interrupt service routine
  attachInterrupt(APDS9960_INT, interruptRoutine, FALLING);

  // Initialize APDS-9960 (configure I2C and initial values)
  if ( apds.init() ) {
    Serial.println(F("APDS-9960 initialization complete"));
  } else {
    Serial.println(F("Something went wrong during APDS-9960 init!"));
  }
  
  // Start running the APDS-9960 gesture sensor engine
  if ( apds.enableGestureSensor(true) ) {
    Serial.println(F("Gesture sensor is now running"));
  } else {
    Serial.println(F("Something went wrong during gesture sensor init!"));
  }
  if (apds.setGestureGain(GGAIN_2X))
    {
      Serial.println("Gesture Gain Set");
    }
   else
  {
    Serial.println(F("Something went wrong during gesture sensor init!"));
  }

  OLEDInit();
  WiFiInit();
  FirebaseInit();
}

void loop() {
  if( isr_flag == 1 ) {
    detachInterrupt(APDS9960_INT);
    handleGesture();
    OLEDUpdate();
    FirebaseWrite();
    isr_flag = 0;
    attachInterrupt(APDS9960_INT, interruptRoutine, FALLING);
  }
}

void interruptRoutine() {
  isr_flag = 1;
}

void handleGesture() {
    if ( apds.isGestureAvailable() ) {
    switch ( apds.readGesture() ) {
      case DIR_UP:
        Serial.println("UP");
        gesture = "UP";
        break;
      case DIR_DOWN:
        Serial.println("DOWN");
        gesture = "DOWN";
        break;
      case DIR_LEFT:
        Serial.println("LEFT");
        gesture = "LEFT";
        break;
      case DIR_RIGHT:
        Serial.println("RIGHT");
        gesture = "RIGHT";
        break;
      case DIR_NEAR:
        Serial.println("NEAR");
        gesture = "NEAR";
        break;
      case DIR_FAR:
        Serial.println("FAR");
        gesture = "FAR";
        break;
      default:
        Serial.println("NONE");
        gesture = "NONE";
    }
  }
}

void OLEDInit(){
  display.init();
  display.setFont(ArialMT_Plain_24);
}

void OLEDUpdate(){
  display.clear();
  display.drawString(0,0,"Gesture :");
  display.drawString(0,30,gesture);
  display.display();
}
void WiFiInit(){
  pinMode(2,OUTPUT);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    digitalWrite(2,!digitalRead(2));
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
   
}
void FirebaseInit(){
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

}
void FirebaseWrite(){
  Firebase.setString(firebaseData,tag,gesture);
 
}
  
