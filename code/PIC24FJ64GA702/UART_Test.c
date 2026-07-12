//#define FCY 4000000UL     // Instruction clock = 4 MHz
//#pragma config JTAGEN = OFF
//#pragma config FWDTEN = OFF
//
//#include <xc.h>
//#include <libpic30.h>     // for __delay_ms()
//
//void UART_Init() {
//    // use C = RB10 = TX
//    // use D = RB11 = RX
//    
//    // see if I can disable Analog ...
//    
//    
//    //ANSBbits.ANSB10 = 0; 
//    //ANSBbits.ANSB11 = 0;
//    
//    
//    TRISBbits.TRISB10 = 0;
//    TRISBbits.TRISB11 = 1;
//    
//    __builtin_write_OSCCONL(OSCCON & 0xbf);
//    
//    // RX1 is bottom byte
//    RPINR18bits.U1RXR = 11; 
//    
//    RPOR5bits.RP10R = 3; // 3 corresponds to TX1
//    
//    __builtin_write_OSCCONL(OSCCON | 0x40);
//
//    //BRGH = 0;
//    U1BRG = 12; // for baud = 9600
//    
//    // for below, check CTS/RTS enable
//    // check Receive polarity, idle is "0" for RX
//    U1MODE = 0x0000 | (1 << 15); // (1 << 15 enables UART)
//    __delay_ms(1);
//    U1STA = (1 << 12) | (1 << 10); 
//}
//
//int SendUART(uint8_t data) { 
//    while (U1STAbits.TRMT == 0); // TX buffer has data!
//    U1TXREG = 0x0000 | data;
//    return 0;
//}
//
//// later check for errors
//uint8_t GetUART() {
//    if (U1STAbits.URXDA == 0) { // no data available
//        return 0;
//    }
//    return U1RXREG & (0x00FF);
//}
//
//void SendString(char* string) {
//    for (int i = 0; i < 100; ++i) {
//        char c = string[i];
//        if (c == '\0') { 
//            return;
//        }
//        SendUART((uint8_t) c);
//    }
//}
//
//
//int main(void) {
//    
//
//    
//    TRISAbits.TRISA2 = 0; // Initialize Error LED
//    LATAbits.LATA2 = 0;
//    
//    UART_Init();
//    
//    char response_message[] = "PIC24F responding over UART!\r\n";
//    
//    uint16_t count = 0;
//    while (1) {
//        if (GetUART() == 'N') {
//            SendString(response_message);
//            LATAbits.LATA2 = 1;
//        }
//        ++count;
//        if (count == 50000) { 
//            SendUART('?');
//        }
//    }
//        
//    return 0;
//}