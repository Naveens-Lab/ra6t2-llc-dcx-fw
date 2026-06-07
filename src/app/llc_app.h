/***********************************************************************************************************************//**
 * @file    llc_app.h
 * @brief   Top-level application: owns the state machine and ties the protection, PWM,
 *          soft-start, and supervision modules together.
 *
 * Called from hal_entry(): llc_app_init() once, then llc_app_run() in a forever loop.
 * The canonical state variable lives here; ISR modules transition it via the accessors.
 **************************************************************************************************************************/

#ifndef LLC_APP_H
#define LLC_APP_H

#include "llc_state.h"

/***********************************************************************************************************************//**
 * @brief One-time init: arm protection, open ADC, open GPT, prime soft-start, start primary.
 **************************************************************************************************************************/
void llc_app_init(void);

/***********************************************************************************************************************//**
 * @brief Main-loop body. Runs the supervision/fault state machine. Call repeatedly.
 **************************************************************************************************************************/
void llc_app_run(void);

/***********************************************************************************************************************//**
 * @brief Get the current firmware state.
 **************************************************************************************************************************/
llc_state_t llc_app_get_state(void);

/***********************************************************************************************************************//**
 * @brief Set the firmware state for NORMAL transitions (INIT/SOFTSTART/RUNNING).
 *
 * FAULT is sticky and terminal: once faulted this function will not move the state away
 * from FAULT. To enter the fault state use llc_app_raise_fault() so the reason is recorded.
 * @param state  New state.
 **************************************************************************************************************************/
void llc_app_set_state(llc_state_t state);

/***********************************************************************************************************************//**
 * @brief Enter the FAULT state and record the reason (first reason wins).
 *
 * Safe to call from any context (ISR or main loop). Idempotent — repeated calls keep the
 * first reason. Pairs with the sticky FAULT in llc_app_set_state so a late, lower-priority
 * transition can never clobber a fault.
 * @param reason  What tripped.
 **************************************************************************************************************************/
void llc_app_raise_fault(llc_fault_t reason);

/***********************************************************************************************************************//**
 * @brief  Reason captured at the first fault (LLC_FAULT_NONE if not faulted).
 **************************************************************************************************************************/
llc_fault_t llc_app_get_fault_reason(void);

/***********************************************************************************************************************//**
 * @brief Safe halt: disable interrupts and spin forever.
 *
 * Two uses: (1) on a failed FSP return during init (before any switching starts), and
 * (2) the Phase-A fault halt AFTER the gates have been cut. Does not return.
 *   if (FSP_SUCCESS != err) { llc_app_fatal(); }
 **************************************************************************************************************************/
void llc_app_fatal(void);

#endif /* LLC_APP_H */
