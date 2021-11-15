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

/*
   FLE modify to read ADCL too.
   Date: 20211115 Remove magic numbers from code
   Read ADCL the lower bits.
   Read ADCH and ADCL into byte array.
*/

const long BAUD_RATE = 1000000; //Change to 1Mbit for speed.

//const int NUMBER_SAMPLES = 1000;
const int NUMBER_SAMPLES = 512;
//const int NUMBER_SAMPLES = 255;
volatile byte channel_1[NUMBER_SAMPLES][2] ;    // Hold the MSB and LSB of Channel 1 ADC conversion

volatile int numSamples = 0;
long t, t0;

void setup()
{
  Serial.begin(BAUD_RATE);

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
  //  channel_1[numSamples][1] = ADCL;  // read 8 LS value from ADC
  channel_1[numSamples][0] = ADCH;  // read 8 MSbit value from ADC
  numSamples++;
}

void loop()
{
  if (numSamples >= NUMBER_SAMPLES)
  {
    //Stop ADC interups
    ADCSRA &= ~(1 << ADIE);  // Disable interrupts when measurement complete
    //noInterrupts();
    numSamples = 0;
    //Clear to disable ADC interupt on finish
    // ADCSRA &= ~(1 << ADIE);

    t = micros() - t0; // calculate elapsed time
    Serial.println("Sample#: valueMSB ");
    for (int ii = 1; ii < NUMBER_SAMPLES; ii++) {
      Serial.print(ii);
      Serial.print(":, ");
      Serial.print(channel_1[ii][0]);
      //      Serial.print(", ");
      //      Serial.print(channel_1[ii][1]);
      Serial.println("");
    }
    Serial.println("Clean up.");
    //  //Report setup and rate.
    //    Serial.print("Sampl Record Length (uS): ");
    //    Serial.println(t);
    //    Serial.print("Sampling frequency: ");
    //    Serial.print((float)1000*NUMBER_SAMPLES / t);
    //    //    Serial.print((float)1000000 / t);
    //    Serial.println(" KHz");



//        interrupts();
    t0 = micros();

    //    ADCSRA |= (1 << ADPS2);                     // 16 prescaler for 76.9 KHz
    //    ADCSRA |= (1 << ADATE); // enable auto trigger
    //    ADCSRA |= (1 << ADIE);  // enable interrupts when measurement complete
    //    ADCSRA |= (1 << ADEN);  // enable ADC
    //    ADCSRA |= (1 << ADSC);  // start ADC measurements
    delay(2000); // so we can read the results.
    //    numSamples = 0;
    // restart
    Serial.println("Restart.");
    ADCSRA |= (1 << ADIE);  // enable interrupts when measurement complete
  
  }// end if (numSamples >= NUMBER_SAMPLES)
}// end loop()
