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

#include <mutex>
#include <iomanip>
#include <iostream>
#include <common/bounded_queue.h>
#include <model_core/register_block.h>
#include <model_core/timer-manager.h>
#include <rmt-object-manager.h>
#include <packet-enqueuer.h>
#include <tofino_lfsr.h>
#include <pktgen.h>
#include <pktgen-reg.h>
#include <pktgen-app-reg.h>
#include <register_adapters.h>


using namespace std;
using namespace model_common;

namespace MODEL_CHIP_NAMESPACE {

constexpr uint8_t PktGenReg::ch_mode_[kChannelModes][RmtDefs::kChlsPerIpb];

PktGenBufferMem::PktGenBufferMem(RmtObjectManager *om, PktGenReg* com) :
    RmtObject(om),
    RegisterBlockIndirect(chip_index(),
            (BUF_ST_ADDR + com->get_pipe() * BUF_ADDR_STEP) >> 4, // Convert to word address
            BUF_ENTRIES,
            false,
            nullptr,
            nullptr,
            "PGR Buffer Mem"),
    buf_{} {
            //uint64_t addr = (BUF_ST_ADDR + com->get_pipe() * BUF_ADDR_STEP) >> 4;
            //cout << "Registered buffer address: " << hex << addr << endl;


    }
bool PktGenBufferMem::write(uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T) {
    uint8_t index = 0;
    RMT_ASSERT(offset < BUF_ENTRIES);
    std::lock_guard<std::mutex> lck(mutex_);
    while (index < BUF_ELEM_SZ) {
        if (index < 8) {
            buf_[offset][index] = data0 & 0xFF;
            data0 >>= 8;
        } else {
            buf_[offset][index] = data1 & 0xFF;
            data1 >>= 8;
        }
        index ++;
    }
    return true;
}
bool PktGenBufferMem::read(uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T) const {

    int8_t index = BUF_ELEM_SZ - 1;
    RMT_ASSERT(offset < BUF_ENTRIES);
    *data0 = 0;
    *data1 = 0;
    while (index >= 0) {
        if (index < 8) {
            *data0 <<= 8;
            *data0 |= static_cast<uint64_t>(buf_[offset][index]);
        } else {
            *data1 <<= 8;
            *data1 |= static_cast<uint64_t>(buf_[offset][index]);
        }
        index --;
    }
    return true;
}

void PktGenBufferMem::get_val(uint8_t* buf, uint16_t st_offset, uint16_t n_entries) {
    uint8_t index = 0;
    uint16_t o_index = 0;
    RMT_ASSERT(st_offset < BUF_ENTRIES);
    //RMT_ASSERT((n_entries % BUF_ELEM_SZ) == 0);
    std::lock_guard<std::mutex> lck(mutex_);
    while (n_entries) {
        if (index == BUF_ELEM_SZ) {
            index = 0;
            st_offset ++;
        }
        buf[o_index++] = buf_[st_offset][index];
        --n_entries;
        index++;
    }
}



PktGenReg::PktGenReg(int chip, int pipe, RmtObjectManager* om, PktGen* pgen, PacketEnqueuer* pe):
    om_(om),
    m_pktgen(pgen),
    packet_enqueuer_(pe),
    m_pipe(pipe),
    m_chip(chip),
    curr_seq_ch_(0)
{
    RMT_ASSERT(m_pktgen != nullptr);
}


P16_regs::P16_regs(int chip, int pipe, RmtObjectManager* om, PktGen* pgen, PacketEnqueuer* pe):
    PktGenReg(chip, pipe, om, pgen, pe),
    pg_port16ctrl(parser_adapter(pg_port16ctrl,chip, pipe, [this](){this->pg_port16ctrl_wb();}))
{
  reset_();
}

bool P16_regs::recirc_packet(Packet** packet) {
    /*
     * Macs not modeled, so no adding this
     uint64_t rval;
     model_timer::ModelTimerGetTime(rval);
     rval += pg_port16ts.offset();
     rval &= 0xFFFFFFFF;
     rval <<= 32;
     rval |= packet->hash();
     packet->append(new PacketBuffer((uint8_t *)&rval, sizeof(rval)));
     */

  // P17 queues the packet internally, so that it can arbitrated between recirculated and packet gen packets
  //  there is no need to do that here, so just enqueue the packet in rmt-packet-coordinator

  (*packet)->set_generated(false);
  m_pktgen->update_recirc_packet(*packet);
  packet_enqueuer_->enqueue((*packet)->port()->port_index(), *packet, false);
  *packet = nullptr;

  return true;
}

void P16_regs::reset_(void) {
    pg_port16ctrl.reset();
    pg_port16ts.reset();

}

uint8_t P16_regs::get_tdm_ch(void) {
    uint64_t c_time;
    model_timer::ModelTimerGetTime(c_time);
    uint8_t ch_idx = (c_time % RmtDefs::kChlsPerIpb);
    // Now get the actual channel
    uint8_t m_channel = ((pg_port16ctrl.channel_seq())>>(ch_idx << 1))&0x3;
    return m_channel;
}
uint8_t P16_regs::get_seq_ch(void) {
    uint8_t mode = pg_port16ctrl.channel_mode();
    uint8_t channel = PktGenReg::ch_mode_[mode][curr_seq_ch_];

    return channel;
}

bool P16_regs::is_ch_enabled(uint8_t ch) {
    return !!(((pg_port16ctrl.channel_en()) >> ch) & 0x1);
}

uint8_t P16_regs::get_channel(bool inc) {
    // Read the channel mode
    uint16_t ch;
    if (m_pktgen->get_tdm()) {
        ch = get_tdm_ch();
    } else {
        ch = get_seq_ch();
        if (inc) {
            curr_seq_ch_ = ((curr_seq_ch_ + 1)%RmtDefs::kChlsPerIpb);
        }
    }
    return ch;
}

P16_regs::~P16_regs() {


}




bool P17_regs::recirc_packet(Packet** packet) {


    /*
     * We don't model Macs, so no need to add CRC/TS
     *
     uint64_t rval;
     model_timer::ModelTimerGetTime(rval);
     rval += pg_port17ts.offset();
     rval &= 0xFFFFFFFF;
     rval <<= 32;
     rval |= packet->hash();
     packet->append(new PacketBuffer((uint8_t *)&rval, sizeof(rval)));
     */
    /*
     * TODO: this comment seems to be false, as this code does not
     *   get called if the channel is not enabled - remove comment or fix?
     * Before you put the packet into recirculation, you need to
     * check for packet generation, and queue an event if that is the case
     * Note this event needs to happen EVEN IF recir packet is dropped, which
     * can be the case if the channel is not enabled
     */

  uint8_t back_in_ch = ((*packet)->port()->port_index() & 0x3);
    if ((pg_port17ctrl2.recir_channel_en() >> back_in_ch) & 0x1) {
        recir_snoop_handle(*packet);
    }

    (*packet)->set_generated(false);
    if (!recir_q_.enqueue(std::move(*packet))) {
        return false;
    }

    *packet = nullptr;
    return true;
}

uint8_t P17_regs::get_channel(bool inc) {
    // Read the channel mode
    uint8_t ch;
    if (m_pktgen->get_tdm()) {
        ch = get_tdm_ch();
    } else {
        ch = get_seq_ch();
        if (inc) {
            curr_seq_ch_ = ((curr_seq_ch_+1)%RmtDefs::kChlsPerIpb);
        }
    }
    return ch;
}


uint8_t P17_regs::get_seq_ch(void) {
    uint8_t mode = pg_port17ctrl1.channel_mode();
    uint8_t channel = PktGenReg::ch_mode_[mode][curr_seq_ch_];
    return channel;
}

bool P17_regs::is_ch_enabled(uint8_t ch) {
    return !!(((pg_port17ctrl1.channel_en()) >> ch) & 0x1);
}
uint8_t P17_regs::get_tdm_ch(void) {
    uint64_t c_time;
    model_timer::ModelTimerGetTime(c_time);
    uint8_t ch_idx = (c_time % RmtDefs::kChlsPerIpb);
    // Now get the actual channel
    uint8_t m_channel = ((pg_port17ctrl1.channel_seq())>>(ch_idx << 1))&0x3;
    return m_channel;
}



P17_regs::P17_regs(int chip, int pipe, RmtObjectManager* om, PktGen* pgen, PacketEnqueuer* pe):
    PktGenReg(chip, pipe, om, pgen, pe),
    gen_q_(25, 0, 20),
    recir_q_(0, 20, 20),
    main_thread(nullptr),
    dur_(PGEN_TMO),
    g_done(false),
    ev_flag_(true),
    lfsr_(0),
    pgen_buffer(om, this),
    pg_port17ctrl1(parser_adapter(pg_port17ctrl1,chip,pipe, [this](){this->pg_port17ctrl1_wb();})),
    pg_port17ctrl2(parser_adapter(pg_port17ctrl2,chip,pipe)),
    pg_port17ts(parser_adapter(pg_port17ts,chip,pipe)),
    pg_portdowndis(parser_adapter(pg_portdowndis,chip,pipe, [this](){this->pg_portdowndis_wb();}))
{

        for (unsigned int i = 0; i < pgen_app_.size(); i ++) {
            pgen_app_[i] = new PktGenAppReg(chip, pipe, i, this);
        }
        reset_();
        //start_();

    }

void P17_regs::port_down(uint16_t port) {
    // Handle onlyt if this port belongs to your pipe

    uint16_t pipe_id = port >> 7;
    port &= 0x7F;

    if (m_pipe != pipe_id) return;
    {
        std::lock_guard<std::mutex> lock(port_down_mutex_);

        if (pg_portdowndis.set(port)) return;

        auto it = find_if(port_down_vec_.begin(),
                port_down_vec_.end(),
                [&](EvData* p_status) {
                return port == static_cast<uint16_t>(p_status->data());
                });
        bool sw_cleared = false;
        if (it != port_down_vec_.end()) {
                // Am I getting a port down event for a port that SW has cleared
            sw_cleared = (*it)->is_sw_done();
        } else sw_cleared = true;

        if (!sw_cleared) return;

        // Even if a port down event for this port has not been handled
        // by HW, we need to queue another event.

        EvData* p_status = new EvData(port);
        PktGenAppReg* m_pgen = nullptr;

        for (unsigned int i = 0; i < pgen_app_.size(); i ++) {
            m_pgen = pgen_app_[i];
            {
                std::lock_guard<std::mutex> lock(m_pgen->r_mutex);
                if (!m_pgen->en() || (m_pgen->m_app_config.app_type != TRIGGER_TYPE_E::PORT_DOWN)) continue;
                p_status->app_add(i);
            }
        }
        if(p_status->is_armed()) {
                port_down_vec_.push_back(p_status);
                queue_evts_(p_status);
        } else {
            delete p_status;
        }
        // HW writes 1
        pg_portdowndis.set(port, 0x1);
    }

}

void P17_regs::queue_evts_(EvData* dat) {
    RMT_ASSERT(dat->is_armed());
    PktGenAppReg* m_pgen = nullptr;
    for (unsigned int i = 0; i < pgen_app_.size(); i ++) {
        m_pgen = pgen_app_[i];
        if(m_pgen->en() && dat->is_app_set(i)) {
            //std::lock_guard<std::mutex> lock(m_pgen->r_mutex);
            m_pgen->ev_q_.push(ev_ids_.next());
        }
    }

}


void P17_regs::recir_snoop_handle(Packet* r_packet) {

    uint8_t buf[4];
    r_packet->get_buf(buf, 0, sizeof(buf));
    uint32_t val = 0;


    for (unsigned int i = 0; i < sizeof(buf); i ++) {
        val <<= 8;
        val |= static_cast<uint32_t>(buf[i]);
    }

    EvData* r_data = new EvData(val);
    PktGenAppReg* m_pgen = nullptr;
    for (unsigned int i = 0; i < pgen_app_.size(); i ++) {
        m_pgen = pgen_app_[i];
        {
            std::lock_guard<std::mutex> lock(m_pgen->r_mutex);
            if (!m_pgen->en() || m_pgen->m_app_config.app_type != TRIGGER_TYPE_E::PACKET_RECIRC) continue;
            uint32_t pkt_masked_key = val | (m_pgen->m_app_config).recir_match_mask;
            uint32_t app_masked_key = (m_pgen->m_app_config).recir_match_value | (m_pgen->m_app_config).recir_match_mask;
            if (pkt_masked_key == app_masked_key) {
                r_data->app_add(i);
            }
        }
    }
    if (r_data->is_armed()) {
        std::unique_lock<std::mutex> lck(recir_mutex_);
        recir_match_.push_back(r_data);
        queue_evts_(r_data);
    } else {
        delete r_data;
    }

}
void P17_regs::reset_(void) {
    pg_port17ctrl1.reset();
    pg_port17ctrl2.reset();
    pg_port17ts.reset();
    pg_portdowndis.reset();
}

P17_regs::~P17_regs() {
    stop();
    for (unsigned int i = 0; i < pgen_app_.size(); i ++) {
        delete pgen_app_[i];
        pgen_app_[i] = nullptr;
    }
}

void P17_regs::start_() {
    curr_seq_ch_ = 0;
    g_done = false;

    std::unique_lock<std::mutex> lock(reg_mutex);
    if (main_thread == nullptr) {
        main_thread = new std::thread(&P17_regs::main_proc, this);
        //cout << "P17_TH:START" << endl;
    }
    if (pgen()) {
        for (unsigned int i = 0; i < pgen_app_.size(); i ++) {
            if (pgen_app_[i]->en()) {
                pgen_app_[i]->start();
            } else {
                pgen_app_[i]->stop();
            }
        }
    }
}
void P17_regs::stop(void) {

    std::unique_lock<std::mutex> lock(reg_mutex);
    for (unsigned int i = 0; i < pgen_app_.size(); i ++) {
        pgen_app_[i]->stop();
    }

    g_done = true;
    {
        std::unique_lock<std::mutex> lck(recir_mutex_);
        for (auto it : recir_match_) {
            delete(it);
        }
        recir_match_.clear();
    }
    {
        std::lock_guard<std::mutex> lock(port_down_mutex_);
        for (auto it: port_down_vec_) {
            delete(it);
        }
        port_down_vec_.clear();
    }

    if (main_thread != nullptr) {
        if (main_thread->joinable()) {
            main_thread->join();
            //cout << "P17:TH_STOP " << endl;
        }
        main_thread = nullptr;
    }
}

void P17_regs::recirc_reap_(void) {

    std::unique_lock<std::mutex> lck(recir_mutex_);
    auto it = recir_match_.begin();
    while (it != recir_match_.end()) {
        if ((*it)->is_done()) {
            delete(*it);
            it = recir_match_.erase(it);
        } else ++it;
    }
}

void P17_regs::port_reap_(bool all, uint32_t port) {

    std::lock_guard<std::mutex> lock(port_down_mutex_);
    auto it = port_down_vec_.begin();
    while (it != port_down_vec_.end()) {
        if (!all && (*it)->data() == port) {
            (*it)->sw_done();
        }
        if ((*it)->is_done()) {
            delete(*it);
            it = port_down_vec_.erase(it);
        } else ++it;
    }
}

// void P17_regs::tx_helper(Packet *p) {
//     uint8_t buf[64*1024] = {};
//     int port_num = p->port()->logical_port_index();
//     uint16_t len = p->len();
//     len = p->get_buf(buf, 0, len);
//     packet_enqueuer_->enqueue(port_num, buf, len);
// }

/*
 * Main process, just alternates between
 * recirc packet & pktgen packet
 * Packet gen has priority
 */
void P17_regs::main_proc(void) {
    Packet* g_packet = nullptr;
    Packet* r_packet = nullptr;
    uint8_t gen_ch_ = CHANNEL_UNDEF;
    uint8_t recir_ch_ = CHANNEL_UNDEF;
    uint8_t curr_ch_ = CHANNEL_UNDEF;
    bool deque_g_pkt = true;
    bool deque_r_pkt = true;
    bool do_next = false;
    while (!g_done) {
        std::this_thread::sleep_for(std::chrono::microseconds(dur_));
        // Lock out the host interface
        port_reap_(true, 0);
        recirc_reap_();
        curr_ch_ = get_channel(true);
        if (deque_g_pkt) {
            if(gen_q_.dequeue(g_packet)) {
                //cout << "DQ:G" << endl;
                gen_ch_ = g_packet->port()->parser_chan();
                if (curr_ch_ != gen_ch_) {
                    deque_g_pkt = false;
                    do_next = true;
                } else {
                    deque_g_pkt = true;
                    if (is_ch_enabled(curr_ch_)) {
                        //cout << "TX:G" << endl;
                        packet_enqueuer_->enqueue(g_packet->port()->port_index(), g_packet, true);
                        m_pktgen->g_pgen_cnt_++;
                    } else {
                        //cout << "DROP:G" << endl;
                        om_->pkt_delete(g_packet);
                        m_pktgen->g_drop_cnt_++;
                    }
                    g_packet = nullptr;
                    do_next = false;
                    continue;
                }
            } else do_next = true;

        } else { do_next = true; }
        if (do_next && deque_r_pkt) {
            if (recir_q_.dequeue(r_packet)) {
                //cout << "DQ:R" << endl;
                recir_ch_ = (r_packet->e2mac_metadata()->egress_unicast_port()) & 0x3;
                if (curr_ch_ != recir_ch_) {
                    deque_r_pkt = false;
                    do_next = true;
                } else {
                    deque_r_pkt = true;
                    if (!is_ch_enabled(curr_ch_)) {
                        //cout << "DROP:R" << endl;
                        om_->pkt_delete(r_packet);
                        m_pktgen->g_drop_cnt_++;
                    } else {
                        //cout << "TX:R" << endl;
                        m_pktgen->update_recirc_packet(r_packet);
                        packet_enqueuer_->enqueue(r_packet->port()->port_index(), r_packet, false);
                    }
                    do_next = false;
                    r_packet = nullptr;
                }

            } else do_next = true;
        }
        if (do_next && ((g_packet != nullptr) && (curr_ch_ == gen_ch_))) {
            if (is_ch_enabled(curr_ch_)) {
                //cout << "TX-2:G" << endl;
                packet_enqueuer_->enqueue(g_packet->port()->port_index(), g_packet, true);
                m_pktgen->g_pgen_cnt_++;
            } else {
                //cout << "DROP-2:G" << endl;
                om_->pkt_delete(g_packet);
                m_pktgen->g_drop_cnt_++;
            }
            g_packet = nullptr;
            deque_g_pkt = true;
            do_next = false;
        }
        if (do_next && ((r_packet != nullptr) && (curr_ch_ == recir_ch_))){
            if (!is_ch_enabled(curr_ch_)) {
                //cout << "DROP-2:R" << endl;
                om_->pkt_delete(r_packet);
                m_pktgen->g_drop_cnt_++;
            } else {
                //cout << "Tx-2:R" << endl;
                m_pktgen->update_recirc_packet(r_packet);
                packet_enqueuer_->enqueue(r_packet->port()->port_index(), r_packet, false);
            }
            r_packet = nullptr;
            deque_r_pkt = true;
        }
    }
}



void P16_regs::pg_port16ctrl_wb()	{
    curr_seq_ch_ = 0;

}
void P17_regs::pg_port17ctrl1_wb() {

    /* First enable the app threads */
    if (recirc() || pgen()) {
        start_();
    } else {
        stop();
    }
}

/*
 * WB from SW to clear port down
 */

void P17_regs::pg_portdowndis_wb() {
    for (unsigned int i = 0; i < RmtDefs::kParsers * RmtDefs::kParserChannels; i ++) {
        if (!pg_portdowndis.set(i)) {
            {

                port_reap_(false, i);
            }
        }
    }
}

} // namespace
