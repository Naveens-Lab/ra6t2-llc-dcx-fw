/***********************************************************************************************************************//**
 * @file    llc_pwm.h
 * @brief   GPT control abstraction for the primary half-bridge (GPT0) and the two
 *          synchronous-rectifier channels (GPT1 phase A, GPT2 phase B).
 *
 * The PWM is hardware-autonomous once started; firmware only opens the channels,
 * sets the period for the soft-start sweep, and starts/enables outputs.
 **************************************************************************************************************************/

#ifndef LLC_PWM_H
#define LLC_PWM_H

#include <stdint.h>

/***********************************************************************************************************************//**
 * @brief Open all three GPT channels (outputs stay off until start).
 **************************************************************************************************************************/
void pwm_init(void);

/***********************************************************************************************************************//**
 * @brief Set the primary period register (drives switching frequency) and the matching
 *        50%-duty compare value.
 * @param period  Triangle-mode period register value (counts).
 **************************************************************************************************************************/
void pwm_set_primary_period(uint32_t period);

/***********************************************************************************************************************//**
 * @brief Start the primary half-bridge PWM (GPT0).
 **************************************************************************************************************************/
void pwm_start_primary(void);

/***********************************************************************************************************************//**
 * @brief Enable both synchronous-rectifier channels (GPT1, GPT2) after soft-start lock.
 **************************************************************************************************************************/
void pwm_enable_sr(void);

#endif /* LLC_PWM_H */
