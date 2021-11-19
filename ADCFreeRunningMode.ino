/*  ADCFreeRunningMode
  from: http://yaab-arduino.blogspot.com/2015/02/fast-sampling-from-analog-input.html?m=1


  /*
  Modified by Forrest Lee Erickson
  Date: 20211115 Remove magic numbers from code
  Read ADCL the lower bits.
  Read ADCH and ADCL into byte array.
  Stop/Start ADC Interrupt in main loop to read array
  Combine ADCH and ADCL to get 10 bit result at 76.88 KSPS.
  License: Dedicated to the public domain.
  Warranty: This code is designed to kill you but not guaranteed to do so.
  20211118 Modularize by moving wink to files.
  20211118 Modularize by ADC setup to files.
  20211119 Got wink working again. Got ADC timing working.

*/

#include "wink.h"
#include "freerunning_adc.h"

const long BAUD_RATE = 1000000; //Change to 1Mbit for speed.

extern volatile int indexOfSample = 0; //Index for iterating ADC sample array.
extern const int NUMBER_SAMPLES = 512;                 // Largest power of 2 we can allocate in the byte array next.
extern volatile byte channel_1[NUMBER_SAMPLES][2] ;    // Hold the MSB and LSB of Channel 1 ADC conversion


void setup()
{
  Serial.begin(BAUD_RATE); //For Serial monitor / Serial Ploter

  //Wink on the LED_BUILTIN
  setupWinkStart();

  setupFreerunningADCStart();  // Including 16 prescaler for 76.9 KHz

  //Wink off the LED_BUILTIN
  void setupWinkEnd(void);
}//end setup()

ISR(ADC_vect)
{
  channel_1[indexOfSample][1] = ADCL;  // read 8 LS value from ADC
  channel_1[indexOfSample][0] = ADCH;  // read 8 MSbit value from ADC
  indexOfSample++;
}

void loop()
{
  // put your main code here, to run repeatedly:
  winkLED_BUILTIN(); //the built in LED.

  printADCArray(); //Prints the ADC array if it is ready.

}// end loop()
