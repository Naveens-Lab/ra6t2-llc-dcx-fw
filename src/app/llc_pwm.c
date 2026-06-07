/***********************************************************************************************************************//**
 * @file    llc_pwm.c
 * @brief   GPT control abstraction for the primary half-bridge and the SR channels.  [TASK 2]
 *
 * GPT0 (primary) generates complementary GTIOC0A/B with hardware dead-time. GPT1/GPT2
 * generate the SR phase-A/phase-B gates. Once started, the PWM is hardware-autonomous;
 * firmware only opens, sets the period (for the soft-start sweep), and starts outputs.
 **************************************************************************************************************************/

#include "llc_pwm.h"
#include "llc_config.h"
#include "llc_app.h"
#include "hal_data.h"

/* GPT channel numbers (also the GTSTR/GTSTP bit positions: bit n = channel n). */
#define GPT_CH_PRIMARY          (0U)    /*!< GPT0 — primary half-bridge */
#define GPT_CH_SR_A             (1U)    /*!< GPT1 — SR phase A          */
#define GPT_CH_SR_B             (2U)    /*!< GPT2 — SR phase B          */

/***********************************************************************************************************************//**
 * @brief 50%-duty compare value for a given triangle-mode period register.
 *
 * In triangle symmetric PWM the full PWM period spans 2x the period register, and the
 * compare (GTCCR) maps directly to duty: duty = 1 - compare/period. So 50% duty needs
 * compare = period/2. (The FSP configurator confirms this: period 75 -> duty 37.)
 * NOTE: compare = period would force the 0%/100% path and jam the gates.
 **************************************************************************************************************************/
#define PWM_DUTY_50PCT(period_)   ((period_) / 2U)

/*--------------------------------------------------------------------------------------------------------------------*/
void pwm_init(void)
{
    fsp_err_t err;

    err = R_GPT_Open(&g_gpt_primary_ctrl, &g_gpt_primary_cfg);
    if (FSP_SUCCESS != err) { llc_app_fatal(); }

    err = R_GPT_Open(&g_gpt_sr_a_ctrl, &g_gpt_sr_a_cfg);
    if (FSP_SUCCESS != err) { llc_app_fatal(); }

    err = R_GPT_Open(&g_gpt_sr_b_ctrl, &g_gpt_sr_b_cfg);
    if (FSP_SUCCESS != err) { llc_app_fatal(); }
}

/*--------------------------------------------------------------------------------------------------------------------*/
void pwm_set_primary_period(uint32_t period)
{
    /* Both writes target buffer registers (GTPBR / GTCCR buffers) and load at the next
     * cycle boundary, so the soft-start sweep updates glitch-free. GTIOC0B follows A
     * complementarily with hardware dead-time, so only the A duty is set here.
     *
     * Called from the soft-start ISR with always-valid arguments; the returns are not
     * checked on this hot path (period/2 is always <= GTPBR+1, so DutyCycleSet cannot
     * reject it). */
    (void) R_GPT_PeriodSet(&g_gpt_primary_ctrl, period);
    (void) R_GPT_DutyCycleSet(&g_gpt_primary_ctrl, PWM_DUTY_50PCT(period), GPT_IO_PIN_GTIOCA);
}

/*--------------------------------------------------------------------------------------------------------------------*/
void pwm_start_primary(void)
{
    fsp_err_t err = R_GPT_Start(&g_gpt_primary_ctrl);
    if (FSP_SUCCESS != err) { llc_app_fatal(); }
}

/*--------------------------------------------------------------------------------------------------------------------*/
void pwm_enable_sr(void)
{
    /* exact SR phase lock: start both SR channels in a SINGLE write to the GPT
     * common software-start register (GTSTR), so GPT1 and GPT2 begin counting on the very
     * same cycle. Sequential R_GPT_Start calls would offset the two phases by the few
     * cycles between calls.
     *
     * GTSTR is start-only (stop is via GTSTP), so the 0 in the primary's bit does not stop
     * the running primary. Direct register write is safe here. */
    R_GPT0->GTSTR = (1U << GPT_CH_SR_A) | (1U << GPT_CH_SR_B);
}
