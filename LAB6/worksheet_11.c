/*
    Section: 2B
    Date: 4/13/17
    File name: Worksheet 11
    Program description:
*/
4 #include <c8051_SDCC.h>
#include <stdio.h>

/* Global Variables */
/////////////////////////
/*
 * NOTE: The equations printed in old Worksheet 11 interpret (int) as (signed int) and
 * (long) as (signed long).  SDCC versions >= 3.6 assume 'unsigned', therefore, we
 * explicitly add 'signed' below.  These are the same equations as previously on the worksheet.
 */
/////////////////////////

unsigned int desired[] = {1800, 3500, 50, 3500};        // set this value
unsigned int actual[] = {3500, 1800, 250, 1800};         // set this value
unsigned int kp = 30;                // set this value
unsigned int kd = 30;                // set this value
signed int pw_neut = 2765;             // set this value
signed int previous_error[] = {1760, 1760, -250, 20};   // set this value
signed int error = 0;               // set this value
signed int temp_motorpw_2byte = 0;
signed long temp_motorpw_alg1 = 0;
signed long temp_motorpw_alg2 = 0;
signed long temp_motorpw_alg3 = 0;
signed long temp_motorpw_alg4 = 0;
signed long temp_motorpw_alg5 = 0;
signed long temp_motorpw_alg6 = 0;
int i = 0;
void main()
{
    Sys_Init();
    putchar(' ');
    while (i < 4) {
        error = desired[i] - actual[i];
//  two byte calculation
        temp_motorpw_2byte = pw_neut + kp * error + kd * (error - previous_error[i]);
        printf("The two byte calculation of motorpw is %d \r\n", temp_motorpw_2byte);
//  equation form 1, long ints
        temp_motorpw_alg1 = pw_neut + kp * error + kd * (error - previous_error[i]);
        printf("Algorithm 1, four byte calculation of motorpw is %ld \r\n", temp_motorpw_alg1);
//  equation form 2, long ints
        temp_motorpw_alg2 = pw_neut + (signed long)kp * error + kd * (error - previous_error[i]);
        printf("Algorithm 2, four byte calculation of motorpw is %ld \r\n", temp_motorpw_alg2);
//  equation form 3, long ints
        temp_motorpw_alg3 = (signed long)( pw_neut + kp * (error) + kd * (error - previous_error[i]));
        printf("Algorithm 3, four byte calculation of motorpw is %ld \r\n", temp_motorpw_alg3);
//  equation form 4, long ints
        temp_motorpw_alg4 = pw_neut + kp * (signed int)(error) + kd * (signed int)(error - previous_error[i]);
        printf("Algorithm 4, four byte calculation of motorpw is %ld \r\n", temp_motorpw_alg4);
//  equation form 5, long ints
        temp_motorpw_alg5 = (signed long)pw_neut + (signed long)(kp * (error)) + (signed long)(kd * (error - previous_error[i]));
        printf("Algorithm 5, four byte calculation of motorpw is %ld \r\n", temp_motorpw_alg5);
//  equation form 6, long ints
        temp_motorpw_alg6 = (signed long)pw_neut + (signed long)kp * (signed long)error + (signed long)kd * (signed long)(error - previous_error[i]);
        printf("Algorithm 6, four byte calculation of motorpw is %ld \r\n", temp_motorpw_alg6);
        i++;
    }
    while(1){}
}
