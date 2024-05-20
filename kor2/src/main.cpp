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
  PORTB |= (1 << PORTB7); // set E pin(PB7) as high
  _delay_us(10);
  PORTB &= ~(1 << PORTB7); // E low
  _delay_us(10);
}

void sendLCDCommand(uint8_t command)
{
  PORTB &= ~(1 << PORTB6); // pull rs pin(PB6) low for sending command

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

  PORTB |= (1 << PORTB6); // pull rs pin(PB6) high for sending data
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
  DDRB |= (1 << DDB6) | (1 << DDB7);    // set as output PB6,4
  PORTB &= ~(1 << DDB6) | ~(1 << DDB7); // clear PB6,4
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

char test[20] = "1";

uint8_t h, m, s, D, M, Y;

uint8_t hexaKeys[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {
        '*',
        '0',
        '#',
        'D',
    }};

uint8_t mapColRow(uint8_t col, uint8_t row)
{
  return hexaKeys[row][col];
}

uint8_t pass[4] = {
    '0', '0', '0', '0'};

uint8_t input[6] = {
    '0', '0', '0', '0', '0', '0'};

uint8_t resetPass[6] = {
    '#', '9', '9', '9', '9', '*'};

uint8_t currentPassPos = 0;
uint8_t isPass = 0;
uint8_t isChange = 0;

ISR(TIMER0_OVF_vect)
{
  timer2++;
  if (timer2 >= 3)
  {
    lcdClearScreen();
    sendLCDData
  }
}

void processKeypadCommand(uint8_t c)
{

  if (c == '*' && currentPassPos == 4)
  {
    sendLCDData('*');
    for (int i = 0; i < 4; i++)
    {
      if (pass[i] != input[i])
      {
        isPass = 0;
        break;
      }
      isPass = 1;
    }
    lcdSecondLine();
    if (isPass)
    {
      lcdDisplayString("Pass");
    }
    else
    {
      lcdDisplayString("Access Denied");
    }
    currentPassPos = 5;
  }
  else if (currentPassPos == 5)
  {
    for (int i = 0; i < 5; i++)
    {
      if (input[i] == resetPass[i])
      {
        isChange = 1;
      }
      else
      {
        isChange = 0;
        break;
      }
    }
    currentPassPos = 0;
    lcdClearScreen();
    if (isChange)
    {
      lcdSecondLine();
      lcdDisplayString("Change Pass");
      lcdFirstLine();
    }
  }
  else if (isChange && currentPassPos < 5)
  {
    sendLCDData(c);
    pass[currentPassPos++] = c;
    if (currentPassPos == 4)
    {
      currentPassPos = 0;
      lcdClearScreen();
      isChange = 0;
    }
  }
  else if (!isChange)
  {
    sendLCDData(c);
    input[currentPassPos++] = c;
  }
}

void setupMatrix()
{
  DDRD &= ~0xF0; // set row(PD4-7) to be input
  PORTD |= 0xF0; // set row pin on ddrD to be internal pull up
  DDRD |= 0x0F;  // set col(PD0-3) to be output 00001111
}
volatile int timer = 1000;
volatile int timer2;
int main()
{
  // timer 0: 8 bit register
  // normal mode, prescaler 1024
  // 8000000 / 1024 prescaler = 7812.5 add per second
  // 7812.5 / 255 = 30.6 overflow per second
  TCCR0B |= (1 << CS00) | (1 << CS02);
  // when counter overflow, it will trigger interrupt
  TIMSK0 |= (1 << TOIE0);
  _delay_ms(1000);
  setupMatrix();
  initLCD();
  lcdClearScreen();
  // rtc_init();
  // rtc_setTime(0x16, 0x44, 0x00, 0x02, 0x05, 0x24); // 16:15:00 02 may 2024
  // {
  //   for (int i = 0; i < 4; i++)
  //   {
  //     PORTD &= ~(1 << i); // set active low

  //     // scan row
  //     for (int j = 0; j < 4; j++)
  //     {
  //       if ((PIND & (1 << j + 4)) == 0)
  //       {
  //         // set pressed output
  //         uint8_t key = mapColRow(3 - i, 3 - j);
  //         test[0] = key;
  //         lcdDisplayString(test);
  //         // processKeypadCommand(mapColRow(3 - i, 3 - j));
  //         while ((PIND & (1 << j + 4)) == 0)
  //           ;
  //       }
  //     }

  //     PORTD |= (1 << i); // set active high
  //   }
  // rtc_getTime(&h, &m, &s, &D, &M, &Y);
  // sprintf(buffer, "%02x:%02x:%02x", h, m, s);
  // sprintf(buffer2, "%02x %s 20%02x", D, month[M], Y);
  // lcdFirstLine();
  // lcdDisplayString(test);
  // lcdSecondLine();
  // lcdClearScreen();
}

ISR(TIMER0_OVF_vect)
{
  // while holding button
  if (!(PIND & (1 << PIND2)))
    timer2++;

  if (timer2 % 30 == 0 && (PIND & (1 << PIND2)))
  {
    timer--;
    sendLCDData(timer);
  }
  if (timer <= 0)
  {
    timer = 1000;
  }
}
