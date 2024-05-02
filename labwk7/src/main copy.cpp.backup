// C++ code
//
#define F_CPU 8000000L

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

volatile int turnOn = 1;
volatile int forceOff = 0;

// our timer when counter overflow, it will trigger this interrupt and add 1
volatile int timer = 0;

ISR(TIMER0_COMPA_vect)
{
  timer++;
  // 7812.5 / 255 = 30.6
  // every 30.6 cycle is 1 second
  if (timer == 61)
  {
    turnOn = !turnOn;
    if (forceOff || !turnOn)
    {
      PORTB &= ~(1 << PORTB1);
    }
    else if (turnOn)
    {
      PORTB |= (1 << PORTB1);
    }

    timer = 0;
  }
}

int main()
{

  // normal mode, prescaler 1024
  TCCR0B |= (1 << CS00) | (1 << CS02);
  // 8000000 / 1024 = 7812.5
  // 7812.5 / 255 = 30.6
  // set as maximum 255 cycle
  OCR0A = 255;
  // use ocie0a as comparator
  TIMSK0 |= (1 << OCIE0A);

  // setup pin B1 as output
  DDRB |= (1 << DDB1);
  // setup pin D2 as input
  PORTD |= (1 << PORTD2);
  // enable external interrupt 0 (pind2)
  EIMSK |= (1 << INT0);
  // enable interupt
  EICRA |= (1 << ISC01);
  sei();
  while (1)
  {
  }
}

ISR(INT0_vect)
{
  forceOff = !forceOff;
  _delay_ms(100);
  PORTB &= ~(1 << PORTB1);
}