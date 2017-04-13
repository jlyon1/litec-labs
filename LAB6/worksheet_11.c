/*
    Section:
    Date:
    File name:
    Program description:
*/

#include <c8051_SDCC.h>
#include <stdio.h>

/* Global Variables */
/////////////////////////
/*
 * NOTE: The equations printed in old Worksheet 11 interpret (int) as (signed int) and
 * (long) as (signed long).  SDCC versions >= 3.6 assume 'unsigned', therefore, we
 * explicitly add 'signed' below.  These are the same equations as previously on the worksheet.
 */
/////////////////////////

unsigned int desired = 0;           // set this value
unsigned int actual = 0;            // set this value
unsigned int kp = 0;                // set this value
unsigned int kd = 0;                // set this value
signed int pw_neut = 0;             // set this value
signed int previous_error = 0;      // set this value
signed int error = 0;               // set this value
signed int temp_motorpw_2byte = 0;
signed long temp_motorpw_alg1 = 0;
signed long temp_motorpw_alg2 = 0;
signed long temp_motorpw_alg3 = 0;
signed long temp_motorpw_alg4 = 0;
signed long temp_motorpw_alg5 = 0;
signed long temp_motorpw_alg6 = 0;

void main()
{
    Sys_Init();
    putchar(' ');
    error = desired-actual;
//  two byte calculation
    temp_motorpw_2byte = pw_neut+kp*error+kd*(error-previous_error);
    printf("The two byte calculation of motorpw is %d \r\n",temp_motorpw_2byte);
//  equation form 1, long ints
    temp_motorpw_alg1 = pw_neut+kp*error+kd*(error-previous_error);
    printf("Algorithm 1, four byte calculation of motorpw is %ld \r\n",temp_motorpw_alg1);
//  equation form 2, long ints
    temp_motorpw_alg2 = pw_neut+(signed long)kp*error+kd*(error-previous_error);
    printf("Algorithm 2, four byte calculation of motorpw is %ld \r\n",temp_motorpw_alg2);
//  equation form 3, long ints
    temp_motorpw_alg3 = (signed long)( pw_neut + kp * (error) + kd * (error - previous_error));
    printf("Algorithm 3, four byte calculation of motorpw is %ld \r\n",temp_motorpw_alg3);
//  equation form 4, long ints
    temp_motorpw_alg4 = pw_neut+kp*(signed int)(error)+kd*(signed int)(error-previous_error);
    printf("Algorithm 4, four byte calculation of motorpw is %ld \r\n",temp_motorpw_alg4);
//  equation form 5, long ints
    temp_motorpw_alg5 = (signed long)pw_neut+(signed long)(kp*(error))+(signed long)(kd*(error-previous_error));
    printf("Algorithm 5, four byte calculation of motorpw is %ld \r\n",temp_motorpw_alg5);
//  equation form 6, long ints
    temp_motorpw_alg6 = (signed long)pw_neut+(signed long)kp*(signed long)error+(signed long)kd*(signed long)(error-previous_error);
    printf("Algorithm 6, four byte calculation of motorpw is %ld \r\n",temp_motorpw_alg6);
}
