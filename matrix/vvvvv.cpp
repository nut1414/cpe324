// C++ code
//

void commitData()
{
  PORTB |= (1 << PORTB5); // set E pin(PB5) as high
  _delay_us(10);
  PORTB &= ~(1 << PORTB5); // E low
  _delay_us(10);
}

void sendLCDCommand(uint8_t comm)
{
  // Clear data pin
  PORTB &= 0xF0;
  PORTB &= ~(1 << PORTB4); // set RS pin low

  // set high nibble data
  PORTB &= 0xF0;
  PORTB |= (comm >> 4);
  commitData();

  PORTB &= 0xF0;
  PORTB |= comm & 0x0F; // send the lower nibble of command to PB0-3
  commitData();
}

void sendLCDData(uint8_t comm)
{
  PORTB &= 0xF0;
  PORTB |= (1 << PORTB4); // pull rs pin(PD2) high for sending data
  // high nibble
  PORTB &= 0xF0;
  PORTB |= comm >> 4; // shift 4 bit to send high nibble of command to PB0-3
  // 11112222 -> 00001111

  commitData();

  // low nibble
  PORTB &= 0xF0;
  PORTB |= comm & 0x0F; // send the lower nibble of command to PB0-3
  // 11112222 -> 00002222

  commitData();
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

void lcdDisplayString(char *str)
{
  while (*str != '\0')
  {
    sendLCDData(*str);
    str++; // move pointer by 1
  }
}

void setupLCD()
{
  DDRB |= 0x3F;     // PB0-5 as output
  PORTB &= ~(0x3F); // clear output to 0
  // B4 - RS
  // B5 - E
  sendLCDCommand(0x33);
  sendLCDCommand(0x32);
  sendLCDCommand(0x28);
  sendLCDCommand(0x0E);
  sendLCDCommand(0x01);

  sendLCDCommand(0x80);
}

void setupMatrix()
{
  DDRD &= ~0xF0; // set row(PD4-7) to be input
  PORTD |= 0xF0; // set row pin on ddrD to be internal pull up
  DDRD |= 0x0F;  // set col(PD0-3) to be output 00001111
}

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

/*
void processKeypadCommand(uint8_t c) {
  if (currentPassPos == 3) {
    sendLCDData(c);
    for (int i = 0; i < 4; i++ ){
        if (pass[i] != input[i]) {
          isPass = 0;
          break;
        }
        isPass = 1;
    }
    lcdSecondLine();
    if (isPass) {
        lcdDisplayString("Pass");
    }else {
        lcdDisplayString("Access Denied");
    }
    currentPassPos++;

  }else if(currentPassPos == 4){
    lcdClearScreen();
    currentPassPos= 0;
  }
  else{
    sendLCDData(c);
    input[currentPassPos++] = c;
  }
}
*/

void setup()
{
  setupLCD();
  setupMatrix();

  while (1)
  {
    // scan col
    for (int i = 0; i < 4; i++)
    {
      PORTD &= ~(1 << i); // set active low

      // scan row
      for (int j = 0; j < 4; j++)
      {
        if ((PIND & (1 << j + 4)) == 0)
        {
          // set pressed output
          uint8_t key = mapColRow(3 - i, 3 - j);

          processKeypadCommand(mapColRow(3 - i, 3 - j));
          while ((PIND & (1 << j + 4)) == 0)
            ;
        }
      }

      PORTD |= (1 << i); // set active high
    }
  }
}

void loop() {}
