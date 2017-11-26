
#include <msp430.h>
#include <stdlib.h>
#include <stdio.h>
#include "circular_buffer.h"

static circular_buffer_t uart_buffer = CIRCULAR_BUFFER_INIT;

#define stop_watchdog() { \
    WDTCTL = WDTPW | WDTHOLD; \
}

#define start_sampling() { \
    ADC12CTL0 |= ADC12SC; \
}

static void configure_adc() {
    ADC12CTL0 = ADC12ON + ADC12SHT02;// + ADC12MSC;
    ADC12CTL1 = ADC12SHP;// + ADC12CONSEQ_1;
    //ADC12MCTL0 = ADC12INCH_0;
    //ADC12MCTL1 = ADC12INCH_1 + ADC12EOS;
    ADC12IE = ADC12IE0;//1;
    ADC12CTL0 |= ADC12ENC;
    P6SEL |= BIT0;
}

#define p1led_off()     {P1OUT &= ~0x1;}
#define p1led_on()      {P1OUT |= 0x1;}
#define p1led_blink()   {P1OUT ^= 0x1;}

int main(void) {
    stop_watchdog();

	P1DIR |= BIT0;					// Set P1.0 to output direction
	p1led_on();
	//P6SEL |= BIT0;// + BIT1;           // Set P6.0 and P6.1
	configure_adc();

	while(1)
	{
	    start_sampling();
	    __bis_SR_register(LPM0_bits + GIE);
	    __no_operation();
	}
}

#define vrefp 3.3
#define vrefm 0
#define mv_to_adc12(mv) (4095 * (mv - vrefm) / (vrefp + vrefm))

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(ADC12_VECTOR))) ADC12_ISR (void)
#else
#error Compiler not supported!
#endif
{
    //static uint8_t cnt = 0;
    switch(__even_in_range(ADC12IV,34))
    {
    case 0x06:
#if 0
        int data_plus, data_minus;
        data_plus = ADC12MEM0;
        data_minus = ADC12MEM1;
        if((data_plus - data_minus) > 372272)
        {
            P1OUT |= 0x01;
        }
        else
        {
            P1OUT &= ~0x01;
        }
#else
        if(ADC12MEM0 >= 0x7ff)
        {
            p1led_on();
        } else
        {
            p1led_off();
        }
        __bic_SR_register_on_exit(LPM0_bits);
#if 0
        if(ADC12MEM1 < 0x7ff)
        {
            P1OUT ^= 0x01;
        }
        else
        {
            P1OUT &= ~0x01;
        }
#endif
#endif
        break;
    }
}

