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

#include <mcn_test.h>
#if !MCN_TEST(MODEL_CHIP_NAMESPACE, rsvd1)
#include <utests/test_util.h>
#else
#include <utests/test_util_ftr.h>
#endif
#include <rmt-object-manager.h>
#include <iostream>
#include <string>
#include <array>
#include <cassert>
#include <chrono>

#include "gtest.h"

#if MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd0) || MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd1)
#include <tamba-mac.h>
#endif
#if !MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd0) && !MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd1)
#include <comira-umac3.h>
#include <comira-umac4.h>
#endif

#include <model_core/model.h>
#include <port.h>

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;


namespace MODEL_CHIP_TEST_NAMESPACE {

bool mac_counters_print = true;

using namespace std;
using namespace MODEL_CHIP_NAMESPACE;


// Template base class that defines the bulk of the test; this will be
// subclassed to specialize for each comira mac type.
template <typename CNTR>
class BFN_TEST_NAME(MacCountersTestBase) : public testing::Test {
 protected:
  // subclasses must override this method to return the index of the tx counter
  // that is equivalent to a given rx counter index when tx is true.
  virtual int MAPTX(bool tx, int rx_index) = 0;
  // sublcasses may override the following increment methods if their mac type
  // has a corresponding counter
  virtual void inc_too_long_err(uint64_t *MyCntrs, bool tx) {};
  virtual void inc_undersized(uint64_t *MyCntrs, bool tx) {};
  virtual void inc_oversized(uint64_t *MyCntrs, bool tx) {};
  virtual void inc_max_frm_size_viol_err(uint64_t *MyCntrs, bool tx) {};
  virtual void inc_invalid_preamble_err(uint64_t *MyCntrs, bool tx) {};
  virtual void inc_normal_len_invalid_crc_err(uint64_t *MyCntrs, bool tx) {};
  virtual void inc_trunc(uint64_t *MyCntrs, bool tx) {};
  virtual void inc_dropped(uint64_t *MyCntrs, bool tx) {};
  virtual void inc_crcErrStomp(uint64_t *MyCntrs, bool tx) {};

 public:
  uint32_t verify_counters(TestUtil *tu, uint64_t *MyCntrs, const int NCNTRS,
                           int portIndex, int chan, bool rx, bool clear,
                           const char* msg, int pkt=-1, uint64_t plen=0) {

    int n_errors = 0;
    for (int cntr = 0; cntr < NCNTRS; cntr++) {
      uint64_t macval = tu->mac_cntr_read(portIndex, cntr, clear);
      if (MyCntrs[cntr] != macval) {
        n_errors++;
        printf("Cntr[%d] mismatch%s: Local=%" PRId64 " InWord=%" PRId64 " "
               "(clear=%c)(%s)(chan=%d)",
               cntr, msg, MyCntrs[cntr], macval,
               (clear)?'T':'F', (rx)?"RX":"TX", chan);
        if (pkt >= 0) {
          printf("(pkt=%d)(plen=%" PRId64 ")\n", pkt, plen);
        } else {
          printf("\n");
        }
      }
      if (clear) MyCntrs[cntr] = UINT64_C(0);
    }
    return n_errors;
  }

  void do_test(const char *NAME,
               const int TYPE,
               const int NCHANS,
               const int NCNTRS,
               const int NPORTS,
               const uint32_t RX_ERR_MASK,
               const uint32_t TX_ERR_MASK) {
    if (mac_counters_print) RMT_UT_LOG_INFO("test_mac_counters %s\n", NAME);

    GLOBAL_MODEL->Reset();
    GLOBAL_MODEL->DestroyAllChips();

    int chip = 7; // Full complement stages
    int pipe = 0;
    int stage = 0;
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);

    // DEBUG setup....................
    uint64_t pipes, stages, types, rows_tabs, cols, flags;
    pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;
    flags = FEW;
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);
    RmtObjectManager *om = tu.get_objmgr();
    ASSERT_TRUE(om != NULL);

    uint64_t seed = UINT64_C(0x1234567890ABCDEF);
    uint32_t n_errors = 0u;

    // Define test packets
    //                          <   DA     ><    SA    ><ET><><><LN><ID><OF><><><CK>< SRC  >< DST  ><SP><DP>
    const char *pktstr_uni54 = "080022AABBCC080011DDEEFF080045000100123400001011FFFF0A1122330A4455661188119900000000000000000000000000000000";
    const char *pktstr_mul54 = "010011AABBCC080011DDEEFF080045000100123400001011FFFF0A1122330A4455661188119900000000000000000000000000000000";
    const char *pktstr_brd54 = "FFFFFFFFFFFF080011DDEEFF080045000100123400001006FFFF0A1122330A4455660688069900000000000000000000000000000000";
    const char *pktstr_pause = "0180c2000001080011DDEEFF88080001080045000100123400001011FFFF0A1122330A445566118811990000000000000000000000000000000000000000000000";
    const char *pktstr_vlan[8] = { // Alternate TPID 0x8100,0x9100
    // <   DA     ><    SA    >< VLAN ><ET><><><LN><ID><OF><><><CK>< SRC  >< DST  ><SP><DP>
      "080022AABBCC080011DDEEFF81000000080045000100123400001011FFFF0A1122330A4455661188119900000000000000000000000000000000",
      "080022AABBCC080011DDEEFF91002000080045000100123400001011FFFF0A1122330A4455661188119900000000000000000000000000000000",
      "080022AABBCC080011DDEEFF81004000080045000100123400001011FFFF0A1122330A4455661188119900000000000000000000000000000000",
      "080022AABBCC080011DDEEFF91006000080045000100123400001011FFFF0A1122330A4455661188119900000000000000000000000000000000",
      "080022AABBCC080011DDEEFF81008000080045000100123400001011FFFF0A1122330A4455661188119900000000000000000000000000000000",
      "080022AABBCC080011DDEEFF9100A000080045000100123400001011FFFF0A1122330A4455661188119900000000000000000000000000000000",
      "080022AABBCC080011DDEEFF8100C000080045000100123400001011FFFF0A1122330A4455661188119900000000000000000000000000000000",
      "080022AABBCC080011DDEEFF8100E000080045000100123400001011FFFF0A1122330A4455661188119900000000000000000000000000000000"
    };


    Packet  *pkt_uni54 = om->pkt_create(pktstr_uni54);
    Packet  *pkt_mul54 = om->pkt_create(pktstr_mul54);
    Packet  *pkt_brd54 = om->pkt_create(pktstr_brd54);
    Packet  *pkt_pause = om->pkt_create(pktstr_pause);
    Packet  *pkt_vlan[8] = { om->pkt_create(pktstr_vlan[0]),
                             om->pkt_create(pktstr_vlan[1]),
                             om->pkt_create(pktstr_vlan[2]),
                             om->pkt_create(pktstr_vlan[3]),
                             om->pkt_create(pktstr_vlan[4]),
                             om->pkt_create(pktstr_vlan[5]),
                             om->pkt_create(pktstr_vlan[6]),
                             om->pkt_create(pktstr_vlan[7])};

    // Go through ports one by one
    for (int portIndex = 0; portIndex < NPORTS; portIndex++) {
      // Figure out Mac from Port
      Port *port = om->port_lookup(portIndex);
      if (port == nullptr) continue; // Skip if null

      Mac *mac = port->mac();
      if (mac == nullptr)  continue; // Skip if null
      if (mac->mac_type() != TYPE) continue; // Skip if wrong type

      EXPECT_GE(port->mac_index(), 0);
      EXPECT_GE(port->mac_chan(), 0);
      EXPECT_EQ(port->mac_index(), mac->mac_block());

      int clear01, rxtx, cntr, pkt;
      int chan = port->mac_chan();

      if ((chan >= 0) && (chan < NCHANS)) {

        if (mac_counters_print) printf(
            "MAC Counter Testing Port=%d {%s[%d] Chan=%d}\n",
            portIndex, NAME, port->mac_index(), port->mac_chan());

        // Keep a local copy of what we think counters should be
        // Note we keep 100 counters and MyCntrs[99] is incremented when there
        // is no TX counterpart for a RX counter value.
        // That's ok as we only ever check the real counters [0..95]
        uint64_t MyCntrs[100];

        // Reset local copy
        for (cntr = 0; cntr < NCNTRS; cntr++) MyCntrs[cntr] = UINT64_C(0);
        // And reset all MAC counters using the MAC object
        for (cntr = 0; cntr < NCNTRS; cntr++) (void)mac->mac_counter_read(chan, cntr, true);

        for (clear01 = 0; ((clear01 == 0) || (clear01 == 1)); clear01++) {
          // Clear mac counters on 2nd iteration; note that counters are
          // cleared *after* each verification so any errors accumulated during
          // the 1st cycle will be observed by the first verification of the
          // 2nd cycle.
          bool clear = (clear01 == 1);

          for (rxtx = 0; ((rxtx == 0) || (rxtx == 1)); rxtx++) {
            bool rx = (rxtx == 0); // RX counters first time, TX second
            bool tx = !rx;

            // 1. Send a unicast packet in/out mac - short pkt so should see error
            mac->increment_counters(rx, chan, pkt_uni54, 0u);
            // ... and work out locally what counters *should* be
            MyCntrs[MAPTX(tx, CNTR::OctetsRcvdAll)] += 54;
            MyCntrs[MAPTX(tx, CNTR::FramesRcvdError)] += 1; // Short
            MyCntrs[MAPTX(tx, CNTR::FramesRcvdAll)] += 1;
            MyCntrs[MAPTX(tx, CNTR::FramesRcvdUnicast)] += 1;
            MyCntrs[MAPTX(tx, CNTR::FramesRcvdPri0)] += 1;
            MyCntrs[MAPTX(tx,CNTR::FramesRcvdSizeLT64)] += 1;
            inc_undersized(MyCntrs, tx);
            // ... then check *all* mac counters match
            n_errors += verify_counters(&tu, MyCntrs, NCNTRS, portIndex, chan, rx, clear, "1");

            // 2. Send a multicast packet in/out mac - short pkt so should see error
            mac->increment_counters(rx, chan, pkt_mul54, 0u);
            // ... and work out locally what counters *should* be
            MyCntrs[MAPTX(tx, CNTR::OctetsRcvdAll)] += 54;
            MyCntrs[MAPTX(tx, CNTR::FramesRcvdError)] += 1; // Short
            MyCntrs[MAPTX(tx, CNTR::FramesRcvdAll)] += 1;
            MyCntrs[MAPTX(tx, CNTR::FramesRcvdMulticast)] += 1;
            MyCntrs[MAPTX(tx, CNTR::FramesRcvdPri0)] += 1;
            MyCntrs[MAPTX(tx, CNTR::FramesRcvdSizeLT64)] += 1;
            inc_undersized(MyCntrs, tx);
            // ... then check *all* mac counters match
            n_errors += verify_counters(&tu, MyCntrs, NCNTRS, portIndex, chan, rx, clear, "2");

            // 3. Send a broadcast packet in/out mac
            mac->increment_counters(rx, chan, pkt_brd54, 0u);
            // ... and work out locally what counters *should* be
            MyCntrs[MAPTX(tx, CNTR::OctetsRcvdAll)] += 54;
            MyCntrs[MAPTX(tx, CNTR::FramesRcvdError)] += 1; // Short
            MyCntrs[MAPTX(tx, CNTR::FramesRcvdAll)] += 1;
            MyCntrs[MAPTX(tx, CNTR::FramesRcvdBroadcast)] += 1;
            MyCntrs[MAPTX(tx, CNTR::FramesRcvdPri0)] += 1;
            MyCntrs[MAPTX(tx, CNTR::FramesRcvdSizeLT64)] += 1;
            inc_undersized(MyCntrs, tx);
            // ... then check mac counters match
            n_errors += verify_counters(&tu, MyCntrs, NCNTRS, portIndex, chan, rx, clear, "3");

            // 4. Send multicast pause packet in/out mac
            mac->increment_counters(rx, chan, pkt_pause, 0u);
            // ... and work out locally what counters *should* be
            MyCntrs[MAPTX(tx, CNTR::OctetsRcvdOK)] += 65;
            MyCntrs[MAPTX(tx, CNTR::OctetsRcvdAll)] += 65;
            MyCntrs[MAPTX(tx, CNTR::FramesRcvdOK)] += 1;
            MyCntrs[MAPTX(tx, CNTR::FramesRcvdAll)] += 1;
            MyCntrs[MAPTX(tx, CNTR::FramesRcvdMulticast)] += 1;
            MyCntrs[MAPTX(tx, CNTR::FramesRcvdPri0)] += 1;
            MyCntrs[MAPTX(tx, CNTR::FramesRcvdPause)] += 1;
            MyCntrs[MAPTX(tx, CNTR::FramesRcvdSize65to127)] += 1;
            // ... then check mac counters match
            n_errors += verify_counters(&tu, MyCntrs, NCNTRS, portIndex, chan, rx, clear, "4");

            // 5. Now send multicast PFC packets with varying length (but fixed 18B hdr)
            uint8_t pphdr[18];
            // Use first 18B of existing pkt_pause packet....
            int ppgot = pkt_pause->get_buf(pphdr, 0, 18);
            EXPECT_EQ(18, ppgot);
            // ... but change bytes 14,15 to be 0x01,0x01 to make a PriPause/PFC packet
            // Set PRI byte 17 to 0 initially - we'll randomise it shortly
            pphdr[14] = 0x01; pphdr[15] = 0x01; pphdr[16] = 0; pphdr[17] = 0;

            for (pkt = 0; pkt < 20; pkt++) {
              seed = tu.mmix_rand64(seed);
              int plen = 400 - pkt; // So len in [400..381]
              uint64_t plen64 = static_cast<uint64_t>(plen);
              // Make random packet - pphdr[0..15], random bytes in [16...plen-1]
              Packet *p = tu.packet_make(seed, plen, pphdr, 16);
              ppgot = p->get_buf(pphdr, 0, 18); // Reget pphdr from p
              EXPECT_EQ(18, ppgot);
              uint16_t pri = static_cast<uint16_t>( pphdr[17] ); // Now use random byte 17 as PRI
              //if (rx && (pri == 0)) pri = 0x100; // Set PRI[8]=1 if RX && PRI[7..0]==0 (RcvdStdPause)
              //if (tx && (pri == 0)) pri = 0x100; // Set PRI[8]=1 if TX && PRI[7..0]==0 (XmitStdPause)    // TBD : only WIP
              if (pri == 0) pri = 0x100; // Set PRI[8]=1 if PRI[7..0]==0 (RcvdStdPause or XmitStdPause)
              mac->increment_counters(rx, chan, p, 0u);
              // ... and work out locally what counters *should* be
              MyCntrs[MAPTX(tx, CNTR::OctetsRcvdOK)] += plen64;
              MyCntrs[MAPTX(tx, CNTR::OctetsRcvdAll)] += plen64;
              MyCntrs[MAPTX(tx, CNTR::FramesRcvdOK)] += 1;
              MyCntrs[MAPTX(tx, CNTR::FramesRcvdAll)] += 1;
              MyCntrs[MAPTX(tx, CNTR::FramesRcvdMulticast)] += 1;
              MyCntrs[MAPTX(tx, CNTR::FramesRcvdSize256to511)] += 1;
              MyCntrs[MAPTX(tx, CNTR::FramesRcvdPriPause)] += 1;
              for (int bit = 0; bit < 9; bit++) {
                if (((pri >> bit) & 1) == 1) {
                  if (bit == 8)
                    MyCntrs[MAPTX(tx,CNTR::RcvdStdPause1US)] += 1;
                  else
                    MyCntrs[MAPTX(tx,CNTR::RcvdPri0Pause1US + bit)] += 1;
                }
              }
              // ... then check *all* mac counters match
              n_errors += verify_counters(&tu, MyCntrs, NCNTRS, portIndex, chan, rx, clear, "5", pkt, plen64);
              tu.packet_free(p);
            }

            // 6. Send unicast VLAN packets in/out mac - these have pri 0..7 - short pkts again
            for (pkt = 0; pkt < 8; pkt++) {
              uint64_t plen64 = pkt_vlan[pkt]->len();
              mac->increment_counters(rx, chan, pkt_vlan[pkt], 0u);
              // ... and work out locally what counters *should* be
              MyCntrs[MAPTX(tx, CNTR::OctetsRcvdAll)] += plen64;
              MyCntrs[MAPTX(tx, CNTR::FramesRcvdError)] += 1; // Short
              MyCntrs[MAPTX(tx, CNTR::FramesRcvdAll)] += 1;
              MyCntrs[MAPTX(tx, CNTR::FramesRcvdUnicast)] += 1;
              MyCntrs[MAPTX(tx, CNTR::FramesRcvdPri0 + pkt)] += 1;
              MyCntrs[MAPTX(tx, CNTR::FramesRcvdSizeLT64)] += 1;
              MyCntrs[MAPTX(tx, CNTR::FramesRcvdVLAN)] += 1;
              inc_undersized(MyCntrs, tx);
              // ... then check *all* mac counters match
              n_errors += verify_counters(&tu, MyCntrs, NCNTRS, portIndex, chan, rx, clear, "6", pkt, plen64);
            }

            // 7. Now send unicast packets with varying lengths (but fixed unicast hdr)
            int lens[24] = { 53,63, 64,64, 65,127, 128,255, 256,511, 512,1023,
                             1024,1518, 1519,2047, 2048,4095, 4096,8191, 8192,9215,
                             9216,9217 };
            uint8_t uhdr[16];
            int ugot = pkt_uni54->get_buf(uhdr, 0, 16);
            EXPECT_EQ(16, ugot);

            for (pkt = 0; pkt < 24; pkt++) {
              seed = tu.mmix_rand64(seed);
              // Make random unicast packets of varying lengths (should be 2 per counter)
              int plen = lens[pkt];
              uint64_t plen64 = static_cast<uint64_t>(plen);
              Packet *p = tu.packet_make(seed, plen, uhdr, 16);
              mac->increment_counters(rx, chan, p, 0u);
              // ... and work out locally what counters *should* be
              if (plen < 64) {
                MyCntrs[MAPTX(tx, CNTR::FramesRcvdError)] += 1;
                inc_undersized(MyCntrs, tx);
              } else if (plen > 9104) {
                MyCntrs[MAPTX(tx, CNTR::FramesRcvdError)] += 1;
                inc_oversized(MyCntrs, tx);
              } else {
                MyCntrs[MAPTX(tx, CNTR::OctetsRcvdOK)] += plen64;
                MyCntrs[MAPTX(tx, CNTR::FramesRcvdOK)] += 1;
              }
              MyCntrs[MAPTX(tx, CNTR::OctetsRcvdAll)] += plen64;
              MyCntrs[MAPTX(tx, CNTR::FramesRcvdAll)] += 1;
              MyCntrs[MAPTX(tx, CNTR::FramesRcvdUnicast)] += 1;
              MyCntrs[MAPTX(tx, CNTR::FramesRcvdPri0)] += 1;
              MyCntrs[MAPTX(tx, CNTR::FramesRcvdSizeLT64 + (pkt / 2))] += 1;
              // Should end up with 2 per counter
              // ... then check *all* mac counters match
              n_errors += verify_counters(&tu, MyCntrs, NCNTRS, portIndex, chan, rx, clear, "7", pkt, plen64);
              tu.packet_free(p);
            }

            // 8. Repeat sending unicast packets with varying lengths (but fixed unicast hdr)
            //      but this time randomly set error flags.
            //
            for (pkt = 0; pkt < 24; pkt++) {
              seed = tu.mmix_rand64(seed);
              int plen = lens[pkt];
              // Skip short/long pkts so we ensure the packets themselves don't introduce errors
              if ((plen < 64) || (plen > 9104)) continue;
              // Make random unicast packets of varying lengths with random errors
              uint64_t plen64 = static_cast<uint64_t>(plen);
              uint32_t perrs = static_cast<uint32_t>( ((seed >> 32)^(seed)) & Cntr::kErrorMask );
              Packet *p = tu.packet_make(seed, plen, uhdr, 16);
              if (rx) perrs &= RX_ERR_MASK; else perrs &= TX_ERR_MASK;
              mac->increment_counters(rx, chan, p, perrs);
              // ... and work out locally what counters *should* be
              if (((perrs >> Cntr::Truncated) & 1) == 1)          inc_trunc(MyCntrs, tx);
              if (((perrs >> Cntr::Oversized) & 1) == 1)          inc_oversized(MyCntrs, tx);
              if (((perrs >> Cntr::Dropped) & 1) == 1)            inc_dropped(MyCntrs, tx);
              if (((perrs >> Cntr::Fragment) & 1) == 1)           MyCntrs[MAPTX(tx,CNTR::FramesRcvdFragments)] += 1;
              if (((perrs >> Cntr::Jabber) & 1) == 1)             MyCntrs[MAPTX(tx,CNTR::FramesRcvdJabber)] += 1;
              if (((perrs >> Cntr::LenErr) & 1) == 1)             MyCntrs[MAPTX(tx,CNTR::FramesRcvdLenError)] += 1;
              if (((perrs >> Cntr::CrcErrStomp) & 1) == 1)        inc_crcErrStomp(MyCntrs, tx);

              // These next 2 are only relevant on ComiraUmac3
              if (((perrs >> Cntr::TooLongErr) & 1) == 1)         inc_too_long_err(MyCntrs, tx);
              if (((perrs >> Cntr::Undersized) & 1) == 1)         inc_undersized(MyCntrs, tx);

              // These next 3 are only relevant on ComiraUmac4
              if (((perrs >> Cntr::MaxFrmSizeViolErr) & 1) == 1)  inc_max_frm_size_viol_err(MyCntrs, tx);
              if (((perrs >> Cntr::InvalidPreambleErr) & 1) == 1) inc_invalid_preamble_err(MyCntrs, tx);
              if (((perrs >> Cntr::NormalLenInvalidCrcErr) & 1) == 1) inc_normal_len_invalid_crc_err(MyCntrs, tx);
              //
              MyCntrs[MAPTX(tx, CNTR::OctetsRcvdAll)] += plen64;
              MyCntrs[MAPTX(tx, CNTR::FramesRcvdAll)] += 1;
              if (perrs != 0u) {
                if ((perrs & Cntr::kCrcErrorMask) != 0u)    MyCntrs[MAPTX(tx,CNTR::FramesRcvdCRCError)] += 1;
                MyCntrs[MAPTX(tx,CNTR::FramesRcvdError)] += 1;
              } else {
                MyCntrs[MAPTX(tx, CNTR::OctetsRcvdOK)] += plen64;
                MyCntrs[MAPTX(tx, CNTR::FramesRcvdOK)] += 1;
              }
              MyCntrs[MAPTX(tx, CNTR::FramesRcvdUnicast)] += 1;
              MyCntrs[MAPTX(tx, CNTR::FramesRcvdPri0)] += 1;
              MyCntrs[MAPTX(tx,CNTR::FramesRcvdSizeLT64 + (pkt/2))] += 1; // Should end up with 2 per counter

              // ... then check *all* mac counters match
              n_errors += verify_counters(&tu, MyCntrs, NCNTRS, portIndex, chan, rx, clear, "8", pkt, plen64);
              tu.packet_free(p);
            }

          } // rxtx
        } // clear01
      } // if ((chan >= 0) && (chan < NCHANS))
    } // portIndex

    printf("Checking...done\n");
    EXPECT_EQ(0u, n_errors);

    // Free up packets
    om->pkt_delete(pkt_uni54);
    om->pkt_delete(pkt_mul54);
    om->pkt_delete(pkt_brd54);
    om->pkt_delete(pkt_pause);
    for (unsigned i = 0; i < 8; ++i) om->pkt_delete(pkt_vlan[i]);

    // Schtumm
    tu.finish_test();
    tu.quieten_log_flags();

    // Cleanup
    GLOBAL_MODEL->Reset();
    GLOBAL_MODEL->DestroyAllChips();
  }
};


#if !MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd0) && !MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd1)
// concrete implementation of the base test fixture for ComiraUmac3
class BFN_TEST_NAME(MacCountersTestVmacC3) :
    public BFN_TEST_NAME(MacCountersTestBase)<VmacC3Cntr> {
 protected:
  int MAPTX(bool tx, int rx_index) override {
    // for each array index that corresponds to a rx counter the array value
    // gives the index of the equivalent tx counter; where there is no tx
    // equivalent the array entry is 99; for each array index that corresponds
    // to a tx counter the array value is 0
    static const int VmacC3MapTxLookup[89] = {
      32, 33, 99, 34, 35, 36, 37, 38, 39, 40, /* 0-9 */
      99, 99, 99, 99, 99, 41, 99, 99, 42, 99, /* 10-19 */
      43, 44, 45, 46, 47, 48, 49, 50, 51, 52, /* 20-29 */
      53, 54,  0,  0,  0,  0,  0,  0,  0,  0, /* 30-39 */
       0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 40-49 */
       0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 50-59 */
       0,  0,  0, 55, 56, 57, 58, 59, 60, 61, /* 60-69 */
      62,  0,  0,  0,  0,  0,  0,  0,  0, 71, /* 70-79 */
      72, 73, 74, 75, 76, 77, 78, 99, 99      /* 80-88 */
    };
    return (tx) ?VmacC3MapTxLookup[rx_index] :rx_index;
  }

  // Comira 3 actually has these counters
  void inc_too_long_err(uint64_t *MyCntrs, bool tx) override {
    MyCntrs[MAPTX(tx,VmacC3Cntr::FramesRcvdFrameTooLong)] += 1;
  }

  void inc_undersized(uint64_t *MyCntrs, bool tx) override {
    MyCntrs[MAPTX(tx,VmacC3Cntr::FramesRcvdUndersized)] += 1;
  }

  void inc_oversized(uint64_t *MyCntrs, bool tx) override {
    MyCntrs[MAPTX(tx,VmacC3Cntr::FramesRcvdOversized)] += 1;
  }

  void inc_trunc(uint64_t *MyCntrs, bool tx) override {
    MyCntrs[MAPTX(tx,VmacC3Cntr::FramesRcvdTrunc)] += 1;
  }

  void inc_dropped(uint64_t *MyCntrs, bool tx) override {
    MyCntrs[MAPTX(tx,VmacC3Cntr::FramesDropped)] += 1;
  }

  void inc_crcErrStomp(uint64_t *MyCntrs, bool tx) override {
    MyCntrs[MAPTX(tx,VmacC3Cntr::FramesRcvdCrcErrStomp)] += 1;
  }
};

TEST_F(BFN_TEST_NAME(MacCountersTestVmacC3),VmacC3) {
  do_test("Comira3",
          MacType::kVmacC3,
          4,
          89, // Counters [0..88]
          512, // 9b - not fully populated
          0x7FF << 14, // No MaxFrmSize/InvalPreamble/NormLenInvalCrc errors
          0u); // No TX errors on ComiraUmac3
}
#endif


#if !MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd0) && !MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd1)
// concrete implementation of the base test fixture for ComiraUmac4
class BFN_TEST_NAME(MacCountersTestVmacC4) :
    public BFN_TEST_NAME(MacCountersTestBase)<VmacC4Cntr> {
 protected:
  int MAPTX(bool tx, int rx_index) {
    // for each array index that corresponds to a rx counter the array value
    // gives the index of the equivalent tx counter; where there is no tx
    // equivalent the array entry is 99; for each array index that corresponds
    // to a tx counter the array value is 0
    static const int VmacC4MapTxLookup[96] = {
      0,   0,  0,  0,  0,  0,  0,  0,  0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*  0-19 */
      0,   0,  0,  0,  0,  0,  0,  0,  0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 20-39 */
      0,   0,  0,  0,  3,  1,  4, 99,  2,  5,  /* 40-49 */
      6,   7,  8, 99, 99, 99, 99, 40,  9, 99,  /* 50-59 */
      99, 10, 99, 11, 12, 13, 14, 15, 16, 17,  /* 60-69 */
      18, 19, 20, 21, 22, 23, 24, 25, 26, 27,  /* 70-79 */
      28, 29, 30, 31, 32, 33, 34, 35, 36, 37,  /* 80-89 */
      38, 99, 42, 99, 99, 99                   /* 90-95 */
    };
    return (tx) ?VmacC4MapTxLookup[rx_index] :rx_index;
  }

  // Comira 4 actually has these counters
  void inc_max_frm_size_viol_err(uint64_t *MyCntrs, bool tx) {
    MyCntrs[MAPTX(tx,VmacC4Cntr::FramesRcvdMaxFrmSizeVio)] += 1;
  };

  void inc_invalid_preamble_err(uint64_t *MyCntrs, bool tx) {
    MyCntrs[MAPTX(tx,VmacC4Cntr::InvalidPreamble)] += 1;
  };

  void inc_normal_len_invalid_crc_err(uint64_t *MyCntrs, bool tx) {
    MyCntrs[MAPTX(tx,VmacC4Cntr::NormalLenInvalidCRC)] += 1;
  };

  void inc_oversized(uint64_t *MyCntrs, bool tx) override {
    MyCntrs[MAPTX(tx,VmacC4Cntr::FramesRcvdOversized)] += 1;
  }

  void inc_trunc(uint64_t *MyCntrs, bool tx) override {
    MyCntrs[MAPTX(tx,VmacC4Cntr::FramesRcvdTrunc)] += 1;
  }

  void inc_dropped(uint64_t *MyCntrs, bool tx) override {
    MyCntrs[MAPTX(tx,VmacC4Cntr::FramesDropped)] += 1;
  }

  void inc_crcErrStomp(uint64_t *MyCntrs, bool tx) override {
    MyCntrs[MAPTX(tx,VmacC4Cntr::FramesRcvdCrcErrStomp)] += 1;
  }
};

TEST_F(BFN_TEST_NAME(MacCountersTestVmacC4),VmacC4) {
  do_test("Comira4",
          MacType::kVmacC4,
          8,
          96, // Counters [0..95]
          512, // 9b - not fully populated
          Cntr::kErrorMask, // All errors supported on RX
          (1u<<Cntr::Truncated) | (1u<<Cntr::Jabber));
}
#endif


#if MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd0) || MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd1)
// Either 9b or 11b port space - though not fully populated
#if MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd0)
#define NUM_PORTS 512
#endif
#if MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd1)
#define NUM_PORTS 2048
#endif

// concrete implementation of the base test fixture for Tamba
class BFN_TEST_NAME(MacCountersTestVmacT1) :
    public BFN_TEST_NAME(MacCountersTestBase)<VmacT1Cntr> {
 protected:
  int MAPTX(bool tx, int rx_index) {
    // for each array index that corresponds to a rx counter the array value
    // gives the index of the equivalent tx counter; where there is no tx
    // equivalent the array entry is 99; for each array index that corresponds
    // to a tx counter the array value is 0
    static const int VmacT1MapTxLookup[96] = {
      0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*  0-19 */
      0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 20-39 */
      0,   0,  0,  0,  0,  0,  0,  0,  0,  1,  /* 40-49 */
      2,   3,  4,  5,  6,  7,  8,  9, 10, 11,  /* 50-59 */
      12, 13, 14, 15, 16, 17, 18, 19, 20, 21,  /* 60-69 */
      22, 23, 24, 25, 26, 27, 28, 29, 30, 31,  /* 70-79 */
      32, 33, 34, 35, 36, 37, 38, 39, 40, 41,  /* 80-89 */
      42, 43, 44, 45, 46, 99                   /* 90-95 */
    };
    return (tx) ?VmacT1MapTxLookup[rx_index] :rx_index;
  }

  void inc_oversized(uint64_t *MyCntrs, bool tx) override {
    MyCntrs[MAPTX(tx,VmacT1Cntr::FramesRcvdOversized)] += 1;
  }
};

TEST_F(BFN_TEST_NAME(MacCountersTestVmacT1),VmacT1) {
  do_test("Tamba",
          MacType::kVmacT1,
          8,
          96, // Counters [0..95]
          NUM_PORTS,
          Cntr::kErrorMask, // All errors supported on RX
          Cntr::kErrorMask);  // All errors supported on TX
}
#endif


}
