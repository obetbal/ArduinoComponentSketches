


/**
 * ACS-85-0001 ATTiny85  Simple Oscillator chip.
 * 
 * Not exactly meant for production, mostly a workbench helper
 * to provide 40106-like signals without all the capacitors
 * and such.  Just drop it into a 5v breadboard and away you go.
 * Nothing is configurable...however feel free to tweek code as needed.
 *
 * External pin 2 = high Freq 0 - about 434 hz
 * External pin 3 = high freq 1 - about 347 hz
 * External pin 5 = high freq 2 - sweep up in frequency.  Exp ramp
 * External pin 6 = LFO about 1hz
 * External pin 7 = LFO about 1.4 hz
 *
 * Explaination.
 * This is built around timer0 interrupts.  There are many ways to do this.
 * I find that counting up and comparing (CTC) gives me a little more flexability.
 *
 * The prescaler is set to 64  (CS00 and CS001)
 * see http://www.atmel.com/Images/atmel-2586-avr-8-bit-microcontroller-attiny25-attiny45-attiny85_datasheet.pdf
 * or google ATTINY85 data sheet and look for that table.
 *
 * assuming 8Mhz/(64*(OCR0A +1)) = 8mhz/(64*6) = 20833
 * So we are interupting at 20.8Khz
 *
 * I have a note counter set at 24
 * toggling every 24 counts is 868 flips of the pin
 * which gives us a frequency of 434.  Close enough to an A.
 *
 * a count of 30 is about 347 hz
 * 
 * checking with my voltmeter..its a few hz off...but again,its not a tuner.
 * If you need precision, add the xtal.
 *
 *
 * V 1.0  -  First Version
 *
 * Observations.
 * Had to use TIMER0_COMPA_vect vs TIM0_COMPA_vect
 * PORTB = 00011111 did not really work.  It worked, but my signal would
 * go away like it was being sucked or drained.  Using setPin I think enables
 * pull down/up properly.
 *
 */


//  ATTiny overview
//                           +-\/-+
//                   Reset  1|    |8  VCC
//         (pin3) PB4 HF_0  2|    |7  LFO_1 PB2 (pin2)
//         (pin0) PB3 HF_1  3|    |6  LFO_2 PB1 (pin1)
//                     GND  4|    |5  HF Ramp PB0 (pin0)




//Counters in the interrupt to toggle pins.
// Increasing the number reduces the frequency.
#define HIGH_FREQ_0 30
#define HIGH_FREQ_1 24
#define LOW_FREQ_0 10416
#define LOW_FREQ_1  7440

//counters for the frequencies
int hf0Counter = 0;
int hf1Counter = 0;
int lfo1Counter = 0;
int lfo0Counter = 0;


//We play with this counters a little more
//for the sweep pin.

int hf2Counter = 0;
int hf2FreqCounter = 0;
#define HIGH_FREQ_2_INCREMENT 20
#define HF_RANGE_LO 40
#define HF_RANGE_HI 8
int hf2MaxCounter = HF_RANGE_LO;



// the setup function runs once when you press reset or power the board
void setup() {

  //DDRB = 0x3F;  could use this too
  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);

  // initialize timer0
  noInterrupts();           // disable all interrupts

  TCCR0A = 0;
  TCCR0B = 0;

  TCCR0A |= (1 << WGM01); //Start timer 1 in CTC mode Table 11.5
  TIMSK |= (1 << OCIE0A); //Enable CTC interrupt see 13.3.6
  OCR0A = 5; //CTC Compare value

  TCCR0B |= (1 << CS00) | (1 << CS01); // Prescaler =64 Table 11.6

  interrupts();             // enable all interrupts

}

ISR(TIMER0_COMPA_vect)          // timer compare interrupt service routine
{

  //Each interrupt..check the counter.  If there is a match,
  //reset and toggle the bit.
  if (lfo0Counter > LOW_FREQ_0) {
    lfo0Counter = 0;
    PORTB ^= (_BV(PB1));
  }
  lfo0Counter++;

  if (lfo1Counter > LOW_FREQ_1) {
    lfo1Counter = 0;
    PORTB ^= (_BV(PB2));
  }
  lfo1Counter++;


  //HF code is the same.  Just smaller counters
  if (hf0Counter > HIGH_FREQ_0) {
    hf0Counter = 0;
    PORTB ^= (_BV(PB4));
  }
  hf0Counter++;

  if (hf1Counter > HIGH_FREQ_1) {
    hf1Counter = 0;
    PORTB ^= (_BV(PB3));
  }
  hf1Counter++;


  //Sweep is several nested loops
  if (hf2Counter > hf2MaxCounter) {
    hf2Counter = 0;

    if (hf2FreqCounter > HIGH_FREQ_2_INCREMENT) {
      hf2FreqCounter = 0;
      hf2MaxCounter--;
      if (hf2MaxCounter<HF_RANGE_HI) {
        hf2MaxCounter = HF_RANGE_LO;
      }
    }
    hf2FreqCounter++;
    PORTB ^= (_BV(PB0));
  }
  hf2Counter++;
  


}




void loop() {
  //Do nothing

}