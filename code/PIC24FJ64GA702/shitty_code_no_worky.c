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
//
//void I2C_Start() {
//    I2C1CONLbits.SEN = 1;         // Start condition
//    while(I2C1CONLbits.SEN);
//}
//
//void I2C_Stop() {
//    I2C1CONLbits.PEN = 1;         // Stop condition
//    while(I2C1CONLbits.PEN);
//}
//
//int I2C_ERROR() {
//    LATBbits.LATB13 = 1;
//    return 1;
//}
//
//// only takes 12 digit number
//int WriteDACvalue(uint16_t value) {  
//    uint8_t first_byte = 0x00FF & value >> 8;
//    uint8_t second_byte = value % 256;
//    
//    first_byte = 0b00000000 | first_byte; // combine with DAC bits
//
//    I2C_Start();
//            
//    // send 11000000
//    // receive ack
//    
//    I2C1TRN = 0b11000000;
//    while(I2C1STATbits.TRSTAT);
//        
//    if (I2C1STATbits.ACKSTAT) { 
//        return I2C_ERROR();
//    }
//    
//    // send 01011000 -> will automatically set output
//
//    I2C1TRN = 0b01011000;
//    while(I2C1STATbits.TRSTAT);
//        
//    if (I2C1STATbits.ACKSTAT) { 
//        return I2C_ERROR();
//    }
//       
//    // send first byte
//    I2C1TRN = first_byte;
//    while(I2C1STATbits.TRSTAT);
//        
//    if (I2C1STATbits.ACKSTAT) { 
//        return I2C_ERROR();
//    }
//    
//    I2C1TRN = second_byte;
//    while(I2C1STATbits.TRSTAT);
//        
//    if (I2C1STATbits.ACKSTAT) { 
//        return I2C_ERROR();
//    }    
//    
//    I2C_Stop();
//    return 0;
//}
//    
//
//void GeneralWrite(uint8_t data) {
//    I2C1TRN = data;
//    while(I2C1STATbits.TRSTAT);
//}
//
//
//int main(void) {
//    
//    // SDA is RB9
//    // SCL is RB8
//    // LDAC is RA2, but should be unused
//    
//    // set up error LED
//    
//#ifdef ANSB
//    ANSBbits.ANSB13 = 0;      // Disable analog function if present
//#endif
//    TRISBbits.TRISB13 = 0;    // RB13 = output
//    
//    LATBbits.LATB13 = 0;
//    
//    TRISBbits.TRISB8 = 1;   // SCL input
//    TRISBbits.TRISB9 = 1;   // SDA input
//
//    // Disable analog (if these pins have analog capability)
//    ANSELBbits.ANSB9 = 0;
//
//    ODCBbits.ODCB8 = 1; // Enable Open-Drain on SCL1 (RB8)
//    ODCBbits.ODCB9 = 1; // Enable Open-Drain on SDA1 (RB9
//    // I2C Config: 
//    
//    PMD1bits.I2C1MD = 0; // enabling I2C module
//    
//    // 1. Completely clear control registers first to reset the I2C state machine
//    I2C1CONL = 0;
//    I2C1CONH = 0;
//
//    // 2. Configure Pin Modes (RB8 is purely digital, only RB9 needs ANSEL)
//    ANSELBbits.ANSB9 = 0;   // SDA1 Pin (RB9) to Digital Mode
//    TRISBbits.TRISB8 = 1;   // SCL1 Pin (RB8) as Input
//    TRISBbits.TRISB9 = 1;   // SDA1 Pin (RB9) as Input
//
//    // 3. Set Baud Rate 
//    I2C1BRG = 18;
//
//    // 4. Enable the I2C Module LAST
//    I2C1CONLbits.I2CEN = 1;    
//    
//    TRISAbits.TRISA2 = 0;
//    LATAbits.LATA2 = 0;
//
//
//    if(I2C1CONLbits.I2CEN == 0) {
//        while(1);
//    }
//    // CALL General Reset
//    I2C_Start();
//
//    GeneralWrite(0b00000000);
//    
//    GeneralWrite(0b00000110);
//    
//    
//    I2C_Stop();
//    
//    __delay_ms(1000);
//    
//    // CALL General Wakeup
//    
//    I2C_Start();
//
//    GeneralWrite(0b00000000);
//    
//    GeneralWrite(0b00001001);
//    
//    I2C_Stop();
//    
//    __delay_ms(1000);
//
//    // Write to DAC
//    
//    while (1) {
//        WriteDACvalue(65535);
//        __delay_ms(100);
//        WriteDACvalue(0);
//    }
//            
//    return 0;
//}