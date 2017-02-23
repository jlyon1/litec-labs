/*	Name: AKHIL OWNS THIS FILE
	Section: 2
	Side: A
	Date: 2/20/2017

	Assignment description:
		get the number of button presses in 15 seconds,
    if the button is down for 3 seconds, increment another counter, print out the results.

	File name: hw6.c
*/

#include <c8051_SDCC.h>// include files. This file is available online
#include <stdio.h>
#include <stdlib.h>

/*

 _        _______ _________ _______      _________ _______
( (    /|(  ___  )\__   __/(  ____ \     \__   __/(  ___  )
|  \  ( || (   ) |   ) (   | (    \/ _      ) (   | (   ) |
|   \ | || |   | |   | |   | (__    (_)     | |   | |   | |
| (\ \) || |   | |   | |   |  __)           | |   | |   | |
| | \   || |   | |   | |   | (       _      | |   | |   | |
| )  \  || (___) |   | |   | (____/\(_)  ___) (___| (___) |
|/    )_)(_______)   )_(   (_______/     \_______/(_______)

 _______           _______  _______ _________
(  ____ \|\     /|(  ____ \(  ____ \\__   __/
| (    \/| )   ( || (    \/| (    \/   ) (
| (_____ | (___) || (__    | (__       | |
(_____  )|  ___  ||  __)   |  __)      | |
      ) || (   ) || (      | (         | |
/\____) || )   ( || (____/\| (____/\   | |
\_______)|/     \|(_______/(_______/   )_(

 _        _______  _______  ______   _______
( (    /|(  ____ \(  ____ \(  __  \ (  ____ \
|  \  ( || (    \/| (    \/| (  \  )| (    \/
|   \ | || (__    | (__    | |   ) || (_____
| (\ \) ||  __)   |  __)   | |   | |(_____  )
| | \   || (      | (      | |   ) |      ) |
| )  \  || (____/\| (____/\| (__/  )/\____) |
|/    )_)(_______/(_______/(______/ \_______)

          _______  ______   _______ __________________ _        _______
|\     /|(  ____ )(  __  \ (  ___  )\__   __/\__   __/( (    /|(  ____ \
| )   ( || (    )|| (  \  )| (   ) |   ) (      ) (   |  \  ( || (    \/
| |   | || (____)|| |   ) || (___) |   | |      | |   |   \ | || |
| |   | ||  _____)| |   | ||  ___  |   | |      | |   | (\ \) || | ____
| |   | || (      | |   ) || (   ) |   | |      | |   | | \   || | \_  )
| (___) || )      | (__/  )| )   ( |   | |   ___) (___| )  \  || (___) |
(_______)|/       (______/ |/     \|   )_(   \_______/|/    )_)(_______)

*/

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------
void Port_Init(void);		//Port Initialization
void Timer_Init(void);     	//Initialize Timer 0
void Interrupt_Init(void); 	//Initialize interrupts
void Timer0_ISR(void) __interrupt 1;
//void ADC_Init(void);
unsigned char read_AD_input(unsigned char n);
void setLeds(int led, int status);
int timeToCounts(int timeInSec);
int random_int(int max);
void wait(int time);

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

int counts;				//overflow tracking variable
unsigned char input;	//input variable
int wait_time;
int points[3];
const int FIFTEEN_MS = 3;
const int ONE_HUNDRED_MS = 23;
const int THREE_SECONDS = 675;
const int FIFTEEN_SECONDS = 3375;

int x =0;
int i =0;
int outs = 0;
int currCount = 0;
int wait_time_counts = 0;
int buttonPressed;
int randomInteger;
int right;

__sbit __at 0xA7 SS;
__sbit __at 0xB7 PB0;
__sbit __at 0xB5 PB1;
__sbit __at 0xB3 PB2;
__sbit __at 0xB0 PB3;

__sbit __at 0xA4 BILED1;
__sbit __at 0xA5 BUZZER;
__sbit __at 0xB6 LED0;
__sbit __at 0xB4 LED1;
__sbit __at 0xB2 LED2;
__sbit __at 0xB1 BILED0;


void ADC_Init(void)
{
	REF0CN = 0x03; /* Set Vref to use internal reference voltage (2.4V) */
	ADC1CN = 0x80; /* Enable A/D converter (ADC1) */
	ADC1CF |= 0x01; /* Set A/D converter gain to 1 */
}

//***************
void main(void)
{
	Sys_Init();      // System Initialization
	putchar(' ');    // the quote fonts may not copy correctly into SiLabs IDE
	Port_Init();
	ADC_Init();
	Interrupt_Init();
	Timer_Init();    // Initialize Timer 0

	TMR0 = 0;
	TR0 = 1;

	while (1) /* the following loop contains the button pressing/tracking code */
	{
    printf("---------------\r\nGuitar Hero\r\n---------------\r\n");
    //while(!SS){}
    wait(FIFTEEN_MS);
    //while(SS){}
    wait(FIFTEEN_MS);
	wait_time = read_AD_input(1) * 5 + 200;
	wait_time_counts = timeToCounts(wait_time);
    printf("Game Start: Wait time: %d\r\n",wait_time);
    for(i = 0; i < 10; i ++){
			int myNum = random_int(6) + 1;
			printf("Turn Start! \r\n");
			BUZZER = 0;
			wait(ONE_HUNDRED_MS * 2);
			BUZZER = 1;
			printf("Wait!\r\n");
			setLeds(0,myNum);
      wait(wait_time_counts);
			printf("STRUM NOW\r\n");
			currCount = counts;
			right = 0;
			setLeds(0,0);
			while(counts-currCount < wait_time_counts/2){
				if(!PB0){
					right = !PB1 + (!PB2 * 2) + (!PB3 * 4);
					printf("You Strummed %d\r\n",right);
					break;
				}
			}
			if(right == myNum){
				printf("Correct!\r\n");
				BILED0 = 0;
				BILED1 = 1;
				wait(THREE_SECONDS/3);
			}else{
				printf("Wrong!\r\n");
				BILED0 = 1;
				BILED1 = 0;
				wait(THREE_SECONDS/3);

			}
    }
		//printf("\033[2J");
		//printf("\033[0;0H");

		// setLeds(0, 1); //turns off all the leds
		// wait_time = read_AD_input(1) * 5 + 200;
		// wait_time_counts = timeToCounts(wait_time);
		// //getchar();
		// printf("got char");
		// points[0] = 0;
		// points[1] =0;
		// points[2]= 0;
		// counts = 0;
		// i =0;
		// x =0;
		// for (i = 0; i < 3; ++i)
		// {
		// 	for (x = 0; x < 8; ++x )
		// 	{
		// 		counts = 0;
		// 		while (counts < wait_time_counts )
		// 		{
		// 			printf("This is the counts%d",counts);
		// 			if (counts < 4)
		// 				BUZZER = 0 ;
		// 			else BUZZER = 1;
		// 		}
		// 		randomInteger  = random_int(7);
    //     printf("My Number: %d" randomInteger);
		// 		if (randomInteger == 0) {
		// 			setLeds(randomInteger, 0);//turn on the 0th led and turn off the rest
		// 			wait(wait_time);//wait for the specfied time
		// 			setLeds(randomInteger, 1); //turn off the 0th led
		// 		}
		// 		if (randomInteger == 1) {
		// 			setLeds(randomInteger, 0);//turn on the 0th led and turn off the rest
		// 			wait(wait_time);//wait for the specfied time
		// 			setLeds(randomInteger, 1); //turn off the 0th led
		// 		}
		// 		if (randomInteger == 2) {
		// 			setLeds(randomInteger, 0);//turn on the 0th led and turn off the rest
		// 			wait(wait_time);//wait for the specfied time
		// 			setLeds(randomInteger, 1); //turn off the 0th led
		// 		}
		// 		if (randomInteger == 3) {
		// 			setLeds(randomInteger, 0);//turn on the 0th led and turn off the rest
		// 			wait(wait_time);//wait for the specfied time
		// 			setLeds(randomInteger, 1); //turn off the 0th led
		// 		}
		// 		counts = 0;
		// 		while (counts < wait_time_counts / 2) {
		// 			if (!PB0)buttonPressed = 0;
		// 			else if (!PB1)buttonPressed = 1;
		// 			else if (!PB2)buttonPressed = 2;
		// 			else if (!PB3)buttonPressed = 3;
		// 			else buttonPressed = -1;
		// 		}
		// 		if (buttonPressed ==  randomInteger)
		// 		{
		// 			points[i]+=1;
		// 		}
		// 	}
		// }
		// for (i = 0; i < 3; ++i)
		// {
		// 	printf("Player %d: scored a %d/8", i, points[i]);
		// }
	}



}


//***************

void setLeds(int led, int status) {
	LED0 = 1;
	LED1 = 1;
	LED2 = 1;
	led = 5;
	status = ~status;
	LED0 = status & 0b001;
	LED1 = status & 0b010;
	LED2 = status & 0b100;
	}
//Revised this function
int timeToCounts(int timeInMs) {
	return (timeInMs * ONE_HUNDRED_MS) / 100;
}

void Port_Init(void)
{
  P3MDOUT = 0b01010110;
  P3 |= ~0b01010110;
  P2MDOUT &= 0b011111111;
  P2 |= 0b10110000;
  P1MDIN &= ~0x02;
  P1MDOUT &= ~0x02;
  P1 |= 0b00000010;
}


unsigned char read_AD_input(unsigned char n)
{
	AMX1SL = n; /* Set P1.n as the analog input for ADC1 */
	ADC1CN = ADC1CN & ~0x20; /* Clear the “Conversion Completed” flag */
	ADC1CN = ADC1CN | 0x10; /* Initiate A/D conversion */
	while ((ADC1CN & 0x20) == 0x00); /* Wait for conversion to complete */
	return ADC1; /* Return digital value in ADC1 register */
}

//***************

void Interrupt_Init(void)
{
	IE |= 0x02;      // enable Timer0 Interrupt request
	EA = 1;       // enable global interrupts
}

//***************
void Timer_Init(void)
{

	CKCON &= ~0x08;    // SYSCLK of SYSCLK/12
	TMOD &= 0xF0;   // clear the 4 least significant bits
	TMOD |= 0x00;   // select TIMER0 mode
	TR0 = 0;         // Stop Timer0
	TL0 = 0;         // Clear low byte of register T0
	TH0 = 0;         // Clear high byte of register T0

}


//***************
void Timer0_ISR(void) __interrupt 1
{
	counts += 1;
}
void wait(int time) {
	int init_counts = counts;
	while (counts - init_counts < time) {}
}
int random_int(int max) {
	return rand() % (max + 1) ;
}
