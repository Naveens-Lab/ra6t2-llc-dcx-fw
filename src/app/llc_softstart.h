/***********************************************************************************************************************//**
 * @file    llc_softstart.h
 * @brief   One-time soft-start frequency sweep. Owns the GPT0 crest callback that walks
 *          the primary period from PERIOD_START down to PERIOD_FR, then locks and enables SR.
 *
 * gpt0_primary_callback is fixed by the generated vector table — do not rename.
 **************************************************************************************************************************/

#ifndef LLC_SOFTSTART_H
#define LLC_SOFTSTART_H

#include "hal_data.h"

/***********************************************************************************************************************//**
 * @brief Reset the sweep to its starting point (period = PERIOD_START, counters cleared).
 **************************************************************************************************************************/
void softstart_reset(void);

/***********************************************************************************************************************//**
 * @brief  Whether the sweep has reached fr and enabled the SR (i.e. ready to run).
 * @retval true  sweep complete — the app may promote SOFTSTART -> RUNNING.
 * @retval false still sweeping.
 **************************************************************************************************************************/
bool softstart_is_complete(void);

/* FSP callback — fires at the PWM crest; runs the sweep. Name fixed by generated code. */
void gpt0_primary_callback(timer_callback_args_t * p_args);

#endif /* LLC_SOFTSTART_H */
