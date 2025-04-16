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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <cstring>
#include "os_privs.h"

#ifdef __linux__

#include <linux/capability.h>

extern "C" {
int capget(cap_user_header_t hdrp, cap_user_data_t datap);
int capset(cap_user_header_t hdrp, const cap_user_data_t datap);
}

static int os_privs_get_capabilities(uint32_t *effective, uint32_t *permitted, uint32_t *inheritable) {
  // XXX: make cap_user_data_t malloc future-proof - allow for 128-bit capability sets (hence 4*)
  cap_user_header_t hdrp = (cap_user_header_t)malloc(sizeof(*hdrp));
  cap_user_data_t   datap = (cap_user_data_t)malloc(4*sizeof(*datap));
  assert((hdrp != NULL) && (datap != NULL));
  hdrp->version = 0;
  hdrp->pid = 0;
  (void)capget(hdrp, datap); // Just to get version
  int ret = capget(hdrp, datap);
  if (ret == 0) {
    if (effective != NULL) *effective = datap->effective;
    if (permitted != NULL) *permitted = datap->permitted;
    if (inheritable != NULL) *inheritable = datap->inheritable;
  } else {
    perror("capget");
  }
  free((void*)hdrp);
  free((void*)datap);
  return ret;
}
static int os_privs_set_capabilities(uint32_t effective, uint32_t permitted, uint32_t inheritable) {
  int ret = -1;
  cap_user_header_t hdrp = (cap_user_header_t)malloc(sizeof(*hdrp));
  cap_user_data_t   datap = (cap_user_data_t)malloc(4*sizeof(*datap));
  assert((hdrp != NULL) && (datap != NULL));
  hdrp->version = 0;
  hdrp->pid = 0;
  (void)capget(hdrp, datap); // Just to get version
  if (capget(hdrp, datap) == 0) {
    (void)std::memset((void*)datap, 0, 4*sizeof(*datap));
    datap->effective = effective;
    datap->permitted = permitted;
    datap->inheritable = inheritable;
    ret = capset(hdrp, datap);
    if (ret != 0) perror("capset");
  }
  free((void*)hdrp);
  free((void*)datap);
  return ret;
}
static int os_privs_has_perm_capability(int cap) { // yes=1 no=0
  uint32_t eff = 0u, perm = 0u, inh = 0u;
  if (os_privs_get_capabilities(&eff, &perm, &inh) == 0) {
    if (((perm >> cap) & 1u) == 1u) return 1;
  }
  return 0;
}
static int os_privs_print_capabilities(const char *pfx) {
  uint32_t eff = 0u, perm = 0u, inh = 0u;
  int ret = os_privs_get_capabilities(&eff, &perm, &inh);
  if (ret == 0) {
    printf("%s PRIVS: Eff=0x%08x Perm=0x%08x Inh=0x%08x\n", pfx, eff, perm, inh);
  }
  return ret;
}
static void os_privs_sanity_check(uint32_t eff, uint32_t perm, uint32_t inh) {
  uint32_t mask = (1u << CAP_DAC_OVERRIDE) | (1u << CAP_DAC_READ_SEARCH) | (1u << CAP_NET_RAW);
  // Expecting mask to be formed from distinct tokens in [0..31]
  // Expecting eff/perm to not have any bits set apart from ones allowed by mask
  // Expecting inh to always be 0u
  if ( (__builtin_popcount(mask) != 3) || ((eff & ~mask) != 0u) || ((perm & ~mask) != 0u) || (inh != 0u) ) {
    fprintf(stderr, "!!!!! Unexpected privilege elevation - exiting !!!!!\n");
    fprintf(stderr, "PRIVS: Eff=0x%08x Perm=0x%08x Inh=0x%08x\n", eff, perm, inh);
    exit(1);
  }
}

int os_privs_init(void) {
  // PROCESS START - clear effective and inheritable
  //               - clear all *except* CAP_DAC_* CAP_NET_RAW from permitted
  uint32_t mask = (1u << CAP_DAC_OVERRIDE) | (1u << CAP_DAC_READ_SEARCH) | (1u << CAP_NET_RAW);
  uint32_t eff = 0u, perm = 0u, inh = 0u;
  int ret1 = os_privs_get_capabilities(&eff, &perm, &inh);
  if (ret1 != 0) return ret1;
  eff = 0u;
  perm &= mask;
  inh = 0u;
  os_privs_sanity_check(eff, perm,  inh);
  int ret2 = os_privs_set_capabilities(eff, perm, inh);
  os_privs_print_capabilities("");
  // Should always be ok to reduce privs
  return ret2;
}
int os_privs_reset(void) {
  // Clear effective privs. Retain permitted privs but check sane.
  uint32_t eff = 0u, perm = 0u, inh = 0u;
  int ret1 = os_privs_get_capabilities(&eff, &perm, &inh);
  if (ret1 != 0) return ret1;
  os_privs_sanity_check(eff, perm,  inh);
  eff = 0u;
  int ret2 = os_privs_set_capabilities(eff, perm, inh);
  //os_privs_print_capabilities("");
  // Should always be ok to reduce privs
  return ret2;
}
int os_privs_has_cap_net_raw(void) { // yes=1 no=0
  return os_privs_has_perm_capability(CAP_NET_RAW);
}
int os_privs_drop_permanently(void) {
  return os_privs_set_capabilities(0u, 0u, 0u);
}
int os_privs_dropped(void) { // yes=1 no=0
  uint32_t eff = 0u, perm = 0u, inh = 0u;
  int ret1 = os_privs_get_capabilities(&eff, &perm, &inh);
  return ((ret1 == 0) && (eff == 0u) && (perm == 0u) && (inh == 0u)) ?1 :0;
}
int os_privs_for_veth_attach(void) {
  // BEFORE VETH ATTACH - set effective = CAP_NET_RAW
  uint32_t eff = 0u, perm = 0u, inh = 0u;
  int ret1 = os_privs_get_capabilities(&eff, &perm, &inh);
  if (ret1 != 0) return ret1;
  os_privs_sanity_check(eff, perm,  inh);
  eff = (1u << CAP_NET_RAW);
  // This might fail
  int ret2 = os_privs_set_capabilities(eff, perm, inh);
  //os_privs_print_capabilities("");
  return ret2;
}
int os_privs_for_file_access(void) {
  // BEFORE FILE ACCESS - set effective = CAP_DAC_* as permitted
  uint32_t eff = 0u, perm = 0u, inh = 0u;
  int ret1 = os_privs_get_capabilities(&eff, &perm, &inh);
  if (ret1 != 0) return ret1;
  os_privs_sanity_check(eff, perm,  inh);
  if (((perm >> CAP_DAC_OVERRIDE) & 1u) == 1u) eff |= (1u << CAP_DAC_OVERRIDE);
  if (((perm >> CAP_DAC_READ_SEARCH) & 1u) == 1u) eff |= (1u << CAP_DAC_READ_SEARCH);
  int ret2 = os_privs_set_capabilities(eff, perm, inh);
  // Given only ever requesting already permitted privs this should never fail
  //os_privs_print_capabilities("");
  return ret2;
}

#else

// TODO: emulate these if building for non-Linux OS

int os_privs_init(void)             { assert(0); }
int os_privs_reset(void)            { assert(0); }
int os_privs_has_cap_net_raw(void)  { assert(0); }
int os_privs_drop_permanently(void) { assert(0); }
int os_privs_for_veth_attach(void)  { assert(0); }
int os_privs_for_file_access(void)  { assert(0); }
int os_privs_dropped(void)          { assert(0); }

#endif
