
#include "LPC17xx.h"

#include <cr_section_macros.h>
#include <NXP/crp.h>


#define MICROSECONDS_TICK					100				// iterate every 100us

//#define POWERUP_DELAY_BETWEEN_LED_BLINK		1000			// time in ms
#define POWERUP_DELAY_BETWEEN_LED_BLINK		5			// time in ms


// *************************************************************************************************************************************
long static timeON_100US[8][10] = {
		{0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
		{0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
		{0,    0,    0,   10,   10,   10,    0,    0,    0,    0},
		{0,    0,    0,   10,   10,   10,    0,    0,    0,    0},
		{0,    0,    0,   10,   10,   10,    0,    0,    0,    0},
		{0,    0,    0,   10,   10,   10,    0,    0,    0,    0},
		{0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
		{0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
};


// *************************************************************************************************************************************
long static timeOFF_100US[8][10] = {
		{0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
		{0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
		{0,    0,    0,   10,   10,   10,    0,    0,    0,    0},
		{0,    0,    0,   10,   10,   10,    0,    0,    0,    0},
		{0,    0,    0,   10,   10,   10,    0,    0,    0,    0},
		{0,    0,    0,   10,   10,   10,    0,    0,    0,    0},
		{0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
		{0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
};





// *************************************************************************************************************************************
// *************************************************************************************************************************************
// ** ****  do not modify beyond this line!  ***** **
// *************************************************************************************************************************************
// *************************************************************************************************************************************
long blinkCounter[8][10] = {
		{0,		0,		0,		0,		0,		0,		0,		0,		0,		0},
		{0,		0,		0,		0,		0,		0,		0,		0,		0,		0},
		{0,		0,		0,		0,		0,		0,		0,		0,		0,		0},
		{0,		0,		0,		0,		0,		0,		0,		0,		0,		0},
		{0,		0,		0,		0,		0,		0,		0,		0,		0,		0},
		{0,		0,		0,		0,		0,		0,		0,		0,		0,		0},
		{0,		0,		0,		0,		0,		0,		0,		0,		0,		0},
		{0,		0,		0,		0,		0,		0,		0,		0,		0,		0}
};

// *************************************************************************************************************************************
long LEDState[8][10] = {
		{0,		0,		0,		0,		0,		0,		0,		0,		0,		0},
		{0,		0,		0,		0,		0,		0,		0,		0,		0,		0},
		{0,		0,		0,		0,		0,		0,		0,		0,		0,		0},
		{0,		0,		0,		0,		0,		0,		0,		0,		0,		0},
		{0,		0,		0,		0,		0,		0,		0,		0,		0,		0},
		{0,		0,		0,		0,		0,		0,		0,		0,		0,		0},
		{0,		0,		0,		0,		0,		0,		0,		0,		0,		0},
		{0,		0,		0,		0,		0,		0,		0,		0,		0,		0}
};


// *************************************************************************************************************************************
// Variable to store CRP value in. Will be placed automatically
// by the linker when "Enable Code Read Protect" selected.
// See crp.h header for more information
__CRP const unsigned int CRP_WORD = CRP_NO_CRP ;

// *************************************************************************************************************************************
#define BIT(x) (((unsigned long) 1)<<(x))

#define P2_CLOCK		BIT(2)
#define P2_DATA			BIT(1)
#define P2_STROBE		BIT(0)

#define P2_ALL			(P2_CLOCK | P2_DATA | P2_STROBE)


#define LPCLED_PIN 		BIT(22)

// *************************************************************************************************************************************
void powerUpShowRunningDot(void) {

	int32_t d, l, w;					// loop counter
	uint8_t TIM0_IR;					// read out timer for base frequency

	for (d=-1; d<81; d++) {				// which dot?

		for (l=0; l<80; l++) {				// go through LEDs
			if (l==d) {								// current LED matches dot?
				LPC_GPIO2->FIOCLR = P2_DATA;			// set port to low -> on
			} else {
				LPC_GPIO2->FIOSET = P2_DATA;			// set port to high -> off
			}
			LPC_GPIO2->FIOSET = P2_CLOCK;			// clock new data out
			LPC_GPIO2->FIOCLR = P2_CLOCK;
		}
		LPC_GPIO2->FIOSET = P2_STROBE;				// activate new data
		LPC_GPIO2->FIOCLR = P2_STROBE;


		// *************************************************************************************************************************************
#define FINAL_DELAY ((1000/MICROSECONDS_TICK)*POWERUP_DELAY_BETWEEN_LED_BLINK)
		for (w=0; w<FINAL_DELAY; w++) {				// wait 500 * 100 us = 50ms
			TIM0_IR = LPC_TIM0->IR;					// Copy content of TIM0 Interrupt Register
			while( ( TIM0_IR & 0x1 ) != 0x1 ) { 	// Check for Interrupt on MR0
				TIM0_IR = LPC_TIM0->IR;
			}
			LPC_TIM0->IR = 0x1; 					// Reset MR0 Interrupt
		}

	}

}


// *************************************************************************************************************************************
// *************************************************************************************************************************************
int main(void) {
	int32_t r, c;							// counter for row and column
	uint32_t LPCLED_Count=1;				// counter for on-board debug LED
	uint8_t TIM0_IR;						// read out timer for base frequency


	// *************************************************************************************************************************************
	LPC_GPIO0->FIOCLR = LPCLED_PIN;			// Set P0 LPC_LED pin to default off
	LPC_GPIO0->FIODIR = LPCLED_PIN; 		// set P0.22 to output


	LPC_GPIO2->FIOCLR = P2_ALL;				// Set all P2 pins to default off
	LPC_GPIO2->FIODIR = P2_ALL; 			// set all P2 output pins


	// *************************************************************************************************************************************
	LPC_TIM0->PR = ((SystemCoreClock/1000000)/4)-1;   // => the timer is updated at 1MHz/ every 1us
	LPC_TIM0->MR0 = MICROSECONDS_TICK;			// Match every x us
	LPC_TIM0->MCR = 0x3; // Reset Timer Counter on Match Register 0 and generate interrupt

	LPC_TIM0->TCR = 0x1; // Start Timer


	// *************************************************************************************************************************************
	powerUpShowRunningDot();

	// *************************************************************************************************************************************
	while(1) {										// Enter an infinite loop


		// *************************************************************************************************************************************
		TIM0_IR = LPC_TIM0->IR;									// Copy content of TIM0 Interrupt Register
		while( ( TIM0_IR & 0x1 ) != 0x1 ) { 					// Check for Interrupt on MR0
			TIM0_IR = LPC_TIM0->IR;
		}
		LPC_TIM0->IR = 0x1; 									// Reset MR0 Interrupt


		// *************************************************************************************************************************************
		for (c=9; c>=0; c--) {
			for (r=7; r>=0; r--) {

				if (blinkCounter[r][c] == 0) {					// reached toggle value?
					if (LEDState[r][c]) {
						LEDState[r][c] = 0;							// set to off
						blinkCounter[r][c] = timeOFF_100US[r][c];	// start with off-time
					} else {
						LEDState[r][c] = timeON_100US[r][c] ? 1 : 0;// set to on
						blinkCounter[r][c] = timeON_100US[r][c];	// start with on-time
					}
				}

				blinkCounter[r][c]--;							// update LED state


				if (LEDState[r][c]) {								// check current state of LED
					LPC_GPIO2->FIOCLR = P2_DATA;					// set port to low
				} else {
					LPC_GPIO2->FIOSET = P2_DATA;					// set port to high
				}

				LPC_GPIO2->FIOSET = P2_CLOCK;					// clock new data out
				LPC_GPIO2->FIOCLR = P2_CLOCK;

			}
		}

		LPC_GPIO2->FIOSET = P2_STROBE;							// activate new data
		LPC_GPIO2->FIOCLR = P2_STROBE;


		// *************************************************************************************************************************************
		LPCLED_Count--;											// board LED at 10Khz / 10000 = 1Hz
		if (LPCLED_Count == 5000) {
			LPC_GPIO0->FIOCLR = LPCLED_PIN;
		}
		if (LPCLED_Count == 0) {
			LPCLED_Count = 10000;
			LPC_GPIO0->FIOSET = LPCLED_PIN;
		}
	}
	return 0 ;
}
