/*
 * buzzer.c
 *
 *
 *      Author: Ayman al adawi
 */

#include "buzzer.h"

/*****************************************************
 * Sets the PWM period of the Buzzer.  The duty cycle
 * will be set to 50%
 *
 * Parameters
 *      ticks_period    :   Period of PWM Pulse
 * Returns
 *      None
 *****************************************************/
void buzzer_init(uint16_t ticks_period)
{
    // The buzzer needs to be be a GPIO OUTPUT Pin
    BUZZER_PORT->DIR |= BUZZER_PIN;

    // Configure the buzzer pin to be controlled by a TIMERA peripheral
    BUZZER_PORT->SEL0 |= BUZZER_PIN;
    BUZZER_PORT->SEL1 &= ~BUZZER_PIN;

    // Turn off timer
    BUZZER_TIMER->CTL = 0;

    // Set the period of the timer.
    BUZZER_TIMER->CCR[0] = ticks_period - 1;

    // Configure BUZZER Duty Cycle to 50%
    BUZZER_TIMER->CCR[BUZZER_CHANNEL] = (ticks_period /2 ) -1;

    // Configure buzzer pin for RESET/SET Mode
    BUZZER_TIMER->CCTL[BUZZER_CHANNEL] = TIMER_A_CCTLN_OUTMOD_7;

    // Select the master clock as the timer source
    BUZZER_TIMER->CTL = TIMER_A_CTL_SSEL__SMCLK;

}

/*****************************************************
 * Turns the Buzzer on
 *
 * Parameters
 *      None
 * Returns
 *      None
 *****************************************************/
void buzzer_on(void)
{
    // Clear the current Mode Control Bits
    BUZZER_TIMER->CTL &= ~TIMER_A_CTL_MC_MASK;

    // Set Mode Control to UP and Clear the current count
    BUZZER_TIMER->CTL |= TIMER_A_CTL_MC__UP | TIMER_A_CTL_CLR;
}

/*****************************************************
 * Turns the Buzzer off
 *
 * Parameters
 *      None
 * Returns
 *      None
 *****************************************************/
void buzzer_off(void)
{
    // Turn off the timer
    BUZZER_TIMER->CTL &= ~TIMER_A_CTL_MC_MASK;
}
/*****************************************************
 * resets the buzzer for the next sound
 * Parameters
 *         tick_period -- buzzer timer period
 * Returns non
 ***************************************************/

void buzzer_reinit(uint16_t ticks_period)
{
    // zero timer
    BUZZER_TIMER->CTL = 0;

    // Set the period of the timer.
    BUZZER_TIMER->CCR[0] = ticks_period - 1;

    // Configure BUZZER Duty Cycle to 50%
    BUZZER_TIMER->CCR[BUZZER_CHANNEL] = (ticks_period /2 ) -1;

    // Configure buzzer pin for RESET/SET Mode
    BUZZER_TIMER->CCTL[BUZZER_CHANNEL] = TIMER_A_CCTLN_OUTMOD_7;

    // Select the master clock as the timer source
    BUZZER_TIMER->CTL = TIMER_A_CTL_SSEL__SMCLK;

    // Set Mode Control to UP and Clear the current count
    BUZZER_TIMER->CTL |= TIMER_A_CTL_MC__UP | TIMER_A_CTL_CLR;
}




