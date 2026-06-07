/***********************************************************************************************************************//**
 * @file    llc_app.c
 * @brief   Application state machine + module orchestration.
 *
 * Owns the canonical firmware state and the fault reason. ISRs only ever RAISE faults
 * (high priority, terminal); the SOFTSTART->RUNNING promotion is done here in the main
 * loop (lowest priority) so a fault can never be clobbered by a late RUNNING transition.
 **************************************************************************************************************************/

#include "llc_app.h"
#include "llc_protection.h"
#include "llc_pwm.h"
#include "llc_softstart.h"
#include "llc_supervision.h"
#include "hal_data.h"

/* Canonical firmware state + fault reason. Faults may be raised from ISR context. */
static volatile llc_state_t s_state        = STATE_IDLE;
static volatile llc_fault_t s_fault_reason = LLC_FAULT_NONE;

/* ~1 ms supervision cadence in RUNNING. */
#define APP_SUPERVISION_PERIOD_MS   (1U)

static void app_enter_fault(void);

/*--------------------------------------------------------------------------------------------------------------------*/
void llc_app_init(void)
{
    llc_app_set_state(STATE_INIT);

    /* Order matters: arm the OCP chain BEFORE any switching, then bring up supervision and
     * the GPT channels (still idle), prime the soft-start period, and only then start the
     * primary. The sweep proceeds from here in gpt0_primary_callback. */
    protection_init();      /* DAC -> ACMPHS -> POEG armed                         */
    adc_init();             /* ADC_B opened + scan configured                      */
    pwm_init();             /* all three GPT channels opened (outputs still off)   */
    softstart_reset();      /* prime primary at PERIOD_START (high freq, low gain) */

    /* State must be SOFTSTART before the primary starts so the first crest callback runs
     * the sweep instead of returning early. */
    llc_app_set_state(STATE_SOFTSTART);
    pwm_start_primary();
}

/*--------------------------------------------------------------------------------------------------------------------*/
void llc_app_run(void)
{
    /* Authoritative hardware-fault check, first thing every iteration. The OCP latch in
     * protection is monotonic and set from a higher-priority ISR; promoting it to a fault
     * here means that even if a state write raced, the system converges to FAULT within one
     * iteration (gates are already cut in hardware regardless). */
    if (protection_is_faulted())
    {
        llc_app_raise_fault(LLC_FAULT_OCP);
    }

    switch (llc_app_get_state())
    {
        case STATE_SOFTSTART:
        {
            /* Promote to RUNNING here (main loop), not in the GPT0 ISR. Faults are only
             * raised by higher-priority ISRs, so doing the transition at the lowest
             * priority guarantees a fault always wins the SOFTSTART -> {RUNNING|FAULT}
             * contest. Re-checked against the fault latch for good measure. */
            if (softstart_is_complete() && !protection_is_faulted())
            {
                llc_app_set_state(STATE_RUNNING);
            }
            break;
        }

        case STATE_RUNNING:
        {
            /* Locked at fr, SR active. Pace slow-rail supervision: evaluate the latest
             * scan results, then kick the next scan (results land via adc_b_callback).
             * Gated on ADC self-calibration completing first (it overlaps soft-start and
             * is normally done well before RUNNING). */
            R_BSP_SoftwareDelay(APP_SUPERVISION_PERIOD_MS, BSP_DELAY_UNITS_MILLISECONDS);
            if (supervision_is_calibrated())
            {
                supervision_check();
                supervision_scan_start();
            }
            break;
        }

        case STATE_FAULT:
        {
            app_enter_fault();          /* cuts gates, then halts — does not return */
            break;
        }

        case STATE_IDLE:
        case STATE_INIT:
        default:
        {
            /* Nothing to do in the loop: the soft-start sweep runs in the GPT0 ISR. */
            break;
        }
    }
}

/***********************************************************************************************************************//**
 * @brief Fault entry: force all gates off, then halt safely.
 *
 * Hardware OCP (ACMPHS->POEG) has already cut the gates by the time we get here; for a
 * software fault (OVP/OTP from supervision_check) this is the active gate kill. Phase A
 * then halts — a restart policy is a later phase.
 **************************************************************************************************************************/
static void app_enter_fault(void)
{
    protection_disable_outputs();
    llc_app_fatal();
}

/*--------------------------------------------------------------------------------------------------------------------*/
llc_state_t llc_app_get_state(void)
{
    return s_state;
}

/*--------------------------------------------------------------------------------------------------------------------*/
void llc_app_set_state(llc_state_t state)
{
    /* FAULT is terminal — never transition away from it. */
    if (STATE_FAULT != s_state)
    {
        s_state = state;
    }
}

/*--------------------------------------------------------------------------------------------------------------------*/
void llc_app_raise_fault(llc_fault_t reason)
{
    /* Capture the first reason only, then latch the state. Order matters: record the reason
     * before flipping the state so a reader that observes FAULT also sees a valid reason. */
    if (STATE_FAULT != s_state)
    {
        s_fault_reason = reason;
    }
    s_state = STATE_FAULT;
}

/*--------------------------------------------------------------------------------------------------------------------*/
llc_fault_t llc_app_get_fault_reason(void)
{
    return s_fault_reason;
}

/*--------------------------------------------------------------------------------------------------------------------*/
void llc_app_fatal(void)
{
    __disable_irq();
    while (1)
    {
        __NOP();
    }
}
