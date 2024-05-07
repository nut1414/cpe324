// C++ code
//
#define F_CPU 8000000L

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char **month = (char *[]){"", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

// from slides
void i2c_init(void)
{
  TWSR &= ~(1 << TWBR0) & ~(1 << TWBR1); // set TWI prescaler = 1
  // TWBR = 0x48;                           // set i2c clock = 100kHz and crystal freq = 16MH
  TWBR = 0x20;         // set i2c clock = 100kHz and crystal freq = 8MHz
  TWCR |= (1 << TWEN); // Enable TWI interface
}

void i2c_start(void)
{
  TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); // write 1 to TWINT to reset Flag!!!
  while ((TWCR & (1 << TWINT)) == 0)
    ; // wait for TWINT return to 1
}

void i2c_write(uint8_t data)
{
  TWDR = data;
  TWCR = (1 << TWINT) | (1 << TWEN);
  while ((TWCR & (1 << TWINT)) == 0)
    ;
}

uint8_t i2c_read(uint8_t ackVal)
{

  TWCR = (1 << TWINT) | (1 << TWEN) | (ackVal << TWEA); // ackVal(0) = read once / ackVal(1) = multiple read
  while ((TWCR & (1 << TWINT)) == 0)
    ;
  return TWDR;
}

void i2c_stop()
{
  TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
}

void rtc_init(void)
{
  i2c_init();
  i2c_start();
  i2c_write(0xD0); // 1101 0000 D$1307's Address
  i2c_write(0x07);
  i2c_write(0x00);
  i2c_stop();
}

void rtc_setTime(uint8_t h, uint8_t m, uint8_t s, uint8_t D, uint8_t M, uint8_t Y)
{
  i2c_start();
  i2c_write(0xD0);
  i2c_write(0x00);
  i2c_write(s);
  i2c_write(m);
  i2c_write(h);
  i2c_write(0x00);
  i2c_write(D);
  i2c_write(M);
  i2c_write(Y);
  i2c_stop();
}

void rtc_getTime(uint8_t *h, uint8_t *m, unsigned char *s, uint8_t *D, uint8_t *M, uint8_t *Y)
{
  i2c_start();
  i2c_write(0xD0);
  i2c_write(0x00);
  i2c_stop();

  i2c_start();

  i2c_write(0xD1);

  *s = i2c_read(1); // multiple reading

  *m = i2c_read(1); // still reading
  *h = i2c_read(1); // still reading
  i2c_read(1);      // skip day of the week
  *D = i2c_read(1); // still reading
  *M = i2c_read(1); // still reading
  *Y = i2c_read(0); // finish reading
  i2c_stop();
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
char buffer2[16];

uint8_t h, m, s, D, M, Y;

int main()
{
  _delay_ms(1000);
  initLCD();
  lcdClearScreen();
  rtc_init();
  rtc_setTime(0x16, 0x44, 0x00, 0x02, 0x05, 0x24); // 16:15:00 02 may 2024
  while (1)
  {
    rtc_getTime(&h, &m, &s, &D, &M, &Y);
    sprintf(buffer, "%02x:%02x:%02x", h, m, s);
    sprintf(buffer2, "%02x %s 20%02x", D, month[M], Y);
    lcdFirstLine();
    lcdDisplayString(buffer);
    lcdSecondLine();
    lcdDisplayString(buffer2);
    _delay_ms(1000);
    lcdClearScreen();
  }
}
