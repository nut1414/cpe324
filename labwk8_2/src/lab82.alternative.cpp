// C++ code
//
#define F_CPU 8000000L

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void commitData()
{
  PORTD |= (1 << PORTD4); // set E pin(PD4) as high
  _delay_us(10);
  PORTD &= ~(1 << PORTD4); // E low
  _delay_us(10);
}

void sendLCDCommand(uint8_t command)
{
  PORTD &= ~(1 << PORTD2); // pull rs pin(PD2) low for sending command

  // high nibble
  PORTC &= 0xF0;
  PORTC |= command >> 4; // shift 4 bit to send high nibble of command to PC0-3
  // 11112222 -> 00001111

  commitData();

  // low nibble
  PORTC &= 0xF0;
  PORTC |= command & 0x0F; // send the lower nibble of command to PC0-3
  // 11112222 -> 00002222

  commitData();
}

void sendLCDData(uint8_t command)
{

  PORTD |= (1 << PORTD2); // pull rs pin(PD2) high for sending data
  // high nibble
  PORTC &= 0xF0;
  PORTC |= command >> 4; // shift 4 bit to send high nibble of command to PC0-3
  // 11112222 -> 00001111

  commitData();

  // low nibble
  PORTC &= 0xF0;
  PORTC |= command & 0x0F; // send the lower nibble of command to PC0-3
  // 11112222 -> 00002222

  commitData();
}

void lcdDisplayString(char *str)
{
  while (*str != '\0')
  {
    sendLCDData(*str);
    _delay_ms(1);
    str++; // move pointer by 1
  }
}

void lcdClearScreen()
{
  sendLCDCommand(0x1);
  _delay_ms(2);
}

void initLCD()
{
  _delay_ms(200);

  DDRC |= 0x0F;                         // set as output PC0-3
  PORTC &= 0xF0;                        // clear PC0-3
  DDRD |= (1 << DDD2) | (1 << DDD4);    // set as output PD2,4
  PORTD &= ~(1 << DDD2) | ~(1 << DDD4); // clear PD2,4

  // send initial command 4bit mode
  sendLCDCommand(0x33);
  sendLCDCommand(0x32);
  sendLCDCommand(0x28);
  sendLCDCommand(0x0E);
  sendLCDCommand(0x01);
  // force cursor to beginning of the 1st line (see list of command)
  sendLCDCommand(0x80);
}

char buffer[16];

// generate trigger pulse
// 10us high pulse
void generateTrigger()
{
  PORTC |= (1 << PORTC4); // set trigger pin high
  _delay_us(10);
  PORTC &= ~(1 << PORTC4); // set trigger pin low
}

int main()
{
  initLCD();
  lcdClearScreen();
  DDRC |= (1 << DDC4);  // PC4 output for trigger ultrasound
  DDRC &= ~(1 << DDC5); // PB0 input for echo ultrasound

  // stop timer
  TCCR1B = 0;

  while (1)
  {
    generateTrigger();
    // wait for rising edge
    while (!(PINC & (1 << PINC5)))

    // clear the timer counter
    TCNT1 = 0;
    // start timer
    TCCR1B |= (1 << CS11); // start timer set prescaler 8 -> 8MHz/8 = 1MHz -> 1us

    // wait for falling edge
    while (PINC & (1 << PINC5))
      ;

    // stop timer
    TCCR1B = 0;

    // calculate duration
    uint16_t duration = TCNT1;

    // calculate distance
    float distance = (duration * 343.0) / (2 * 100 * 100); // 1/58

    itoa(distance, buffer, 10);
    lcdDisplayString("Dist: ");
    lcdDisplayString(buffer);
    lcdDisplayString("cm");

    _delay_ms(1000);
    lcdClearScreen();
  }
}
