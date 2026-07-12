#define FCY 4000000UL     // Instruction clock = 4 MHz
#pragma config JTAGEN = OFF



#pragma config FNOSC = FRC              // Oscillator Source Selection (Internal Fast RC (FRC))
#pragma config PLLMODE = DISABLED       // PLL Mode Selection (No PLL used; PLLEN bit is not available)
#pragma config IESO = OFF                // Two-speed Oscillator Start-up Enable bit (Start up device with FRC, then switch to user-selected oscillator source)

// FOSC
#pragma config POSCMD = NONE            // Primary Oscillator Mode Select bits (Primary Oscillator disabled)
#pragma config OSCIOFCN = OFF           // OSC2 Pin Function bit (OSC2 is clock output)
#pragma config SOSCSEL = ON             // SOSC Power Selection Configuration bits (SOSC is used in crystal (SOSCI/SOSCO) mode)
#pragma config PLLSS = PLL_PRI          // PLL Secondary Selection Configuration bit (PLL is fed by the Primary oscillator)
#pragma config IOL1WAY = ON             // Peripheral pin select configuration bit (Allow only one reconfiguration)
#pragma config FCKSM = CSDCMD           // Clock Switching Mode bits (Both Clock switching and Fail-safe Clock Monitor are disabled)

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
    
    // 1st and 4th bits are VREF and GAIN, set to 0
    // VREF = 0 (VDD)
    // DAC0 = 0)
    // DAC1 = 0
    // GAIN = 0 (1x Gain)
    first_byte = 0b00000000 | first_byte;
    
    I2C_Start();
                    
    // addressing
    I2C1TRN = 0b11000000;
    while(I2C1STATbits.TRSTAT);
        
    if (I2C1STATbits.ACKSTAT) { 
        return I2C_ERROR();
    }
    
    // send 01011000 -> will automatically set output
    // bits for single-write command
    I2C1TRN = 0b01011000;
    while(I2C1STATbits.TRSTAT);
        
    if (I2C1STATbits.ACKSTAT) { 
        return I2C_ERROR();
    }
       
    
    // send first byte
    I2C1TRN = first_byte;
    while(I2C1STATbits.TRSTAT);
        
    if (I2C1STATbits.ACKSTAT) { 
        return I2C_ERROR();
    }
    
    I2C1TRN = second_byte;
    while(I2C1STATbits.TRSTAT);
        
    if (I2C1STATbits.ACKSTAT) { 
        return I2C_ERROR();
    }    
    
    I2C_Stop();
    return 0;
}

int WriteGain() {
   I2C_Start();
   
   I2C1TRN = 0b11000000;
   while(I2C1STATbits.TRSTAT);
       
   if (I2C1STATbits.ACKSTAT) { 
       return I2C_ERROR();
   }
   
   I2C1TRN = 0b11000000;
   while(I2C1STATbits.TRSTAT);
       
   if (I2C1STATbits.ACKSTAT) { 
       return I2C_ERROR();
   }
   
   I2C_Stop();
   return 0;
}


int WriteVREF() {
   I2C_Start();

   I2C1TRN = 0b11000000;
   while(I2C1STATbits.TRSTAT);
       
   if (I2C1STATbits.ACKSTAT) { 
       return I2C_ERROR();
   }
   I2C1TRN = 0b10000000;
   while(I2C1STATbits.TRSTAT);
       
   if (I2C1STATbits.ACKSTAT) { 
       return I2C_ERROR();
   }
   I2C_Stop();
   return 0;
}


int FastWriteDAC(uint16_t value) {
    first_byte = 0x0F & (value >> 8);
    second_byte = value & (0xFF);    
    
    // for Fast write command
    // first two bits are 0
    // next two bits are channel select
    
    first_byte = 0b00000000 | first_byte;
    
    I2C_Start();
                    
    // addressing
    I2C1TRN = 0b11000000;
    while(I2C1STATbits.TRSTAT);
        
    if (I2C1STATbits.ACKSTAT) { 
        return I2C_ERROR();
    }
       
    
    // send first byte
    I2C1TRN = first_byte;
    while(I2C1STATbits.TRSTAT);
        
    if (I2C1STATbits.ACKSTAT) { 
        return I2C_ERROR();
    }
    
    I2C1TRN = second_byte;
    while(I2C1STATbits.TRSTAT);
        
    if (I2C1STATbits.ACKSTAT) { 
        return I2C_ERROR();
    }    
    
    I2C_Stop();
    return 0;
}
   

int GeneralWrite(uint8_t data) {
   I2C1TRN = data;
   while(I2C1STATbits.TRSTAT);
   
   if (I2C1STATbits.ACKSTAT) { 
       return I2C_ERROR();
   }    
   return 0;
}


uint32_t state = 0x00AAAAAA; // 24 bit LFSR

uint16_t random_number() {
    uint32_t bit = 0;
   
    // 24th, 23rd, 22nd, 17th, and 1
    bit = (0x01) ^ (state >> 23) ^ (state >> 22) ^ (state >> 21) ^ (state >> 17);
    bit &= 0x01;
    state = (state >> 1) | (bit << 23);
 
    return state & (0x0000FFFF);
}

// RA3 is drain
void TurnOffDrain() {
    LATAbits.LATA3 = 0;
}

void TurnOnDrain() {
    LATAbits.LATA3 = 1;
}


uint16_t cosc;
uint16_t nosc;

int main(void) {
   
   // SDA is RB9
   // SCL is RB8
   // LDAC is RA2, but should be unused
   
   // set up error LED
   
#ifdef ANSB
   ANSBbits.ANSB13 = 0;      // Disable analog function if present
   ANSAbits.ANSA3 = 0;
#endif
   TRISBbits.TRISB13 = 0;    // RB13 = output
   
   TurnOffDrain(); // turn off and initialize Drain
   
   TRISAbits.TRISA3 = 0;
    
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

   CLKDIVbits.RCDIV = 0;   // No division

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
   
   
   
   
   first_byte = CLKDIVbits.RCDIV;
cosc = OSCCONbits.COSC;
nosc = OSCCONbits.NOSC;
      
   I2C_Start();

    GeneralWrite(0b00000000);
    
    GeneralWrite(0b00000110);
    
    I2C_Stop();
    
    __delay_ms(1000);
    
    // CALL General Wakeup
    
    I2C_Start();

    GeneralWrite(0b00000000);
    
    GeneralWrite(0b00001001);
    
    I2C_Stop();   
    
    WriteVREF();
    
    WriteGain();
    
    FastWriteDAC(0x0000);

    TurnOnDrain(); 
    
    uint16_t increment = 20;
    while(1) {
        LATBbits.LATB13 = 0;
        FastWriteDAC(0x0000);
        __delay_ms(500);
        
        LATBbits.LATB13 = 1;
        FastWriteDAC(0x0000);
        __delay_ms(500);
    }            
    return 0;
}