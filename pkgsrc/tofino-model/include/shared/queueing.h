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

#ifndef _SHARED_QUEUING_
#define _SHARED_QUEUING_

#include <iostream>
#include <mutex>
#include <queue>
#include <rmt-defs.h>
#include <rmt-object.h>
#include <packet.h>
#include <port.h>

namespace MODEL_CHIP_NAMESPACE {

  class Queueing : private RmtObject {
    static constexpr int kQGroups = RmtDefs::kTmPipesMax;
    static constexpr int kQsPerGroup = RmtDefs::kPortGroupsPerPipe * RmtDefs::kQidsPerPortGroup;

  public:
    static bool kAllowMissingDies; // This defined in rmt-config.cpp

    // After an ingress packet has passed the deparser, it can be passed into
    // the enqueue() function with a pointer to the packet.
    void enqueue(Packet *pkt);
    void enqueue_and_resume(Packet *pkt);
    // Enqueues a packet against the specified port with the specified cos.
    void enqueue_mc_copy(Packet *pkt, int port);
    // For the specified pipeline, dequeue (return) the next packet.  Sets
    // pkt_out to NULL if there are no packets.
    void dequeue(int pipe, int portGrp, int qid, Packet *&pkt_out);
    void dequeue(int pipe, int portGrp, Packet *&pkt_out);
    void dequeue(int pipe, Packet *&pkt_out);
    void dequeue(Packet *&pkt_out);
    // Returns true if the specified pipeilne has packets which can/need to
    // be dequeued.
    bool pipeHasPackets(int pipe);
    bool portGrpHasPackets(int pipe, int portGrp);
    bool queueHasPackets(int pipe, int portGrp, int qid);
    // Returns true if any packets are ready to dequeue.
    bool hasPackets();

    // These funcs to handle multi-chip scenarios
    void enqueue_local(Packet *pkt, int port, bool resume);
    void enqueue_local_read(Packet *pkt, int port, bool resume);
    // These next ones CHIP_SPECIFIC
    void enqueue_read(Packet *pkt, int port);
    void enqueue_local_die(Packet *pkt);
    void enqueue_write_die(Packet *pkt);
    void enqueue_die(Packet *pkt);


    // Constructor / Destructor
    Queueing(RmtObjectManager *om) : RmtObject(om) {};
    ~Queueing() {};

  private:
    void add_to_queue(Packet *pkt, int port);
    // FIXME - This should be a configuration table giving per port;
    //         100g, 4 bit port, 5 bit qid
    //          40g, 5 bit port, 4 bit qid
    //          10g, 6 bit port, 3 bit qid
    uint16_t queue_id(int pipe, int port, uint8_t qid) { return ((Port::get_group_num(port) << RmtDefs::kI2qQidWidth) | (qid & RmtDefs::kI2qQidMask)); }

    std::atomic<uint32_t>                                              ts_cntr_     = {};
    std::array<std::array<std::queue<Packet*>, kQsPerGroup>, kQGroups> port_queues_;
    std::array<std::array<std::mutex,          kQsPerGroup>, kQGroups> queue_mtxs_  = {};

    // for UT - simulate pkt drop
    uint32_t    dod_pkt_counter_ = 0;

    bool q_full(uint8_t pipe, int32_t  queue) { (void)pipe; (void)queue;
                                                return (++dod_pkt_counter_ % 10) == 0; }
    uint32_t qfull_qsize(uint8_t pipe, int32_t  queue) { (void)pipe; (void)queue;
                                                return dod_pkt_counter_ * 100; }
  };
}

#endif
