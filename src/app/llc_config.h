/***********************************************************************************************************************//**
 * @file    llc_config.h
 * @brief   Single source of truth for every tunable in the LLC DCX firmware.
 *
 * All frequencies, period-register values, soft-start timing, and protection
 * thresholds live here. No other module should define a magic number.
 *
 * Target: R7FA6T2AD3CNE (RA6T2), PCLKD = 120 MHz feeds the GPT.
 **************************************************************************************************************************/

#ifndef LLC_CONFIG_H
#define LLC_CONFIG_H

/*--------------------------------------------------------------------------------------------------------------------*
 * Clocks
 *--------------------------------------------------------------------------------------------------------------------*/
#define GPT_CLK_HZ              (120000000U)    /*!< GPT counter clock = PCLKD                                       */

/*--------------------------------------------------------------------------------------------------------------------*
 * Switching frequencies
 *--------------------------------------------------------------------------------------------------------------------*/
#define FR_HZ                   (800000U)       /*!< Resonant frequency (placeholder — tune to tank on bench)        */
#define FSTART_HZ               (1400000U)      /*!< Soft-start starting frequency (above fr → low gain)             */

/*--------------------------------------------------------------------------------------------------------------------*
 * Triangle-wave symmetric PWM period-register values.
 * For triangle mode: period_reg = GPT_CLK / (2 * f).
 *--------------------------------------------------------------------------------------------------------------------*/
#define PERIOD_FR               (GPT_CLK_HZ / (2U * FR_HZ))      /*!< 75 counts  @ 800 kHz                            */
#define PERIOD_START            (GPT_CLK_HZ / (2U * FSTART_HZ))  /*!< 43 counts  @ 1.4 MHz                            */

/*--------------------------------------------------------------------------------------------------------------------*
 * Soft-start frequency sweep.
 * Step the period register by PERIOD_STEP every SOFTSTART_STEP_CYCLES PWM cycles.
 * At 800 kHz: 400 cycles = 0.5 ms/step; (75 - 43) = 32 steps => ~16 ms total sweep.
 *--------------------------------------------------------------------------------------------------------------------*/
#define SOFTSTART_STEP_CYCLES   (400U)          /*!< PWM cycles between each period increment (~0.5 ms)              */
#define PERIOD_STEP             (1U)            /*!< Period-register increment per step                             */

/*====================================================================================================================*
 * PHASE B — PHYSICAL SENSE-CHAIN PARAMETERS
 *
 *  !!  ALL COMPONENT VALUES BELOW ARE ILLUSTRATIVE PLACEHOLDERS  !!
 *  Replace every *_OHM / *_GAIN / *_TRIP_* value with the real numbers from the board
 *  schematic and bench measurement BEFORE applying power. The math is correct; only the
 *  inputs are unknown here. The DAC12 and the ADC_B are both 12-bit on a 3.3 V reference
 *====================================================================================================================*/
#define ADC_VREF_V              (3.3f)          /*!< AVCC reference (DAC12 + ADC_B)                                  */
#define ADC_FULL_SCALE          (4095.0f)       /*!< 12-bit full scale                                              */

/* Voltage present at an MCU analog pin (or DAC output) -> raw 12-bit counts. Compile-time
 * constant: the float math folds to an integer, no runtime FP. */
#define V_TO_COUNTS_12B(v_)     ((uint16_t)(((v_) / ADC_VREF_V) * ADC_FULL_SCALE))

/* Resistive-divider ratio (voltage at pin / rail voltage) = R_bot / (R_top + R_bot). */
#define DIV_RATIO(rtop_, rbot_) ((rbot_) / ((rtop_) + (rbot_)))

/*------ Vbus sense (PA01 / AN001), high-voltage divider off the 800 V bus ------*/
#define VBUS_R_TOP_OHM          (2000000.0f)    /*!< high-side                       PLACEHOLDER                     */
#define VBUS_R_BOT_OHM          (8200.0f)       /*!< low-side                        PLACEHOLDER                     */
#define VBUS_OVP_RAIL_V         (900.0f)        /*!< trip rail voltage (~800 V bus)  PLACEHOLDER                     */
#define VBUS_OVP_COUNTS         V_TO_COUNTS_12B(VBUS_OVP_RAIL_V * DIV_RATIO(VBUS_R_TOP_OHM, VBUS_R_BOT_OHM))

/*------ Vout sense (PA02 / AN002), divider off the 48 V output ------*/
#define VOUT_R_TOP_OHM          (47000.0f)      /*!< high-side                       PLACEHOLDER                     */
#define VOUT_R_BOT_OHM          (3300.0f)       /*!< low-side                        PLACEHOLDER                     */
#define VOUT_OVP_RAIL_V         (54.0f)         /*!< trip rail voltage (~48 V out)   PLACEHOLDER                     */
#define VOUT_OVP_COUNTS         V_TO_COUNTS_12B(VOUT_OVP_RAIL_V * DIV_RATIO(VOUT_R_TOP_OHM, VOUT_R_BOT_OHM))

/*------ Over-temperature (PA03 / AN003), NTC thermistor ------
 * The NTC response is non-linear, so this is left as a directly bench-measured raw count
 * rather than a closed-form derivation. NOTE: depending on whether the NTC is the top or
 * bottom leg of its divider, the OTP condition may be reading > or < this value — confirm
 * the compare direction in llc_supervision.c against the actual divider topology. */
#define TEMP_OTP_COUNTS         (3200U)         /*!< raw count at max temp           PLACEHOLDER                     */

/*------ Overcurrent (DAC12 -> ACMPHS IVREF) ------
 * Trip voltage at the comparator input = I_trip * R_shunt * amplifier_gain. */
#define OCP_SHUNT_OHM           (0.001f)        /*!< current-sense shunt             PLACEHOLDER                     */
#define OCP_AMP_GAIN            (20.0f)         /*!< sense-amplifier gain (V/V)      PLACEHOLDER                     */
#define OCP_TRIP_CURRENT_A      (40.0f)         /*!< primary OCP trip current        PLACEHOLDER                     */
#define DAC_OCP_THRESHOLD       V_TO_COUNTS_12B(OCP_TRIP_CURRENT_A * OCP_SHUNT_OHM * OCP_AMP_GAIN)

#endif /* LLC_CONFIG_H */
