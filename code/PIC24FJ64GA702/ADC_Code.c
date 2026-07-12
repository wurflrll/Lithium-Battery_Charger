#define FCY 4000000UL     // Instruction clock = 4 MHz
#pragma config JTAGEN = OFF

#include <xc.h>
#include <libpic30.h>     // for __delay_ms()

void I2C_Start() {
   I2C1CONLbits.SEN = 1;         // Start condition
   while(I2C1CONLbits.SEN);
}

void I2C_Stop() {
   I2C1CONLbits.PEN = 1;         // Stop condition
   while(I2C1CONLbits.PEN);
}

int I2C_ERROR() {
   LATBbits.LATB13 = 1;
   return 1;
}


uint8_t first_byte;
uint8_t second_byte;

// only takes 12 bit number
int WriteDACvalue(uint16_t value) {  
    first_byte = 0x0F & (value >> 8);
    second_byte = value & (0xFF);    
    
    
    _MI2C1IF = 0;
    
    while(!_MI2C1IF); _MI2C1IF = 0;
    // 1st and 4th bits are VREF and GAIN, set to 0
    // VREF = 0 (VDD)
    // DAC0 = 0)
    // DAC1 = 0
    // GAIN = 0 (1x Gain)
    first_byte = 0b00000000 | first_byte;
    
    I2C_Start();
                    
    // addressing
    I2C1TRN = 0b11000000;
    //while(I2C1STATbits.TRSTAT);
    //while(!_MI2C1IF); _MI2C1IF = 0;
    while(I2C1STATbits.TBF);
    
    if (I2C1STATbits.ACKSTAT) { 
        return I2C_ERROR();
    }
    
    // send 01011000 -> will automatically set output
    // bits for single-write command
    I2C1TRN = 0b01011000;
    //while(I2C1STATbits.TRSTAT);
    //while(!_MI2C1IF); _MI2C1IF = 0;
    while(I2C1STATbits.TBF);

    if (I2C1STATbits.ACKSTAT) { 
        return I2C_ERROR();
    }
       
    
    // send first byte
    I2C1TRN = first_byte;
    //while(I2C1STATbits.TRSTAT);
    //while(!_MI2C1IF); _MI2C1IF = 0;
    while(I2C1STATbits.TBF);
  
    if (I2C1STATbits.ACKSTAT) { 
        return I2C_ERROR();
    }
    
    I2C1TRN = second_byte;
    //while(I2C1STATbits.TRSTAT);
    //while(!_MI2C1IF); _MI2C1IF = 0;
    while(I2C1STATbits.TBF);
    
    if (I2C1STATbits.ACKSTAT) { 
        return I2C_ERROR();
    }    
    
    I2C_Stop();
    return 0;
}

int WriteGain() {
    
    _MI2C1IF = 0;

        
   I2C_Start();
   
   I2C1TRN = 0b11000000;
   //while(!_MI2C1IF); _MI2C1IF = 0;
    while(I2C1STATbits.TBF);
       
   if (I2C1STATbits.ACKSTAT) { 
       return I2C_ERROR();
   }
   
   I2C1TRN = 0b11000000;
   //while(!_MI2C1IF); _MI2C1IF = 0;
    while(I2C1STATbits.TBF);
       
   if (I2C1STATbits.ACKSTAT) { 
       return I2C_ERROR();
   }
   
   I2C_Stop();
   return 0;
}


int WriteVREF() {
   I2C_Start();

   
   _MI2C1IF = 0;

   I2C1TRN = 0b11000000;
   //while(!_MI2C1IF); _MI2C1IF = 0;
    while(I2C1STATbits.TBF);
       
   if (I2C1STATbits.ACKSTAT) { 
       return I2C_ERROR();
   }
   I2C1TRN = 0b10000000;
   //while(!_MI2C1IF); _MI2C1IF = 0;
    while(I2C1STATbits.TBF);
       
   if (I2C1STATbits.ACKSTAT) { 
       return I2C_ERROR();
   }
   I2C_Stop();
   return 0;
}


int FastWriteDAC(uint16_t value) {
    
    
    _MI2C1IF = 0;

        
    first_byte = 0x0F & (value >> 8);
    second_byte = value & (0xFF);    
    
    // for Fast write command
    // first two bits are 0
    // next two bits are channel select
    
    first_byte = 0b00000000 | first_byte;
    
    I2C_Start();
                    
    // addressing
    I2C1TRN = 0b11000000;
    //while(!_MI2C1IF); _MI2C1IF = 0;
    while(I2C1STATbits.TBF);
        
    if (I2C1STATbits.ACKSTAT) { 
        return I2C_ERROR();
    }
       
    
    // send first byte
    I2C1TRN = first_byte;
    //while(!_MI2C1IF); _MI2C1IF = 0;
    while(I2C1STATbits.TBF);
        
    if (I2C1STATbits.ACKSTAT) { 
        return I2C_ERROR();
    }
    
    I2C1TRN = second_byte;
    //while(!_MI2C1IF); _MI2C1IF = 0;
    while(I2C1STATbits.TBF);
        
    if (I2C1STATbits.ACKSTAT) { 
        return I2C_ERROR();
    }    
    
    I2C_Stop();
    return 0;
}
   

int GeneralWrite(uint8_t data) {
    
   I2C1TRN = data;
   //while(!_MI2C1IF); _MI2C1IF = 0;
    while(I2C1STATbits.TBF);
   
   if (I2C1STATbits.ACKSTAT) { 
       return I2C_ERROR();
   }    
   return 0;
}


int main(void) {
   
   // SDA is RB9
   // SCL is RB8
   // LDAC is RA2, but should be unused
   
   // set up error LED
   
#ifdef ANSB
   ANSBbits.ANSB13 = 0;      // Disable analog function if present
#endif
   TRISBbits.TRISB13 = 0;    // RB13 = output
   
   LATBbits.LATB13 = 0; // ERROR LED    
   
   ODCBbits.ODCB8 = 1; // Enable Open-Drain on SCL1 (RB8)
   ODCBbits.ODCB9 = 1; // Enable Open-Drain on SDA1 (RB9
   // I2C Config: 
   
   PMD1bits.I2C1MD = 0; // disable before setting
   
   // 1. Completely clear control registers first to reset the I2C state machine
   I2C1CONL = 0;
   I2C1CONH = 0;

   ANSELBbits.ANSB9 = 0; // Basic initialization for SDA/SCL pins
   TRISBbits.TRISB8 = 0;
   TRISBbits.TRISB9 = 0;

   // 3. Set Baud Rate 
   I2C1BRG = 18;

   // 4. Enable the I2C Module LAST
   I2C1CONLbits.I2CEN = 1;    
   
   TRISAbits.TRISA2 = 0; // Initialize Error LED
   LATAbits.LATA2 = 0;


   if(I2C1CONLbits.I2CEN == 0) {
       while(1);
   }
   // CALL General Reset
   I2C_Start();

   GeneralWrite(0b00000000);
    
   GeneralWrite(0b00000110);
    
   I2C_Stop();
   
   __delay_ms(20);
    
    // CALL General Wakeup
    
    I2C_Start();

    GeneralWrite(0b00000000);
    
    GeneralWrite(0b00001001);
  
    I2C_Stop();   
    __delay_ms(20);
    
    WriteVREF();
    __delay_ms(20);

    
    WriteGain();
    __delay_ms(20);

    
    
    while(1) {
        
        for (uint16_t i = 0; i < 4096; ++i) {
            WriteDACvalue(i);
            __delay_ms(5);
        }
    }            
    return 0;
}