// C++ code
//
#define F_CPU 8000000L

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

volatile int turnOn = 1;

// pin led
// x  y(short)
// x - PB1
// y - GND

// pin button
// |  |
// i  j
// k  l
// |  |
// i - VCC
// j - PD2

ISR(INT0_vect)
{
  turnOn = !turnOn;
  _delay_ms(100);
  PORTB &= ~(1 << PORTB1);
}

int main()
{
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

    if (turnOn)
    {

      PORTB |= (1 << PORTB1);
      _delay_ms(2000);

      PORTB &= ~(1 << PORTB1);
      _delay_ms(2000);
    }
  }
}
