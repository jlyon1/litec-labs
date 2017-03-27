//modified 4/28/2010 RPK, 11/17/2011 RPK (return 0 when not wired correctly)
//LCD Model: Devantech LCD02
//LCD Documentation: http://www.robot-electronics.co.uk/htm/Lcd02tech.htm
/*
kpdlcdtestPCA.c - Keypad and LCD test program - Uses PCA0 rather than Timer 0
Spring 2010
Use this program to exercise the LCD display and keypad to see how they work.
This program assumes SMB is on P0.2 & P0.3 (XBR0 = 0x05)

**** It is OK to change the XBR0 setting to 0x07 to match your wiring ****

Students were expected to remove Timer 0 delays from kpdlcdtest.c, the old
version of this demo and integrate it into their Gondola s/w, resulting in
something similar to this program.
*/

#include <stdio.h>
#include <stdlib.h>
#include <c8051_SDCC.h> // Include files. This file is available online in LMS
#include <i2c.h>        // Get from LMS, THIS MUST BE INCLUDED AFTER stdio.h
#define PCA_START 28672 // 28672 for exactly 20ms
//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------
void Port_Init(void);   // Initialize ports for input and output
void Interrupt_Init(void);
void PCA_Init(void);
void SMB0_Init(void);
void PCA_ISR(void) __interrupt 9;
void wait(void);
void pause(void);

// Global variables
unsigned int Counts, nCounts, nOverflows;
int myNum = 0;
//*****************************************************************************
void main(void)
{
    unsigned char keypad;
    Sys_Init();     // System Initialization - MUST BE 1st EXECUTABLE STATEMENT
    Port_Init();    // Initialize ports 2 and 3 - XBR0 set to 0x05, UART0 & SMB
    Interrupt_Init();   // You may want to change XBR0 to match your SMB wiring
    PCA_Init();
    SMB0_Init();
    putchar('\r');  // Dummy write to serial port
    printf("\nStart\r\n");

    Counts = 0;
    while (Counts < 1); // Wait a long time (1s) for keypad & LCD to initialize

    lcd_clear();
    lcd_print("Calibration:\nHello world!\n012_345_678:\nabc def ghij");
    while (1)
    {
		while(read_keypad() == 0xFF){

		}
        keypad = read_keypad();
        while(read_keypad() != 0xFF){
			pause();
        }
		lcd_clear();
		lcd_print("%c",keypad);
		myNum = 10*(keypad - 0x30);
		while(read_keypad() == 0xFF){

		}
        keypad = read_keypad();
		lcd_print("%c",keypad);
        while(read_keypad() != 0xFF){
			pause();
        }
		myNum += (keypad - 0x30);
		printf("\r\n%u",myNum);
        pause();    // This pauses for 1 PCA0 counter clock cycle (20ms)
                    // If the keypad is read too frequently (no delay), it will
                    // lock up and stop responding. Must power down to reset.
        if (keypad != 0xFF)   // keypad = 0xFF if no key is pressed
        {           // Note: fast read results in multiple lines on terminal
                    // A longer delay will reduce multiple keypad reads but a
                    // better approach is to wait for a -1 between keystrokes.
            lcd_clear();
            lcd_print("You entered:\n %u",myNum);
            printf("You entered:\n %u",myNum);
            if(keypad == 0)printf("   **Wire Connection/XBR0 Error**   ");
        }           // A returned value of 0 (0x00) indicates wiring error
    }
}
//*****************************************************************************

void Port_Init(void)	//0x05
{
    XBR0 = 0x05;    // NOTE: Only UART0 & SMB enabled; SMB on P0.2 & P0.3
}                   // No CEXn are used; no ports to initialize

void Interrupt_Init(void)
{
    IE |= 0x02;
    EIE1 |= 0x08;
    EA = 1;
}

void PCA_Init(void)
{
    PCA0MD = 0x81;      // SYSCLK/12, enable CF interrupts, suspend when idle
//  PCA0CPMn = 0xC2;    // 16 bit, enable compare, enable PWM; NOT USED HERE
    PCA0CN |= 0x40;     // enable PCA
}

void SMB0_Init(void)    // This was at the top, moved it here to call wait()
{
    SMB0CR = 0x93;      // Set SCL to 100KHz
    ENSMB = 1;          // Enable SMBUS0
}

void PCA_ISR(void) __interrupt 9
{
    if (CF)
    {
        CF = 0;                     // clear the interrupt flag
        nOverflows++;               // continuous overflow counter
        nCounts++;
        PCA0L = PCA_START & 0xFF;   // low byte of start count
        PCA0H = PCA_START >> 8;     // high byte of start count
        if (nCounts > 50)
        {
            nCounts = 0;
            Counts++;               // seconds counter
        }
     }
     else PCA0CN &= 0xC0;           // clear all other 9-type interrupts
}

void pause(void)
{
    nCounts = 0;
    while (nCounts < 1);// 1 count -> (65536-PCA_START) x 12/22118400 = 20ms
}                       // 6 counts avoids most of the repeated hits

void wait(void)
{
    nCounts = 0;
    while (nCounts < 50);    // 50 counts -> 50 x 20ms = 1000ms
}
