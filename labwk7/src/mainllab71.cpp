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

ISR(TIMER1_OVF_vect)
{
  timer++;
  // normal mode, no prescaler
  // 8000000 / 65536 = 122 overflow per second
  // every 122 overflow is 1 second
  if (timer >= 244)
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
  // timer 1: 16 bit register
  // normal mode, no prescaler
  // 8000000 / 65536 = 122 overflow per second
  // 16 bit: 65536 maximum cycle
  TCCR1B |= (1 << CS10); // no prescaler
  // use TOIE1, enable overflow interrupt
  // when counter overflow, it will trigger interrupt
  TIMSK1 |= (1 << TOIE1);

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