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

#ifndef _SHARED_PACKET_
#define _SHARED_PACKET_

#include <common/rmt-assert.h>
#include <cstdint>
#include <string>
#include <vector>
#include <i2queueing-metadata.h>
#include <queueing2e-metadata.h>
#include <e2mac-metadata.h>
#include <mirror-metadata.h>
#include <bridge-metadata.h>
#include <rmt-defs.h>
#include <rmt-object.h>
#include <packet-body.h>
#include <phv.h>
#include <time-info.h>
#include <common/rmt-util.h>


namespace MODEL_CHIP_NAMESPACE {

#define MAX_LOG_PKT_SIG_LEN     64

  class PacketPipeData;
  class Port;
  class Clot;
  class Teop;

  class IEInfo {
    // Default initial vals
    // XXX: packet version should be 0 initially
    static constexpr uint8_t kIEInfoVersion = 0;
    static constexpr bool    kIEInfoMetaData = false;

 public:
    IEInfo()  { };
    ~IEInfo() { phv_ = NULL; port_ = NULL; clot_ = NULL; }

    inline Phv     *phv()                  const { return phv_; }
    inline Port    *port()                 const { return port_; }
    inline Clot    *clot()                 const { return clot_; }
    inline uint8_t  version()              const { return version_; }
    inline bool     metadata()             const { return metadata_; }
    inline uint16_t orig_hdr_len()         const { return orig_hdr_len_; }
    inline uint16_t parsable_len()         const { return parsable_len_; }
    inline void     set_phv(Phv *phv)            { phv_ = phv; }
    inline void     set_port(Port *port)         { port_ = port; }
    inline void     set_clot(Clot *clot)         { clot_ = clot; }
    inline void     set_version(uint8_t v)       { version_ = v; }
    inline void     set_metadata(bool m)         { metadata_ = m; }
    inline void     set_orig_hdr_len(uint16_t l) { orig_hdr_len_ = l; }
    inline void     set_parsable_len(uint16_t l) { parsable_len_ = l; }

 private:
    Phv     *phv_ = NULL;
    Port    *port_ = NULL;
    Clot    *clot_ = NULL;
    uint8_t  version_ = kIEInfoVersion;
    bool     metadata_ = kIEInfoMetaData;
    uint16_t orig_hdr_len_ = 0;
    uint16_t parsable_len_ = 0xFFFF;
  };



  class Packet : private RmtObject {
    // XXX: packet version should be 0 initially
    static constexpr uint8_t kPacketInitialVersion = 0;

 public:
    static constexpr uint16_t kPktFcsLen = 4;

    // Packet Id:
    // 63:62 = type (normal, mirror, gen, mcast)
    // 61:32 = type specific id bits
    // 31:0  = Id
    static constexpr uint64_t kPktTypeWidth = 4;
    static constexpr uint64_t kPktTypeShift = (64-kPktTypeWidth);
    static constexpr uint64_t kPktTypeMask  = ((UINT64_C(1) << kPktTypeWidth) - 1) << kPktTypeShift;
    static constexpr uint64_t kPktIdWidth = 32;
    static constexpr uint64_t kPktIdShift = 0;
    static constexpr uint64_t kPktIdMask  = ((UINT64_C(1) << kPktIdWidth) - 1) << kPktIdShift;

    // Multicast:
    // 59 = C2C flag
    // 57:56 = PRE Id (Maps 1-to-1 with pipe id)
    // 51:32 = Count to distinguish copies
    static constexpr uint64_t kPktIdMcastC2C = UINT64_C(1) << 59;
    static constexpr uint64_t kPktIdMcastPipeShift = 56;
    static constexpr uint64_t kPktIdMcastPipeMask  = (UINT64_C(RmtDefs::kPresTotal - 1)) << kPktIdMcastPipeShift;
    static constexpr uint64_t kPktIdMcastCopyShift = 32;
    static constexpr uint64_t kPktIdMcastCopyMask  = UINT64_C(0xFFFFF) << kPktIdMcastCopyShift;

    static bool kPktInitTimeRandOnAlloc; // Defined in rmt-config.cpp

    Packet(RmtObjectManager *om);
    Packet(RmtObjectManager *om, size_t len);
    Packet(RmtObjectManager *om, PacketBuffer *pb);
    Packet(RmtObjectManager *om, const uint8_t buf[], size_t len);
    Packet(RmtObjectManager *om, const std::shared_ptr<uint8_t>& buf, size_t len);
    Packet(RmtObjectManager *om, const std::string hexstr);
    Packet(RmtObjectManager *om, const char* hexstr);
    ~Packet();

    void reset_meta();
    void reset_i2q();
    void reset_q2e();
    void reset_br();
    void reset_qing_data();
    void reset_clots();
    void reset_time_info();
    void reset_bufs();
    void reset_ids();
    void reset_except_bufs();
    void reset();
    void destroy();
    void prepend(PacketBuffer* pb, int replace_bytes=0);
    void prepend(const BitVector<128> &bv, int replace_bytes=0);
    void prepend_metadata_hdr(const uint8_t metadata[], size_t size);
    void trim_metadata_hdr();
    void append(PacketBuffer* pb);
    void append(Packet* pkt);    // append buffers from another packet
    void append_zeros(const int n);
    int  trim_front(int bytes_to_trim);
    int  trim_back(int bytes_to_trim);
    int  get_buf(uint8_t buf[], int start_pos, int len) const;
    void set_byte(int pos, uint8_t val);


    PacketBody *packet_body() { return &packet_body_; }
    inline uint32_t hash() const { return packet_body_.hash(); }
    inline int len() const { return packet_body_.len(); }



    // TODO: should be no need to duplicate info ultimately
    inline void     set_egress()            { egress_ = true; egress_info_ = ingress_info_; egress_info_.set_clot(NULL); }
    inline void     set_ingress()           { egress_ = false; }
    inline bool     is_egress()       const { return egress_; }
    inline bool     is_ingress()      const { return !egress_; }
    inline IEInfo  *ingress_info()          { return &ingress_info_; }
    inline IEInfo  *egress_info()           { return &egress_info_; }
    inline const IEInfo  *egress_info()  const { return &egress_info_; }
    inline const IEInfo  *ingress_info() const { return &ingress_info_; }
    inline IEInfo  *ie()                       { return is_egress() ?egress_info() :ingress_info(); }
    inline const IEInfo  *const_ie()     const { return is_egress() ?egress_info() :ingress_info(); }

    inline uint16_t orig_ingress_pkt_len()               { return orig_ingress_pkt_len_; }
    inline void     set_orig_ingress_pkt_len(uint16_t l) { orig_ingress_pkt_len_ = l; }
    inline uint8_t  priority()              { return priority_; }
    inline void     set_priority(uint8_t p) { priority_ = p; }
    inline Teop    *teop()                  { return teop_; }
    inline void     set_teop(Teop *teop)    { teop_ = teop; }
    inline bool     is_generated() const    { return generated_pkt_; }
    inline void     set_generated(bool x)   { generated_pkt_ = x; }
    inline uint64_t generated_T()  const    { return generated_T_; }
    inline void     set_generated_T(uint64_t T)   { generated_T_ = T; }
    inline int      metadata_len()   const  { return metadata_len_; }
    inline void     set_metadata_len(int l) { metadata_len_ = l; }

    inline Phv     *phv()                   { return ie()->phv(); }
    inline Port    *port()            const { return const_ie()->port(); }
    inline Clot    *clot()            const { return const_ie()->clot(); }
    inline uint8_t  version()               { return ie()->version(); }
    inline bool     is_version_0()          { return (version() & 0x1); }
    inline bool     is_version_1()          { return (version() & 0x2); }
    inline bool     metadata_added()        { return ie()->metadata(); }
    inline uint16_t orig_hdr_len()          { return ie()->orig_hdr_len(); }
    inline uint16_t parsable_len()          { return ie()->parsable_len(); }

    inline void     set_phv(Phv *phv)                { ie()->set_phv(phv); }
    inline void     set_port(Port *port)             { ie()->set_port(port); }
    inline void     set_clot(Clot *clot)             { ie()->set_clot(clot); }
    inline void     set_version(uint8_t version)     { ie()->set_version(version); }
    inline void     set_metadata_added(bool tf=true) { ie()->set_metadata(tf); }
    inline void     set_orig_hdr_len(uint16_t l)     { ie()->set_orig_hdr_len(l); }
    inline void     set_parsable_len(uint16_t l)     { ie()->set_parsable_len(l); }

    inline Packet  *clone(bool cp_meta=true) const { return clone_internal(cp_meta); }


    // Get functions for queueing specific state.
    inline uint32_t next_L1()          const { return qing_data_.next_L1_; }
    inline int      ph_ver()           const { return qing_data_.ph_ver_; }
    inline int      l1_cnt()           const { return qing_data_.count_L1_; }
    inline int      l2_cnt()           const { return qing_data_.count_L2_; }
    inline uint16_t cur_mgid()         const { return qing_data_.cur_mgid_; }
    inline uint32_t mc_cpy_cnt()             { return qing_data_.mc_cpy_cnt_++; }
    inline uint8_t  mirr_cpy_cnt()           { return qing_data_.mirr_cpy_cnt_++; }
    // Set functions for queueing specific state.
    inline void     set_next_L1(uint32_t x)  { qing_data_.next_L1_ = x; }
    inline void     set_ph_ver(int v)        { qing_data_.ph_ver_ = v; }
    inline void     set_L1_cnt(int c)        { qing_data_.count_L1_ = c; }
    inline void     set_L2_cnt(int c)        { qing_data_.count_L2_ = c; }
    inline void     inc_L1_cnt()             { ++qing_data_.count_L1_; }
    inline void     inc_L2_cnt()             { ++qing_data_.count_L2_; }
    inline void     set_cur_mgid(uint16_t m) { qing_data_.cur_mgid_ = m; }

    PacketBuffer *get_resubmit_header()      { return resubmit_header_; }
    void set_resubmit_header(PacketBuffer *r){ resubmit_header_ = r; }
    bool is_resubmit()                 const { return resubmit_; }
    void set_resubmit(bool tf)               { resubmit_ = tf; }
    void mark_for_resubmit()                 { set_resubmit(true); }
    void unmark_for_resubmit()               { set_resubmit(false); }
    bool     truncated()               const { return truncated_; }
    void     set_truncated(bool tf)          { truncated_ = tf; }
    uint32_t recirc_cnt(void)          const { return recirc_cnt_; }
    void     set_recirc_cnt(uint32_t c)      { recirc_cnt_ = c; }

    // Time related funcs
    TimeInfoType& get_time_info(bool eop) {
      return eop ? eop_time_info_ : hdr_time_info_;
    }
    // use this to initialise to plausible values, first_tick_time is
    //  used as the tick time for the first mau and the seed for the
    //  random numbers (it doesn't bother to do the random accurately)
    // TODO: should use the packet length to work out how much later the EOP is
    void setup_time_info(uint64_t first_tick_time) {
      hdr_time_info_.set_all(first_tick_time);
      eop_time_info_.set_all(first_tick_time+2);
    }

    I2QueueingMetadata *i2qing_metadata()    { return &i2qing_metadata_; }
    Queueing2EMetadata *qing2e_metadata()    { return &qing2e_metadata_; }
    E2MacMetadata  *e2mac_metadata()         { return &e2mac_metadata_; }
    MirrorMetadata *mirror_metadata()        { return &mirror_metadata_; }
    BridgeMetadata *bridge_metadata()        { return &bridge_metadata_; }

    enum pkt_type_ {
        PKT_TYPE_NORM = 0,
        PKT_TYPE_MIRROR = 1,
        PKT_TYPE_GEN = 2,
        PKT_TYPE_MC_COPY = 3,
    };
    inline  uint64_t pkt_id() const          { return pkt_id_;}
    inline  uint64_t pkt_id(uint64_t id)     { return pkt_id_ = (pkt_id_ & ~kPktIdMask) |
                                                                ((id << kPktIdShift) & kPktIdMask) ;}
    inline  uint64_t pkt_type(uint64_t type) { return pkt_id_ = (pkt_id_ & ~kPktTypeMask) |
                                                                ((type << kPktTypeShift) & kPktTypeMask); }

    inline  uint64_t pkt_id_mc_c2c(uint64_t id) { pkt_id_cpy_common(id, PKT_TYPE_MC_COPY);
                                                  return pkt_id_ = pkt_id_ | kPktIdMcastC2C; }
    inline  uint64_t pkt_id_mc_cpy(uint64_t id, uint64_t pipe, uint64_t cpy) {
                                   pkt_id_cpy_common(id, PKT_TYPE_MC_COPY);
                                   return pkt_id_ = pkt_id_ |
                                                    ((pipe << kPktIdMcastPipeShift) & kPktIdMcastPipeMask) |
                                                    ((cpy  << kPktIdMcastCopyShift) & kPktIdMcastCopyMask);
                                             }
    inline  uint64_t pkt_id_mirr_cpy(uint64_t id, uint64_t pipe, uint64_t cpy) {
                                   pkt_id_cpy_common(id, PKT_TYPE_MIRROR);
                                   return pkt_id_ = pkt_id_ |
                                                   ((pipe << kPktIdMcastPipeShift) & kPktIdMcastPipeMask) |
                                                   ((cpy  << kPktIdMcastCopyShift) & kPktIdMcastCopyMask);
                                             }
    void pkt_sig_set(const std::string &pkt_sig);
    std::string pkt_sig_get();
    void extract_signature();


    // compares the contents of packets (not metadata)
    static bool compare_packets(const Packet* a, const Packet* b) {
      const int chunk_size = 128;
      uint8_t buf_a[chunk_size];
      uint8_t buf_b[chunk_size];

      int start_pos = 0;
      int len_a,len_b;
      do {
        len_a = a->get_buf( buf_a, start_pos, chunk_size );
        len_b = b->get_buf( buf_b, start_pos, chunk_size );

        if (len_a != len_b) {
          return false;
        }

        for (int i=0;i<len_a;++i) {
          if (buf_a[i] != buf_b[i]) {
            return false;
          }
        }
        start_pos += len_a;

      } while ( len_a != 0 );
      return true;
    }

    // Get next mirror entry and clear bit from bitmap. Return -1 if no remaining entries.
    inline int  next_mirror() {
      auto mirror_bmp = i2qing_metadata()->mirror_bmp();
      for (int i = 0; i < RmtDefs::kMirrorsPerPacket; ++i) {
        uint16_t current = 0x1 << i;
        if (mirror_bmp & current) {
          i2qing_metadata()->set_mirror_bmp(mirror_bmp & ~current);
          return i;
        }
      }
      return -1;
    }

    // Funcs to allow DV to get/set cached PacketPipeData
    PacketPipeData *pipe_data();
    void     set_pipe_data_pipe(int pipe);
    void     set_pipe_data(bool ing, int parser, int what_data, int bit_offset,
                           uint64_t data, int width);
    uint64_t get_pipe_data(bool ing, int parser, int what_data, int bit_offset, int width);
    void     set_pipe_data_ctrl(bool ing, int parser, int what_data, uint8_t ctrl);
    uint8_t  get_pipe_data_ctrl(bool ing, int parser, int what_data);
    void     free_pipe_data();
    bool     has_pipe_data();


   private:
    DISALLOW_COPY_AND_ASSIGN(Packet);
    Packet *clone_internal(bool cp_meta) const;
    void pkt_id_cpy_common(uint64_t id, pkt_type_ copy_type) {
      pkt_id_ = 0;
      pkt_id(id);
      pkt_type(copy_type);
    }

    PacketBody                 packet_body_{};
    IEInfo                     ingress_info_;
    IEInfo                     egress_info_;
    bool                       egress_ = false;
    uint16_t                   orig_ingress_pkt_len_ = 0;
    uint8_t                    priority_ = 0;  // Only ingress
    Teop                      *teop_ = nullptr;
    bool                       generated_pkt_ = false; // Created by pkt-gen.
    uint64_t                   generated_T_ = UINT64_C(0);
    int                        metadata_len_ = 0;  // len of prepended metadata

    // Used by queueing block to keep track of the packet.
    // Some fields are passed to the egress deparser as well.
    struct {
      uint32_t                 next_L1_ = 0;
      int                      ph_ver_ = 0;
      int                      count_L1_ = 0;
      int                      count_L2_ = 0;
      uint16_t                 cur_mgid_ = 0;
      uint32_t                 mc_cpy_cnt_ = 0;
      uint8_t                  mirr_cpy_cnt_ = 0;
    } qing_data_;

    // stash resubmit header
    PacketBuffer              *resubmit_header_ = nullptr;
    bool                       resubmit_ = false;
    bool                       truncated_ = false;
    TimeInfoType               hdr_time_info_{};
    TimeInfoType               eop_time_info_{};

    I2QueueingMetadata         i2qing_metadata_;
    Queueing2EMetadata         qing2e_metadata_;
    E2MacMetadata              e2mac_metadata_;
    MirrorMetadata             mirror_metadata_;
    BridgeMetadata             bridge_metadata_;

    // !!!!!!!!!! Any new member variable must be reset in Packet::reset() !!!!!!!!!!

    // XXX: But these *NOT* RESET - preserved across recirc
    PacketPipeData            *pipe_data_ = nullptr;
    uint32_t                   recirc_cnt_ = 0u;
    uint64_t                   pkt_id_ = UINT64_C(-1);
    std::string                pkt_signature_;

  };
}
#endif // _SHARED_PACKET_
