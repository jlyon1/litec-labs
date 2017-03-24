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
unsigned int ReadRanger(void);
unsigned int ReadCompass(void);
void calibrateSteering(void);


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
int heading_error = 0;
float heading_k = 0.99;
int heading_target = 900;
int drive_error = 0;
float drive_k = 0.91;
int drive_target = 2760;
__sbit __at 0xB7 ss;

//-----------------------------------------------------------------------------
// Main Function
//-----------------------------------------------------------------------------
void main(void)
{
  // initialize board
  Sys_Init();
  putchar(' '); //the quotes in this line may not format correctly
  //system inits
  Port_Init();
  XBR0_Init();
  PCA_Init();

  PCA0CP2 = PW_CENTER;
 
  SMB_init();

  //reset timer counterx`x
  count = 0;
//  calibrateSteering(); //calibrate steering from left to right

  while (1) {
    //printf("in while%u\r\n",compFlag);
    if (flag == 5) {

      unsigned char asdf[2];
      flag = 0;
      asdf[0] = 0x51; // write 0x51 to reg 0 of the ranger:
      ranger = ReadRanger();
      i2c_write_data(0xE0, 0, asdf, 1); // write one byte of data to reg 0 at addr
      //printf("ready to read the ranger");

    }
    if (compFlag == 2) {
      heading = ReadCompass();
      compFlag = 0;
    }
    //Print here to not slow down
    if (count % 40 == 0) {
      printf("Range %u\r\n", ranger);
      printf("Heading %u\r\n", heading);
    }


    // input = getchar();
    // if (input == 'l')
    // {drive_target += 10;}
    // else if (input == 'k') {
    //   drive_target -= 10;
    // }
    // else if (input == 'a') {
    //   heading_target += 10;
    // }
    // else if (input == 's') {
    //   heading_target -= 10;
    // }
    // if (input == 't') {
    //   heading_k += 0.02;
    //   drive_k += 0.02;
    //   printf("heading k: %u", heading_k);
    //   printf("driving_k: %u", drive_k);
    // }
    // if (input == 'g') {
    //   heading_k -= 0.02;
    //   drive_k -= 0.02;
    //   printf("heading k: %u", heading_k);
    //   printf("driving_k: %u", drive_k);
    // }
    if (heading_target > PW_MAX) {
      heading_target = PW_MAX;
    }
    if (heading_target > PW_MIN) {
      heading_target = PW_MIN;
    }
    if (drive_target > PW_MAX) {
      drive_target = PW_MAX;
    }
    if (drive_target > PW_MIN) {
      drive_target = PW_MIN;
    }
    Steering_Servo();


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
  P3MDOUT |= 0xFF;
  P3 &= 0x00;
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

  if (CF) {
    CF = 0;
    PCA0 = 28670;
    flag += 1;
    compFlag += 1;
    count += 1;
  }

}

void calibrateSteering(void) {
  while (input != 'c') {

    input = getchar();
    if (input == 'l') // single character input to decrease the pulsewidth
    {
      printf("\r\nturn left\r\n");

      PW -= 10;
      if (PW < PW_MIN) { // check if less than pulsewidth minimum
        PW = PW_MIN;    // set SERVO_PW to a minimum value
      }
    }
    else if (input == 'r') // single character input to increase the pulsewidth
    {
      printf("\r\nturn right\r\n");

      PW += 10;
      if (PW > PW_MAX) // check if pulsewidth maximum exceeded
        PW = PW_MAX;     // set PW to a maximum value
    }
    PCA0CP0 = 0xFFFF - PW;
  }
  PW_CENTER = PW;
  printf("\r\nCalibration press v when left");


  while (input != 'v') {

    input = getchar();
    if (input == 'l') // single character input to decrease the pulsewidth
    {
      printf("\r\nturn left\r\n");

      PW -= 10;
      if (PW < PW_MIN) { // check if less than pulsewidth minimum
        PW = PW_MIN;    // set SERVO_PW to a minimum value
      }
    }
    else if (input == 'r') // single character input to increase the pulsewidth
    {
      printf("\r\nturn right\r\n");

      PW += 10;
      if (PW > PW_MAX) // check if pulsewidth maximum exceeded
        PW = PW_MAX;     // set PW to a maximum value
    }
    PCA0CP0 = 0xFFFF - PW;
  }
  PW_MIN = PW;
  printf("\r\nDone, left: %d", PW_MIN);
  printf("\r\nCalibration press b when right");


  while (input != 'c') {

    input = getchar();
    if (input == 'l') // single character input to decrease the pulsewidth
    {
      printf("\r\nturn left\r\n");

      PW -= 10;
      if (PW < PW_MIN) { // check if less than pulsewidth minimum
        PW = PW_MIN;    // set SERVO_PW to a minimum value
      }
    }
    else if (input == 'r') // single character input to increase the pulsewidth
    {
      printf("\r\nturn right\r\n");

      PW += 10;
      if (PW > PW_MAX) // check if pulsewidth maximum exceeded
        PW = PW_MAX;     // set PW to a maximum value
    }
    PCA0CP0 = 0xFFFF - PW;
  }
  PW_MAX = PW;
  printf("\r\nDone, right: %d", PW_MAX);

}


void Steering_Servo()
{
//  printf("SS %u", ss);
  if (1) {
    heading_error = heading_target - heading;
    PW = (heading_k * (heading_error) + PW_CENTER);
    drive_error = drive_target - ranger;
    DRIVE_PW = (drive_k * (drive_error) + PW_CENTER);
    if (ranger > 30 && ranger < 40)
    {
      DRIVE_PW = PW_CENTER;
    }
    if (count % 40 == 0) {
      printf("PW: %u\r\n", PW);
      printf("DRIVE_PW: %u\r\n", DRIVE_PW);
      printf("Heading error: %u\r\n================\r\n", heading_error);
    }
    if (DRIVE_PW > PW_MAX) {
      DRIVE_PW = PW_MAX;
    }

    if (DRIVE_PW < PW_MIN) {
      DRIVE_PW = PW_MIN;
    }
    if (PW > PW_MAX) {
      PW = PW_MAX;
    }
    if (PW < PW_MIN) {
      PW = PW_MIN;
    }
    PCA0CP0 = 0xFFFF - PW;
    PCA0CP2 = 0xFFFF - DRIVE_PW;
  }
  else {
    printf("switch is off and stopping motors\r\n");
    PCA0CP0 = 0xFFFF - PW;
    PCA0CP2 = 0xFFFF - PW_CENTER;
  }
}

void SMB_init(void) {
  SMB0CR = 0x93;
  ENSMB = 1;
}

unsigned int ReadCompass(void) {
  unsigned char addr = 0xC0; // the address of the sensor, 0xC0 for the compass
  unsigned char Data[2]; // Data is an array with a length of 2
  unsigned int heading; // the heading returned in degrees between 0 and 3599
  i2c_read_data(addr, 2, Data, 2); // read two byte, starting at reg 2
  heading = (((unsigned int)Data[0] << 8) | Data[1]); //combine the two values
  //heading has units of 1/10 of a degree
  return heading; // the heading returned in degrees between 0 and 3599
}


unsigned int ReadRanger()
{
  unsigned char Data[2];
  unsigned int range = 0;
  unsigned char addr = 0xE0; // the address of the ranger is 0xE0
  i2c_read_data(addr, 2, Data, 2); // read two bytes, starting at reg 2
  range = (((unsigned int)Data[0] << 8) | Data[1]);

  return range;
}
