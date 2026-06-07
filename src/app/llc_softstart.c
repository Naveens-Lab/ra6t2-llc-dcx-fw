/***********************************************************************************************************************//**
 * @file    llc_softstart.c
 * @brief   One-time soft-start frequency sweep.
 * 
 * The LLC stage starts at a frequency well above resonance (low gain, low output) and
 * sweeps down to fr over ~16 ms. Higher period register = lower frequency = more gain,
 * so the sweep walks the primary period from PERIOD_START up to PERIOD_FR. On reaching
 * fr it enables the synchronous rectifier (phase-aligned, at the crest) and flags
 * completion; the app's main loop performs the SOFTSTART -> RUNNING transition.
 *
 * Driven from the GPT0 crest interrupt (gpt0_primary_callback). This ISR never writes the
 * RUNNING state itself — keeping the promotion in the lowest-priority main loop is what
 * lets a higher-priority fault always win the transition (see llc_app_run).
 **************************************************************************************************************************/

#include "llc_softstart.h"
#include "llc_config.h"
#include "llc_pwm.h"
#include "llc_app.h"

/* Sweep state (modified in GPT0 crest ISR context). */
static volatile uint32_t s_step_counter   = 0U;
static volatile uint32_t s_current_period = PERIOD_START;
static volatile bool     s_complete       = false;

/*--------------------------------------------------------------------------------------------------------------------*/
void softstart_reset(void)
{
    s_step_counter   = 0U;
    s_current_period = PERIOD_START;
    s_complete       = false;

    /* Prime the primary at the (high) start frequency before it is started. */
    pwm_set_primary_period(PERIOD_START);
}

/*--------------------------------------------------------------------------------------------------------------------*/
bool softstart_is_complete(void)
{
    return s_complete;
}

/***********************************************************************************************************************//**
 * @brief GPT0 crest callback (priority 3) — runs the soft-start sweep.
 *
 * Fires once per PWM period at the counter crest. In triangle mode the crest event is
 * delivered as TIMER_EVENT_CYCLE_END (== TIMER_EVENT_CREST). Every SOFTSTART_STEP_CYCLES
 * crests, the primary period is nudged one step toward PERIOD_FR. Once at fr, the SR is
 * enabled (here, so the GTSTR start is aligned to the crest) and completion is flagged;
 * the main loop then promotes to RUNNING. The state guard makes this a no-op afterwards.
 **************************************************************************************************************************/
void gpt0_primary_callback(timer_callback_args_t * p_args)
{
    if (TIMER_EVENT_CYCLE_END != p_args->event)  { return; }
    if (STATE_SOFTSTART != llc_app_get_state())  { return; }

    /* Pace the sweep: act only every SOFTSTART_STEP_CYCLES crests (~0.5 ms). */
    if (++s_step_counter < SOFTSTART_STEP_CYCLES) { return; }
    s_step_counter = 0U;

    if (s_current_period < PERIOD_FR)
    {
        /* Increase period -> lower frequency -> higher gain. */
        s_current_period += PERIOD_STEP;
        if (s_current_period > PERIOD_FR)
        {
            s_current_period = PERIOD_FR;
        }
        pwm_set_primary_period(s_current_period);
    }
    else if (!s_complete)
    {
        /* Locked at fr: bring up the synchronous rectifier phase-aligned to this crest,
         * then flag completion. The main loop owns the SOFTSTART -> RUNNING transition. */
        pwm_enable_sr();
        s_complete = true;
    }
    else
    {
        /* Already complete; waiting for the main loop to promote to RUNNING. */
    }
}
