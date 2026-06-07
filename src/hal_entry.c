/***********************************************************************************************************************//**
 * @file    hal_entry.c
 * @brief   Thin application entry point for the RA6T2 LLC DCX firmware.
 *
 * Called by the BSP after startup. All real work lives in the llc_app module and the
 * FSP callbacks (gpt0_primary_callback, poeg_a_callback, acmphs0_callback, adc_b_callback)
 * in their owning modules under src/app/.
 *
 * Target: R7FA6T2AD3CNE (Cortex-M33, 240 MHz), GPT clk = PCLKD = 120 MHz.
 * See PROJECT_REFERENCE.md and CLAUDE.md for the full design and task plan.
 **************************************************************************************************************************/

#include "hal_data.h"
#include "app/llc_app.h"

/***********************************************************************************************************************//**
 * @brief Application entry point (called by the BSP).
 **************************************************************************************************************************/
void hal_entry(void)
{
    llc_app_init();

    while (1)
    {
        llc_app_run();
    }
}
