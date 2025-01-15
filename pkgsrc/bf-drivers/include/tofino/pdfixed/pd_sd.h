/*******************************************************************************
 *  Copyright (C) 2024 Intel Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing,
 *  software distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions
 *  and limitations under the License.
 *
 *
 *  SPDX-License-Identifier: Apache-2.0
 ******************************************************************************/


#ifndef _TOFINO_PDFIXED_PD_SD_H
#define _TOFINO_PDFIXED_PD_SD_H

/**
 * @file pd_sd.h
 *
 * @brief PD Fixed APIs for SerDes management.
 *
 */

#include <tofino/pdfixed/pd_common.h>
#include <dvm/bf_drv_intf.h>
#include <port_mgr/bf_port_if.h>
#include <port_mgr/bf_serdes_if.h>

typedef struct rx_eq_status_t {
  int status;
  bool cal_done;    /**< 1: Completed. 0: In progress                   */
  int cal_good;     /**< 1: Good Eye. 0: Poor Eye. -1: Bad Eye          */
  int cal_eye;      /**< (mVppd) Eye height observed by cal algorithm   */
  int ctle_dc;      /**< DC boost (0..255) */
  int ctle_lf;      /**< Peaking filter low frequency gain (0..15) */
  int ctle_hf;      /**< Peaking filter high frequency gain (0..15) */
  int ctle_bw;      /**< Peaking filter peakign frequency (0..15) */
  int dfe_taps[16]; /**< Array of DFE Taps (each -127..127) */
} rx_eq_status_t;

/** @brief Set SerDes Management Interface Clock Source
 *
 * Set the clock source for the management interface.
 *
 * The clock source can be either the 156.25MHz ETH_REFCLK or the
 * 100MHz PCIE_REFCLK.
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port     : Port identifier
 * @param[in]  lane     : Logical lane within port (0..3, mode dependent)
 *
 * @param[in]  clk_src  : Clock source selector
 *
 * @return Status of the API call
 *
 */
p4_pd_status_t p4_pd_sd_mgmt_clksel_set(bf_dev_id_t dev_id,
                                        bf_dev_port_t dev_port,
                                        int lane,
                                        int clk_sel);

/** @brief Get SerDes Management Interface Clock Source
 *
 * Get the clock source for the management interface.
 *
 * The clock source can be either the 156.25MHz ETH_REFCLK or the
 * 100MHz PCIE_REFCLK.
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port     : Port identifier
 * @param[in]  lane     : Logical lane within port (0..3, mode dependent)
 *
 * @param[out]  clk_src  : Clock source selector
 *
 * @return Status of the API call
 *
 */
p4_pd_status_t p4_pd_sd_mgmt_clksel_get(bf_dev_id_t dev_id,
                                        bf_dev_port_t dev_port,
                                        int lane,
                                        int *clk_sel);

/** @brief Set SerDes Management Bus Acess Method
 *
 * SerDes can be accessed either through SerDes Bus (SBUS) or through the
 * switch core.
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  method   : BF_SDS_ACCESS_SBUS or BF_SDS_ACCESS_CORE
 *
 * @return Status of the API call
 *
 */
p4_pd_status_t p4_pd_sd_mgmt_access_method_set(bf_dev_id_t dev_id, int method);

/** @brief Get SerDes Management Bus Acess Method
 *
 * SerDes can be accessed either through SerDes Bus (SBUS) or through the
 * switch core.
 *
 * @param[in]  dev_id   : Device identifier
 * @param[out] method  : BF_SDS_ACCESS_SBUS or BF_SDS_ACCESS_CORE
 *
 * @return Status of the API call
 *
 */
p4_pd_status_t p4_pd_sd_mgmt_access_method_get(bf_dev_id_t dev_id, int *method);

/** @brief Set SerDes Management Interface Broadcast Mode
 *
 * Set SerDes management interface broadcast mode.  If enabled, the specified
 * SerDes lane will respond to broadcast write commands.
 *
 * Broadcast mode is only availabe if bus access is set to SerDes Bus
 * (BF_SDS_ACCESS_SBUS)
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port     : Port identifier
 * @param[in]  lane     : Logical lane within port (0..3, mode dependent)
 *
 * @param[in]  bcast_en : 1: Broadcast Enable. 0: Broadcast Disable
 *
 * @return Status of the API call
 *
 */
p4_pd_status_t p4_pd_sd_mgmt_bcast_set(
    bf_dev_id_t dev_id, bf_dev_port_t dev_port, int lane, bool tx_dir, bool en);

/** @brief Get SerDes Management Interface Broadcast Mode
 *
 * Get SerDes management interface broadcast mode.  If enabled, the specified
 * SerDes lane will respond to broadcast write commands.
 *
 * Broadcast mode is only availabe if bus access is set to SerDes Bus
 * (BF_SDS_ACCESS_SBUS)
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port     : Port identifier
 * @param[in]  lane     : Logical lane within port (0..3, mode dependent)
 *
 * @param[out] bcast_en : 1: Broadcast Enable. 0: Broadcast Disable
 *
 * @return Status of the API call
 *
 */
p4_pd_status_t p4_pd_sd_mgmt_bcast_get(bf_dev_id_t dev_id,
                                       bf_dev_port_t dev_port,
                                       int lane,
                                       bool tx_dir,
                                       bool *en);

/* This is an internal / debug only function. */
p4_pd_status_t p4_pd_sd_mgmt_reg_set(bf_dev_id_t dev_id,
                                     bf_dev_port_t dev_port,
                                     int lane,
                                     bool tx_dir,
                                     int reg,
                                     int data);

/* This is an internal / debug only function. */
p4_pd_status_t p4_pd_sd_mgmt_reg_get(bf_dev_id_t dev_id,
                                     bf_dev_port_t dev_port,
                                     int lane,
                                     bool tx_dir,
                                     int reg,
                                     int *data);

/* This is an internal / debug only function. */
p4_pd_status_t p4_pd_sd_mgmt_uc_int(bf_dev_id_t dev_id,
                                    bf_dev_port_t dev_port,
                                    int lane,
                                    bool tx_dir,
                                    int interrupt,
                                    int int_data,
                                    int *rtn_data);

/** @brief Set Transmit and Receive lane mapping in a port
 *
 * Configures the port (quad lane) based lane mapper to map physical lanes
 * to logical lanes.
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port     : Port identifier
 * @param[in]  tx_lane[4]   : TX physical lane # (0-3/7) for logical lane
 *0-3/7
 * @param[in]  rx_lane[4]   : RX physical lane # (0-3/7) for logical lane
 *0-3/7
 *
 * @return Status of the API call
 *
 */
p4_pd_status_t p4_pd_sd_port_lane_map_set(bf_dev_id_t dev_id,
                                          bf_dev_port_t dev_port,
                                          int tx[4],
                                          int rx[4]);

/** @brief Get Transmit and Receive lane mapping in a port
 *
 * Gets the port (quad lane) based lane mapper config.
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port     : Port identifier
 * @param[out] tx_lane[4]   : TX physical lane # (0-3) for logical lane 0-3
 * @param[out] rx_lane[4]   : RX physical lane # (0-3) for logical lane 0-3
 *
 * @return Status of the API call
 *
 */
p4_pd_status_t p4_pd_sd_port_lane_map_get(bf_dev_id_t dev_id,
                                          bf_dev_port_t dev_port,
                                          int tx[4],
                                          int rx[4]);

/** @brief Set RX EQ Periodic Calibration Round Robin Limit
 *
 * Sets the number of lanes across the entire device that will run RX EQ
 * fine tuning concurrently.
 *
 * @param[in]  dev_id   : Device identifier
 *
 * @param[in]  fine_tune_lane_cnt : Number of lanes running fine tuning (2..32)
 * for the whole device. Use even numbers only.
 * If odd number, -1 will be applied first.
 * If out of range, default of 8 will be used.
 *
 * @return Status of the API call
 *
 */
p4_pd_status_t p4_pd_sd_dev_rx_eq_cal_rr_set(bf_dev_id_t dev_id,
                                             int fine_tune_lane_cnt);

/** @brief Get RX EQ Periodic Calibration Round Robin Limit
 *
 * Gets the number of lanes across the entire device that will run RX EQ
 * fine tuning concurrently.
 *
 * @param[in]  dev_id   : Device identifier
 *
 * @param[out] fine_tune_lane_cnt : Number of lanes running fine tuning (2..32)
 * for the whole device.
 *
 * @return Status of the API call
 *
 */
p4_pd_status_t p4_pd_sd_dev_rx_eq_cal_rr_get(bf_dev_id_t dev_id,
                                             int *fine_tune_lane_cnt);

/** @brief Set TX PLL REFCLK Source
 *
 *  Sets the reference clock source for the TX PLL.
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port     : Port identifier
 * @param[in]  lane         : Logical lane within port (0..3, mode dependent)
 *
 * @param[in]  clk_source   : TX PLL clock source
 *
 * @return Status of the API call
 *
 */
p4_pd_status_t p4_pd_sd_tx_pll_clksel_set(bf_dev_id_t dev_id,
                                          bf_dev_port_t dev_port,
                                          int lane,
                                          int clk_source);

/** @brief Get TX PLL REFCLK Source
 *
 *  Gets the reference clock source for the TX PLL.
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port     : Port identifier
 * @param[in]  lane         : Logical lane within port (0..3, mode dependent)
 *
 * @param[out] clk_source   : TX PLL clock source
 *
 * @return Status of the API call
 *
 */
p4_pd_status_t p4_pd_sd_tx_pll_clksel_get(bf_dev_id_t dev_id,
                                          bf_dev_port_t dev_port,
                                          int lane,
                                          int *clk_source);

/** @brief Run SerDes Initialization
 *
 * Configures TX and RX sub-blocks inside the SerDes lane to bring the SerDes
 * to normal operation state.
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port     : Port identifier
 * @param[in]  lane         : Logical lane within port (0..3, mode dependent)
 *
 * @param[in]  line_rate    : Line rate for this lane
 * @param[in]  init_rx      : if 1, initialize RX blocks
 * @param[in]  init_tx      : If 1, initialize TX blocks
 * @param[in]  tx_drv_en    : Enable TX driver after initialization
 * @param[in]  tx_phase_cal : TX data clock domain crossing clock phase cal
 * Needs to run once at start up
 *
 * @return Status of the API call
 *
 */
p4_pd_status_t p4_pd_sd_lane_init_run(bf_dev_id_t dev_id,
                                      bf_dev_port_t dev_port,
                                      int lane,
                                      int line_rate,
                                      bool init_rx,
                                      bool init_tx,
                                      bool tx_drv_en,
                                      bool phase_cal);

/** @brief Get TX PLL Locked
 *
 * Checks if TX PLL has successfully calibrated and frequency locked
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port     : Port identifier
 * @param[in]  lane         : Logical lane within port (0..3, mode dependent)
 *
 * @param[out] locked       : 1 if PLL locked.  0 if not locked
 *
 * @return Status of the API call
 *
 */
p4_pd_status_t p4_pd_sd_tx_pll_lock_get(bf_dev_id_t dev_id,
                                        bf_dev_port_t dev_port,
                                        int lane,
                                        bool *locked);

/** @brief Get RX CDR Locked
 *
 * Checks if RX CDR has successfully calibrated and frequency locked
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in]  lane         : Logical lane within port (0..3, mode dependent)
 *
 * @param[out] locked       : 1 if CDR locked.  0 if not locked
 *
 * @return Status of the API call
 *
 */
p4_pd_status_t p4_pd_sd_rx_cdr_lock_get(bf_dev_id_t dev_id,
                                        bf_dev_port_t dev_port,
                                        int lane,
                                        bool *locked);

/** @brief Get TX PLL Status
 *
 * Get detailed TX PLL status information including PLL lock, line rate
 * divider and PLL frequency
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in]  lane         : Logical lane within port (0..3, mode dependent)
 *
 * @param[out] locked       : 1 if PLL locked.  0 if not locked
 * @param[out] div          : PLL feedback divider
 * @param[out] freq         : Frequency in MHz
 *
 * @return Status of the API call
 *
 */
p4_pd_status_t p4_pd_sd_tx_pll_status_get(bf_dev_id_t dev_id,
                                          bf_dev_port_t dev_port,
                                          int lane,
                                          bool *locked,
                                          int *div,
                                          int *freq);

/** @brief Get RX CDR Status
 *
 * Get detailed RX CDR status information including PLL lock, line rate
 * divider and CDR frequency
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in]  lane         : Logical lane within port (0..3, mode dependent)
 *
 * @param[out] locked       : 1 if CDR locked.  0 if not locked
 * @param[out] div          : CDR feedback divider
 * @param[out] freq         : Frequency in MHz
 *
 * @return Status of the API call
 *
 */
p4_pd_status_t p4_pd_sd_rx_cdr_status_get(bf_dev_id_t dev_id,
                                          bf_dev_port_t dev_port,
                                          int lane,
                                          bool *locked,
                                          int *div,
                                          int *freq);

/** @brief Set SerDes Loopback Mode
 *
 * Selects between normal mode (no loopback), TX-to-RX serial (near end)
 * loopback and RX-to-TX parallel (far end) loopback modes.
 *
 * For RX-to-TX parallel loopback, this function will call
 * bf_serdes_tx_pll_clksel_set() to select RX recovered clock (RXCLK)
 * as reference clock.
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in]  lane         : Logical lane within port (0..3, mode dependent)
 *
 * @param[in]  loopback_mode : SerDes loopback mode
 *
 * @return Status of the API call
 *
 * @see bf_serdes_tx_pll_clksel_set(), bf_serdes_tx_pll_clksel_get()
 *
 */
p4_pd_status_t p4_pd_sd_lane_loopback_set(bf_dev_id_t dev_id,
                                          bf_dev_port_t dev_port,
                                          int lane,
                                          int loopback_mode);

/** @brief Get SerDes Loopback Mode
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in]  lane         : Logical lane within port (0..3, mode dependent)
 *
 * @param[out] loopback_mode : SerDes loopback mode
 *
 * @return Status of the API call
 *
 */
p4_pd_status_t p4_pd_sd_lane_loopback_get(bf_dev_id_t dev_id,
                                          bf_dev_port_t dev_port,
                                          int lane,
                                          int *loopback_mode);

/** @brief Set Transmit Path Enable
 *
 * This function sets the TX path enable which also functions as a reset.
 * Some TX settings become effect when TX state goes from Disable to Enable
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in]  lane     : Logical lane within port (0..3, mode dependent)
 *
 * @param[in]  tx_en    : 1: TX path enabled. 0: disabled
 *
 * @return Status of the API call
 *
 */
p4_pd_status_t p4_pd_sd_tx_en_set(bf_dev_id_t dev_id,
                                  bf_dev_port_t dev_port,
                                  int lane,
                                  bool en);

/** @brief Get Transmit Path Enable
 *
 * This function gets the TX path enable which also functions as a reset.
 * Some TX settings become effect when TX state goes from Disable to Enable
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in]  lane     : Logical lane within port (0..3, mode dependent)
 *
 * @param[out] en       : 1: TX path enabled. 0: disabled
 *
 * @return Status of the API call
 *
 */
p4_pd_status_t p4_pd_sd_tx_en_get(bf_dev_id_t dev_id,
                                  bf_dev_port_t dev_port,
                                  int lane,
                                  bool *en);

/** @brief Set Transmit Output Enable
 *
 * This function sets the TX Output enable.  When disabled, output pins
 * P/N will drive to AVDD
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in]  lane     : Logical lane within port (0..3, mode dependent)
 *
 * @param[in]  tx_drv_en    : 1: TX Output enabled.  0: disabled
 *
 * @return Status of the API call
 *
 */
p4_pd_status_t p4_pd_sd_tx_drv_en_set(bf_dev_id_t dev_id,
                                      bf_dev_port_t dev_port,
                                      int lane,
                                      bool en);

/** @brief Get Transmit Output Enable
 *
 * This function gets the TX Output enable.
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in]  lane     : Logical lane within port (0..3, mode dependent)
 *
 * @param[out] en       : 1: TX Output enabled.  0: disabled
 *
 * @return Status of the API call
 *
 */
p4_pd_status_t p4_pd_sd_tx_drv_en_get(bf_dev_id_t dev_id,
                                      bf_dev_port_t dev_port,
                                      int lane,
                                      bool *en);

/** @brief Set Transmit Output Polarity Inversion
 *
 * This function sets the TX output polarity inversion.
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in]  lane     : Logical lane within port (0..3, mode dependent)
 *
 * @param[in]  tx_inv   : 1: invert P/N. 0: no inversion
 *
 * @return Status of the API call
 *
 */
p4_pd_status_t p4_pd_sd_tx_drv_inv_set(bf_dev_id_t dev_id,
                                       bf_dev_port_t dev_port,
                                       int lane,
                                       bool inv);

/** @brief Get Transmit Output Polarity Inversion
 *
 * This function gets the TX output polarity inversion.
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in]  lane     : Logical lane within port (0..3, mode dependent)
 *
 * @param[out] inv      : 1: invert P/N. 0: no inversion
 *
 * @return Status of the API call
 *
 */
p4_pd_status_t p4_pd_sd_tx_drv_inv_get(bf_dev_id_t dev_id,
                                       bf_dev_port_t dev_port,
                                       int lane,
                                       bool *inv);

/** @brief Valid Check for Transmit EQ attenuation setting
 *
 * This funciton checks to see if the specified TX EQ attenuation settings
 * are valid.
 *
 * @param[in]  attn_main    : Main cursor setting (  0 to 23)
 * @param[in]  attn_post    : Post cursor setting (-31 to 31)
 * @param[in]  attn_pre     : Pre  cursor setting (-31 to 31)
 *
 * @return Status of the API call
 *
 */
p4_pd_status_t p4_pd_sd_tx_drv_attn_is_valid(int attn_main,
                                             int attn_post,
                                             int attn_pre);

/** @brief Set Transmit EQ based on Attenuation
 *
 * This function allows users to set transmit EQ attenuation settings directly.
 *
 * Range limits:
 *
 *  - attn_main
 *      - <= 23 for general applications
 *      - <= 16 for KR applications
 *  - attn_pre + attn_main + attn_post <= 32
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in]  lane         : Logical lane within port (0..3, mode dependent)
 *
 * @param[in]  attn_main    : Main cursor setting (  0 to 23)
 * @param[in]  attn_post    : Post cursor setting (-31 to 31)
 * @param[in]  attn_pre     : Pre  cursor setting (-31 to 31)
 *
 * @return Status of the API call
 *
 * @see bf_serdes_tx_drv_amp_set()
 */
p4_pd_status_t p4_pd_sd_tx_drv_attn_set(bf_dev_id_t dev_id,
                                        bf_dev_port_t dev_port,
                                        int lane,
                                        int attn_main,
                                        int attn_post,
                                        int attn_pre);

/** @brief Get Transmit EQ based on Attenuation
 *
 * This function allows users to get transmit EQ attenuation settings directly.
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in]  lane         : Logical lane within port (0..3, mode dependent)
 *
 * @param[out]  attn_main    : Main cursor settoutg (  0 to 23)
 * @param[out]  attn_post    : Post cursor settoutg (-31 to 31)
 * @param[out]  attn_pre     : Pre  cursor settoutg (-31 to 31)
 *
 * @return Status of the API call
 *
 * @see bf_serdes_tx_drv_amp_get()
 *
 */
p4_pd_status_t p4_pd_sd_tx_drv_attn_get(bf_dev_id_t dev_id,
                                        bf_dev_port_t dev_port,
                                        int lane,
                                        int *attn_main,
                                        int *attn_post,
                                        int *attn_pre);

/** @brief Set Transmit EQ based on Output Amplitude
 *
 * This function allows the user to configure output waveform based on
 * amplitude at various UI location relative to the edge transition.
 *
 *  - amp_pre  (amplitude 1  UI before edge transition, 962 to 156 mVppd)
 *  - amp_main (amplitude 1  UI after  edge transition, 962 to 364 mVppd)
 *  - amp_post (amplitude 2+ UI after  edge transition, 962 to 156 mVppd)
 *
 * This function translates amplitude setting into attenuation setting used by
 * the hardware.
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in]  lane     : Logical lane within port (0..3, mode dependent)
 *
 * @param[in]  amp_main : Main-cursor amplitude
 * @param[in]  amp_post : Post-cursor amplitude
 * @param[in]  amp_pre  : Pre-cursor amplitude
 *
 * @return Status of the API call
 *
 * @see bf_serdes_tx_drv_attn_set()
 *
 */
p4_pd_status_t p4_pd_sd_tx_drv_amp_set(bf_dev_id_t dev_id,
                                       bf_dev_port_t dev_port,
                                       int lane,
                                       int amp_main,
                                       int amp_post,
                                       int amp_pre);

/** @brief Get Transmit EQ based on Output Amplitude
 *
 * This functions gets the hardware setting (TX EQ attenuation) and converts
 * them to TX amplitude setting

 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in]  lane     : Logical lane within port (0..3, mode dependent)
 *
 * @param[out] amp_main : Main-cursor amplitude
 * @param[out] amp_post : Post-cursor amplitude
 * @param[out] amp_pre  : Pre-cursor amplitude
 *
 * @return Status of the API call
 *
 * @see bf_serdes_tx_drv_attn_get()
 *
 */
p4_pd_status_t p4_pd_sd_tx_drv_amp_get(bf_dev_id_t dev_id,
                                       bf_dev_port_t dev_port,
                                       int lane,
                                       int *amp_main,
                                       int *amp_post,
                                       int *amp_pre);

/** @brief Get Transmit Driver Status
 *
 * Get TX Driver status including: TX path enable, output enable,
 * TX EQ (by amplitude), output inversion.
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in]  lane     : Logical lane within port (0..3, mode dependent)
 *
 * @param[out] tx_en     : TX status
 * @param[out] tx_drv_en : TX driver status
 * @param[out] tx_inv    : 1: invert P/N. 0: no inversion
 * @param[out] amp_main  : Main-cursor amplitude
 * @param[out] amp_post  : Post-cursor amplitude
 * @param[out] amp_pre   : Pre-cursor amplitude
 *
 * @return Status of the API call
 *
 */
p4_pd_status_t p4_pd_sd_tx_drv_status_get(bf_dev_id_t dev_id,
                                          bf_dev_port_t dev_port,
                                          int lane,
                                          bool *tx_en,
                                          bool *tx_drv_en,
                                          bool *tx_inv,
                                          int *amp_main,
                                          int *amp_post,
                                          int *amp_pre);

/** @brief Set Receive Path Enable
 *
 * This function sets the RX path enable which also functions as a reset.
 * Some RX settings become effect when RX state goes from Disable to Enable
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in]  lane     : Logical lane within port (0..3, mode dependent)
 *
 * @param[in]  rx_en    : 1: RX path enabled. 0: disabled
 *
 * @return Status of the API call
 *
 */
p4_pd_status_t p4_pd_sd_rx_en_set(bf_dev_id_t dev_id,
                                  bf_dev_port_t dev_port,
                                  int lane,
                                  bool en);

/** @brief Get Receive Path Enable
 *
 * This function gets the RX path enable which also functions as a reset.
 * Some RX settings become effect when TX state goes from Disable to Enable
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in]  lane     : Logical lane within port (0..3, mode dependent)
 *
 * @param[out] en       : 1: RX path enabled. 0: disabled
 *
 * @return Status of the API call
 *
 */
p4_pd_status_t p4_pd_sd_rx_en_get(bf_dev_id_t dev_id,
                                  bf_dev_port_t dev_port,
                                  int lane,
                                  bool *en);

/** @brief Set Receive Input Polarity Inversion
 *
 * This function sets the RX input polarity inversion.
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in]  lane     : Logical lane within port (0..3, mode dependent)
 *
 * @param[in]  rx_inv   : 1: invert P/N. 0: no inversion
 *
 * @return Status of the API call
 *
 */
p4_pd_status_t p4_pd_sd_rx_afe_inv_set(bf_dev_id_t dev_id,
                                       bf_dev_port_t dev_port,
                                       int lane,
                                       bool inv);

/** @brief Get Receive Input Polarity Inversion
 *
 * This function gets the RX input polarity inversion.
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in]  lane     : Logical lane within port (0..3, mode dependent)
 *
 * @param[out] inv      : 1: invert P/N. 0: no inversion
 *
 * @return Status of the API call
 *
 */
p4_pd_status_t p4_pd_sd_rx_afe_inv_get(bf_dev_id_t dev_id,
                                       bf_dev_port_t dev_port,
                                       int lane,
                                       bool *inv);

/** @brief Set Receive Input Termination
 *
 * Set RX AFE (analog front end) serial input buffer termination option.
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in]  lane     : Logical lane within port (0..3, mode dependent)
 *
 * @param[in]  rx_term  : Termination option
 *
 * @return Status of the API call
 *
 */
p4_pd_status_t p4_pd_sd_rx_afe_term_set(bf_dev_id_t dev_id,
                                        bf_dev_port_t dev_port,
                                        int lane,
                                        int rx_term);

/** @brief Get Receive Input Termination
 *
 * Get RX AFE (analog front end) serial input buffer termination option.
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in]  lane     : Logical lane within port (0..3, mode dependent)
 *
 * @param[out] rx_term  : Termination option
 *
 * @return Status of the API call
 *
 */
p4_pd_status_t p4_pd_sd_rx_afe_term_get(bf_dev_id_t dev_id,
                                        bf_dev_port_t dev_port,
                                        int lane,
                                        int *rx_term);

/** @brief Set Receive Loss of Signal Threshold
 *
 * Set RX input LOS threshold in DAC steps.  This function uses PCIe signal
 * detect circuit to detect LOS condition.
 *
 * Threshold range is
 * For A0, 0-15
 * For B0, 0-255
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in]  lane     : Logical lane within port (0..3, mode dependent)
 * @param[in]  rx_los_en    : LOS enable
 * @param[in]  rx_los_thres : LOS threshold DAC setting
 *
 * @return Status of the API call
 *
 */
p4_pd_status_t p4_pd_sd_rx_afe_los_thres_set(bf_dev_id_t dev_id,
                                             bf_dev_port_t dev_port,
                                             int lane,
                                             bool rx_los_en,
                                             int rx_los_thres);

/** @brief Get Receive Loss of Signal Threshold
 *
 * Get RX input LOS threshold in DAC steps
 *
 * Threshold range is (for A0):
 *    0-15
 * Threshold range is (for B0):
 *    0-255
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in]  lane     : Logical lane within port (0..3, mode dependent)
 * @param[out] rx_los_en    : LOS enable
 * @param[out] rx_los_thres : LOS threshold in DAC steps
 *
 * @return Status of the API call
 *
 */
p4_pd_status_t p4_pd_sd_rx_afe_los_thres_get(bf_dev_id_t dev_id,
                                             bf_dev_port_t dev_port,
                                             int lane,
                                             bool *rx_los_en,
                                             int *rx_los_thres);

/** @brief Get Signal Detect Status
 *
 * If signal detect circuit is enabled, this function will report if a signal
 * loss condition (average amplitude below threshold) was recorded since the
 * last time this function was called.
 *
 * Upon calling this function, the sticky LOS signal will be reset.
 *
 * *Warning: This is intended as a debug feature as signal detect circuit is
 * not guaranted for non-PCIe Gen1/2 line rates.*
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in]  lane     : Logical lane within port (0..3, mode dependent)
 *
 * @param[out] rx_los   : Loss of signal indicator
 *
 * @return Status of the API call
 *
 */
p4_pd_status_t p4_pd_sd_rx_afe_los_get(bf_dev_id_t dev_id,
                                       bf_dev_port_t dev_port,
                                       int lane,
                                       bool *rx_los);

/** @brief RX EQ Calibration in Progress
 *
 * Check to see if PHY microcontroller (uC) is busy running RX EQ calibration.
 *
 * This function will check up to chk_cnt times with chk_wait (ms) delays in
 * between each check.  If uC busy is detected, the function will return
 * right away without finishing all the chk_cnt.
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in]  lane     : Logical lane within port (0..3, mode dependent)
 *
 * @param[in]  chk_cnt  : Number of times to check
 * @param[in]  chk_wait : (ms) Time to wait between checks
 * @param[out] uc_busy  : 1: microcontroller is busy. 0: not busy
 *
 * @return Status of the API call
 *
 */
p4_pd_status_t p4_pd_sd_rx_eq_cal_busy_get(bf_dev_id_t dev_id,
                                           bf_dev_port_t dev_port,
                                           int lane,
                                           int chk_cnt,
                                           int chk_wait,
                                           bool *uc_busy);

/** @brief Set RX EQ CTLE Fixed Settings
 *
 * Sets fixed RX EQ CTLE settings.
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in]  lane     : Logical lane within port (0..3, mode dependent)
 *
 * @param[in]  ctle_dc : DC boost (0..255)
 * @param[in]  ctle_lf : Peaking filter low frequency gain (0..15)
 * @param[in]  ctle_hf : Peaking filter high frequency gain (0..15)
 * @param[in]  ctle_bw : Peaking filter peaking frequency (0..7)
 *
 * @return Status of the API call
 *
 * @see bf_serdes_rx_eq_mode_set()
 *
 */
p4_pd_status_t p4_pd_sd_rx_eq_ctle_set(bf_dev_id_t dev_id,
                                       bf_dev_port_t dev_port,
                                       int lane,
                                       int ctle_dc,
                                       int ctle_lf,
                                       int ctle_hf,
                                       int ctle_bw);

/** @brief Get RX EQ CTLE Fixed Settings
 *
 * Gets live RX EQ CTLE settings.
 *
 * If this function is called after iCal, the calibrated CTLE results will
 * be returned.
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in]  lane     : Logical lane within port (0..3, mode dependent)
 *
 * @param[out] ctle_dc : DC boost (0..255)
 * @param[out] ctle_lf : Peaking filter low frequency gain (0..15)
 * @param[out] ctle_hf : Peaking filter high frequency gain (0..15)
 * @param[out] ctle_bw : Peaking filter peaking frequency (0..7)
 *
 * @return Status of the API call
 *
 * @see bf_serdes_rx_eq_mode_set()
 *
 */
p4_pd_status_t p4_pd_sd_rx_eq_ctle_get(bf_dev_id_t dev_id,
                                       bf_dev_port_t dev_port,
                                       int lane,
                                       int *ctle_dc,
                                       int *ctle_lf,
                                       int *ctle_hf,
                                       int *ctle_bw);

/** @brief Set Specific RX EQ DFE Tap (Advanced)
 *
 * Set DFE Tap value for a specific tap.
 *
 * This is an advanced function for internal and debug use only.
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in]  lane     : Logical lane within port (0..3, mode dependent)
 *
 * @param[in]  dfe_tap_num : DFE Tap Number (1 or higher)
 * @param[in]  dfe_tap_val : DFE Tap Value (-15..15)
 *
 * @return Status of the API call
 *
 * @see bf_serdes_rx_eq_dfe_set()
 *
 */
p4_pd_status_t p4_pd_sd_rx_eq_dfe_adv_set(bf_dev_id_t dev_id,
                                          bf_dev_port_t dev_port,
                                          int lane,
                                          int tap_num,
                                          int tap_val);

/** @brief Get Specific RX EQ DFE Tap (Advanced)
 *
 * Get DFE Tap value for a specific tap (tap 2 and beyond).
 * This is an advanced function for debug only.
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in]  lane     : Logical lane within port (0..3, mode dependent)
 *
 * @param[in]  tap_num : DFE Tap Number (2 or higher)
 * @param[out] tap_val : DFE Tap Value (-15..15)
 *
 * @return Status of the API call
 *
 * @see bf_serdes_rx_eq_dfe_get()
 *
 */
p4_pd_status_t p4_pd_sd_rx_eq_dfe_adv_get(bf_dev_id_t dev_id,
                                          bf_dev_port_t dev_port,
                                          int lane,
                                          int tap_num,
                                          int *tap_val);

/** @brief Set RX EQ DFE Tap
 *
 * Set DFE Tap values if RX EQ mode is one of FIX_DFE modes.
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in]  lane     : Logical lane within port (0..3, mode dependent)
 *
 * @param[in]  dfe_gain   : DFE Gain (0..15)
 * @param[in]  tap1..4    : DFE Taps 1 to 4 (-127..127). dfe_tap[0]=Tap 1
 *
 * @return Status of the API call
 *
 * @see bf_serdes_rx_eq_mode_set()
 *
 */
p4_pd_status_t p4_pd_sd_rx_eq_dfe_set(bf_dev_id_t dev_id,
                                      bf_dev_port_t dev_port,
                                      int lane,
                                      int dfe_gain,
                                      int tap1,
                                      int tap2,
                                      int tap3,
                                      int tap4);

/** @brief Get RX EQ DFE Tap
 *
 * Get DFE Tap values.
 *
 * If rx_eq_mode is ADP_DFE, this will retrieve adapted value.  If rx_eq_mode
 * is FIX_DFE, this will retrieve the fixed value.
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in]  lane     : Logical lane within port (0..3, mode dependent)
 *
 * @param[out] dfe_gain   : DFE Gain (0..15)
 * @param[out] tap1..4    : DFE Taps 1 to 4 (-127..127). dfe_tap[0]=Tap 1
 *
 * @return Status of the API call
 *
 * @see bf_serdes_rx_eq_mode_set()
 *
 */
p4_pd_status_t p4_pd_sd_rx_eq_dfe_get(bf_dev_id_t dev_id,
                                      bf_dev_port_t dev_port,
                                      int lane,
                                      int *dfe_gain,
                                      int *tap1,
                                      int *tap2,
                                      int *tap3,
                                      int *tap4);

/** @brief Set RX EQ Calibration Parameters (Advanced)
 *
 * Set RX EQ calibration parameters if the default settings do not work well
 * for a given type of channel.
 *
 * This is a debug feature not recommended for general usage.
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in]  lane     : Logical lane within port (0..3, mode dependent)
 *
 * @param[in]  ctle_dc_hint     : Hint for DC boost (0..255)
 * @param[in]  dfe_gain_range   : DFE gain range (bits 7:0)
 * @param[in]  pcal_loop_cnt    : Number of fine adjustment loops to run
 *                                  for each pCal run (1..15)
 *
 * @return Status of the API call
 *
 */
p4_pd_status_t p4_pd_sd_rx_eq_cal_param_set(bf_dev_id_t dev_id,
                                            bf_dev_port_t dev_port,
                                            int lane,
                                            int ctle_dc_hint,
                                            int dfe_gain_range,
                                            int pcal_loop_cnt);

/** @brief Get RX EQ Calibration Parameters (Advanced)
 *
 * Get RX EQ calibration parameters
 *
 * This is a debug feature not recommended for general usage.
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in]  lane     : Logical lane within port (0..3, mode dependent)
 *
 * @param[out] ctle_dc_hint     : Hint for DC boost (0..255)
 * @param[out] dfe_gain_range   : DFE gain range (bits 7:0)
 * @param[out] pcal_loop_cnt    : Number of fine adjustment loops to run
 *                                  for each pCal run (1..15)
 *
 * @return Status of the API call
 *
 */
p4_pd_status_t p4_pd_sd_rx_eq_cal_param_get(bf_dev_id_t dev_id,
                                            bf_dev_port_t dev_port,
                                            int lane,
                                            int *ctle_dc_hint,
                                            int *dfe_gain_range,
                                            int *pcal_loop_cnt);

/** @brief Run RX EQ Calibration (Advanced)
 *
 * Run RX EQ Calibration.  This is the advanced version of
 * bf_serdes_rx_ical_run() and bf_serdes_rx_eq_pcal_run() with more control
 * over the adaptation behavior.
 *
 * This function is intended for internal use.  User may call this function
 * if a standard calibration setting does not work well for user's channel.
 *
 * If CTLE or DFE calibration is bypassed, fixed settings need to be specified
 * prior to calling this function via bf_serdes_rx_eq_ctle_set() and
 * bf_serdes_rx_eq_dfe_set().
 *
 * If special EQ tuning parameters are needed, set up special params using
 * bf_serdes_rx_eq_cal_param_set() prior to calling this function.
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in]  lane     : Logical lane within port (0..3, mode dependent)
 *
 * @param[in]  cal_cmd  : Command to control the calibration routine
 * @param[in]  ctle_cal_cfg : 5 bits [4:0]. Default is 0x00
 *                          bit[0]: 1: Do not tune DC.    0: Tune DC
 *                          bit[1]: 1: Do not tune LF.    0: Tune LF
 *                          bit[2]: 1: Do not tune HF.    0: Tune HF
 *                          bit[3]: 1: Do not tune BW.    0: Tune BW
 *                          bit[4]: 1: Use DC tune hint.  0: Do not use Hint
 * @param[in]  dfe_fixed  : 1: Do not tune DFE.  0: tune DFE
 *
 * @return Status of the API call
 *
 * @see bf_serdes_rx_eq_ctle_set(), bf_serdes_rx_eq_dfe_set()
 * @see bf_serdes_rxeq_cal_param_set()
 * @see bf_serdes_rx_eq_ical_run(), bf_serdes_rx_eq_pcal_run()
 *
 */
p4_pd_status_t p4_pd_sd_rx_eq_cal_adv_run(bf_dev_id_t dev_id,
                                          bf_dev_port_t dev_port,
                                          int lane,
                                          int cal_cmd,
                                          int ctle_cal_cfg,
                                          int dfe_fixed);

/** @brief Get RX EQ Cal Observed Eye Height (Advanced)
 *
 * Gets the eye height observed by the EQ calibration routine following
 * bf_serdes_rx_eq_ical_run().
 *
 * This is an advanced function since the eye reported by adaptation will
 * typically be larger than that reported by 2D eye scan which spends more
 * time accumulating errors at the eye boundary.  So adp_eye cannot be
 * evaluated quantatively.  It is a rough indicator.
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in]  lane     : Logical lane within port (0..3, mode dependent)
 *
 * @param[out] cal_eye  : (mVppd) Eye height observed by cal algorithm
 *
 * @return Status of the API call
 *
 * @see bf_serdes_rx_eq_ical_run()
 *
 */
p4_pd_status_t p4_pd_sd_rx_eq_cal_eye_get(bf_dev_id_t dev_id,
                                          bf_dev_port_t dev_port,
                                          int lane,
                                          int *cal_eye);

/** @brief RX Equalizer Calibration is Done
 *
 * Check to see if RX EQ iCal has completed and if calibration
 * is successful.  cal_good is only relavent if cal_done = 1.
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in]  lane     : Logical lane within port (0..3, mode dependent)
 *
 * @param[in]  cal_good_thres : (mVppd) cal_good=1 if cal_eye is above
 *                              threshold.  125mVppd is recommended.
 * @param[out] cal_done :  1: Completed. 0: In progress
 * @param[out] cal_good :  1: cal_eye above threshold.
 *                         0: cal_eye below threshold.
 *                        -1: cal_eye is < 50mVppd
 * @param[out] cal_eye  : (mVppd) Eye height observed by cal algorithm
 *
 * @return Status of the API call
 *
 * @see bf_serdes_rx_eq_ical_run()
 *
 */
p4_pd_status_t p4_pd_sd_rx_eq_ical_eye_get(bf_dev_id_t dev_id,
                                           bf_dev_port_t dev_port,
                                           int lane,
                                           int cal_good_thres,
                                           int *cal_done,
                                           int *cal_good,
                                           int *cal_eye);

/** @brief Run RX EQ Initial Calibration
 *
 * Start RX EQ initial calibration.  This function performs the following
 * calibrations:
 *  - Slicer offset calibration
 *  - CTLE Calibration
 *  - DFE Calibration
 *
 * bf_serdes_rx_eq_ical_eye_get() can be called to check for completion and
 * if calibration was successful.
 *
 * If the user's channel cannot be calibrated well with standard ical settings,
 * bf_serdes_rx_eq_cal_adv_run() may be used for finer RX EQ cal control.
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in]  lane     : Logical lane within port (0..3, mode dependent)
 *
 * @return Status of the API call
 *
 * @see bf_serdes_rx_eq_ical_eye_get()
 * @see bf_serdes_rx_eq_cal_adv_run()
 *
 */
p4_pd_status_t p4_pd_sd_rx_eq_ical_run(bf_dev_id_t dev_id,
                                       bf_dev_port_t dev_port,
                                       int lane);

/** @brief Start/Stop RX EQ Periodic Calibration
 *
 * Start periodic cal (pCal) or doing a one time pCal, which is equivalent to
 * stopping the pCal.
 *
 * pCal is fine DFE adjustment to adjust for slow changing temperature and
 * voltage conditions.  It will not disrupt traffic.  Prior to running pCal,
 * RX EQ initial calibration (iCal) must first be run to make sure the EQ
 * is tuned for a given channel.
 *
 * There is a device wide round robin mechanism to run pCal for each lane.
 * The round robin mechanism is controlled by bf_dev_rx_eq_cal_rr_set()
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in]  lane     : Logical lane within port (0..3, mode dependent)
 *
 * @param[in]  cal_cont : 1: Continuous pCal  0: One time pCal
 *
 * @return Status of the API call
 *
 * @see bf_serdes_rx_eq_ical_run()
 * @see bf_dev_rx_eq_cal_rr_set()
 *
 */
p4_pd_status_t p4_pd_sd_rx_eq_pcal_run(bf_dev_id_t dev_id,
                                       bf_dev_port_t dev_port,
                                       int lane,
                                       int cal_count);

/** @brief Get RX Equalizer Status
 *
 * Get RX EQ CTLE/DFE settings for a port including:
 *      - RXEQ Mode
 *      - adp_done
 *      - adp_success
 *      - adp_eye
 *      - ctle settings (ctle_dc/lf/hf/bw)
 *      - dfe_taps
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in]  lane     : Logical lane within port (0..3, mode dependent)
 *
 * @param[out] st       : RX EQ status
 *
 * @return Status of the API call
 *
 */
p4_pd_status_t p4_pd_sd_rx_eq_status_get(bf_dev_id_t dev_id,
                                         bf_dev_port_t dev_port,
                                         int lane,
                                         rx_eq_status_t *st);

/** @brief Set RX Comparison Slicer Position (Advanced)
 *
 * This is an advanced debug function to check eye margin.
 * This function sets the RX comparison slicer's x, y position (phase, vertical
 * offset) and enables comparison between data slicer and comparison slicer.
 *
 * To observe error count, call bf_serdes_pat_rx_err_cnt_get() or
 *
 * @note
 * Continuous adaptation for this channel will be disabled to access the
 * comparison slicer while this function is enabled.
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in]  lane     : Logical lane within port (0..3, mode dependent)
 *
 * @param[in]  offset_en : 1: Enable comparison slicer offset.
 *                         0: Disable (Normal Traffic).
 * @param[in]  pos_x     : Horizontal Position ( -32.. 31 phase setting)
 * @param[in]  pos_y     : Vertical Position   (-500..500 mV)
 *
 *
 * @return Status of the API call
 * @see bf_serdes_pat_rx_err_cnt_get()
 */
p4_pd_status_t p4_pd_sd_rx_eye_offset_set(bf_dev_id_t dev_id,
                                          bf_dev_port_t dev_port,
                                          int lane,
                                          int offset_en,
                                          int pos_x,
                                          int pos_y);

/** @brief Measure Vertical or Horizontal Eye Opening
 *
 * Perform Vertical or Horizontal eye scan to a particular BER.
 * Eye opening in mV or mUI is returned.
 *
 * The eye opening qualifying condition is that there can be up to
 * 1 bit error in 3/BER received bits.  E.g. for BER=1e-6, 1 bit error in
 * 3e6 received bits.
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in]  lane     : Logical lane within port (0..3, mode dependent)
 *
 * @param[in]  meas_mode : Specify vertical or horizontal eye scan
 * @param[in]  meas_ber  : Specify 1e-6 or 1e-9 BER target
 * @param[out] meas_eye  : Measured eye height (mV) or eye width (mUI)
 *
 * @return Status of the API call
 *
 */
p4_pd_status_t p4_pd_sd_rx_eye_get(bf_dev_id_t dev_id,
                                   bf_dev_port_t dev_port,
                                   int lane,
                                   int meas_mode,
                                   int meas_ber,
                                   int *meas_eye);

/** @brief 3D Eye Scan
 *
 * Perform 3D eye scan for link debug.
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in]  lane     : Logical lane within port (0..3, mode dependent)
 *
 * @param[in]  meas_ber  : Specify 1e-6 or 1e-9 BER target
 * @param[out] meas_eye : Memory allocated by user to store measured data
 *
 * @return Status of the API call
 *
 */
p4_pd_status_t p4_pd_sd_rx_eye_3d_get(bf_dev_id_t dev_id,
                                      bf_dev_port_t dev_port,
                                      int lane,
                                      int meas_ber,
                                      char *meas_eye,
                                      int max_eye_data);

/** @brief Force inject Tx bit errors on a serdes slice
 *
 * Insert multiple single bit errors (as specified by num_bits) on the
 * transmit data output.  This is a debug feature to intentionally corrupts
 * transmit serial data.
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in]  lane     : Logical lane within port (0..3, mode dependent)
 *
 * @param[in]  num_bits : Number or error bits to inject (0..65535)
 *
 * @return Status of the API call
 *
 */
p4_pd_status_t p4_pd_sd_tx_err_inj_set(bf_dev_id_t dev_id,
                                       bf_dev_port_t dev_port,
                                       int lane,
                                       int num_bits);

/** @brief Force inject Rx bit errors on a serdes slice
 *
 * Insert multiple single bit errors (as specified by num_bits) on the
 * receive data input.  This is a debug feture to intentionally corrupts
 * receive serial data.
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in]  lane     : Logical lane within port (0..3, mode dependent)
 *
 * @param[in]  num_bits : Number or error bits to inject (0..65535)
 *
 * @return Status of the API call
 *
 */
p4_pd_status_t p4_pd_sd_rx_err_inj_set(bf_dev_id_t dev_id,
                                       bf_dev_port_t dev_port,
                                       int lane,
                                       int num_bits);

/** @brief Set Transmit Pattern Generator
 *
 * Sets TX Pattern Generator.  Supported modes are:
 *      - Normal Traffic:   Core data, PRBS off
 *      - PRBS Pattern:     PRBS-7/9/11/15/23/31
 *      - Fixed Pattern:    80b User defined fixed pattern
 *
 * To set 80b TX fixed pattern, call bf_serdes_tx_fixed_pat_set()
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in]  lane     : Logical lane within port (0..3, mode dependent)
 *
 * @param[in]  tx_patsel : Select transmit data / PRBS pattern
 *
 * @return Status of the API call
 *
 * @see bf_serdes_tx_userpat_set()
 *
 */
p4_pd_status_t p4_pd_sd_tx_patsel_set(bf_dev_id_t dev_id,
                                      bf_dev_port_t dev_port,
                                      int lane,
                                      int tx_patsel);

/** @brief Get Transmit Pattern Generator
 *
 * Gets TX Pattern Generator.  Supported modes are:
 *      - Normal Traffic:   Core data, PRBS off
 *      - PRBS Pattern:     PRBS-7/9/11/15/23/31
 *      - Fixed Pattern:    80b User defined fixed pattern
 *
 * To get 80b TX fixed pattern, call bf_serdes_tx_fixed_pat_get()
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in]  lane     : Logical lane within port (0..3, mode dependent)
 *
 * @param[out] tx_patsel : Select transmit data / PRBS pattern
 *
 * @return Status of the API call
 *
 * @see bf_serdes_tx_userpat_get()
 *
 */
p4_pd_status_t p4_pd_sd_tx_patsel_get(bf_dev_id_t dev_id,
                                      bf_dev_port_t dev_port,
                                      int lane,
                                      int *tx_patsel);

/** @brief Set Receive Pattern Checker
 *
 * Sets RX Pattern Checker.  Supported modes are:
 *      - Normal Traffic:       Core data, PRBS off
 *      - PRBS Pattern:         PRBS-7/9/11/15/23/31
 *      - Fixed Pattern:        80b Fixed Pattern
 *
 * The PRBS and Fixed pattern checker performs auto reseeding.  If errors are
 * detected for two consecutive words, a new seed (main data) will be used
 * to reseed the PRBS and fixed pattern checker.
 *
 * Fixed pattern uses incoming data to seed a reference, then compares
 * subsequent incoming data against it.
 *
 * @note    Continuous pCal needs to be disabled when RX pattern checker is
 *          enabled.  User needs to re-enable pCal by calling
 *          bf_serdes_rx_eq_pcal_run()
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in]  lane     : Logical lane within port (0..3, mode dependent)
 *
 * @param[in]  rx_patsel : Receive data / PRBS pattern
 *
 * @return Status of the API call
 *
 * @see bf_serdes_rx_eq_pcal_run()
 *
 */
p4_pd_status_t p4_pd_sd_rx_patsel_set(bf_dev_id_t dev_id,
                                      bf_dev_port_t dev_port,
                                      int lane,
                                      int rx_patsel);

/** @brief Get Receive Pattern Checker
 *
 * Gets RX Pattern Checker.  Supported modes are:
 *      - Normal Traffic:       Core data, PRBS off
 *      - PRBS Pattern:         PRBS-7/9/11/15/23/31
 *      - Fixed Pattern:        80b Fixed Pattern
 *
 * The PRBS and Fixed pattern checker performs auto reseeding.  If errors are
 * detected for two consecutive words, a new seed (main data) will be used
 * to reseed the PRBS and fixed pattern checker.
 *
 * Fixed pattern uses incoming data to seed a reference, then compares
 * subsequent incoming data against it.
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in]  lane     : Logical lane within port (0..3, mode dependent)
 *
 * @param[out] rx_patsel : Receive data / PRBS pattern
 *
 * @return Status of the API call
 *
 */

p4_pd_status_t p4_pd_sd_rx_patsel_get(bf_dev_id_t dev_id,
                                      bf_dev_port_t dev_port,
                                      int lane,
                                      int *rx_patsel);

/** @brief Get Receive Comparator Error Count
 *
 * Get RX comparator error count.
 *
 * For the error counter to activate, bf_serdes_rx_patsel_set() needs to be
 * called first to put the receive in PRBS or Fixed pattern checking mode.
 *
 * The error counter self-clears on read and saturates at 0xFFFF_FFFF.
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in]  lane     : Logical lane within port (0..3, mode dependent)
 *
 * @param[out] err_cnt : Accumulated error count, pegs at 0xffffffff
 *
 * @return Status of the API call
 *
 * @see bf_serdes_rx_patsel_set()
 *
 */
p4_pd_status_t p4_pd_sd_rx_err_cnt_get(bf_dev_id_t dev_id,
                                       bf_dev_port_t dev_port,
                                       int lane,
                                       uint32_t *err_cnt);

/** @brief Set Transmit Fixed Pattern
 *
 * Sets 80b fixed transmit user pattern.  This is useful for PLL output jitter
 * measurement where a clock pattern is needed, or for debugs where a specific
 * data pattern (pulses or walking 1's pattern) is needed.
 *
 * The 80b fixed pattern is configured via four 32b words.  Only bits[19:0] of
 * each word is used.  Transmit order is LSBit first.  First bit transmitted
 * out serially is tx_fixed_pat[0][0].
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in]  lane     : Logical lane within port (0..3, mode dependent)
 *
 * @param[in]  tx_fixed_pat[4] : 80b fixed pattern in 4 32b words. Only
 *                               bits[19:0] are used.  LSBit transmitted first.
 *
 * @return Status of the API call
 *
 */
p4_pd_status_t p4_pd_sd_tx_fixed_pat_set(bf_dev_id_t dev_id,
                                         bf_dev_port_t dev_port,
                                         int lane,
                                         int tx_fixed_pat_0,
                                         int tx_fixed_pat_1,
                                         int tx_fixed_pat_2,
                                         int tx_fixed_pat_3);

/** @brief Get Transmit Fixed Pattern
 *
 * Gets 80b fixed transmit pattern.  This is useful for PLL output jitter
 * measurement where a clock pattern is needed, or for debugs where a specific
 * data pattern (pulses or walking 1's pattern) is needed.
 *
 * The 80b fixed pattern is stored in four 32b words.  Only bits[19:0] of
 * each word is used.  Transmit order is LSBit first.  First bit transmitted
 * out serially is tx_fixed_pat[0][0].
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in]  lane     : Logical lane within port (0..3, mode dependent)
 *
 * @param[out] tx_fixed_pat0..3 : 80b fixed pattern in 4 32b words. Only
 *                                bits[19:0] are used.  LSBit transmitted first.
 *
 * @return Status of the API call
 *
 */
p4_pd_status_t p4_pd_sd_tx_fixed_pat_get(bf_dev_id_t dev_id,
                                         bf_dev_port_t dev_port,
                                         int lane,
                                         int *tx_fixed_pat_0,
                                         int *tx_fixed_pat_1,
                                         int *tx_fixed_pat_2,
                                         int *tx_fixed_pat_3);

/** @brief Captured 80b of Received Data
 *
 * Capture 80b of received data pattern.  This is a debug function to see what
 * data is being received (no data, random or known pattern).  This function
 * is most useful if a known repeated pattern < 80b is sent.
 *
 * The 80b captured pattern is stored in four 32b words.  Only bits[19:0] of
 * each word is used.  Receive order is LSBit first.  First bit received
 * serially is rx_cap_pat[0][0].
 *
 * @note    Continuous pCal needs to be disabled when RX pattern checker is
 *          enabled.  User needs to re-enable pCal by calling
 *          bf_serdes_rx_eq_pcal_run()
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in]  lane     : Logical lane within port (0..3, mode dependent)
 *
 * @param[out] rx_cap_pat0..3 : 80b captured pattern in 4 32b words. Only
 *                              bits[19:0] are used.  LSBit transmitted first.
 *
 * @return Status of the API call
 *
 */
p4_pd_status_t p4_pd_sd_rx_data_cap_get(bf_dev_id_t dev_id,
                                        bf_dev_port_t dev_port,
                                        int lane,
                                        int *rx_cap_pat_0,
                                        int *rx_cap_pat_1,
                                        int *rx_cap_pat_2,
                                        int *rx_cap_pat_3);

/** @brief Get the Tx equalization settings on a Tofino serdes lane
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in] lane      : logical lane (within port) 0-3, depending upon mode
 * @param[out] pre      : pre-cursor
 * @param[out] atten    : attenuation
 * @param[out] post     : post-cursor
 *
 * @return Status of the API call
 *
 */
p4_pd_status_t p4_pd_sd_get_tx_eq(bf_dev_id_t dev_id,
                                  bf_dev_port_t dev_port,
                                  int lane,
                                  int *pre,
                                  int *atten,
                                  int *post);

/** @brief Set the Tx equalization parameters on a Tofino serdes lane
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in] lane      : logical lane (within port) 0-3, depending upon mode
 * @param[in] pre       : pre-cursor
 * @param[in] atten     : attenuation
 * @param[in] post      : post-cursor
 *
 * @return Status of the API call
 *
 */
p4_pd_status_t p4_pd_sd_set_tx_eq(bf_dev_id_t dev_id,
                                  bf_dev_port_t dev_port,
                                  int lane,
                                  int pre,
                                  int atten,
                                  int post);

/** @brief Check that the PLLs match the expected divider
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in] lane      : logical lane (within port) 0-3, depending upon mode
 * @param[in] divider   : expected Tx/Rx divider (assumed to be the same)
 *
 * @return Status of the API call
 *
 */
p4_pd_status_t p4_pd_sd_get_pll_state(bf_dev_id_t dev_id,
                                      bf_dev_port_t dev_port,
                                      int lane,
                                      int expected_divider);

/** @brief Get current state of tx_output_en from a serdes slice
 *
 * @param[in]  dev_id   : Device identifier
 * @param[in]  dev_port : Port identifier
 * @param[in] lane      : logical lane (within port) 0-3, depending upon mode
 * @param[out] en       : returned state of tx_output_en
 *
 * @return Status of the API call
 *
 */
p4_pd_status_t p4_pd_sd_get_tx_output_en(bf_dev_id_t dev_id,
                                         bf_dev_port_t dev_port,
                                         int lane,
                                         bool *en);

#endif
