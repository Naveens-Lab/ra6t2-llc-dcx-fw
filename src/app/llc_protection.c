/***********************************************************************************************************************//**
 * @file    llc_protection.c
 * @brief   Hardware overcurrent-protection chain and fault callbacks.  [TASK 1]
 *
 * Arms, in order, the three peripherals that form the CPU-less OCP path:
 *   1. DAC12   - writes the trip threshold (routed internally to ACMPHS IVREF).
 *   2. ACMPHS  - compares current-sense (IVCMP) against the DAC reference.
 *   3. POEG    - on an ACMPHS event, cuts every GPT output in hardware.
 *
 * The comparator output rising edge triggers POEG directly; the CPU is not in the
 * trip path. The two callbacks here are notification/latching only.
 **************************************************************************************************************************/

#include "llc_protection.h"
#include "llc_config.h"
#include "llc_app.h"

/* Latched on any protection event; read via protection_is_faulted(). */
static volatile bool s_fault_latched = false;

/*--------------------------------------------------------------------------------------------------------------------*/
void protection_init(void)
{
    fsp_err_t          err;
    comparator_info_t  info;

    /* 1. DAC: write the trip threshold and start output BEFORE the comparator opens,
     *    so ACMPHS always has a valid reference on IVREF. */
    err = R_DAC_Open(&g_dac0_ctrl, &g_dac0_cfg);
    if (FSP_SUCCESS != err) { llc_app_fatal(); }

    err = R_DAC_Write(&g_dac0_ctrl, DAC_OCP_THRESHOLD);
    if (FSP_SUCCESS != err) { llc_app_fatal(); }

    err = R_DAC_Start(&g_dac0_ctrl);
    if (FSP_SUCCESS != err) { llc_app_fatal(); }

    /* 2. ACMPHS: open, wait the hardware-specified stabilization time, then enable the
     *    output. Skipping the wait can produce false trips or missed events on enable. */
    err = R_ACMPHS_Open(&g_acmphs0_ctrl, &g_acmphs0_cfg);
    if (FSP_SUCCESS != err) { llc_app_fatal(); }

    err = R_ACMPHS_InfoGet(&g_acmphs0_ctrl, &info);
    if (FSP_SUCCESS != err) { llc_app_fatal(); }

    R_BSP_SoftwareDelay(info.min_stabilization_wait_us, BSP_DELAY_UNITS_MICROSECONDS);

    err = R_ACMPHS_OutputEnable(&g_acmphs0_ctrl);
    if (FSP_SUCCESS != err) { llc_app_fatal(); }

    /* 3. POEG: arm the hardware kill switch (trigger source = ACMPHS0, set in FSP). */
    err = R_POEG_Open(&g_poeg_a_ctrl, &g_poeg_a_cfg);
    if (FSP_SUCCESS != err) { llc_app_fatal(); }
}

/*--------------------------------------------------------------------------------------------------------------------*/
bool protection_is_faulted(void)
{
    return s_fault_latched;
}

/*--------------------------------------------------------------------------------------------------------------------*/
void protection_disable_outputs(void)
{
    (void) R_POEG_OutputDisable(&g_poeg_a_ctrl);
}

/***********************************************************************************************************************//**
 * @brief POEG event callback (priority 1, highest).
 *
 * All GPT outputs are already cut by POEG hardware before this runs. This latches the
 * fault and drives the state machine to FAULT; no gate action is needed here.
 **************************************************************************************************************************/
void poeg_a_callback(poeg_callback_args_t * p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);

    s_fault_latched = true;
    llc_app_raise_fault(LLC_FAULT_OCP);
}

/***********************************************************************************************************************//**
 * @brief ACMPHS comparator callback (priority 2).
 *
 * POEG (priority 1) fires first and owns the state transition; this is a secondary
 * confirmation of the comparator event.
 **************************************************************************************************************************/
void acmphs0_callback(comparator_callback_args_t * p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);

    s_fault_latched = true;
}
