/* Sample code for Lab 3.1. This code provides a basic start. */
#include <c8051_SDCC.h>
#include <stdio.h>
#include <stdlib.h>
//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------
void Port_Init(void);
void PCA_Init (void);
void XBR0_Init();
void Steering_Servo(void);
void PCA_ISR ( void ) __interrupt 9;

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------
unsigned int PW_CENTER = 2760;
unsigned int PW_MIN; = 2030;
unsigned int PW_MAX; = 3500;
unsigned int PW = 0;

//-----------------------------------------------------------------------------
// Main Function
//-----------------------------------------------------------------------------
void main(void)
{
    // initialize board
    Sys_Init();
    putchar(' '); //the quotes in this line may not format correctly
    Port_Init();
    XBR0_Init();
    PCA_Init();

    //print beginning message
    printf("Embedded Control Steering Calibration\n");
    // set the PCA output to a neutral setting
    //__________________________________________
    //__________________________________________
    PW = PW_CENTER;
    //__________________________________________
    //__________________________________________
    while(1)
        Steering_Servo();
}

//-----------------------------------------------------------------------------
// Port_Init
//-----------------------------------------------------------------------------
//
// Set up ports for input and output
//
void Port_Init()
{
    P1MDOUT |= 0x04;  //set output pin for CEX2 in push-pull mode
}

//-----------------------------------------------------------------------------
// XBR0_Init
//-----------------------------------------------------------------------------
//
// Set up the crossbar
//
void XBR0_Init()
{
    XBR0 = 0x27;  //configure crossbar as directed in the laboratory

}

//-----------------------------------------------------------------------------
// PCA_Init
//-----------------------------------------------------------------------------
//
// Set up Programmable Counter Array
//
void PCA_Init(void)
{
    PCA0MD &= 0b11110001;//Set to sysclock/12
    PCA0CPM0 = 0xC2;
    PCA0CN = 0x40;
    EIE1 |= 0x04;
}

//-----------------------------------------------------------------------------
// PCA_ISR
//-----------------------------------------------------------------------------
//
// Interrupt Service Routine for Programmable Counter Array Overflow Interrupt
//
void PCA_ISR ( void ) __interrupt 9
{
    // reference to the sample code in Example 4.5 -Pulse Width Modulation
    // implemented using the PCA (Programmable Counter Array), p. 50 in Lab Manual.
}

void Steering_Servo()
{
    char input;
    //wait for a key to be pressed
    input = getchar();
    if(input == 'r')  // single character input to increase the pulsewidth
    {
        // ___________________________
        // ___________________________
        //if(PW < PW_MIN)  // check if less than pulsewidth minimum
        //PW = _______;    // set SERVO_PW to a minimum value
    }
    else if(input == 'l')  // single character input to decrease the pulsewidth
    {
        // ___________________________
        // ___________________________
        //if(PW > _______)  // check if pulsewidth maximum exceeded
        //PW = _______;     // set PW to a maximum value
    }
    printf("PW: %u\n", PW);
    PCA0CP0 = 0xFFFF - PW;

}
