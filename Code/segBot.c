/*******************************************************************************
 * @file                segBot.c
 * @brief               Motor controller and tachometer
 * @date                October 25th, 2014
 * @author              av333, kb476, avh34
 * @version             1.0
 *
 * This program uses the TRT operating system to schedule three main tasks
 *
 ******************************************************************************/

/*** Program Macros ***********************************************************/


/* Serial communication library, don't mess with the semaphores */
#define SEM_RX_ISR_SIGNAL 1
#define SEM_STRING_DONE   2

/* Semaphore to protect shared variable */
#define SEM_S 3
#define SEM_P 4
#define SEM_I 5
#define SEM_D 6


/*** Include Files ************************************************************/
#include "trtSettings.h"
#include "trtkernel_1284.c"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>           /* Used for strlen function */
#include <avr/io.h>
#include <avr/sleep.h>
#include <util/delay.h>

#include "trtUart.h"
#include "trtUart.c"


/*** Program Variables ********************************************************/

/* UART file descriptor putchar and getchar are in uart.c */
FILE uart_str = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);

/* Input arguments to each thread, not actually used in this example */
int args[2];

float p = 1.6;   /*PID proportional gain*/
float i = 0.3;   /*PID integral gain*/
float d = 0.2;   /*PID differential gain*/
float s = 1000;  /*Desired fan speed*/
float err = 0;

const float iFact = .975;


/*** Program Function Prototypes **********************************************/

int  main      (void);
void initialize(void);

void serialTask  (void *args);
void motorTask   (void *args);


/*** Program Functions ********************************************************/

int main(void)
{
	initialize();

	trt_uart_init();

	/* Print a statement to the serial communication terminal when the system is reset. */
	stdout = stdin = stderr = &uart_str;
	fprintf(stdout,"TinyRealTime: 2/9/09\n\r");

	/* Sets up the kernel data structures.
	 * The parameter is the desired starck size of the idle task.
	 * For a null idle task, a stack size of 80 should be sufficient. */
	trtInitKernel(80);

	/* Creates semaphores with identifer semnumber and initial values initval. */
	trtCreateSemaphore(SEM_RX_ISR_SIGNAL, 0);
	trtCreateSemaphore(SEM_STRING_DONE  , 0);
	trtCreateSemaphore(SEM_S            , 1);
	trtCreateSemaphore(SEM_P            , 1);
	trtCreateSemaphore(SEM_I            , 1);
	trtCreateSemaphore(SEM_D            , 1);

	/* Identifies the three functions to the kernel as threads.
	 * The parameters specify pointers to the functions, the
	 * desired stack size, the initial release time, the initial deadline time,
	 * and an abitrary data input structure */
	trtCreateTask(serialTask  , 200, SECONDS2TICKS(0.1), SECONDS2TICKS(0.2 ), &(args[0]));
	trtCreateTask(motorTask   , 200, SECONDS2TICKS(0.3), SECONDS2TICKS(0.32), &(args[1]));

	/* Choose our preferred sleep mode */
	set_sleep_mode(SLEEP_MODE_IDLE);
	/* Set sleep enable (SE) bit */
	sleep_enable();

	/* Sleep the CPU when a task isn't running. */
	while (1) {
		sleep_cpu();
	}

}

void initialize(void)
{
	/* Prescaler = 256 so 1 tick is 12.8 microsec */
	TCCR0B = 4;

	/* Toggle OC0A on compare match and turn on comp-match */
	TCCR0A = (1 << COM0A1) | (1 << COM0B1) | (1 << WGM00) | (1 << WGM01);

	DDRB = (1 << PINB3) | (1 << PINB4);

	/* Set up INT0 */
	EIMSK = 1 << INT0;  // Turn on INT0
	EICRA = 3;          // Rising edge

	/* Turn on timer 2 to be read in INT0 ISR */
	TCCR2B = 1;

	/* Turn on timer 2 overflow ISR for double precision time */
	TIMSK2 = 1;
}

void serialTask(void* args)
{
	char  cmd[4];
	float num = 0;

	while(1)
	{
		fscanf(stdin, "%s %f", cmd, &num);

		if (cmd[0] == 's') {
			/* Set desired motor speed */
			trtWait  (SEM_S);
			s = num;
			trtSignal(SEM_S);

			/* Statment confirming the new speed is printed. */
			fprintf(stdout, "\n\rDesired motor speed is now %.2f\n\r\n\r", (double) num);
		}
		else if (cmd[0] == 'p') {
			/* Set proportional gain */
			trtWait  (SEM_P);
			p = num;
			trtSignal(SEM_P);

			/* Statment confirming the new proportional gain is printed. */
			fprintf(stdout, "\n\rProportional gain is now %.2f\n\r\n\r", (double) num);
		}
		else if (cmd[0] == 'i') {
			/* Set integral gain */
			trtWait  (SEM_I);
			i = num;
			trtSignal(SEM_I);

			/* Statment confirming the new integral gain is printed. */
			fprintf(stdout, "\n\rIntegral gain is now %.2f\n\r\n\r", (double) num);
		}
		else if (cmd[0] == 'd') {
			/* Set differential gain */
			trtWait  (SEM_D);
			d = num;
			trtSignal(SEM_D);

			/*Statment confirming the new differential gain is printed.*/
			fprintf(stdout, "\n\rDifferential gain is now %.2f\n\r\n\r", (double) num);
		}

	}

}

void motorTask(void* args)
{
	uint32_t rel, dead;
	float motor_rpm = 0;   /* Actual motor RPM. */
	float pErr = 0;		   /* Proportional gain error term. */
	float iErr = 0;		   /* Integral gain error term. */
	float dErr = 0;		   /* Differential gain error term. */
	float output;		   /* Summation of the three aforementioned error terms. */

	while(1)
	{
		/* Calculation of the actual motor RPM. */
		motor_rpm = 60/(7*((float) motor_period)/F_CPU);

		/* Calculation of the deviation between the desired and actual fan speed. */
		err   = s - motor_rpm;


		if (err > 0) {
			iErr += err;
		}
		else {
			iErr *= iFact;
		}
		dErr  = err - pErr;

		/* Summation of the three aforementioned error terms. */
		output = p*err + i*iErr + d*dErr;

		/* PWM will drive the motor at 12V, which is full speed. */
		if (output > 255) {
			OCR0A = 255;
		}
		/* PWM will drive the motor at 0V, which shuts it off. */
		else if (output < 0) {
			OCR0A = 0;
		}
		/* PWM will drive the motor at the desired rate. */
		else {
			OCR0A = (unsigned char) output;
		}

		pErr = err; // Save current error as previous

		/* Ensure there is a maximum bound on the motor RPM. */
		if (motor_rpm > 3269) {
			OCR0B = 255;
		}
		/* Ensure there is a minimum bound on the motor RPM. */
		else if (motor_rpm < 0) {
			OCR0B = 0;
		}
		/* Scale the motor RPM to a value between 0 and 255. */
		else {
			OCR0B = (unsigned char)(0.078 * motor_rpm);
		}

		rel  = trtCurrentTime() + SECONDS2TICKS(0.01);
		dead = trtCurrentTime() + SECONDS2TICKS(0.01);
		trtSleepUntil(rel, dead);
	}

}
