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


#ifndef LLD_EFUSE_H_INCLUDED
#define LLD_EFUSE_H_INCLUDED

/* Allow the use in C++ code.  */
#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  LLD_PORT_SPEED_RED_100G = 0,
  LLD_PORT_SPEED_RED_50G = 1,
  LLD_PORT_SPEED_RED_25G = 3,
} lld_efuse_port_speed_t;

typedef enum {
  LLD_PCIE_LANES_4 = 0,
  LLD_PCIE_LANES_2 = 1,
  LLD_PCIE_LANES_1 = 3,
} lld_efuse_pcie_lane_t;

typedef struct lld_efuse_data_t {
  uint32_t resubmit_disable;
  uint32_t mau_tcam_reduction;
  uint32_t mau_sram_reduction;
  uint32_t packet_generator_disable;
  uint32_t pipe_disable;
  uint32_t mau_stage_disable;
  uint64_t port_disable_map_hi;
  uint64_t port_disable_map_lo;
  uint64_t tm_memory_disable;
  uint32_t port_speed_reduction;
  uint32_t cpu_port_speed_reduction;
  uint32_t pcie_lane_reduction;
  uint32_t baresync_disable;
  uint32_t frequency_reduction;
  uint32_t frequency_check_disable;
  uint32_t versioning;
  uint32_t chip_part_number;
  uint32_t part_revision_number;
  uint32_t package_id;
  uint32_t silent_spin;
  uint64_t chip_id;
  uint32_t pmro_and_skew;
  uint32_t voltage_scaling;
  uint32_t soft_pipe_dis;
  uint32_t tof2_pipe_mau_stage_disable[4];
  uint32_t tof3_pipe_mau_stage_disable[4];

  uint64_t tof2_eth_port_speed_reduction;
  uint64_t tof3_eth_port_speed_reduction;
  uint16_t device_id;
  uint16_t tof2_die_rotate;
  uint16_t tof3_die_config;  // 0-12.8T, 1-25.65T
  uint64_t tof3_fuse_serdes_odd;
  uint64_t tof3_fuse_serdes_even;
  uint32_t tof3_constant0;
  uint32_t tof3_constant1;
} lld_efuse_data_t;

int lld_efuse_load(bf_dev_id_t dev_id,
                   bf_subdev_id_t subdev_id,
                   bf_dev_init_mode_t warm_init_mode);
uint32_t lld_subdev_efuse_get_chip_part_number(bf_dev_id_t dev_id,
                                               bf_subdev_id_t subdev_id);
uint32_t lld_efuse_get_chip_part_number(bf_dev_id_t dev_id);
uint32_t lld_subdev_efuse_get_part_revision_number(bf_dev_id_t dev_id,
                                                   bf_subdev_id_t subdev_id);
uint32_t lld_efuse_get_part_revision_number(bf_dev_id_t dev_id);
uint32_t lld_efuse_get_package_id(bf_dev_id_t dev_id);
uint64_t lld_subdev_efuse_get_chip_id(bf_dev_id_t dev_id,
                                      bf_subdev_id_t subdev_id);
uint64_t lld_efuse_get_chip_id(bf_dev_id_t dev_id);
uint32_t lld_subdev_efuse_get_silent_spin(bf_dev_id_t dev_id,
                                          bf_subdev_id_t subdev_id);
uint32_t lld_efuse_get_silent_spin(bf_dev_id_t dev_id);
uint32_t lld_efuse_get_voltage_scaling(bf_dev_id_t dev_id);
uint32_t lld_subdev_efuse_get_die_config(bf_dev_id_t dev_id,
                                         bf_subdev_id_t subdev_id);
uint32_t lld_efuse_get_die_config(bf_dev_id_t dev_id);
uint32_t lld_efuse_get_constant0(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id);
uint32_t lld_efuse_get_constant1(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id);
uint32_t lld_subdev_efuse_get_soft_pipe_dis(bf_dev_id_t dev_id,
                                            bf_subdev_id_t subdev_id);
uint64_t lld_subdev_efuse_get_tm_memory_disable(bf_dev_id_t dev_id,
                                                bf_subdev_id_t subdev_id);
uint64_t lld_subdev_efuse_get_tm_memory_disable(bf_dev_id_t dev_id,
                                                bf_subdev_id_t subdev_id);
uint64_t lld_efuse_get_tm_memory_disable(bf_dev_id_t dev_id);
uint32_t lld_subdev_efuse_get_mau_stage_disable(bf_dev_id_t dev_id,
                                                bf_subdev_id_t subdev_id,
                                                uint32_t phy_pipe);
uint32_t lld_efuse_get_mau_stage_disable(bf_dev_id_t dev_id, uint32_t phy_pipe);
uint32_t lld_subdev_efuse_get_mau_sram_reduction(bf_dev_id_t dev_id,
                                                 bf_subdev_id_t subdev_id);
uint32_t lld_subdev_efuse_get_mau_tcam_reduction(bf_dev_id_t dev_id,
                                                 bf_subdev_id_t subdev_id);
uint32_t lld_efuse_get_pipe_disable(bf_dev_id_t dev_id);
uint32_t lld_subdev_efuse_get_pipe_disable(bf_dev_id_t dev_id,
                                           bf_subdev_id_t subdev_id);
uint32_t lld_efuse_get_port_disable(bf_dev_id_t dev_id,
                                    uint64_t *map_hi,
                                    uint64_t *map_lo);
uint32_t lld_subdev_efuse_get_port_disable(bf_dev_id_t dev_id,
                                           bf_subdev_id_t subdev_id,
                                           uint64_t *map_hi,
                                           uint64_t *map_lo);
uint32_t lld_subdev_efuse_get_port_speed_reduction(bf_dev_id_t dev_id,
                                                   bf_subdev_id_t subdev_id);
uint32_t lld_efuse_get_port_speed_reduction(bf_dev_id_t dev_id);
uint32_t lld_subdev_efuse_get_cpu_port_speed_reduction(
    bf_dev_id_t dev_id, bf_subdev_id_t subdev_id);
uint64_t lld_subdev_efuse_get_eth_port_speed_reduction(
    bf_dev_id_t dev_id, bf_subdev_id_t subdev_id);
uint32_t lld_subdev_efuse_get_pcie_lane_reduction(bf_dev_id_t dev_id,
                                                  bf_subdev_id_t subdev_id);
uint32_t lld_subdev_efuse_get_packet_generator_disable(
    bf_dev_id_t dev_id, bf_subdev_id_t subdev_id);
uint32_t lld_subdev_efuse_get_versioning(bf_dev_id_t dev_id,
                                         bf_subdev_id_t subdev_id);
uint32_t lld_efuse_get_frequency_reduction(bf_dev_id_t dev_id);
uint32_t lld_efuse_get_frequency_reduction_bps(bf_dev_id_t dev_id);
uint32_t lld_efuse_get_frequency_reduction_pps(bf_dev_id_t dev_id);
uint32_t lld_efuse_get_frequency_check_disable(bf_dev_id_t dev_id);
uint32_t lld_subdev_efuse_get_resubmit_disable(bf_dev_id_t dev_id,
                                               bf_subdev_id_t subdev_id);
uint32_t lld_subdev_efuse_get_baresync_disable(bf_dev_id_t dev_id,
                                               bf_subdev_id_t subdev_id);
uint32_t lld_subdev_efuse_get_pmro_and_skew(bf_dev_id_t dev_id,
                                            bf_subdev_id_t subdev_id);
uint32_t lld_efuse_get_device_id(bf_dev_id_t dev_id);
int lld_efuse_get_vmin(bf_dev_id_t dev_id);
bool lld_efuse_get_die_rotated(bf_dev_id_t dev_id);
void lld_efuse_wafer_str_get(bf_dev_id_t dev_id,
                             bf_subdev_id_t subdev_id,
                             int s_len,
                             char *s);
uint32_t lld_efuse_get_serdes_dis_even(bf_dev_id_t dev_id,
                                       bf_subdev_id_t subdev_id);
uint32_t lld_efuse_get_serdes_dis_odd(bf_dev_id_t dev_id,
                                      bf_subdev_id_t subdev_id);

#ifdef __cplusplus
}
#endif /* C++ */

#endif  // LLD_EFUSE_H_INCLUDED
