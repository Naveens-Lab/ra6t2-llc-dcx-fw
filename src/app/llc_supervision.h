/***********************************************************************************************************************//**
 * @file    llc_supervision.h
 * @brief   Slow ADC supervision (Vbus / Vout / Temp) and OVP/OTP threshold checks.
 *          Current is NOT supervised here — that is the hardware ACMPHS/POEG path.
 *
 * adc_b_callback is fixed by the generated vector table — do not rename.
 **************************************************************************************************************************/

#ifndef LLC_SUPERVISION_H
#define LLC_SUPERVISION_H

#include "hal_data.h"
#include "llc_state.h"

/***********************************************************************************************************************//**
 * @brief Open the ADC_B unit for supervision (calibration deferred to TASK 7).
 **************************************************************************************************************************/
void adc_init(void);

/***********************************************************************************************************************//**
 * @brief Compare the latest supervision results against OVP/OTP thresholds and request a
 *        fault transition if exceeded. Called from the main loop in STATE_RUNNING.
 **************************************************************************************************************************/
void supervision_check(void);

/***********************************************************************************************************************//**
 * @brief Kick off one ADC supervision scan (Scan Group 0). Results arrive via adc_b_callback.
 **************************************************************************************************************************/
void supervision_scan_start(void);

/***********************************************************************************************************************//**
 * @brief  Whether ADC self-calibration has completed.
 * @retval true  calibration done — scans may be started.
 * @retval false calibration still in progress.
 **************************************************************************************************************************/
bool supervision_is_calibrated(void);

/***********************************************************************************************************************//**
 * @brief Read-only access to the latest ADC supervision results.
 * @return Pointer to the shared results struct.
 **************************************************************************************************************************/
const llc_adc_results_t * supervision_get_results(void);

/* FSP callback — fires on Scan Group 0 completion. Name + signature fixed by generated
 * code: the ADC_B module uses the generic adc_callback_args_t (see ra_gen/hal_data.h). */
void adc_b_callback(adc_callback_args_t * p_args);

#endif /* LLC_SUPERVISION_H */
