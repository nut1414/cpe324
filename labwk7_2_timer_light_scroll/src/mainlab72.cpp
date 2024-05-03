// C++ code
//
#define F_CPU 8000000L

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

volatile int running = 1;
volatile int direction = 0;
volatile int turnOn = 1;
volatile int forceOff = 0;

// our timer when counter overflow, it will trigger this interrupt and add 1
volatile int timer = 0;
volatile int timer2 = 0;

volatile int isHolding = 0;

void turnOnLed(uint8_t pin)
{
  PORTB &= ~(0x0F);
  PORTB |= (1 << pin);
}

ISR(TIMER1_OVF_vect)
{
  timer++;
  // normal mode, no prescaler
  // 8000000 / 65536 = 122 overflow per second
  // every 122 overflow is 1 second
  if (timer >= 122)
  {

    if (running)
    {
      if (turnOn == 0 || turnOn == 2)
      {
        turnOn = 1;
        direction = !direction;
      }
      else
      {
        if (direction)
          turnOn = turnOn + 1;
        else
          turnOn = turnOn - 1;
      }
    }
    // 3 led cycle moving from left to right
    if (forceOff)
    {
      PORTB &= ~(0x0F);
    }
    else
    {
      turnOnLed(turnOn);
    }

    timer = 0;
  }
}

ISR(TIMER0_OVF_vect)
{
  // while holding button
  if (!(PIND & (1 << PIND2)))
    timer2++;

  // 7812.5 / 255 = 30.6
  // every 30.6 overflow is 1 second

  // hold less than 3 sec
  if (!forceOff && timer2 > 0 && timer2 < 92 && (PIND & (1 << PIND2)))
  {
    running = !running;
    timer2 = 0;
  }
  // hold more than = 3 sec
  else if (timer2 >= 92 && (PIND & (1 << PIND2))) // 3 sec
  {
    PORTB &= ~(0x0F);
    forceOff = !forceOff;
    timer2 = 0;
  }
}

// int example16mhzctc(){
//   // set to ctc mode
//   TCCR1B |= (1 << WGM12);
//   // set prescaler to 1024
//   TCCR1B |= (1 << CS12) | (1 << CS10);

//   // only trigger on 1 second
//   // 8000000 / 1024 = 7812.5
//   OCR1A = 7812;

//   // enable interrupt

//   TIMSK1 |= (1 << OCIE1A);

// }
// ISR(TIMER1_COMPA_vect)
// {
//   // do something
// }

int main()
{
  // timer 1: 16 bit register
  // normal mode, no prescaler
  // 8000000 / 65536 = 122 overflow per second
  // 16 bit: 65536 maximum cycle
  TCCR1B |= (1 << CS10); // no prescaler
  // use TOIE1, enable overflow interrupt
  // when counter overflow, it will trigger interrupt
  TIMSK1 |= (1 << TOIE1);

  // timer 0: 8 bit register
  // normal mode, prescaler 1024
  // 8000000 / 1024 prescaler = 7812.5 add per second
  // 7812.5 / 255 = 30.6 overflow per second
  TCCR0B |= (1 << CS00) | (1 << CS02);
  // when counter overflow, it will trigger interrupt
  TIMSK0 |= (1 << TOIE0);

  // setup pin B0,1,2 as output
  DDRB |= (1 << DDB1) | (1 << DDB2) | (1 << DDB0);

  // setup pin D2 as input
  PORTD |= (1 << PORTD2);
  // enable external interrupt 0 (pind2)
  // EIMSK |= (1 << INT0);
  // // enable interupt
  // EICRA |= (1 << ISC00); // any logical change
  sei();
  while (1)
  {
  }
}
