#include <xc.h>
#include <stdint.h>

#define _XTAL_FREQ 16000000

// CONFIG1
#pragma config FOSC = INTOSC
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config MCLRE = OFF
#pragma config CP = OFF
#pragma config BOREN = ON
#pragma config CLKOUTEN = OFF

// CONFIG2
#pragma config WRT = OFF
#pragma config STVREN = ON
#pragma config LVP = OFF

void OSC_Init(void)
{
    // 16 MHz HFINTOSC
    OSCCON = 0b01111010;
}

void GPIO_Init(void)
{
    // RA4 = analog
    ANSELA = 0b00010000;

    TRISAbits.TRISA4 = 1;
    TRISAbits.TRISA2 = 0;
    TRISAbits.TRISA5 = 0;

    LATA = 0;
}

void ADC_Init(void)
{
    // AN3 = RA4
    ADCON0bits.CHS = 0b0011;
    ADCON1bits.ADFM = 1;      // Right justified
    ADCON1bits.ADCS = 0b110;  // Fosc/64
    ADCON0bits.ADON = 1;
}

uint16_t ADC_Read(void)
{
    __delay_us(10);

    ADCON0bits.GO = 1;
    while (ADCON0bits.GO);

    return ((uint16_t)ADRESH << 8) | ADRESL;
}

void PWM4_Init(void)
{
    //
    // Timer2
    //
    PR2 = 255;

    T2CONbits.T2CKPS = 0;     // 1:1
    T2CONbits.TMR2ON = 1;

    //
    // Initial duty = 0
    //
    PWM4DCH = 0;
    PWM4DCL = 0;

    //
    // Enable PWM4 output
    //
    PWM4CONbits.PWM4OE = 1;
    PWM4CONbits.PWM4EN = 1;
}

void PWM4_SetDuty(uint16_t duty)
{
    if (duty > 1023)
        duty = 1023;

    PWM4DCH = duty >> 2;
    PWM4DCL = (duty & 0x03) << 6;
}


// 
uint16_t increment = 48;
uint16_t duty_value = 1023 >> 1;

void PeformFeedback(uint16_t set_voltage) { 
    uint16_t measured = ADC_Read();
    uint16_t increment = 48;
    if (measured > set_voltage) { 
        if (increment + duty_value < duty_value) { 
            duty_value = 1023;
        }
        else {
            duty_value += increment;
        }
    }
    else {
        if (duty_value - increment > duty_value) {
            duty_value = 0;
        }
        else {
            duty_value -= increment;
        }
    }
    PWM4_SetDuty(duty_value);
}




int main(void)
{
    uint16_t adc;
    uint16_t count = 0;

    OSC_Init();
    GPIO_Init();
    ADC_Init();
    PWM4_Init();

    
    PWM4_SetDuty(511);

    while (1)
    {
        // 0x03FF = 10.321
        // so 0.01008 nominal V / bit
        PeformFeedback(500);
//        adc = ADC_Read();
//
//        if (++count >= 45000)
//        {
//            LATAbits.LATA2 = 1;
//        }
//        else {
//            LATAbits.LATA2 = 0;
//        }
        
    }
}