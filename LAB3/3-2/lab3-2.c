/*Lab 3.1. Code for adjusting speed and sterr */
#include <c8051_SDCC.h>
#include <stdio.h>
#include <stdlib.h>
#include <i2c.h>
//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------
void Port_Init(void);
void PCA_Init (void);
void XBR0_Init();
void Steering_Servo(void);
void PCA_ISR ( void ) __interrupt 9;
void SMB_init(void);
unsigned int ReadRanger();
//-----------------------------------------------------------ll------------------
// Global Variables
//-----------------------------------------------------------------------------
unsigned int PW_CENTER = 2760;
unsigned int PW_MIN = 2130;
unsigned int PW_MAX = 3450;
unsigned int PW = 0;
unsigned int DRIVE_PW = 0;
unsigned int val;
int count;
char input;
unsigned int result = 0;
unsigned int flag = 0;

//-----------------------------------------------------------------------------
// Main Function
//-----------------------------------------------------------------------------
void main(void)            
{
\
    // initialize board
  Sys_Init();
  putchar(' '); //the quotes in this line may not format correctly
  Port_Init();
  XBR0_Init();
  PCA_Init();
  SMB_init();

  count = 0;
	//i2c_start();
  while(1){

    if(flag == 5){
		
	  unsigned char asdf[2];
	  flag = 0;
      asdf[0] = 0x51; // write 0x51 to reg 0 of the ranger:
	  result = ReadRanger();
      i2c_write_data(0xE0, 0, asdf, 1); // write one byte of data to reg 0 at addr
	  //printf("ready to read the ranger");
      
	  printf("Range %u\r\n", result);
    }
  }
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
	P0MDOUT &= 0b11110011;
	P0 |= ~0b11110011;
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
    PCA0MD = 0b01110001;//Set to sysclock/12
	//PCA0MD = 0x
    PCA0CPM0 = 0xC2;
	PCA0CPM2 = 0xC2;
    PCA0CN = 0x40;
    EIE1 |= 0b00001000;
	EA = 1;

}

//-----------------------------------------------------------------------------
// PCA_ISR
//-----------------------------------------------------------------------------
//
// Interrupt Service Routine for Programmable Counter Array Overflow Interrupt
//
void PCA_ISR ( void ) __interrupt 9
{
	
	if(CF){
		CF = 0;
		PCA0 = 28670;
		flag += 1;
		count += 1;
	}
	
}

void Steering_Servo()
{
    char input;
    //wait for a key to be pressed
	input = getchar();
    if(input == 'l')  // single character input to decrease the pulsewidth
    {
		printf("\r\nturn left\r\n");

		PW -= 10;
        if(PW < PW_MIN){  // check if less than pulsewidth minimum
        	PW = PW_MIN;    // set SERVO_PW to a minimum value
		}
    }
    else if(input == 'r')  // single character input to increase the pulsewidth
    {
       	printf("\r\nturn right\r\n");

		PW += 10;
        if(PW > PW_MAX)  // check if pulsewidth maximum exceeded
        	PW = PW_MAX;     // set PW to a maximum value
    }else if(input == 'b')  // single character input to decrease the pulsewidth
    {
		printf("\r\ndecrease speed \r\n");

		DRIVE_PW -= 10;

    }
    else if(input == 'f')  // single character input to increase the pulsewidth
    {
       	printf("\r\nincrease speed\r\n");

		DRIVE_PW += 10;

    }

    printf("PW: %u\n", PW);
	printf("DRIVE_PW: %u\n", DRIVE_PW);
    PCA0CP0 = 0xFFFF - PW;
	PCA0CP2 = 0xFFFF - DRIVE_PW;

}

void SMB_init(void){
	SMB0CR = 0x93;
	ENSMB=1;
}


unsigned int ReadRanger()
{
  unsigned char Data[2];
  unsigned int range =0;
  unsigned char addr=0xE0; // the address of the ranger is 0xE0
  i2c_read_data(addr, 2, Data, 2); // read two bytes, starting at reg 2
  range = (((unsigned int)Data[0] << 8) | Data[1]);

  return range;
}