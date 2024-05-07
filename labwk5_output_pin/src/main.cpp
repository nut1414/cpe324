#define F_CPU 8000000L

#include <avr/io.h>
#include <util/delay.h>

// DDRB |= (1 << DDB3);    // set pin 3 of Port B as output
// PORTB |= (1 << PB3);    // set pin 3 of Port B high
// PORTB &= ~(1 << PB3);   // set pin 3 of Port B low
// PORTB |= (1 << PORTB3); // set pin 3 high again
// PORTB ^= (1 << PB3);    // toggles the state of the bit
// uint8_t pinValue = PINB & (1 << PINB4);

int main(void)
{
  /* Replace with your application code */
  DDRB |= (1 << DDB1);
  while (1)
  {
    PORTB |= (1 << PORTB1);
    _delay_ms(1000);
    PORTB &= ~(1 << PORTB1);
    _delay_ms(1000);
  }
}