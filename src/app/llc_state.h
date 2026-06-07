/***********************************************************************************************************************//**
 * @file    llc_state.h
 * @brief   Shared types for the LLC DCX firmware: the top-level state enum and the
 *          ADC supervision result struct.
 *
 * The state variable itself is owned by llc_app (see llc_app_get_state /
 * llc_app_set_state) so there is a single writer of the canonical state. ISR
 * modules transition state through those accessors.
 **************************************************************************************************************************/

#ifndef LLC_STATE_H
#define LLC_STATE_H

#include <stdint.h>
#include <stdbool.h>

/*--------------------------------------------------------------------------------------------------------------------*
 * Top-level firmware state machine.
 *  IDLE      -> power-on, nothing armed
 *  INIT      -> peripherals being opened/armed
 *  SOFTSTART -> primary running, frequency sweeping down toward fr
 *  RUNNING   -> locked at fr, SR enabled, supervision active
 *  FAULT     -> POEG has cut all gates in hardware; firmware halted/latched
 *--------------------------------------------------------------------------------------------------------------------*/
typedef enum
{
    STATE_IDLE = 0,
    STATE_INIT,
    STATE_SOFTSTART,
    STATE_RUNNING,
    STATE_FAULT
} llc_state_t;

/*--------------------------------------------------------------------------------------------------------------------*
 * Fault reason — captured at the first fault so a post-mortem (debugger / future telemetry)
 * can tell what tripped. OCP is the hardware path (ACMPHS->POEG); the rest are software
 * supervision trips.
 *--------------------------------------------------------------------------------------------------------------------*/
typedef enum
{
    LLC_FAULT_NONE = 0,
    LLC_FAULT_OCP,          /*!< overcurrent — ACMPHS comparator -> POEG (hardware)         */
    LLC_FAULT_OVP_VBUS,     /*!< Vbus over-voltage (ADC supervision)                        */
    LLC_FAULT_OVP_VOUT,     /*!< Vout over-voltage (ADC supervision)                        */
    LLC_FAULT_OTP           /*!< over-temperature (ADC supervision)                         */
} llc_fault_t;

/*--------------------------------------------------------------------------------------------------------------------*
 * ADC supervision results (raw ADC counts). Populated by adc_b_callback().
 *  vbus -> AN001 / PA01 (800 V divider)
 *  vout -> AN002 / PA02 (48 V divider)
 *  temp -> AN003 / PA03 (NTC thermistor)
 *--------------------------------------------------------------------------------------------------------------------*/
typedef struct
{
    uint16_t vbus;
    uint16_t vout;
    uint16_t temp;
} llc_adc_results_t;

#endif /* LLC_STATE_H */
