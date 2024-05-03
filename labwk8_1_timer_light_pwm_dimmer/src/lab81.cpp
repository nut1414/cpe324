#define F_CPU 8000000L

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// variable resistor pin
// x   z
//   y
// x - GND
// y - ADC5 (PC5)
// z - VCC

// led pin
// x  y(short)
// x - PB2
// y - GND

volatile uint16_t adc_value = 0; // store adc value
int init_adc()
{
  // set reference voltage to AVCC
  ADMUX |= (1 << REFS0);
  // pg 202 microship manual https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf
  ADMUX |= (0b00000100); // set mux as for adc4
  // enable ADC
  // pg 218
  // set clock for adc -> division factor 128 , enable interupt by ADIE
  ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0) | (1 << ADIE);
}
ISR(ADC_vect)
{
  adc_value = ADC;
}

// use OC1B as pwm output
int init_pwm()
{

  // set fast pwm mode 10-bit
  // pg 109 or ajarn slide 10
  TCCR1A |= (1 << WGM11) | (1 << WGM10);
  TCCR1B |= (1 << WGM12);

  // set OC1B
  TCCR1A |= (1 << COM1B1);

  // // set prescaler to 1
  // // pg 113
  TCCR1B |= (1 << CS10);

  // set OC1B as output

  DDRB |= (1 << DDB2);
}

// 0 25 75 100

void set_pwm_duty_cycle(uint8_t duty_cycle)
{
  // Calculate the compare value corresponding to the desired duty cycle
  uint16_t compare_value = (duty_cycle * 1024.0) / 100.0;

  // Set the compare value in OCR1B
  OCR1B = compare_value;
}

int main()
{
  // set B2 as output
  DDRB |= (1 << DDB2);
  init_adc(); // set mux as for adc5
  init_pwm();

  // enable global interrupt
  sei();

  // set up ADC
  // set reference voltage to AVCC
  while (1)
  {
    ADCSRA |= (1 << ADSC);
    set_pwm_duty_cycle((adc_value / 1024.0) * 100.0);
    // set_pwm_duty_cycle(50);

    // start adc conversion
  }

  return 0;
}