/* generated HAL header file - do not edit */
#ifndef HAL_DATA_H_
#define HAL_DATA_H_
#include <stdint.h>
#include "bsp_api.h"
#include "common_data.h"
#include "r_adc_b.h"
#include "r_adc_api.h"
#include "r_acmphs.h"
#include "r_comparator_api.h"
#include "r_dac.h"
#include "r_dac_api.h"
#include "r_poeg.h"
#include "r_poeg_api.h"
#include "r_gpt.h"
#include "r_timer_api.h"
FSP_HEADER
/** ADC on ADC_B instance. */
extern const adc_instance_t g_adc_b;

/** Access the ADC_B instance using these structures when calling API functions directly (::p_api is not used). */
extern adc_b_instance_ctrl_t g_adc_b_ctrl;
extern const adc_cfg_t g_adc_b_cfg;
extern const adc_b_scan_cfg_t g_adc_b_scan_cfg;

#ifndef adc_b_callback
void adc_b_callback(adc_callback_args_t *p_args);
#endif
/** Comparator Instance. */
extern const comparator_instance_t g_acmphs0;

/** Access the Comparator instance using these structures when calling API functions directly (::p_api is not used). */
extern acmphs_instance_ctrl_t g_acmphs0_ctrl;
extern const comparator_cfg_t g_acmphs0_cfg;

#ifndef acmphs0_callback
void acmphs0_callback(comparator_callback_args_t *p_args);
#endif
/** DAC on DAC Instance. */
extern const dac_instance_t g_dac0;

/** Access the DAC instance using these structures when calling API functions directly (::p_api is not used). */
extern dac_instance_ctrl_t g_dac0_ctrl;
extern const dac_cfg_t g_dac0_cfg;
/** POEG Instance. */
extern const poeg_instance_t g_poeg_a;

/** Access the POEG instance using these structures when calling API functions directly (::p_api is not used). */
extern poeg_instance_ctrl_t g_poeg_a_ctrl;
extern const poeg_cfg_t g_poeg_a_cfg;

#ifndef poeg_a_callback
void poeg_a_callback(poeg_callback_args_t *p_args);
#endif
/** Timer on GPT Instance. */
extern const timer_instance_t g_gpt_sr_b;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t g_gpt_sr_b_ctrl;
extern const timer_cfg_t g_gpt_sr_b_cfg;

#ifndef NULL
void NULL(timer_callback_args_t *p_args);
#endif
/** Timer on GPT Instance. */
extern const timer_instance_t g_gpt_sr_a;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t g_gpt_sr_a_ctrl;
extern const timer_cfg_t g_gpt_sr_a_cfg;

#ifndef NULL
void NULL(timer_callback_args_t *p_args);
#endif
/** Timer on GPT Instance. */
extern const timer_instance_t g_gpt_primary;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t g_gpt_primary_ctrl;
extern const timer_cfg_t g_gpt_primary_cfg;

#ifndef gpt0_primary_callback
void gpt0_primary_callback(timer_callback_args_t *p_args);
#endif
void hal_entry(void);
void g_hal_init(void);
FSP_FOOTER
#endif /* HAL_DATA_H_ */
