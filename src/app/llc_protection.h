/***********************************************************************************************************************//**
 * @file    llc_protection.h
 * @brief   Hardware overcurrent-protection chain: DAC12 (threshold) -> ACMPHS (compare)
 *          -> POEG (gate kill). No CPU is in the trip path.
 *
 * Owns the two protection FSP callbacks (poeg_a_callback, acmphs0_callback). Their
 * names are fixed by the generated vector table and must not change.
 **************************************************************************************************************************/

#ifndef LLC_PROTECTION_H
#define LLC_PROTECTION_H

#include "hal_data.h"
#include <stdbool.h>

/***********************************************************************************************************************//**
 * @brief Arm the full OCP hardware chain BEFORE any switching starts.
 *        DAC writes the threshold, ACMPHS starts comparing, POEG arms the kill switch.
 **************************************************************************************************************************/
void protection_init(void);

/***********************************************************************************************************************//**
 * @brief  Latched fault state accessor.
 * @retval true  a protection event has tripped (POEG cut the gates).
 * @retval false no fault latched.
 **************************************************************************************************************************/
bool protection_is_faulted(void);

/***********************************************************************************************************************//**
 * @brief Software-forced gate kill via the POEG group-A output disable.
 *
 * Cuts every GPT output routed to POEG group A (primary + both SR). Used on software
 * faults (OVP/OTP) where the hardware OCP path has not fired. Idempotent if OCP already
 * tripped POEG in hardware.
 **************************************************************************************************************************/
void protection_disable_outputs(void);

/* FSP callbacks — names fixed by generated code, C linkage required. */
void poeg_a_callback(poeg_callback_args_t * p_args);
void acmphs0_callback(comparator_callback_args_t * p_args);

#endif /* LLC_PROTECTION_H */
