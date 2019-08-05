/* Arduino 7 Segment Display Thermometer with DS18B20 temp Sensor
   Dev: Vasilakis Michalis // Date: 25/7/2015 // www.ardumotive.com */

//http://www.instructables.com/id/Seven-Segment-Display-Thermometer-Arduino-Based/

//Libraries
#include "SevenSeg.h"
//#include <OneWire.h>
//#include <DallasTemperature.h>

/*---------------DS18B20--------------------*/
// Data wire is plugged into pin 2 on the Arduino
//#define ONE_WIRE_BUS A1
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
//OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature. 
//DallasTemperature sensors(&oneWire);

// Thermistor Example #3 from the Adafruit Learning System guide on Thermistors 
// https://learn.adafruit.com/thermistor/overview by Limor Fried, Adafruit Industries
// MIT License - please keep attribution and consider buying parts from Adafruit

// which analog pin to connect
#define THERMISTORPIN A0         
// resistance at 25 degrees C
#define THERMISTORNOMINAL 1800   /1,8KOhm   
// temp. for nominal resistance (almost always 25 C)
#define TEMPERATURENOMINAL 25   
// how many samples to take and average, more takes longer
// but is more 'smooth'
#define NUMSAMPLES 5
// The beta coefficient of the thermistor (usually 3000-4000)
#define BCOEFFICIENT 3000
// the value of the 'other' resistor
#define SERIESRESISTOR 9980    //10KOhm

int samples[NUMSAMPLES];


/*---------7 Segment 2 digit display--------*/
//Defines the segments A-G: SevenSeg(A, B, C, D, E, F, G);
SevenSeg disp (2,3,4,5,12,7,8);
//Number of 7 segments
const int numOfDigits =3;
//CC(or CA) pins of segment
int digitPins [numOfDigits]={11,10,9};//6-right,11-right, 10-midle, 9-left

//Variables
int digitL;
int digitR;
int temp; 

void setup() {
   Serial.begin(9600);
 // sensors.begin();
 //analogReference(EXTERNAL);
  //Defines the number of digits to be "numOfDigits" and the digit pins to be the elements of the array "digitPins"
    disp.setDigitPins ( numOfDigits , digitPins );
    //Only for common cathode 7segments
    disp.setCommonCathode();
    //Control brightness (values 0-100);
    disp.setDutyCycle(70);
    
    disp.setTimer (2);
    disp.startTimer ();
    disp . setDPPin (13) ;
  
}

void loop(){
  //Get temperature  
 // sensors.requestTemperatures();  
// float temp = sensors.getTempCByIndex(0);
uint8_t i;
  float average;

  // take N samples in a row, with a slight delay
  for (i=0; i< NUMSAMPLES; i++) {
   samples[i] = analogRead(THERMISTORPIN);
   delay(10);
  }
  
  // average all the samples out
  average = 0;
  for (i=0; i< NUMSAMPLES; i++) {
     average += samples[i];
  }
  average /= NUMSAMPLES;

  Serial.print("Average analog reading "); 
  Serial.println(average);
  
  // convert the value to resistance
  average = 1023 / average - 1;
  average = SERIESRESISTOR / average;
  Serial.print("Thermistor resistance "); 
  Serial.println(average);
  
  float steinhart;
  steinhart = average / THERMISTORNOMINAL;     // (R/Ro)
  steinhart = log(steinhart);                  // ln(R/Ro)
  steinhart /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart;                 // Invert
  steinhart -= 273.15;                         // convert to C
  
  Serial.print("Temperature "); 
  Serial.print(steinhart);
  Serial.println(" *C");
  delay(1000);
 
  disp.write(steinhart,1);
}
ISR( TIMER2_COMPA_vect ){
 disp.interruptAction ();
 }
