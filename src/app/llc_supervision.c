/***********************************************************************************************************************//**
 * @file    llc_supervision.c
 * @brief   Slow ADC supervision (Vbus / Vout / Temp) and OVP/OTP checks.
 *
 * Current is NOT supervised here — that is the hardware ACMPHS/POEG path (llc_protection).
 * The ADC only watches the slow rails. adc_b_callback latches the latest scan results;
 * supervision_check (called from the main loop in RUNNING) compares them to thresholds.
 *
 * Calibration runs asynchronously at init and completes via the CALEND ISR
 * (ADC_EVENT_CALIBRATION_COMPLETE); a scan-config step is required so scans/reads work.
 **************************************************************************************************************************/

#include "llc_supervision.h"
#include "llc_config.h"
#include "llc_app.h"

/* Latest supervision results (written in adc_b_callback, read via accessor / check). */
static volatile llc_adc_results_t s_results = {0U, 0U, 0U};

/* Set true once ADC self-calibration completes (CALEND). Scans are gated on this. */
static volatile bool s_calibrated = false;

/*--------------------------------------------------------------------------------------------------------------------*/
void adc_init(void)
{
    fsp_err_t err;

    err = R_ADC_B_Open(&g_adc_b_ctrl, &g_adc_b_cfg);
    if (FSP_SUCCESS != err) { llc_app_fatal(); }

    /* ScanCfg sets the driver's "initialized" flag; without it ScanStart and Read both
     * return FSP_ERR_NOT_INITIALIZED. (The Phase-A draft omitted this.) */
    err = R_ADC_B_ScanCfg(&g_adc_b_ctrl, &g_adc_b_scan_cfg);
    if (FSP_SUCCESS != err) { llc_app_fatal(); }

    /* Kick off self-calibration. It runs asynchronously (force-stops the converter) and
     * signals completion via adc_b_callback / ADC_EVENT_CALIBRATION_COMPLETE. We do NOT
     * block here: calibration overlaps the ~16 ms soft-start sweep, and the main loop
     * gates the first scan on supervision_is_calibrated(). */
    s_calibrated = false;
    err = R_ADC_B_Calibrate(&g_adc_b_ctrl, NULL);
    if (FSP_SUCCESS != err) { llc_app_fatal(); }
}

/*--------------------------------------------------------------------------------------------------------------------*/
bool supervision_is_calibrated(void)
{
    return s_calibrated;
}

/*--------------------------------------------------------------------------------------------------------------------*/
void supervision_check(void)
{
    /* Software supervision of the slow rails. Over-voltage / over-temperature raise a fault
     * (with reason); the FAULT state entry in llc_app then actively stops the gates — unlike
     * OCP, these events do NOT cut gates in hardware. Thresholds are PLACEHOLDERS until
     * bench calibration (see llc_config.h Phase-B section). */
    if (s_results.vbus > VBUS_OVP_COUNTS) { llc_app_raise_fault(LLC_FAULT_OVP_VBUS); }
    if (s_results.vout > VOUT_OVP_COUNTS) { llc_app_raise_fault(LLC_FAULT_OVP_VOUT); }
    if (s_results.temp > TEMP_OTP_COUNTS) { llc_app_raise_fault(LLC_FAULT_OTP); }
}

/*--------------------------------------------------------------------------------------------------------------------*/
void supervision_scan_start(void)
{
    (void) R_ADC_B_ScanStart(&g_adc_b_ctrl);
}

/*--------------------------------------------------------------------------------------------------------------------*/
const llc_adc_results_t * supervision_get_results(void)
{
    return (const llc_adc_results_t *) &s_results;
}

/***********************************************************************************************************************//**
 * @brief ADC_B callback — handles both calibration end (CALEND) and scan end (ADI0).
 *
 * The same callback serves multiple ADC_B interrupts; dispatch on p_args->event:
 *  - ADC_EVENT_CALIBRATION_COMPLETE: self-calibration done -> allow scanning.
 *  - ADC_EVENT_SCAN_COMPLETE:        read the three supervised channels.
 *
 * R_ADC_B_Read indexes by PHYSICAL ADC channel (adc_channel_t -> ADDR[channel]), not by
 * virtual-channel number:
 *   AN001/PA01 -> ADC_CHANNEL_1 -> Vbus
 *   AN002/PA02 -> ADC_CHANNEL_2 -> Vout
 *   AN003/PA03 -> ADC_CHANNEL_3 -> Temp
 * Each read returns a uint16_t directly; a failed read leaves the previous value untouched.
 **************************************************************************************************************************/
void adc_b_callback(adc_callback_args_t * p_args)
{
    uint16_t value;

    switch (p_args->event)
    {
        case ADC_EVENT_CALIBRATION_COMPLETE:
        {
            s_calibrated = true;
            break;
        }

        case ADC_EVENT_SCAN_COMPLETE:
        {
            if (FSP_SUCCESS == R_ADC_B_Read(&g_adc_b_ctrl, ADC_CHANNEL_1, &value))
            {
                s_results.vbus = value;
            }
            if (FSP_SUCCESS == R_ADC_B_Read(&g_adc_b_ctrl, ADC_CHANNEL_2, &value))
            {
                s_results.vout = value;
            }
            if (FSP_SUCCESS == R_ADC_B_Read(&g_adc_b_ctrl, ADC_CHANNEL_3, &value))
            {
                s_results.temp = value;
            }
            break;
        }

        default:
        {
            /* Other ADC_B events (errors, overflow) are not handled in Phase B. */
            break;
        }
    }
}
