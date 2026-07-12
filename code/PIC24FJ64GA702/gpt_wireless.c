#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

//-----------------------------------------------------
// Configuration
//-----------------------------------------------------

// Example assumes FCY = 16 MHz
#define FCY 16000000UL

//-----------------------------------------------------
// Globals
//-----------------------------------------------------

volatile uint8_t txByte = 0;
volatile bool txByteAvailable = false;

static volatile uint8_t shiftReg = 0xAA;
static volatile uint8_t bitCount = 8;

//-----------------------------------------------------
// GPIO Init
//-----------------------------------------------------

static void GPIO_Init(void)
{
    // Digital pins
    //ANSBbits.ANSB6 = 0;
    //ANSBbits.ANSB10 = 0;
    //ANSBbits.ANSB11 = 0;

    TRISBbits.TRISB6 = 0;
    TRISBbits.TRISB10 = 1;
    TRISBbits.TRISB11 = 0;

    LATBbits.LATB6 = 0;
    LATBbits.LATB11 = 0;
}

//-----------------------------------------------------
// Timer1
//-----------------------------------------------------

static void Timer1_Init(void)
{
    T1CON = 0;

    // Prescaler = 1:1
    T1CONbits.TCKPS = 0;

    // 100 us @ 16 MHz instruction clock
    // PR1 = 1600-1
    PR1 = 1599;

    TMR1 = 0;

    IFS0bits.T1IF = 0;
    IEC0bits.T1IE = 1;

    T1CONbits.TON = 1;
}

//-----------------------------------------------------
// IOC on RB10
//-----------------------------------------------------

static void IOC_Init(void)
{
    CNEN1bits.CN0IE = 1;      // <-- verify CN number for RB10
    CNPU1bits.CN0PUE = 1;     // optional pullup

    IFS1bits.CNIF = 0;
    IEC1bits.CNIE = 1;

    // Dummy read
    volatile uint16_t dummy = PORTB;
    (void)dummy;
}

//-----------------------------------------------------
// Queue one byte
//-----------------------------------------------------

bool TxLoadByte(uint8_t data)
{
    if(txByteAvailable)
        return false;

    txByte = data;
    txByteAvailable = true;

    return true;
}

//-----------------------------------------------------
// Timer ISR
//-----------------------------------------------------

void __attribute__((interrupt, auto_psv))
_T1Interrupt(void)
{
    IFS0bits.T1IF = 0;

    //------------------------------------
    // Load next byte when current complete
    //------------------------------------
    if(bitCount == 8)
    {
        if(txByteAvailable)
        {
            shiftReg = txByte;
            txByteAvailable = false;
        }
        else
        {
            shiftReg = 0xAA;
        }

        bitCount = 0;
    }

    //------------------------------------
    // Output one bit (LSB first)
    //------------------------------------
    LATBbits.LATB11 = shiftReg & 1;

    shiftReg >>= 1;

    bitCount++;
}

//-----------------------------------------------------
// Change Notification ISR
//-----------------------------------------------------

void __attribute__((interrupt, auto_psv))
_CNInterrupt(void)
{
    volatile uint16_t dummy = PORTB;
    (void)dummy;

    LATBbits.LATB6 ^= 1;

    IFS1bits.CNIF = 0;
}

//-----------------------------------------------------
// Main
//-----------------------------------------------------

int main(void)
{
    GPIO_Init();
    IOC_Init();
    Timer1_Init();

    __builtin_enable_interrupts();

    while(1)
    {
        // Example
        TxLoadByte(0x3C);

        // Application code here
    }
}