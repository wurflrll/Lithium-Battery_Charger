/*
 * File:   wireless_test.c
 * Author: drwur
 *
 * Created on July 8, 2026, 3:56 PM
 */


#pragma config JTAGEN = OFF
#pragma config FWDTEN = OFF


#define FCY 4000000UL

#include "xc.h"

#include <libpic30.h> 

// set LED

void LED_Init() { 
    ANSBbits.ANSB13 = 0;      // Disable analog function if present
    TRISBbits.TRISB13 = 0;
}

void Timer2_Init(void)
{
    T2CONbits.TON = 0;        // Stop timer

    T2CONbits.TCKPS = 0;      // 1:1 prescaler

    TMR2 = 0;
    
    // for approx 5kb rate
    //PR2 = 399;                 // 40 timer ticks = 10 us

    PR2 = 1999;
    
    IPC1bits.T2IP = 5;        // Interrupt priority
    IFS0bits.T2IF = 0;        // Clear flag
    IEC0bits.T2IE = 1;        // Enable interrupt

    T2CONbits.TON = 1;        // Start timer
}




// bit index = 

uint8_t sending_byte = 0b10101010;
uint8_t bit_index = 0;

void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void)
{
    
    
    IFS0bits.T2IF = 0;        // Clear interrupt flag

    LATBbits.LATB11 = (sending_byte >> bit_index) & 0x01;     // Toggle output

    bit_index = (bit_index + 1) % 8;
    
    // Put your control code here
}

void Transmit_Init() { 
    
    __builtin_write_OSCCONL(OSCCON & 0xBF);

    // Unmap any peripheral output from RP11
    RPOR5bits.RP11R = 0;

    
    // Lock PPS
    __builtin_write_OSCCONL(OSCCON | 0x40);

    TRISBbits.TRISB11 = 0;
    TRISBbits.TRISB10 = 1;
}

int main(void) {
    // D = RB11 is transmitter
    // C = RB10 is receiver
    LED_Init();
    
    Transmit_Init();

    Timer2_Init();
    
    while (1) {
        
        //LATBbits.LATB13 = LATBbits.LATB10;
        
        LATBbits.LATB13 = 1;
        __delay_ms(100);
        
        
        //LATBbits.LATB13 = LATBbits.LATB10;

        LATBbits.LATB13 = 0;
        __delay_ms(100);
    }
    return 0;
}
