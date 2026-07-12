///*
// * File:   newmainXC16.c
// * Author: drwur
// *
// * Created on June 19, 2026, 12:18 AM
// */
//
//#define FCY 4000000UL     // Instruction clock = 4 MHz
//
//#include <xc.h>
//#include <libpic30.h>     // for __delay_ms()
//
//// Adjust to your oscillator frequency
//
//int main(void) {
//    /* Configure RB13 as digital output */
//#ifdef ANSB
//    ANSBbits.ANSB13 = 0;      // Disable analog function if present
//#endif
//
//    TRISBbits.TRISB13 = 0;    // RB13 = output
//
//    while(1)
//    {
//        LATBbits.LATB13 = 1;  // LED ON
//        __delay_ms(500);
//
//        LATBbits.LATB13 = 0;  // LED OFF
//        __delay_ms(500);
//    }
//
//    return 0;
//}