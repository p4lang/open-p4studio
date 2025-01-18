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

#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <algorithm>
#include <common/bounded_queue.h>
#include <model_core/timer-manager.h>
#include <model_core/timer.h>
#include <pktgen-reg.h>
#include <pktgen-app-reg.h>
#include <register_adapters.h>
#include <mcn_test.h>



namespace MODEL_CHIP_NAMESPACE {

using namespace std;
using namespace model_common;


app_counters::app_counters(int chip, int pipe, int app_id):
    pg_batchctr(parser_adapter(pg_batchctr,chip, pipe, app_id)),
    pg_pktctr(parser_adapter(pg_pktctr,chip, pipe, app_id)),
    pg_triggerctr(parser_adapter(pg_triggerctr,chip, pipe, app_id)){
        reset_all();
    }


app_timer::app_timer(PktGenAppReg* p):
    b_pkt(nullptr),
    pgen_(p),
    ing_p(0),
    ing_p_inc(false),
    o_recir_h(0),
    n_recir_h(0),
    down_port(0),
    ch_(0),
    ibg_t([&](uint64_t tid) { this->batch_cb(); }),
    ipg_t([&](uint64_t tid) { this->pkt_cb(); }),
    lfsr_b_t([&](uint64_t tid) { this->lfsr_b_cb(); }),
    lfsr_p_t([&](uint64_t tid) { this->lfsr_p_cb(); }),
    p_count(0),
    b_count(0),
    lfsr_p_val(0),
    lfsr_b_val(0),
    b_expired(false) {


    }


app_timer::~app_timer() {
    if (b_pkt != nullptr) {
        pgen_->m_pgen->om_->pkt_delete(b_pkt);
        b_pkt = nullptr;
    }
}

void app_timer::lfsr_b_cb(void) {
    if (pgen_->g_done) return;

    // Poke the LFSR
    //
    pgen_->m_pgen->lfsr_.clock(pgen_->m_app_config.diff_clks);
    uint64_t val = pgen_->m_pgen->lfsr_.get_value();
    uint32_t lfsr_val = ((val & 0xFFFFFFFF) & (~pgen_->m_app_config.event_ibg_jitter_mask));

    lfsr_b_t.stop();
    if (lfsr_val <= lfsr_b_val) {
        if(p_count.load() < pgen_->m_app_config.packet_num) {
            b_expired = true;
        } else {
            b_count ++;
            new_batch_();
        }
    } else {
        lfsr_b_t.run(1, model_timer::Timer::Once);
    }

}
void app_timer::lfsr_p_cb(void) {
    if (pgen_->g_done) return;

    // Poke the LFSR
    pgen_->m_pgen->lfsr_.clock(pgen_->m_app_config.ipg_count);
    uint32_t lfsr_val = ((pgen_->m_pgen->lfsr_.get_value() & 0xFFFFFFFF) & (~pgen_->m_app_config.event_ipg_jitter_mask));
    lfsr_p_t.stop();
    if (lfsr_val <= lfsr_p_val) {
        new_packet_();
    } else {
        lfsr_p_t.run(1, model_timer::Timer::Once);
    }
}

void app_timer::new_batch_(void) {

    if (pgen_->g_done) return;

    if (b_count.load() == 0) {
        pgen_->m_app_counters.batch_inc();
        p_count = 0;
        b_expired = false;
        ibg_t.run(pgen_->m_app_config.ibg_count, model_timer::Timer::Once);
        new_packet_();
    } else {
        if (b_count.load() < pgen_->m_app_config.batch_num) {
            pgen_->m_app_counters.batch_inc();
            if (p_count.load() == pgen_->m_app_config.packet_num) {
                p_count = 0;
                ibg_t.stop();
                b_expired = false;
                ibg_t.run(pgen_->m_app_config.ibg_count, model_timer::Timer::Once);
                new_packet_();
            }
        } else {
            stop();

        }
    }
}


void app_timer::new_packet_(void) {
    uint64_t tm_;
    model_timer::ModelTimerGetTime(tm_);

    if (pgen_->g_done) return;

    if (p_count.load() == pgen_->m_app_config.packet_num) {
        if (b_expired.load()) {
            if (b_count.load() == (pgen_->m_app_config.batch_num - 1)) {
                stop();
                return;
            }
            b_count ++;
            new_batch_();
        }
    } else {
        //cout << "CLK:" << tm_ << ":APP:" << static_cast<uint16_t>(pgen_->m_id) << ":Batch:" << b_count << ":Packet:" << p_count << endl;
        gen_packet_();
        p_count ++;
        ipg_t.stop();
        ipg_t.run(pgen_->m_app_config.ipg_count, model_timer::Timer::Once);
    }

}

void app_timer::pkt_cb(void) {
    if (pgen_->g_done) return;

    bool do_lfsr = (((pgen_->m_app_config.event_ipg_jitter_value) & ~(pgen_->m_app_config.event_ipg_jitter_mask)) != 0);
    if (do_lfsr) {
        lfsr_p_val = (pgen_->m_app_config.event_ipg_jitter_value & (~pgen_->m_app_config.event_ipg_jitter_mask));
        lfsr_p_t.run(1, model_timer::Timer::Once);
    } else {
        new_packet_();
    }
}

void app_timer::batch_cb(void) {
    if (pgen_->g_done) return;

    bool do_lfsr = (((pgen_->m_app_config.event_ibg_jitter_value) & ~(pgen_->m_app_config.event_ibg_jitter_mask)) != 0);
    if (do_lfsr) {
        lfsr_b_val = (pgen_->m_app_config.event_ibg_jitter_value & (~pgen_->m_app_config.event_ibg_jitter_mask));
        lfsr_b_t.run(1, model_timer::Timer::Once);
    } else {
        if (p_count.load() < pgen_->m_app_config.packet_num) {
            b_expired = true;
        } else {
            b_count ++;
            new_batch_();
        }
    }
}

void app_timer::start(void) {
    if (pgen_->g_done) return;
    setup_data_();
    b_count = 0;
    new_batch_();
}

void app_timer::stop(void) {
    ibg_t.stop();
    ipg_t.stop();
    lfsr_b_t.stop();
    lfsr_p_t.stop();
    atomic_thread_fence(std::memory_order_release);
    pgen_->m_pgen->ev_flag_ = true;
}

void app_timer::gen_packet_(void) {

    Packet* e_packet = pgen_->m_pgen->om_->pkt_create();
    e_packet->append(b_pkt);

    // Reset the port number to the configured base at the start of each batch.
    if (ing_p_inc && !p_count)
        ing_p = pgen_->m_app_config.app_ingr_port;

    auto gen_pipe = (pgen_->m_pgen->pg_port17ctrl2.pgen_pipe_id() & 0x3);

    PacketBuffer* header_ = nullptr;
    uint8_t p_header[6];
    p_header[5] = (p_count) & 0xFF;
    p_header[4] = (p_count >> 8) & 0xFF;
    switch(pgen_->m_app_config.app_type) {
        case TRIGGER_TYPE_E::ONETIME:
        case TRIGGER_TYPE_E::PERIODIC:
            p_header[3] = (b_count) & 0xFF;
            p_header[2] = (b_count >> 8) & 0xFF;
            p_header[1] = 0;
            break;
        case TRIGGER_TYPE_E::PORT_DOWN:
            p_header[3] = down_port & 0x7F;
            p_header[3] |= ((gen_pipe & 0x1) << 7);
            p_header[2] =  (gen_pipe >> 1);
            p_header[1] = 0;
            break;

        case TRIGGER_TYPE_E::PACKET_RECIRC:
            p_header[3] = (n_recir_h)&0xFF;
            p_header[2] = (n_recir_h >> 8)&0XFF;
            if (p_count == pgen_->m_app_config.packet_num - 1) {
                n_recir_h++;
            }
            p_header[1] = (o_recir_h >> 16) & 0xFF;
            break;
    }
    p_header[0] = (pgen_->m_id & 0x7) | (gen_pipe << 3);
    header_ = new PacketBuffer(p_header, sizeof(p_header));
    e_packet->prepend(header_);
    if (pgen_->m_pgen->m_pktgen->get_test()) {
        e_packet->i2qing_metadata()->set_egress_unicast_port(ing_p);
    }
    // Physical ingress port is set to the generated port
    e_packet->i2qing_metadata()->set_physical_ingress_port(ing_p);
    // Port is set to the Packet Gen P17 + channel
    uint16_t port = pgen_->m_pgen->m_pktgen->get_port_num();
    uint16_t i_port = ( pgen_->m_pipe << 7) | ((port << 2) | ch_);
    e_packet->set_port(pgen_->m_pgen->om_->port_get(i_port));
    e_packet->set_ingress();
    e_packet->set_generated(true);
    uint64_t tm_;
    model_timer::ModelTimerGetTime(tm_);
    e_packet->set_generated_T(tm_);

    if (ing_p_inc) {
        ing_p = (ing_p + 1)%RmtDefs::kPortsTotal;
    }
    if (!pgen_->m_pgen->gen_q_.enqueue(std::move(e_packet))) {
        pgen_->m_pgen->m_pktgen->g_drop_cnt_++;
        e_packet = nullptr;
    }
    pgen_->m_app_counters.packet_inc();
}

uint8_t app_timer::next_channel_(void) {
    uint8_t mask = pgen_->m_pgen->pg_port17ctrl2.pgen_channel_en();
    if (!mask) pgen_->set_en(0);
    uint8_t ch_idx_ = ch_;
    while (1) {
        if ((mask >> ch_idx_) & 0x1) return ch_idx_;
        ch_idx_++;
        ch_idx_%=RmtDefs::kChlsPerIpb;
    }
    return ch_idx_;
}

void app_timer::setup_data_(void) {

    if (b_pkt != nullptr) {
        pgen_->m_pgen->om_->pkt_delete(b_pkt);
        b_pkt = nullptr;
    }
    if (pgen_->m_app_config.payload_buf != nullptr) {
        b_pkt = pgen_->m_pgen->om_->pkt_create(pgen_->m_app_config.payload_buf, pgen_->m_app_config.payload_buf_sz);
    } else {
        b_pkt = pgen_->m_pgen->om_->pkt_create();
    }
    ch_ = next_channel_();
    ing_p = pgen_->m_app_config.app_ingr_port;
    ing_p_inc = (pgen_->m_app_config.app_ingr_port_inc == 1);
    if (pgen_->m_app_config.app_type == TRIGGER_TYPE_E::PORT_DOWN) {
        EvData* p = nullptr;
        for (auto& it : pgen_->m_pgen->port_down_vec_) {
            if (it->is_app_set(pgen_->m_id)) {
                p = (it);
                break;
            }

        }
        if (p != nullptr) {
            down_port = p->data();
            p->app_done(pgen_->m_id);
        }

    } else if (pgen_->m_app_config.app_type == TRIGGER_TYPE_E::PACKET_RECIRC) {
        EvData* p = nullptr;
        o_recir_h = 0;
        n_recir_h = 0;
        {
            std::unique_lock<std::mutex> lck(pgen_->m_pgen->recir_mutex_);
            for (auto& it : pgen_->m_pgen->recir_match_) {
                if ((it)->is_app_set(pgen_->m_id)) {
                    p = (it);
                    break;
                }
            }

            if (p != nullptr) {
                o_recir_h = n_recir_h = p->data();
                p->app_done(pgen_->m_id);
            }
        }
    }
}




PktGenAppReg::PktGenAppReg(int chip, int pipe, int app_id, P17_regs* pgen):
    m_app_config(),
    m_app_counters(chip, pipe, app_id),
    a_timer(this),
    m_pgen(pgen),
    m_pipe(pipe),
    m_id(app_id),
    g_done(false),
    app_th(nullptr),
    _en(0),
    dur_(pgen->dur_),
    recir_done_(false),
    main_t([this](uint64_t tid) { this->main_cb(tid); }),
    pg_ctrl(parser_adapter(pg_ctrl,chip, pipe, app_id, [this](){this->pg_ctrl_wb();})),
    pg_eventibg(parser_adapter(pg_eventibg,chip,pipe,app_id)),
    pg_eventipg(parser_adapter(pg_eventipg,chip,pipe,app_id)),
    pg_eventnumber(parser_adapter(pg_eventnumber,chip,pipe,app_id)),
    pg_eventtimer(parser_adapter(pg_eventtimer,chip,pipe,app_id)),
    pg_portctrl(parser_adapter(pg_portctrl,chip,pipe,app_id)),
    pg_payloadctrl(parser_adapter(pg_payloadctrl,chip,pipe,app_id)),
    pg_recirmatchmask(parser_adapter(pg_recirmatchmask,chip,pipe,app_id)),
    pg_recirmatchvalue(parser_adapter(pg_recirmatchvalue,chip,pipe,app_id)),
    pg_ipgjittermask(parser_adapter(pg_ipgjittermask,chip, pipe, app_id, register_classes::PgrAppEventJitterMask::PartyPgrAppRegRspecEnum::kEventIpgJitterMask)),
    pg_ipgjittervalue(parser_adapter(pg_ipgjittervalue,chip, pipe, app_id, register_classes::PgrAppEventJitterValue::PartyPgrAppRegRspecEnum::kEventIpgJitterValue)),
    pg_ibgjittermask(parser_adapter(pg_ibgjittermask,chip, pipe, app_id, register_classes::PgrAppEventJitterMask::PartyPgrAppRegRspecEnum::kEventIbgJitterMask)),
    pg_ibgjittervalue(parser_adapter(pg_ibgjittervalue,chip, pipe, app_id, register_classes::PgrAppEventJitterValue::PartyPgrAppRegRspecEnum::kEventIbgJitterValue)){
        reset_();
    }

void PktGenAppReg::main_cb(uint64_t tid) {
    if (g_done) return;

    uint64_t ev_id = m_pgen->ev_ids_.next();
    uint64_t tm_;
    model_timer::ModelTimerGetTime(tm_);
    //cout << "CLK:" << tm_ << ":APP:" << static_cast<uint16_t>(m_id) << ":EVENT_QED#:" << ev_id << endl;
    ev_q_.push(ev_id);
}


PktGenAppReg::~PktGenAppReg() {
    stop();
}


void PktGenAppReg::start(void) {
    if (app_th == nullptr) {
        setup_app_();
        g_done = false;
        app_th = new std::thread(&PktGenAppReg::app_process_, this);
    }
}


void PktGenAppReg::stop(void) {
    //std::lock_guard<std::mutex> lock(m_pgen->reg_mutex);
    if (app_th != nullptr) {
        g_done = true;
        if (app_th->joinable()) {
            app_th->join();
        }
        app_th = nullptr;
    }
}



void PktGenAppReg::reset_(void) {
    pg_ctrl.reset();
    pg_eventibg.reset();
    pg_eventnumber.reset();
    pg_eventtimer.reset();
    pg_portctrl.reset();
    pg_payloadctrl.reset();
    pg_recirmatchmask.reset();
    pg_recirmatchvalue.reset();
    pg_ipgjittermask.reset();
    pg_ipgjittervalue.reset();
    pg_ibgjittermask.reset();
    pg_ibgjittervalue.reset();
    m_app_counters.reset_all();
}

void PktGenAppReg::process_ev_(void) {
    if (g_done) return;
    RMT_ASSERT(!ev_q_.empty());
    uint64_t tm_;
    //uint64_t ev_id = ev_q_.front();
    std::unique_lock<std::mutex> l(m_pgen->ev_mutex_);

    if (m_pgen->port_cv_.wait_for(l, std::chrono::microseconds(dur_), [&]() { return (m_pgen->ev_flag_.load() && !g_done); })) {
        m_pgen->ev_flag_ = false;
        ev_q_.pop();
        m_pgen->m_pktgen->g_triggers_++;
        m_app_counters.trigger_inc();
        model_timer::ModelTimerGetTime(tm_);
        //cout << "CLK:" << tm_ << ":APP:" << static_cast<uint16_t>(m_id) << ":EV_START:" << ev_id << endl;
        _copy_all();
        a_timer.start();
    }
}



void PktGenAppReg::app_process_(void) {


    while(!g_done) {
        if (ev_q_.empty()) {
            std::this_thread::sleep_for(std::chrono::microseconds(dur_));
            continue;
        }
        // Acquire resource lock
        std::unique_lock<std::mutex> r_lock(r_mutex);
        if (r_cv.wait_for(r_lock, std::chrono::microseconds(dur_), [&]() { return (!ev_q_.empty() && !g_done); })) {
            if (m_pgen->ev_flag_.load()) process_ev_();
            if (!g_done && (m_app_config.app_type == TRIGGER_TYPE_E::PERIODIC)) {
                main_t.stop();
                main_t.run(m_app_config.event_timer, model_timer::Timer::Once);
            }
        }
        r_lock.unlock();
    }
    clear_pending_();
    a_timer.stop();
}

void PktGenAppReg::_copy_all(void) {

    if (m_app_config.payload_buf != nullptr) {
        delete[] m_app_config.payload_buf;
        m_app_config.payload_buf = nullptr;
    }

    m_app_config.payload_buf_sz = pg_payloadctrl.app_payload_size();
    if (m_app_config.payload_buf_sz != 0) {
        m_app_config.payload_buf = new uint8_t[m_app_config.payload_buf_sz];
        m_pgen->pgen_buffer.get_val(m_app_config.payload_buf,
                pg_payloadctrl.app_payload_addr(),
                m_app_config.payload_buf_sz);
    }
    m_app_config.app_ingr_port = pg_portctrl.app_ingr_port();
    m_app_config.app_ingr_port_inc = pg_portctrl.app_ingr_port_inc();
    m_app_config.recir_match_value = pg_recirmatchvalue.recir_match_value();
    m_app_config.recir_match_mask = pg_recirmatchmask.recir_match_mask();
    m_app_config.batch_num = pg_eventnumber.batch_num() + 1;
    m_app_config.packet_num = pg_eventnumber.packet_num() + 1;
    m_app_config.ibg_count = pg_eventibg.ibg_count();
    if (m_app_config.ibg_count == 0) m_app_config.ibg_count++;
    m_app_config.ipg_count = pg_eventipg.ipg_count();
    if (m_app_config.ipg_count == 0) m_app_config.ipg_count++;

    m_app_config.diff_clks = (m_app_config.ibg_count) - (m_app_config.ipg_count * (m_app_config.packet_num - 1));
    if (m_app_config.diff_clks <= 0) m_app_config.diff_clks = 1;

    m_app_config.event_timer = pg_eventtimer.timer_count();
    if (m_app_config.event_timer == 0) m_app_config.event_timer ++;

    m_app_config.event_ipg_jitter_mask = pg_ipgjittermask.mask();
    m_app_config.event_ipg_jitter_value = pg_ipgjittervalue.value();
    m_app_config.event_ibg_jitter_mask = pg_ibgjittermask.mask();
    m_app_config.event_ibg_jitter_value = pg_ibgjittervalue.value();

}


void PktGenAppReg::clear_ports_(void) {
    std::lock_guard<std::mutex> lock(m_pgen->port_down_mutex_);
    for(auto&& m_ev: m_pgen->port_down_vec_) {
        m_ev->app_done(m_id);
    }
}
void PktGenAppReg::clear_recir_(void) {

    std::unique_lock<std::mutex> lck(m_pgen->recir_mutex_);
    for(auto&& m_ev:m_pgen->recir_match_) {
        m_ev->app_done(m_id);
    }
}


void PktGenAppReg::clear_pending_(void) {
    switch(m_app_config.app_type) {
        case TRIGGER_TYPE_E::ONETIME:
        case TRIGGER_TYPE_E::PERIODIC:
            main_t.stop();
            break;
        case TRIGGER_TYPE_E::PORT_DOWN:
            clear_ports_();
            break;
        case TRIGGER_TYPE_E::PACKET_RECIRC:
            clear_recir_();
            break;
    }
    while (!ev_q_.empty())ev_q_.pop();
}

void PktGenAppReg::setup_app_(void) {
    uint64_t tm_;
    model_timer::ModelTimerGetTime(tm_);

    m_app_config.app_type = pg_ctrl.app_type();
    m_app_config.event_timer = pg_eventtimer.timer_count();
    if (m_app_config.event_timer == 0) m_app_config.event_timer ++;
    m_app_config.recir_match_value = pg_recirmatchvalue.recir_match_value();
    m_app_config.recir_match_mask = pg_recirmatchmask.recir_match_mask();
    if((m_app_config.app_type == TRIGGER_TYPE_E::ONETIME) ||
            (m_app_config.app_type == TRIGGER_TYPE_E::PERIODIC)) {
        main_t.run(m_app_config.event_timer, model_timer::Timer::Once);
    }
}

void PktGenAppReg::pg_ctrl_wb() {
    if (_en == pg_ctrl.app_en()) return;
    _en = pg_ctrl.app_en();
    if (m_pgen->pgen()) {
        if (en()) {
            start();
        } else {
            stop();
        }
    } else {
        stop();
    }

}


/*
 * Peek Support: Not needed, keeping here just in case
 *
 bool PktGenAppReg::recir_match_ev_(void) {
 bool got_packet = false;
 bool match_flag_ = false;
 got_packet = m_pgen->recir_q_.peek([&](Packet* r_pkt){
 RMT_ASSERT(r_pkt != nullptr);
 match_flag_ = false;
 if (r_pkt->i2qing_metadata()->peek_done()) return;
 uint8_t buf[4];
 r_pkt->get_buf(buf, 0, sizeof(buf));
 uint32_t val = 0;
 for (unsigned int i = 0; i < sizeof(buf); i ++) {
 val |= (buf[i]);
 val <<= 8;
 }
 val |= m_app_config.recir_match_mask;
 if (val == (m_app_config.recir_match_value | m_app_config.recir_match_mask)) {
 match_flag_ = true;
 m_app_config.recir_key = val;
 recir_done_ = (((val >> 31) & 0x1) == 0x1);
 match_flag_ &= !recir_done_;
 }
 r_pkt->i2qing_metadata()->set_peek_done(true);

 });
 if (!got_packet) return false;
 return match_flag_;

 }
 */
/* Peek Logic
   if (recir_done_ && a_timer.is_running) {
   recir_done_ = false;
   a_timer.stop();
   m_pgen->port_flag_ = true;
   l.unlock();
   continue;
   }
   if (!recir_match_ev_()) {
   l.unlock();
   m_pgen->port_flag_ = true;
   continue;
   }
   if (m_app_config.app_type == TRIGGER_TYPE_E::PORT_DOWN) {
   } else if (m_app_config.app_type == TRIGGER_TYPE_E::PACKET_RECIRC) {
   a_timer.recir_key_0 = ((m_app_config.recir_key)>>24)&0xFF;
   a_timer.recir_key_1 = ((m_app_config.recir_key)) & 0xFFFF;
   }
   a_timer.start();
   while (a_timer.is_running) {
   if (!_en || g_done) break;
   std::this_thread::sleep_for(std::chrono::microseconds(dur_));
   }
   a_timer.stop();
   m_pgen->port_flag_ = true;
   l.unlock();
   }
   }
   */
}
