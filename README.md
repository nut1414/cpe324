# CPE327: Embedded System

## Browse by lab

Week 5

- [labwk5_output_pin](/labwk5_output_pin/src/main.cpp)

Week 6

- [labwk6_1_interrupt_toggle_light](/labwk6_1_interrupt_toggle_light/src/main.cpp)
- [labwk6_2_adcInterrupt_lcd](/labwk6_2_adcInterrupt_lcd/src/main.cpp)

Week 7

- [labwk7_1_timer_light_blink](/labwk6_2_adcInterrupt_lcd/)
- [labwk7_2_timer_light_scroll](/labwk7_2_timer_light_scroll/)

Week 8

- [labwk8_1_timer_light_pwm_dimmer](/labwk8_1_timer_light_pwm_dimmer/src/lab81.cpp)
- [labwk8_2_timer_ultrasonic_capture](/labwk8_2_timer_ultrasonic_capture/src/lab82.cpp)
- [labwk8_2_timer_ultrasonic_capture (alternative version)](/labwk8_2_timer_ultrasonic_capture/src/lab82.alternative.cpp)

Week 9

- [labwk9_1_spi_adc_temperature](/labwk9_1_spi_adc_temperature/src/main.cpp)
- [labwk9_2_i2c_clock](/labwk9_2_i2c_clock/src/main.cpp)

## Browse function by topic

### Input/Output Pin

- [Setting pin basics](/labwk5_output_pin/src/main.cpp#L6)

### External Interrupt

- [Enabling external interrupt using EIMSK/EICRA](/labwk6_1_interrupt_toggle_light/src/main.cpp#L38)

### LCD

- [LCD command](/labwk9_2_i2c_clock/src/main.cpp#L99)
  - [void commitData()](/labwk9_2_i2c_clock/src/main.cpp#L99)
  - [void sendLCDCommand(uint8_t command)](/labwk9_2_i2c_clock/src/main.cpp#L107)
  - [void lcdDisplayString(char \*str)](/labwk9_2_i2c_clock/src/main.cpp#L145)
  - [void lcdClearScreen()](/labwk9_2_i2c_clock/src/main.cpp#L155)
  - [void lcdFirstLine()](/labwk9_2_i2c_clock/src/main.cpp#L161)
  - [void lcdSecondLine()](/labwk9_2_i2c_clock/src/main.cpp#L167)
  - [void initLCD()](/labwk9_2_i2c_clock/src/main.cpp#L173)

### ADC

- [Using ADC](/labwk8_1_timer_light_pwm_dimmer/src/lab81.cpp#L20)
  - [Using ADC Interrupt by ADIE](/labwk8_1_timer_light_pwm_dimmer/src/lab81.cpp#L29)
  - [ISR of ADC](/labwk8_1_timer_light_pwm_dimmer/src/lab81.cpp#L32)
  - [Start ADC conversion](/labwk8_1_timer_light_pwm_dimmer/src/lab81.cpp#L83)

### Timer

- [Enabling Timer1 by setting prescaler](/labwk7_1_timer_light_blink/src/mainllab71.cpp#L43)
- [Overflow mode](/labwk7_1_timer_light_blink/src/mainllab71.cpp#L15)
  - [Using Timer0 overflow interrupt](/labwk7_2_timer_light_scroll/src/mainlab72.cpp#L122)
  - [Using Timer1 overflow interrupt](/labwk7_1_timer_light_blink/src/mainllab71.cpp#L46)
  - [ISR of overflow interrupt](/labwk7_1_timer_light_blink/src/mainllab71.cpp#L15)
- [CTC mode](/labwk7_2_timer_light_scroll/src/mainlab72.cpp#L88)
  - [Using Timer1 ctc interrupt](/labwk7_2_timer_light_scroll/src/mainlab72.cpp#L88)
  - [ISR of ctc interrupt](/labwk7_2_timer_light_scroll/src/mainlab72.cpp#L88)
- [Fast PWM non-inverting mode (on pin OC1B)](/labwk8_1_timer_light_pwm_dimmer/src/lab81.cpp#L38)
  - [int init_pwm()](/labwk8_1_timer_light_pwm_dimmer/src/lab81.cpp#L38)
  - [void set_pwm_duty_cycle(uint8_t duty_cycle)](/labwk8_1_timer_light_pwm_dimmer/src/lab81.cpp#L60)
- [Input capture mode](/labwk8_2_timer_ultrasonic_capture/src/lab82.cpp#L118)
  - [Using Timer1 input capture](/labwk8_2_timer_ultrasonic_capture/src/lab82.cpp#L118)
  - [Choosing edge input capture](/labwk8_2_timer_ultrasonic_capture/src/lab82.cpp#L126)
  - [Polling Timer1 input capture](/labwk8_2_timer_ultrasonic_capture/src/lab82.cpp#L122)

### SPI

- [Generic SPI command](/labwk9_1_spi_adc_temperature/src/main.cpp#L53)
  - [void initSPI()](/labwk9_1_spi_adc_temperature/src/main.cpp#L54)
  - [uint8_t sendSPI(uint8_t data)](/labwk9_1_spi_adc_temperature/src/main.cpp#L67)
  - [uint8_t receiveSPI()](/labwk9_1_spi_adc_temperature/src/main.cpp#L80)
- [External ADC](/labwk9_1_spi_adc_temperature/src/main.cpp#L12)
  - [void initSPIforExternalADC()](/labwk9_1_spi_adc_temperature/src/main.cpp#L12)
  - [uint16_t receiveSPIExternalADC()](/labwk9_1_spi_adc_temperature/src/main.cpp#L25)

### I2C

- [Generic i2c command](/labwk9_2_i2c_clock/src/main.cpp#L15)
  - [void i2c_init(void)](/labwk9_2_i2c_clock/src/main.cpp#L15)
  - [void i2c_start(void)](/labwk9_2_i2c_clock/src/main.cpp#L23)
  - [void i2c_write(uint8_t data)](/labwk9_2_i2c_clock/src/main.cpp#L30)
  - [uint8_t i2c_read(uint8_t ackVal)](/labwk9_2_i2c_clock/src/main.cpp#L38)
  - [void i2c_stop()](/labwk9_2_i2c_clock/src/main.cpp#L47)
- [RTC](/labwk9_2_i2c_clock/src/main.cpp#L52)
  - [void rtc_init(void)](/labwk9_2_i2c_clock/src/main.cpp#L52)
  - [void rtc_setTime(uint8_t h, uint8_t m, uint8_t s, uint8_t D, uint8_t M, uint8_t Y)](/labwk9_2_i2c_clock/src/main.cpp#L62)
  - [void rtc_getTime(uint8_t *h, uint8_t *m, unsigned char *s, uint8_t *D, uint8_t *M, uint8_t *Y)](/labwk9_2_i2c_clock/src/main.cpp#L77)
