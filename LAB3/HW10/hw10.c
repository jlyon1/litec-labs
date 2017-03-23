/*Homework 10. Code for adjusting speed and sterr */
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
unsigned int ReadRanger(void);
unsigned int ReadCompass(void);

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
unsigned int ranger = 0;
unsigned int flag = 0;
unsigned int compFlag = 0;
unsigned int heading = 0;

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
  SMB_init();

  count = 0;
	//i2c_start();
  while(1){

    if(flag == 4){
      unsigned char Software_Version = 0;
      unsigned char Data[2];
      unsigned int range =0;
      unsigned char addr=0xE0; // the address of the ranger is 0xE0
      i2c_read_data(addr, 0, Data, 1); // read one byte, starting at reg 0
      Software_Version = Data[0];
      range = (((unsigned int)Data[0] << 8) | Data[1]);

      unsigned char asdf[2];
      flag = 0;
      /*Ping the ranger*/
      asdf[0] = 0x51; // write 0x51 to reg 0 of the ranger:
      i2c_write_data(0xE0, 0, asdf, 1); // write one byte of data to reg 0 at addr
      //printf("ready to read the ranger");

    }
    /*Reading the bytes for the hw*/
    if(compFlag == 2){
      unsigned char Software_Version = 0;
      unsigned int Hi_Low_Bytes = 0;
      unsigned char addr = 0xC0; // the address of the sensor, 0xC0 for the compass
      unsigned char Data[2]; // Data is an array with a length of 2
      unsigned int heading; // the heading returned in degrees between 0 and 3599
      i2c_read_data(addr, 0, Data, 1); // read One byte, starting at reg o
      Software_Version=(Data[0]); //combine the two values
      i2c_read_data(addr, 12, Data, 2); // read One byte, starting at reg o
      Hi_Low_Bytes=(((unsigned int)Data[0] << 8) | Data[1]); //combine the two values
    }
    //Print here to not slow down
    if(count % 4 == 0){
      printf("Range %u\r\n", ranger);
      printf("Heading %u\r\n", heading)
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
    compFlag += 1;
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

unsigned int ReadCompass(void){
  unsigned char addr = 0xC0; // the address of the sensor, 0xC0 for the compass
  unsigned char Data[2]; // Data is an array with a length of 2
  unsigned int heading; // the heading returned in degrees between 0 and 3599
  i2c_read_data(addr, 2, Data, 2); // read two byte, starting at reg 2
  heading =(((unsigned int)Data[0] << 8) | Data[1]); //combine the two values
   //heading has units of 1/10 of a degree
  return heading; // the heading returned in degrees between 0 and 3599
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
