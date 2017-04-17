/*
Lab 6 Code, gandola
Akhil Jacob, Joey Lyon, Donovan Gonzales
Read and process sensor input to control the
steering and speed of the car depending on external conditions.
*/

//-----------------------------------------------------------------------------
//INCLUDE STATEMENTS
//-----------------------------------------------------------------------------
#include <c8051_SDCC.h>
#include <stdio.h>
#include <stdlib.h>
#include <i2c.h>


//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------
void XBR0_Init();
void Port_Init(void);
void PCA_Init (void);
void SMB_init(void);
void steer_control(void);
unsigned int ReadCompass(void);
unsigned int ReadRanger(void);
void PCA_ISR ( void ) __interrupt 9;
void read_keypad_values();


//-----------------------------------------------------------------------------
// DEFINITIONS -  these are constant values
//-----------------------------------------------------------------------------
#define PW_MIN 2130
#define PW_MAX 3450
#define PW_CENTER 2760

#define COMPASS_GAIN 0
#define RANGER_GAIN 1

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------
// pulse width inits
unsigned int PW = 0;
unsigned int DRIVE_PW = 0;

// input and timer init
int count;
char key;

// ranger and compass value inits
int heading = 0;
unsigned int flag = 0;
unsigned int ranger = 0;
unsigned int compFlag = 0;

// porportion control values
int heading_error = 0;
int heading_target = 135;
int prev_error = 0;
int drive_error = 0;
int drive_target = 2760;
__xdata float drive_k = 130;
__xdata float kp = 0.99;
__xdata float kd = 0.99;
float error =0;
int tempForGainRead = 0;

__xdata int readGains = 0;
__xdata int gainReadState = COMPASS_GAIN;

int motorControlState = 0 ;


unsigned char read_AD_input(unsigned char n)
{
  AMX1SL = n; /* Set P1.n as the analog input for ADC1 */
  ADC1CN = ADC1CN & ~0x20; /* Clear the “Conversion Completed” flag */
  ADC1CN = ADC1CN | 0x10; /* Initiate A/D conversion */
  while ((ADC1CN & 0x20) == 0x00); /* Wait for conversion to complete */
  return ADC1; /* Return digital value in ADC1 register */
}

void ADC_Init(void)
{
  REF0CN = 0x03; /* Set Vref to use internal reference voltage (2.4V) */
  ADC1CN = 0x80; /* Enable A/D converter (ADC1) */
  ADC1CF |= 0x01; /* Set A/D converter gain to 1 */
}


//-----------------------------------------------------------------------------
// Main Function
//-----------------------------------------------------------------------------
void main(void)
{
  // initialize EVB
  Sys_Init();
  putchar(' ');
  Port_Init();
  XBR0_Init();
  PCA_Init();
  SMB_init();
  ADC_Init();
  // calibrate the drive motor
  PCA0CP0 = PW_CENTER;
  while (count < 60);

  // reset timer variable
  count = 0;

  //infinite while loop
  lcd_clear();
  lcd_print("Calibration:\nHello world!\n012_345_678:\nabc def ghij");

  while (1) {
  	
    if (read_keypad() != 0xFF) {
      key = read_keypad();

      if (key == 35) { // Start reading gains
        readGains = 1;
        compFlag = 0;
        flag = 0;
      }
      if (key == 42) {
        readGains = 2;
      }
    }
    key = 0;

    //read new gain values if and only if the pound key has already been pressed
    read_keypad_values();

    // wait so that ranger is not read to often
    if (flag >= 4) {
      unsigned char data[2]; // array to store the ranger data
      flag = 0; // reset the flag variable
      data[0] = 0x51; // write 0x51 to reg 0 of the ranger:
      ranger = ReadRanger(); // read the value of the ranger
      i2c_write_data(0xE0, 0, data, 1); // finally ping to the ranger
    }

    // wait so compass isn't read too often
    if (flag >= 2) {
      heading = ReadCompass(); // read compass value
      compFlag = 0; // reset the compass flag

    }
    //printf("%d  %d  %d  %d\r\n", error, heading, 0,  (int)(read_AD_input(1) * (15.0 / 255.0)));
	// print every few ms to prevent slowing down i2c communications
    if (count % 40 == 0) {
      printf("%d  %d\r\n", error, heading);
      lcd_clear();
      lcd_clear();
      lcd_print("Heading: %u,\n Range: %u", heading, heading_error, ranger);
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
  //P1MDIN &= ~0x02; //set pin 1 for analog input
  //P1MDOUT &= ~0x02; // Set to open drain
  P1MDOUT |= 0x04;  //set output pin for CEX2 in push-pull mode
  P0MDOUT &= 0b11110011;
  P0 |= ~0b11110011;
  P3MDOUT &= 0x00;
  P3 |= 0xFF;
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
// PCA_ISR - Interrupt Service Routine for Programmable Counter Array Overflow Interrupt
//-----------------------------------------------------------------------------
void PCA_ISR ( void ) __interrupt 9 {
  if (CF) {
    CF = 0;
    PCA0 = 28670;
    flag += 1;
    compFlag += 1;
    count += 1;
  }
}

//-----------------------------------------------------------------------------
// SMB_init - i2c inits
//-----------------------------------------------------------------------------
void SMB_init(void) {
  SMB0CR = 0x93;
  ENSMB = 1;
}

//-----------------------------------------------------------------------------
// ReadCompass - use i2c functions to read the compass value and return it
//-----------------------------------------------------------------------------
unsigned int ReadCompass(void) {
  unsigned char addr = 0xC0; // the address of the sensor, 0xC0 for the compass
  unsigned char Data[2]; // Data is an array with a length of 2
  unsigned int heading; // the heading returned in degrees between 0 and 3599
  i2c_read_data(addr, 2, Data, 2); // read two byte, starting at reg 2
  heading = (((unsigned int)Data[0] << 8) | Data[1]); //combine the two values
  //heading has units of 1/10 of a degree
  // return heading; // the heading returned in degrees between 0 and 3599
  printf("Reading: %u;%u",heading, heading/10);
  return (heading /10); //returns a value between 0 and 360
}


//-----------------------------------------------------------------------------
// ReadRanger - use i2c functions to read the ranger value and return it
//-----------------------------------------------------------------------------
unsigned int ReadRanger()
{
  unsigned char Data[2];
  unsigned int range = 0;
  unsigned char addr = 0xE0; // the address of the ranger is 0xE0
  i2c_read_data(addr, 2, Data, 2); // read two bytes, starting at reg 2
  range = (((unsigned int)Data[0] << 8) | Data[1]);

  return range;
}

//-----------------------------------------------------------------------------
// Steering control - This controls the steering by using the steering fan
//-----------------------------------------------------------------------------
void steering_control(void) {
  heading_error = heading_target - heading;
  PW = PW_CENTER + kp * heading_error + kd * (heading_error - prev_error);
  prev_error = heading_error;
  PCA0CP0 = 0xFF - PW;
}


//-----------------------------------------------------------------------------
// READ KEYPAD VALUES - this reads new gains from the keypad and displays some messages when pound
//                      key is pressed.
//-----------------------------------------------------------------------------
void read_keypad_values(void) {
  while (readGains == 2) {
    while (compFlag < 20) {
      //printf("wait");
    }
    lcd_clear();
    lcd_print("Enter heading for compass and press #\n");
    tempForGainRead = 0;
    while (key != 0x23) {
      while (read_keypad() == 0xFF) {

      }
      key = read_keypad();
      lcd_print("%c", key);
      if (tempForGainRead != 0 && key != 0x23) {
        tempForGainRead = tempForGainRead * 10;

      }
      if (key != 0x23) {
        tempForGainRead += (key - 0x30);
        //printf("val %u\r\n", tempForGainRead);
      }
      while (read_keypad() != 0xFF) {

      }
    }
    heading_target = tempForGainRead;
    tempForGainRead = 0;
    readGains = 0;

  }
  while (readGains == 1) {
    while (compFlag < 20) {
      printf("wait");
    }

    if (gainReadState == COMPASS_GAIN) {
      lcd_clear();
      lcd_print("Enter gain for compass and press #\n Note, this is multiplied by 10^-2\n");
      while (key != 0x23) {
        while (read_keypad() == 0xFF) {

        }
        key = read_keypad();
        lcd_print("%c", key);
        if (tempForGainRead != 0 && key != 0x23) {
          tempForGainRead = tempForGainRead * 10;

        }
        if (key != 0x23) {
          tempForGainRead += (key - 0x30);
          //printf("val %u\r\n", tempForGainRead);
        }
        while (read_keypad() != 0xFF) {

        }
      }
      kp = (float)((float)tempForGainRead * .01f);
      gainReadState = RANGER_GAIN;
      key = 0;
      tempForGainRead = 0;

    } else if (gainReadState == RANGER_GAIN) {

      while (flag < 100) {
        printf("wait");
      }

      lcd_clear();
      lcd_print("Enter gain for ranger and press #\n Note, this is multiplied by 10^-2\n");
      while (key != 0x23) {
        while (read_keypad() == 0xFF) {

        }
        key = read_keypad();
        lcd_print("%c", key);
        if (tempForGainRead != 0) {
          tempForGainRead = tempForGainRead * 10;
        }
        if (key != 0x23) {
          tempForGainRead += (key - 0x30);
          printf("val %u\r\n", tempForGainRead);
        }
        while (read_keypad() != 0xFF) {

        }
      }
      key = 0;
      drive_k = (float)tempForGainRead * .01;
      gainReadState = COMPASS_GAIN;
      printf("kp %u, ranger_k %u\r\n", kp, drive_k);
      tempForGainRead = 0;
      readGains = 0;
    }
  }

}
