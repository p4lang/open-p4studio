################################################################################
 #  Copyright (C) 2024 Intel Corporation
 #
 #  Licensed under the Apache License, Version 2.0 (the "License");
 #  you may not use this file except in compliance with the License.
 #  You may obtain a copy of the License at
 #
 #  http://www.apache.org/licenses/LICENSE-2.0
 #
 #  Unless required by applicable law or agreed to in writing,
 #  software distributed under the License is distributed on an "AS IS" BASIS,
 #  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 #  See the License for the specific language governing permissions
 #  and limitations under the License.
 #
 #
 #  SPDX-License-Identifier: Apache-2.0
################################################################################

import cog
import re

# generated meta_vars using: ~/tmp/extract_dprsr_meta.py dprsr_reg_defs.csr
meta_vars = [
    {'type':'dprsr_meta_pov_dis_shft_r', 'name':'m_learn_sel', 'string':'Learn Quantum table selector', 'group':'dprsr_input_ingress_only_g' },
    {'type':'dprsr_meta_pov_dis_shft_r', 'name':'m_resub_sel', 'string':'Resubmit table selector', 'group':'dprsr_input_ingress_only_g' },
    {'type':'dprsr_meta_pov_dis_shft_r', 'name':'m_pgen', 'string':'Packet Generator flag', 'group':'dprsr_input_ingress_only_g' },
    {'type':'dprsr_meta_pov_dis_shft_r', 'name':'m_pgen_len', 'string':'Packet Generator Length field', 'group':'dprsr_input_ingress_only_g' },
    {'type':'dprsr_meta_pov_dis_shft_r', 'name':'m_pgen_addr', 'string':'Packet Generator Address field', 'group':'dprsr_input_ingress_only_g' },
    {'type':'dprsr_meta_pov_dis_r', 'name':'m_egress_unicast_port', 'string':'Unicast Egress port', 'group':'dprsr_input_ingress_only_g' },
    {'type':'dprsr_meta_pov_dis_r', 'name':'m_mgid1', 'string':'Multicast ID 1 (A)', 'group':'dprsr_input_ingress_only_g' },
    {'type':'dprsr_meta_pov_dis_r', 'name':'m_mgid2', 'string':'Multicast ID 2 (B)', 'group':'dprsr_input_ingress_only_g' },
    {'type':'dprsr_meta_pov_dis_shft_r', 'name':'m_copy_to_cpu', 'string':'Copy-to-CPU', 'group':'dprsr_input_ingress_only_g' },
    {'type':'dprsr_meta_pov_dis_shft_r', 'name':'m_mirr_sel', 'string':'Mirror Table Selector', 'group':'dprsr_input_ingress_only_g' },
    {'type':'dprsr_meta_pov_dis_shft_r', 'name':'m_drop_ctl', 'string':'Pkt Drop Control', 'group':'dprsr_input_ingress_only_g' },
    {'type':'dprsr_meta_pov_dis_r', 'name':'m_egress_unicast_port', 'string':'Unicast Egress port', 'group':'dprsr_input_egress_only_g' },
    {'type':'dprsr_meta_pov_dis_shft_r', 'name':'m_mirr_sel', 'string':'Mirror Table Selector', 'group':'dprsr_input_egress_only_g' },
    {'type':'dprsr_meta_pov_dis_shft_r', 'name':'m_drop_ctl', 'string':'Pkt Drop Control', 'group':'dprsr_input_egress_only_g' },
    {'type':'dprsr_meta_pov_r', 'name':'m_hash1', 'string':'Hash value 1', 'group':'dprsr_ingress_hdr_meta_for_input_g' },
    {'type':'dprsr_meta_pov_r', 'name':'m_hash2', 'string':'Hash value 2', 'group':'dprsr_ingress_hdr_meta_for_input_g' },
    {'type':'dprsr_meta_pov_r', 'name':'m_copy_to_cpu_cos', 'string':'CoS for Copy-to-CPU packet', 'group':'dprsr_ingress_hdr_meta_for_input_g' },
    {'type':'dprsr_meta_pov_r', 'name':'m_deflect_on_drop', 'string':'Deflect-on-Drop flag', 'group':'dprsr_ingress_hdr_meta_for_input_g' },
    {'type':'dprsr_meta_pov_r', 'name':'m_icos', 'string':'Class of Service  (Cos)', 'group':'dprsr_ingress_hdr_meta_for_input_g' },
    {'type':'dprsr_meta_pov_r', 'name':'m_pkt_color', 'string':'Packet color', 'group':'dprsr_ingress_hdr_meta_for_input_g' },
    {'type':'dprsr_meta_pov_r', 'name':'m_qid', 'string':'Queue ID', 'group':'dprsr_ingress_hdr_meta_for_input_g' },
    {'type':'dprsr_meta_pov_r', 'name':'m_xid_l1', 'string':'Level 1 XID', 'group':'dprsr_ingress_hdr_meta_for_input_g' },
    {'type':'dprsr_meta_pov_r', 'name':'m_xid_l2', 'string':'Level 2 XID', 'group':'dprsr_ingress_hdr_meta_for_input_g' },
    {'type':'dprsr_meta_pov_r', 'name':'m_rid', 'string':'Replication ID  (RID)', 'group':'dprsr_ingress_hdr_meta_for_input_g' },
    {'type':'dprsr_meta_pov_r', 'name':'m_bypss_egr', 'string':'Egress bypass flag', 'group':'dprsr_ingress_hdr_meta_for_input_g' },
    {'type':'dprsr_meta_pov_r', 'name':'m_ct_disable', 'string':'Cut-through Disable flag', 'group':'dprsr_ingress_hdr_meta_for_input_g' },
    {'type':'dprsr_meta_pov_r', 'name':'m_ct_mcast', 'string':'Multicast Cut-through Enable flag', 'group':'dprsr_ingress_hdr_meta_for_input_g' },
    {'type':'dprsr_meta_pov_r', 'name':'m_mirr_io_sel', 'string':'Mirror Incoming or Outgoing packet', 'group':'dprsr_ingress_hdr_meta_for_input_g' },
    {'type':'dprsr_meta_pov_r', 'name':'m_mirr_hash', 'string':'Mirror Hash', 'group':'dprsr_ingress_hdr_meta_for_input_g' },
    {'type':'dprsr_meta_pov_r', 'name':'m_mirr_epipe_port', 'string':'Mirror Epipe port', 'group':'dprsr_ingress_hdr_meta_for_input_g' },
    {'type':'dprsr_meta_pov_r', 'name':'m_mirr_qid', 'string':'Mirror QID', 'group':'dprsr_ingress_hdr_meta_for_input_g' },
    {'type':'dprsr_meta_pov_r', 'name':'m_mirr_dond_ctrl', 'string':'Mirror Deflect on Drop ctrl', 'group':'dprsr_ingress_hdr_meta_for_input_g' },
    {'type':'dprsr_meta_pov_r', 'name':'m_mirr_icos', 'string':'Mirror ICOS', 'group':'dprsr_ingress_hdr_meta_for_input_g' },
    {'type':'dprsr_meta_pov_r', 'name':'m_mirr_mc_ctrl', 'string':'Mirror MC ctrl', 'group':'dprsr_ingress_hdr_meta_for_input_g' },
    {'type':'dprsr_meta_pov_r', 'name':'m_mirr_c2c_ctrl', 'string':'Mirror C2C ctrl', 'group':'dprsr_ingress_hdr_meta_for_input_g' },
    {'type':'dprsr_meta_pov_r', 'name':'m_mirr_coal_smpl_len', 'string':'Mirror Coalesced packet max sample length (quad bytes)', 'group':'dprsr_ingress_hdr_meta_for_input_g' },
    {'type':'dprsr_meta_pov_r', 'name':'m_afc', 'string':'Advanced Flow Control for TM', 'group':'dprsr_ingress_hdr_meta_for_input_g' },
    {'type':'dprsr_meta_pov_r', 'name':'m_mtu_trunc_len', 'string':'MTU for truncation check', 'group':'dprsr_ingress_hdr_meta_for_input_g' },
    {'type':'dprsr_meta_pov_r', 'name':'m_mtu_trunc_err_f', 'string':'MTU truncation error flag', 'group':'dprsr_ingress_hdr_meta_for_input_g' },
    {'type':'dprsr_meta_pov_r', 'name':'m_force_tx_err', 'string':'Force Tx Error', 'group':'dprsr_egress_hdr_meta_for_input_g' },
    {'type':'dprsr_meta_pov_r', 'name':'m_capture_tx_ts', 'string':'Capture Tx Timestamp', 'group':'dprsr_egress_hdr_meta_for_input_g' },
    {'type':'dprsr_meta_pov_r', 'name':'m_tx_pkt_has_offsets', 'string':'Tx packet has offsets', 'group':'dprsr_egress_hdr_meta_for_input_g' },
    {'type':'dprsr_meta_pov_r', 'name':'m_mirr_io_sel', 'string':'Mirror Incoming or Outgoing packet', 'group':'dprsr_egress_hdr_meta_for_input_g' },
    {'type':'dprsr_meta_pov_r', 'name':'m_mirr_hash', 'string':'Mirror Hash', 'group':'dprsr_egress_hdr_meta_for_input_g' },
    {'type':'dprsr_meta_pov_r', 'name':'m_mirr_epipe_port', 'string':'Mirror Epipe port', 'group':'dprsr_egress_hdr_meta_for_input_g' },
    {'type':'dprsr_meta_pov_r', 'name':'m_mirr_qid', 'string':'Mirror QID', 'group':'dprsr_egress_hdr_meta_for_input_g' },
    {'type':'dprsr_meta_pov_r', 'name':'m_mirr_dond_ctrl', 'string':'Mirror Deflect on Drop ctrl', 'group':'dprsr_egress_hdr_meta_for_input_g' },
    {'type':'dprsr_meta_pov_r', 'name':'m_mirr_icos', 'string':'Mirror ICOS', 'group':'dprsr_egress_hdr_meta_for_input_g' },
    {'type':'dprsr_meta_pov_r', 'name':'m_mirr_mc_ctrl', 'string':'Mirror MC ctrl', 'group':'dprsr_egress_hdr_meta_for_input_g' },
    {'type':'dprsr_meta_pov_r', 'name':'m_mirr_c2c_ctrl', 'string':'Mirror C2C ctrl', 'group':'dprsr_egress_hdr_meta_for_input_g' },
    {'type':'dprsr_meta_pov_r', 'name':'m_mirr_coal_smpl_len', 'string':'Mirror output Coalesced packet max length', 'group':'dprsr_egress_hdr_meta_for_input_g' },
    {'type':'dprsr_meta_pov_r', 'name':'m_afc', 'string':'Advanced Flow Control for TM', 'group':'dprsr_egress_hdr_meta_for_input_g' },
    {'type':'dprsr_meta_pov_r', 'name':'m_mtu_trunc_len', 'string':'MTU for truncation check', 'group':'dprsr_egress_hdr_meta_for_input_g' },
    {'type':'dprsr_meta_pov_r', 'name':'m_mtu_trunc_err_f', 'string':'MTU truncation error flag', 'group':'dprsr_egress_hdr_meta_for_input_g' },
    {'type':'dprsr_meta_dis_shft_r', 'name':'m_hash1', 'string':'Hash value 1', 'group':'dprsr_header_ingress_meta_g' },
    {'type':'dprsr_meta_dis_shft_r', 'name':'m_hash2', 'string':'Hash value 2', 'group':'dprsr_header_ingress_meta_g' },
    {'type':'dprsr_meta_dis_shft_r', 'name':'m_copy_to_cpu_cos', 'string':'CoS for Copy-to-CPU packet', 'group':'dprsr_header_ingress_meta_g' },
    {'type':'dprsr_meta_dis_shft_r', 'name':'m_deflect_on_drop', 'string':'Deflect-on-Drop flag', 'group':'dprsr_header_ingress_meta_g' },
    {'type':'dprsr_meta_dis_shft_r', 'name':'m_icos', 'string':'Class of Service   (Cos)', 'group':'dprsr_header_ingress_meta_g' },
    {'type':'dprsr_meta_dis_shft_r', 'name':'m_pkt_color', 'string':'Packet color', 'group':'dprsr_header_ingress_meta_g' },
    {'type':'dprsr_meta_dis_shft_r', 'name':'m_qid', 'string':'Queue ID', 'group':'dprsr_header_ingress_meta_g' },
    {'type':'dprsr_meta_dis_shft_r', 'name':'m_xid_l1', 'string':'Level 1 XID', 'group':'dprsr_header_ingress_meta_g' },
    {'type':'dprsr_meta_dis_shft_r', 'name':'m_xid_l2', 'string':'Level 2 XID', 'group':'dprsr_header_ingress_meta_g' },
    {'type':'dprsr_meta_dis_shft_r', 'name':'m_rid', 'string':'Replication ID   (RID)', 'group':'dprsr_header_ingress_meta_g' },
    {'type':'dprsr_meta_dis_shft_r', 'name':'m_bypss_egr', 'string':'Egress bypass flag', 'group':'dprsr_header_ingress_meta_g' },
    {'type':'dprsr_meta_dis_shft_r', 'name':'m_ct_disable', 'string':'Cut-through Disable flag', 'group':'dprsr_header_ingress_meta_g' },
    {'type':'dprsr_meta_dis_shft_r', 'name':'m_ct_mcast', 'string':'Multicast Cut-through Enable flag', 'group':'dprsr_header_ingress_meta_g' },
    {'type':'dprsr_meta_dis_shft_r', 'name':'m_mirr_io_sel', 'string':'Mirror Incoming or Outgoing packet', 'group':'dprsr_header_ingress_meta_g' },
    {'type':'dprsr_meta_dis_shft_r', 'name':'m_mirr_hash', 'string':'Mirror Hash', 'group':'dprsr_header_ingress_meta_g' },
    {'type':'dprsr_meta_dis_shft_r', 'name':'m_mirr_epipe_port', 'string':'Mirror Epipe port', 'group':'dprsr_header_ingress_meta_g' },
    {'type':'dprsr_meta_dis_shft_r', 'name':'m_mirr_qid', 'string':'Mirror QID', 'group':'dprsr_header_ingress_meta_g' },
    {'type':'dprsr_meta_dis_shft_r', 'name':'m_mirr_dond_ctrl', 'string':'Mirror Deflect on Drop ctrl', 'group':'dprsr_header_ingress_meta_g' },
    {'type':'dprsr_meta_dis_shft_r', 'name':'m_mirr_icos', 'string':'Mirror ICOS', 'group':'dprsr_header_ingress_meta_g' },
    {'type':'dprsr_meta_dis_shft_r', 'name':'m_mirr_mc_ctrl', 'string':'Mirror MC ctrl', 'group':'dprsr_header_ingress_meta_g' },
    {'type':'dprsr_meta_dis_shft_r', 'name':'m_mirr_c2c_ctrl', 'string':'Mirror C2C ctrl', 'group':'dprsr_header_ingress_meta_g' },
    {'type':'dprsr_meta_dis_shft_r', 'name':'m_mirr_coal_smpl_len', 'string':'Mirror Coalesced packet max sample length (quad bytes)', 'group':'dprsr_header_ingress_meta_g' },
    {'type':'dprsr_meta_dis_shft_r', 'name':'m_afc', 'string':'Advanced Flow Control for TM', 'group':'dprsr_header_ingress_meta_g' },
    {'type':'dprsr_meta_dis_shft_r', 'name':'m_mtu_trunc_len', 'string':'MTU for truncation check', 'group':'dprsr_header_ingress_meta_g' },
    {'type':'dprsr_meta_dis_shft_r', 'name':'m_mtu_trunc_err_f', 'string':'MTU truncation error flag', 'group':'dprsr_header_ingress_meta_g' },
    {'type':'dprsr_meta_dis_shft_r', 'name':'m_force_tx_err', 'string':'Force Tx Error', 'group':'dprsr_header_egress_meta_g' },
    {'type':'dprsr_meta_dis_shft_r', 'name':'m_capture_tx_ts', 'string':'Capture Tx Timestamp', 'group':'dprsr_header_egress_meta_g' },
    {'type':'dprsr_meta_dis_shft_r', 'name':'m_tx_pkt_has_offsets', 'string':'Tx packet has offsets', 'group':'dprsr_header_egress_meta_g' },
    {'type':'dprsr_meta_dis_shft_r', 'name':'m_mirr_io_sel', 'string':'Mirror Incoming or Outgoing packet', 'group':'dprsr_header_egress_meta_g' },
    {'type':'dprsr_meta_dis_shft_r', 'name':'m_mirr_hash', 'string':'Mirror Hash', 'group':'dprsr_header_egress_meta_g' },
    {'type':'dprsr_meta_dis_shft_r', 'name':'m_mirr_epipe_port', 'string':'Mirror Epipe port', 'group':'dprsr_header_egress_meta_g' },
    {'type':'dprsr_meta_dis_shft_r', 'name':'m_mirr_qid', 'string':'Mirror QID', 'group':'dprsr_header_egress_meta_g' },
    {'type':'dprsr_meta_dis_shft_r', 'name':'m_mirr_dond_ctrl', 'string':'Mirror Deflect on Drop ctrl', 'group':'dprsr_header_egress_meta_g' },
    {'type':'dprsr_meta_dis_shft_r', 'name':'m_mirr_icos', 'string':'Mirror ICOS', 'group':'dprsr_header_egress_meta_g' },
    {'type':'dprsr_meta_dis_shft_r', 'name':'m_mirr_mc_ctrl', 'string':'Mirror MC ctrl', 'group':'dprsr_header_egress_meta_g' },
    {'type':'dprsr_meta_dis_shft_r', 'name':'m_mirr_c2c_ctrl', 'string':'Mirror C2C ctrl', 'group':'dprsr_header_egress_meta_g' },
    {'type':'dprsr_meta_dis_shft_r', 'name':'m_mirr_coal_smpl_len', 'string':'Mirror output Coalesced packet max length', 'group':'dprsr_header_egress_meta_g' },
    {'type':'dprsr_meta_dis_shft_r', 'name':'m_afc', 'string':'Advanced Flow Control for TM', 'group':'dprsr_header_egress_meta_g' },
    {'type':'dprsr_meta_dis_shft_r', 'name':'m_mtu_trunc_len', 'string':'MTU for truncation check', 'group':'dprsr_header_egress_meta_g' },
    {'type':'dprsr_meta_dis_shft_r', 'name':'m_mtu_trunc_err_f', 'string':'MTU truncation error flag', 'group':'dprsr_header_egress_meta_g' },
]


metadata_widths = {
# ingress
    'm_learn_sel' : 3,           # Learn Quantum Config Type. Only needed if Learning is enabled
    'm_resub_sel' : 3,           # resubmit config type
    'm_pgen' : 1,                # Pgen  Cfg (overridden if resub cfg type is valid)
    'm_pgen_len' : 14,           # PGEN Length
    'm_pgen_addr' : 10,          # PGEN Address
    'm_mgid1' : 16,              # Egress multicast group A
    'm_mgid2' : 16,              # Egress multicast group B
    'm_copy_to_cpu' : 1,         # Copy-to-Cpu
    'm_egress_unicast_port' : 9, # Unicast Egress Port
    'm_mirr_sel' : 4,            # Mirror Config Type
    'm_drop_ctl' : 3,            # drop_ctrl
    'm_hash1' : 13,              # hash 1 for multicast
    'm_hash2' : 13,              # hash 2 for multicast
    'm_copy_to_cpu_cos' : 3,     # iCoS for Copy-to-Cpu
    'm_deflect_on_drop' : 1,     # deflect on drop (if Qing systemdrops then deflect to redirection port)
    'm_icos' : 3,                # Class of Service (iCoS)
    'm_pkt_color' : 2,           # packet color
    'm_qid' : 7,                 # Qid
    'm_xid_l1' : 16,             # Level1 XID (eXclusion ID used at level 1 of replication tree)
    'm_xid_l2' : 9,              # Level2 XID (eXclusion ID used at level 2 of replication tree)
    'm_rid' : 16,                # RID (Replication ID used for pruning)
    'm_bypss_egr' : 1,           # Bypass Egress (warp mode)
    'm_ct_disable' : 1,          # Disable unicast cutthrough
    'm_ct_mcast' : 1,            # Enable multicast cutthrough
    'm_mirr_io_sel' : 1,         # Mirror Incoming or Outgoing packet
    'm_mirr_hash' : 13,          # Hash for mirror

    'm_mirr_mc_ctrl' : 1,        # mc_ctrl for Mirror
    'm_mirr_c2c_ctrl' : 1,       # c2c_ctrl for Mirror
    'm_mirr_coal_smpl_len' : 8,   # Mirror Coalesce Pkt Length (Ingress)
    'm_afc' : 32,                # Advanced FLC for TM
    'm_mtu_trunc_len' : 14,      # MTU for Truncation/Check
    'm_mtu_trunc_err_f' : 1,     # MTU for Truncation/Check Err flag
     'm_mirr_epipe_port' : 9,    # Mirror Epipe port
     'm_mirr_qid' : 7,           # Mirror QID
     'm_mirr_dond_ctrl' : 1,     # Mirror Deflect on Drop ctrl
     'm_mirr_icos' : 3,          # Mirror ICOS

    'm_force_tx_err' : 1,        # Force Tx Error (at end of pkt)
    'm_capture_tx_ts' : 1,       # Capture TS on Tx
    'm_tx_pkt_has_offsets' : 1,  # Update Delay on TX (for 1588 Transparent Clock)
    'm_mirr_io_sel' : 1,         # Mirror Incoming or Outgoing packet
    'm_mirr_hash' : 13,          # Hash for Mirror
    'm_mirr_mc_ctrl' : 1,        # mc_ctrl for Mirror
    'm_mirr_c2c_ctrl' : 1,       # c2c_ctrol for Mirror
    'm_mirr_coal_smpl_len' : 8,   # Mirror Coalesce Pkt Length (egress)
    'm_afc' : 32,                # Adv FLC for TM
    'm_mtu_trunc_len' : 14,      # MTU for Truncation/Check
    'm_mtu_trunc_err_f' : 1,     # MTU for Truncation/Check Err flag
     }

# These are the functions that set the metadata in I2QueueingMetadata
#   (only for metadata that gets put in I2QueueingMetadata)
ingress_setters = {
     'm_egress_unicast_port' : 'set_egress_unicast_port',
     'm_mgid1' : 'set_mgid1',
     'm_mgid2' : 'set_mgid2',
     'm_copy_to_cpu' : 'set_copy_to_cpu',
     'm_hash1' : 'set_hash1',
     'm_hash2' : 'set_hash2',
     'm_copy_to_cpu_cos' : 'set_copy_to_cpu_cos',
     'm_deflect_on_drop' : 'set_dod',    # inconsistent
     'm_icos' : 'set_icos',
     'm_pkt_color' : 'set_meter_color',  # inconsistent
     'm_qid' : 'set_qid',
     'm_xid_l1' : 'set_xid',  # needs updating?
     'm_xid_l2' : 'set_yid',  # needs updating?
     'm_rid' : 'set_irid',    # why irid?
     'm_bypss_egr' : 'set_bypass_egr_mode', # inconsistent
     'm_ct_disable' : 'set_ct_disable_mode',  # inconsistent
     'm_ct_mcast' : 'set_ct_mcast_mode',     # inconsistent

     'm_mirr_sel' :        'set_mirr_sel',
     'm_mirr_epipe_port' : 'set_mirr_epipe_port',
     'm_mirr_qid' :        'set_mirr_qid',
     'm_mirr_dond_ctrl' :  'set_mirr_dond_ctrl',
     'm_mirr_icos' :       'set_mirr_icos',

     'm_mirr_io_sel' : 'set_mirr_io_sel',
     'm_mirr_hash' : 'set_mirr_hash',
     'm_mirr_mc_ctrl' : 'set_mirr_mc_ctrl',
     'm_mirr_c2c_ctrl' : 'set_mirr_c2c_ctrl',
     'm_mirr_coal_smpl_len' : 'set_coal_len', # inconsistent
     'm_afc' : 'set_afc',
     'm_mtu_trunc_len' : 'set_mtu_trunc_len',
     'm_mtu_trunc_err_f' : 'set_mtu_trunc_err_f',
     }

# These are the functions that set the metadata in E2MacMetadata
#   (only for metadata that gets put in E2MacMetadata)
egress_setters = {
     'm_egress_unicast_port' : 'set_egress_unicast_port',
     'm_force_tx_err' : 'set_force_tx_error',   # inconsistent
     'm_capture_tx_ts' : 'set_capture_tx_ts',
     'm_tx_pkt_has_offsets' : 'set_update_delay_on_tx', # inconsistent
     'm_ecos' : 'set_ecos',
     'm_mirr_io_sel' : 'set_mirr_io_sel',
     'm_mirr_hash' : 'set_mirr_hash',
     'm_mirr_mc_ctrl' : 'set_mirr_mc_ctrl',
     'm_mirr_c2c_ctrl' : 'set_mirr_c2c_ctrl',
     'm_mirr_coal_smpl_len' : 'set_coal_len', # inconsistent

     'm_mirr_sel' :        'set_mirr_sel',
     'm_mirr_epipe_port' : 'set_mirr_epipe_port',
     'm_mirr_qid' :        'set_mirr_qid',
     'm_mirr_dond_ctrl' :  'set_mirr_dond_ctrl',
     'm_mirr_icos' :       'set_mirr_icos',

     'm_afc' : 'set_afc',
     'm_mtu_trunc_len' : 'set_mtu_trunc_len',
     'm_mtu_trunc_err_f' : 'set_mtu_trunc_err_f',
     }


includes = ""
accessors = ""
width_defs = ""
defines = ""
init_list = ""
reset_list = ""
reset_slice_list = ""
ingress_extract_list = ""
egress_extract_list = ""
ingress_mirr_extract_list = ""
egress_mirr_extract_list = ""
n_slices = 4
# count names to spot ones that are the same on ingress and egress
names_cnt = {}
for v in meta_vars:
     if v['type'] != 'dprsr_meta_pov_r' :  # don't count the pov only registers
          name = v['name']
          if not name in names_cnt :
               names_cnt[ name ] = 0
          names_cnt[ name ] += 1

for v in meta_vars:
     is_in_slice = v['group'] == 'dprsr_header_ingress_meta_g' or v['group'] == 'dprsr_header_egress_meta_g'
     name = v['name']
     whole_name = v['group'] + "_" + name
     fn_name = name + '_info'
     fn_name = re.sub( '^m_','', fn_name )
     var_name = name + "_"

     if v['type'] == 'dprsr_meta_pov_r' :  # register that holds the POV for another register
          var_name = var_name + 'pov_reg_'

     width_var = 'k_' + name + '_width'  # will be converted to camel case
     width_var = re.sub( 'k_m_',  'k_', width_var );
     width_var = re.sub( '_([a-zA-Z])',  lambda m: m.group(1).upper(), width_var );

     if names_cnt[ name ] == 2 :
          if re.search( 'ingress', v['group'] ) :
               var_name = re.sub( '^m_', 'm_i_', var_name)
               fn_name  = 'i_' + fn_name
               width_var = re.sub( '^k', 'kIngress', width_var)
          else :
               var_name = re.sub( '^m_', 'm_e_', var_name)
               fn_name  = 'e_' + fn_name
               width_var = re.sub( '^k', 'kEgress', width_var)
     class_name = whole_name
     # convert to camel case
     class_name = re.sub( '^([a-z])',     lambda m: m.group(1).upper(), class_name );
     class_name = re.sub( '_([a-zA-Z])',  lambda m: m.group(1).upper(), class_name );
     shift_fn = ''
     slice_index = ''
     slice_arg = ''
     if is_in_slice :
          slice_index = '.reg_[slice]'
          slice_arg   = 'int slice,'

     # Generate accessors (not for the pov only registers)
     # These are for registers that contain the pov bits with all the others
     if v['type'] == 'dprsr_meta_pov_dis_shft_r' or v['type'] == 'dprsr_meta_pov_dis_r' :
          if v['type'] == 'dprsr_meta_pov_dis_shft_r' :
               shift_fn = '_with_shift'
          # Note: in format string use {{ to get a single {
          accessors += """
                  bool get_{4}({5} const Phv &phv, const BitVector<kPovWidth> &pov, uint64_t* ret_value) {{
                       return get_metadata{6}({3}, phv, pov, {2}, ret_value );
                  }}
                  """.format( name, class_name, width_var, var_name + slice_index, fn_name, slice_arg, shift_fn)
     # These are for registers that have a separate register for the pov bits
     elif v['type'] == 'dprsr_meta_dis_shft_r' or v['type'] == 'dprsr_meta_dis_r' :
          if v['type'] == 'dprsr_meta_dis_shft_r' :
               shift_fn = '_with_shift'
          # Note: in format string use {{ to get a single {
          accessors += """
                  bool get_{4}({5} const Phv &phv, const BitVector<kPovWidth> &pov, uint64_t* ret_value) {{
                       return get_metadata_pov_reg{6}({3}, {7}, phv, pov, {2}, ret_value );
                  }}
                  """.format( name, class_name, width_var, var_name + slice_index, fn_name, slice_arg, shift_fn, var_name + 'pov_reg_')


     includes += "#include <register_includes/{0}.h>\n".format(whole_name)
     if is_in_slice :
          defines += "{0:70} {1};\n".format(
               "SliceReg<register_classes::{0}>".format( class_name ),
               var_name)
     else :
          defines += "register_classes::{1:52} {2};\n".format( name, class_name, var_name )

     if v['type'] != 'dprsr_meta_pov_r' :
          width_defs += "static constexpr int {0:32} = {1};\n".format( width_var, metadata_widths[ name ] )

     if is_in_slice :
          spaces = "    "
          comma = spaces;
          init_list += "{0}{{ chip,pipe }},\n".format( var_name )
     else :
          init_list += "{0}{{deparser_in_hdr_adapter({0}, chip, pipe )}},\n".format( var_name )

     if is_in_slice :
          reset_slice_list += "" #"{0}[slice].reset();\n".format( var_name )
     else:
          reset_list += "{0}.reset();\n".format( var_name )

     if is_in_slice :
          get_slice_arg = "slice, "
     else:
          get_slice_arg = ""

     is_mirror = re.search("mirr_", fn_name)

     # don't need setters for the pov only registers
     if v['type'] != 'dprsr_meta_pov_r' :
          # some names are duplicated and have fn_names with i_ (ingress) or e_ (egress)
          if name in ingress_setters and not re.match('^e_',fn_name) :
               if not is_mirror :
                    ingress_extract_list += "if ( deparser_reg_.get_{1}({2}phv, pov, &ret_value) ) i2q_md->{0}( ret_value );\n".format( ingress_setters[name] , fn_name, get_slice_arg )
               else :
                    ingress_mirr_extract_list += "if ( deparser_reg_.get_{1}({2}phv, pov, &ret_value) ) mirror_metadata->{0}( ret_value );\n".format( ingress_setters[name] , fn_name, get_slice_arg )
          if name in egress_setters and not re.match('^i_',fn_name) :
               if not is_mirror :
                    egress_extract_list += "if ( deparser_reg_.get_{1}({2}phv, pov, &ret_value) ) e2mac_metadata->{0}( ret_value );\n".format( egress_setters[name] , fn_name, get_slice_arg )
               else :
                    egress_mirr_extract_list += "if ( deparser_reg_.get_{1}({2}phv, pov, &ret_value) ) mirror_metadata->{0}( ret_value );\n".format( egress_setters[name] , fn_name, get_slice_arg )
