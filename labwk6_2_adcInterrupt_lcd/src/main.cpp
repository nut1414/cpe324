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
    _delay_ms(10);
    str++; // move pointer by 1
  }
}

void lcdClearScreen()
{
  sendLCDCommand(0x1);
  _delay_ms(10);
}

void initLCD()
{
  _delay_ms(100);

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

uint16_t adcValue;

int main()
{
  initLCD();
  lcdClearScreen();
  //  pg20-21
  //   set adc target pin
  ADMUX &= 0x00; // clear mux
  // set reference voltatge to external avcc
  ADMUX |= (0b00000100); // set mux as for adc4
  ADMUX |= (1 << REFS0); // set REFS0 as 1, aref with external capacitor at aref
  // pg25
  ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0) | (1 << ADIE); // set clock for adc -> division factor 128 , enable interupt by ADIE

  sei();

  while (1)
  {

    // see timing diagram pg28-29
    //  start conversion
    //  ADCSRA ADC Control and Status Register A
    ADCSRA |= (1 << ADSC);

    // interrupt will be triggered when conversion is done

    // ADIF -> ADC Interupt Flag
    // read done
    // adc is value 0-1024 = 0-5V
    // adc to voltage mV : convert to mVolt scale (adcValue/1024.0 * 5) /1000
    // T = (V-500)/10
    // // uint16_t voltage = (adcValue / 1024.0 * 5000); //
    // itoa(voltage, buffer, 10);
    lcdDisplayString("Voltage : ");
    // lcdDisplayString(buffer);
    lcdDisplayString("mv");

    _delay_ms(1000);
    lcdClearScreen();
  }
}

ISR(ADC_vect)
{
  adcValue = ADC;
}