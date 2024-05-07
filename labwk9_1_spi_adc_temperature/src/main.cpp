// C++ code
//
#define F_CPU 8000000L

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void initSPIforExternalADC()
{
  // set MOSI, CS as output
  DDRB |= (1 << DDB2) | (1 << DDB5); // set as output PB2,5

  // mode 0, MSB first
  SPCR |= (1 << SPE) | (1 << MSTR); // spi enable, // master mode
  // fosc/8
  // 8mhz/8 = 1mhz -> maximum for external adc chip 3201
  SPCR |= (1 << SPR0);
  SPSR |= (1 << SPI2X);
}

uint16_t receiveSPIExternalADC()
{
  // 0-8192 max

  PORTB &= ~(1 << PORTB2); // set CS pin(PB2) as low

  // start transmission
  SPDR = 0; // send dummy data
  // wait for transmission complete
  while (!(SPSR & (1 << SPIF)))
    ;
  // read data
  uint8_t highByte = SPDR; // first 4 bit
                           // start transmission
  SPSR |= (1 << SPIF);     // clear flag
  SPDR = 0;                // send dummy data
  // wait for transmission complete
  while (!(SPSR & (1 << SPIF)))
    ;
  // read data
  uint8_t lowByte = SPDR; // last  bit
  PORTB |= (1 << PORTB2); // set CS pin(PB2) as high

  uint16_t data = (((highByte & 0x1F) << 8) | lowByte); // highbyte first 8 bit, lowbyte 8 bit -> 13 bit

  return data;
}

// generic spi init function
void initSPI()
{
  // set MOSI, CS as output
  DDRB |= (1 << DDB2) | (1 << DDB5); // set as output PB2,5
  // mode 0, MSB first
  SPCR |= (1 << SPE) | (1 << MSTR); // spi enable, // master mode
  // fosc/8
  // 8mhz/8 = 1mhz
  SPCR |= (1 << SPR0);
  SPSR |= (1 << SPI2X);
}

// generic spi send and receive function
uint8_t sendSPI(uint8_t data)
{
  // start transmission
  SPDR = data; // send  data
  // wait for transmission complete
  while (!(SPSR & (1 << SPIF)))
    ;
  // read data
  uint8_t receivedData = SPDR;
  return receivedData;
}

// generic spi receive only function
uint8_t receiveSPI()
{
  uint8_t receivedData = sendSPI(0);
  return receivedData;
}

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

void lcdFirstLine()
{
  sendLCDCommand(0x80);
  _delay_ms(2);
}

void lcdSecondLine()
{
  sendLCDCommand(0xC0);
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

int main()
{
  _delay_ms(1000);
  initLCD();
  lcdClearScreen();
  initSPIforExternalADC();

  while (1)
  {

    // // receive data from external adc
    float voltage = receiveSPIExternalADC() / 8192.0 * 5; // 0-8192 -> 0-5000
    // 100c -> 1.5v
    // 0c -> 0.40v
    // convert voltage to temperature on straight line
    float temperature = (voltage - 0.40) / 1.5 * 100; // 0-100c
    itoa(temperature, buffer, 10);
    lcdFirstLine();
    lcdDisplayString("T: ");
    lcdDisplayString(buffer);
    lcdDisplayString("c");
    lcdSecondLine();
    lcdDisplayString("Temperature ");

    _delay_ms(1000);
    lcdClearScreen();
  }
}
