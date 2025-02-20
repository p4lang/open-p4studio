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

#ifndef _SHARED_MAU_IO_
#define _SHARED_MAU_IO_

#include <rmt-defs.h>
#include <mau-defs.h>
#include <pipe-object.h>
#include <nxt-tab.h>

namespace MODEL_CHIP_NAMESPACE {

  class Mau;
  
  class MauIO : public PipeObject {

    static constexpr int kType               = RmtTypes::kRmtTypeMauIO;
    static constexpr int kIngress            = 0;
    static constexpr int kEgress             = 1;
    static constexpr int kGhost              = 2;
    static constexpr int kNxtTabs            = 3; // Ingress,Egress,Ghost
    static constexpr int kSnapshotTriggereds = 2; // Ingress,Egress

 public:
    MauIO(RmtObjectManager *om, int pipeIndex, int mauIOIndex, Mau *mau)
        : PipeObject(om, pipeIndex, mauIOIndex, kType), mau_(mau) {
      reset();
    }
    MauIO(RmtObjectManager *om, int pipeIndex, int mauIOIndex)
        : MauIO(om, pipeIndex, mauIOIndex, NULL) { // Delegate CTOR
    }
    ~MauIO() { }
    
    void reset_snap() {
      for (int j = 0; j < kSnapshotTriggereds; j++)
        snapshot_triggereds_[j] = false;
    }
    void reset_pred() {
      for (int i = 0; i < kNxtTabs; i++) 
        nxt_tabs_[i] = NxtTab::inval_next_table();
      global_exec_ = 0;
      long_branch_ = 0;
      for (int i = 0; i < kNxtTabs; i++)
        mpr_nxt_tabs_[i] = NxtTab::inval_next_table();
      mpr_global_exec_ = 0;
      mpr_long_branch_ = 0;
    }
    void reset() {
      reset_snap();
      reset_pred();
    }
    
    // Getters
    Mau *mau() const override               { return mau_; }
    bool snapshot_triggered(int j)    const { return snapshot_triggereds_[j]; }
    bool ingress_snapshot_triggered() const { return snapshot_triggered(kIngress); }
    bool egress_snapshot_triggered()  const { return snapshot_triggered(kEgress); }

    int  nxt_tab(int i)    const { return nxt_tabs_[i]; }
    int  ingress_nxt_tab() const { return nxt_tab(kIngress); }
    int  egress_nxt_tab()  const { return nxt_tab(kEgress); }
    int  ghost_nxt_tab()   const { return nxt_tab(kGhost); } // Only used on JBay
    uint16_t global_exec() const { return global_exec_; } // Only used on JBay
    uint8_t  long_branch() const { return long_branch_; } // Only used on JBay

    int  mpr_nxt_tab(int i)    const { return mpr_nxt_tabs_[i]; }
    int  ingress_mpr_nxt_tab() const { return mpr_nxt_tab(kIngress); }
    int  egress_mpr_nxt_tab()  const { return mpr_nxt_tab(kEgress); }
    int  ghost_mpr_nxt_tab()   const { return mpr_nxt_tab(kGhost); } // Only used on JBay
    uint16_t mpr_global_exec() const { return mpr_global_exec_; } // Only used on JBay
    uint8_t  mpr_long_branch() const { return mpr_long_branch_; } // Only used on JBay

    
    // Setters
    void set_snapshot_triggered(int j, bool tf=true)  { snapshot_triggereds_[j] = tf; }
    void set_ingress_snapshot_triggered(bool tf=true) { set_snapshot_triggered(kIngress, tf); }
    void set_egress_snapshot_triggered(bool tf=true)  { set_snapshot_triggered(kEgress, tf); }

    void set_nxt_tab(int i, int nxt_tab) {
      // Allow invalid nxt_tab values to be set (if positive), but always mask them appropriately
      //nxt_tabs_[i] = NxtTab::next_table_ok(nxt_tab) ?nxt_tab :NxtTab::inval_next_table();
      nxt_tabs_[i] = (nxt_tab >= 0) ?nxt_tab & NxtTab::next_table_mask() :NxtTab::inval_next_table();
    }
    void set_ingress_nxt_tab(int nxt_tab)      { set_nxt_tab(kIngress, nxt_tab); }
    void set_egress_nxt_tab(int nxt_tab)       { set_nxt_tab(kEgress, nxt_tab); }
    void set_ghost_nxt_tab(int nxt_tab)        { set_nxt_tab(kGhost, nxt_tab); } // JBay only
    void set_global_exec(uint16_t global_exec) { global_exec_ = global_exec; } // JBay only
    void set_long_branch(uint8_t  long_branch) { long_branch_ = long_branch; } // JBay only
    void set_pred(uint16_t global_exec, uint8_t long_branch) {
      set_global_exec(global_exec); set_long_branch(long_branch);
    }
    void set_pred(int i_nxt, int e_nxt, int g_nxt, uint16_t global_exec, uint8_t long_branch) {
      set_ingress_nxt_tab(i_nxt); set_egress_nxt_tab(e_nxt); set_ghost_nxt_tab(g_nxt);
      set_pred(global_exec, long_branch);
    }

    void set_mpr_nxt_tab(int i, int nxt_tab) {
      //mpr_nxt_tabs_[i] = NxtTab::next_table_ok(nxt_tab) ?nxt_tab :NxtTab::inval_next_table();
      mpr_nxt_tabs_[i] = (nxt_tab >= 0) ?nxt_tab & NxtTab::next_table_mask() :NxtTab::inval_next_table();
    }
    void set_ingress_mpr_nxt_tab(int nxt_tab)      { set_mpr_nxt_tab(kIngress, nxt_tab); }
    void set_egress_mpr_nxt_tab(int nxt_tab)       { set_mpr_nxt_tab(kEgress, nxt_tab); }
    void set_ghost_mpr_nxt_tab(int nxt_tab)        { set_mpr_nxt_tab(kGhost, nxt_tab); } // JBay only
    void set_mpr_global_exec(uint16_t global_exec) { mpr_global_exec_ = global_exec; } // JBay only
    void set_mpr_long_branch(uint8_t  long_branch) { mpr_long_branch_ = long_branch; } // JBay only
    void set_mpr(uint16_t global_exec, uint8_t long_branch) {
      set_mpr_global_exec(global_exec); set_mpr_long_branch(long_branch);
    }
    void set_mpr(int i_nxt, int e_nxt, int g_nxt, uint16_t global_exec, uint8_t long_branch) {
      set_ingress_mpr_nxt_tab(i_nxt); set_egress_mpr_nxt_tab(e_nxt); set_ghost_mpr_nxt_tab(g_nxt);
      set_mpr(global_exec, long_branch);
    }

 private:
    Mau                                          *mau_;
    std::array< bool,     kSnapshotTriggereds >   snapshot_triggereds_;
    std::array< uint16_t, kNxtTabs >              nxt_tabs_;
    uint16_t                                      global_exec_;
    uint8_t                                       long_branch_;
    std::array< uint16_t, kNxtTabs >              mpr_nxt_tabs_;
    uint16_t                                      mpr_global_exec_;
    uint8_t                                       mpr_long_branch_;
    
  }; // MauIO
    
}

#endif // _SHARED_MAU_IO_
