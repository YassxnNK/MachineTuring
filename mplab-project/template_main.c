/*
 * File:   pic24fj64ga002.c
 * Author: yassi
 *
 * Created on 8 juin 2025, 11:36
 */


#define FCY 8000000UL
#include <xc.h>
#include <libpic30.h>
#include <stdint.h>
#include <string.h>

// I2C address
#define LCD_ADDR 0x78
#define COMMAND  0x00
#define DATA     0x40

#define _XTAL_FREQ 8000000UL


/* Configuration Bits for PIC24FJ64GA002 */

#pragma config FNOSC = FRC        // Oscillator Source Selection: Fast RC Oscillator
#pragma config POSCMOD = NONE     // Primary Oscillator Mode: Disabled
#pragma config I2C1SEL = PRI      // Use default SCL1/SDA1 pins
#pragma config IOL1WAY = OFF      // Allow multiple reconfigurations of PPS
#pragma config OSCIOFNC = OFF     // CLKO output enabled on OSC2/RC15 (useful for debug)
#pragma config FCKSM = CSDCMD     // Clock Switching and Fail-Safe Clock Monitor disabled
#pragma config SOSCSEL = SOSC     // Secondary Oscillator (default)
#pragma config WUTSEL = FST       // Fast Wake-up Timer
#pragma config IESO = OFF         // Internal/External Switch Over disabled

#pragma config WDTPS = PS128      // Watchdog Timer Postscaler
#pragma config FWPSA = PR32       // WDT Prescaler Ratio Select
#pragma config WINDIS = ON        // Standard WDT (window mode disabled)
#pragma config FWDTEN = OFF       // Watchdog Timer disabled

#pragma config ICS = PGx3         // Debugging on PGEC3/PGED3 (selon ton Pickit3)
#pragma config BKBUG = OFF        // Device resets in normal mode
#pragma config GWRP = OFF         // General Segment Write Protection disabled
#pragma config GCP = OFF          // General Code Protection disabled
#pragma config JTAGEN = OFF       // JTAG port disabled

void I2C1_Init(void) {
    I2C1BRG = 37; // ~100kHz with Fcy = 4MHz
    I2C1CONbits.I2CEN = 1;
    __delay_ms(10);
}

void I2C1_Start(void) {
    I2C1CONbits.SEN = 1;
    while (I2C1CONbits.SEN);
}

void I2C1_Stop(void) {
    I2C1CONbits.PEN = 1;
    while (I2C1CONbits.PEN);
}

void I2C1_Write(uint8_t data) {
    I2C1TRN = data;
    while (I2C1STATbits.TRSTAT); // wait for transmit
    while (I2C1STATbits.ACKSTAT); // check for ACK
}

void LCD_SendCommand(uint8_t cmd) {
    I2C1_Start();
    I2C1_Write(LCD_ADDR);
    I2C1_Write(COMMAND);
    I2C1_Write(cmd);
    I2C1_Stop();
    __delay_ms(2);
}

void LCD_SendData(uint8_t data) {
    I2C1_Start();
    I2C1_Write(LCD_ADDR);
    I2C1_Write(DATA);
    I2C1_Write(data);
    I2C1_Stop();
}

void LCD_Init(void) {
    LCD_SendCommand(0x38); // function set
    __delay_ms(1);
    LCD_SendCommand(0x39); // function set (extended)
    __delay_ms(1);
    LCD_SendCommand(0x14); // internal OSC frequency
    LCD_SendCommand(0x78); // contrast set
    LCD_SendCommand(0x5E); // power/icon/contrast
    LCD_SendCommand(0x6D); // follower control
    LCD_SendCommand(0x0C); // display ON
    LCD_SendCommand(0x01); // clear display
    __delay_ms(2);
    LCD_SendCommand(0x06); // entry mode set
}

void LCD_WriteText(const char *text) {
    I2C1_Start();
    I2C1_Write(LCD_ADDR);
    I2C1_Write(DATA);
    for (int i = 0; i < strlen(text); i++) {
        I2C1_Write(text[i]);
    }
    I2C1_Stop();
}

void LCD_Reset(void) {
    LATBbits.LATB14 = 0;
    __delay_ms(5);
    LATBbits.LATB14 = 1;
    __delay_ms(5);
}

int main(void) {
    // Set RESET pin as output (RB14)
    TRISBbits.TRISB14 = 0;
    
    I2C1_Init();
    LCD_Reset();
    LCD_Init();

    LCD_WriteText("Hello, world!");

    while(1);
    return 0;
}