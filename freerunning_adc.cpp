/* freerunning_adc.cpp
  Author: Forrest Lee Erickson
   Date: 20211118
   License: Dedicated to the public domain.
   Warranted: This program is designed to kill you and dist toy the universe but is not guaranteed to do so.

   ADCFreeRunningMode
  Inspired by: http://yaab-arduino.blogspot.com/2015/02/fast-sampling-from-analog-input.html?m=1

  Interrupts
  A better strategy is to avoid calling the analogRead() function and use the 'ADC Free Running mode'. This is a mode in which the ADC continuously converts the input and throws an interrupt at the end of each conversion. This approach has two major advantages:
  Do not waste time waiting for the next sample allowing to execute additional logic in the loop function.
  Improve accuracy of sampling reducing jitter.
  In this new test program I set the prescale to 16 as the example above getting a 76.8 KHz sampling rate.

  Results:
  Sampling frequency: 76.78 KHz
  Sampling frequency: 76.92 KHz
  Sampling frequency: 76.90 KHz
  Sampling frequency: 76.90 KHz
  Sampling frequency: 76.95 KHz
  Sampling frequency: 76.95 KHz
  Sampling frequency: 76.95 KHz
  Sampling frequency: 76.95 KHz
  Sampling frequency: 76.92 KHz
*/



//--------------- Includes ---------------------------
#include "Arduino.h"
#include "freerunning_adc.h"

extern volatile int indexOfSample ;

long lastADCArraytime = 0;
long nextADCArray = 2000; //time in ms.
const int NUMBER_SAMPLES = 512;
volatile byte channel_1[NUMBER_SAMPLES][2];

long t, t0;                 //For measuring the time to aquire NUMBER_SAMPLES

//Prints min, mid and maximum axis and dac data sample by sample
//Prints text following to make a legend.
void printADCoutSerial(void) {
  //    Serial.println("Sample#: value ");
  for (int i = 1; i < NUMBER_SAMPLES; i++) {
    //10 bit result = (ADCH << 8) |ADCL; // And then right shift to leave only 10 bits.
    Serial.print((uint16_t(channel_1[i][0] << 8 | channel_1[i][1]) >> 6)); //Plot data.
    Serial.println(", 1023, 0, 511");     //Red, Green Yellow, for MAX MIN and MID grid lines.
  }
  //Report setup and rate for plot, sort of, legend.
  Serial.print("SampleInterval=");
  Serial.print(t);
  Serial.print("uS. ");
  Serial.print("SampleNumber=");
  Serial.print(NUMBER_SAMPLES);
  Serial.print(" ");
  Serial.print("SamplingFrequency=");
  Serial.print((float)1000 * NUMBER_SAMPLES / t);
  Serial.println("KSPS");
}

//Get and print an array full of data.
void printADCArray(void) {
  //Get array if time to do so
  if (((millis() - lastADCArraytime) > nextADCArray) || (millis() < lastADCArraytime)) {
    //Lets get an array full
    indexOfSample = 0;
    t0 = micros();  //Reset the t0 before another burst of conversions.
    ADCSRA |= (1 << ADIE);  // enable interrupts to start capture

    //Block whiel sample array fills.
    while (indexOfSample < NUMBER_SAMPLES)
    {
      ;
    }// end if (indexOfSample >= NUMBER_SAMPLES)

    ADCSRA &= ~(1 << ADIE);       // Disable interrupts to stop capture
    t = micros() - t0;            // calculate elapsed time
    printADCoutSerial();
    lastADCArraytime = millis();
  }//end printADCArray()
}



/*Function for Arduino UNO to set up ADC input and clock and start ACE freerunning

*/
void setupFreerunningADCStart(void) {
  ADCSRA = 0;             // clear ADCSRA register
  ADCSRB = 0;             // clear ADCSRB register
  ADMUX |= (0 & 0x07);    // set A0 analog input pin
  ADMUX |= (1 << REFS0);  // set reference voltage
  ADMUX |= (1 << ADLAR);  // left align ADC value to 8 bits from ADCH register

  // sampling rate is [ADC clock] / [prescaler] / [conversion clock cycles]
  // for Arduino Uno ADC clock is 16 MHz and a conversion takes 13 clock cycles
  //ADCSRA |= (1 << ADPS2) | (1 << ADPS0);    // 32 prescaler for 38.5 KHz
  ADCSRA |= (1 << ADPS2);                     // 16 prescaler for 76.9 KHz
  //ADCSRA |= (1 << ADPS1) | (1 << ADPS0);    // 8 prescaler for 153.8 KHz

  ADCSRA |= (1 << ADATE); // enable auto trigger
  ADCSRA |= (1 << ADIE);  // enable interrupts when measurement complete
  ADCSRA |= (1 << ADEN);  // enable ADC
  ADCSRA |= (1 << ADSC);  // start ADC measurements
}// End of setupFreerunningADCStart
