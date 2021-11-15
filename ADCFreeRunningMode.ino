/*  ADCFreeRunningMode
  from: http://yaab-arduino.blogspot.com/2015/02/fast-sampling-from-analog-input.html?m=1

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
  Sampling frequency: 100.16 KHz
  Sampling frequency: 100.20 KHz
  Sampling frequency: 76.95 KHz
  Sampling frequency: 76.95 KHz
  Sampling frequency: 76.95 KHz
  Sampling frequency: 76.92 KHz
*/


int numSamples = 0;
long t, t0;

void setup()
{
  Serial.begin(1000000);  //Change to 1Mbit for speed.

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
}//end setup()

ISR(ADC_vect)
{
  byte x = ADCH;  // read 8 bit value from ADC
  numSamples++;
}

void loop()
{
  if (numSamples >= 1000)
  {
    t = micros() - t0; // calculate elapsed time

    Serial.print("Sampling frequency: ");
    Serial.print((float)1000000 / t);
    Serial.println(" KHz");
    delay(2000);

    // restart
    t0 = micros();
    numSamples = 0;
  }
}// end loop()
