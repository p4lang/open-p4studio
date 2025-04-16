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

#include <mau-ignored-regs.h>

namespace MODEL_CHIP_NAMESPACE {

MauIgnoredRegs::MauIgnoredRegs(int chipIndex, int pipeIndex, int mauIndex, Mau *mau) :
  i_IdletimeLogicalToPhysicalSweepGrantCtlArray_  { {
      {default_adapter(i_IdletimeLogicalToPhysicalSweepGrantCtlArray_[0], chipIndex,pipeIndex,mauIndex,0,[this](uint32_t i){this->not_implemented("IdletimeLogicalToPhysicalSweepGrantCtlArray");} )},
      {default_adapter(i_IdletimeLogicalToPhysicalSweepGrantCtlArray_[1], chipIndex,pipeIndex,mauIndex,1,[this](uint32_t i){this->not_implemented("IdletimeLogicalToPhysicalSweepGrantCtlArray");} )},
      {default_adapter(i_IdletimeLogicalToPhysicalSweepGrantCtlArray_[2], chipIndex,pipeIndex,mauIndex,2,[this](uint32_t i){this->not_implemented("IdletimeLogicalToPhysicalSweepGrantCtlArray");} )},
      {default_adapter(i_IdletimeLogicalToPhysicalSweepGrantCtlArray_[3], chipIndex,pipeIndex,mauIndex,3,[this](uint32_t i){this->not_implemented("IdletimeLogicalToPhysicalSweepGrantCtlArray");} )},
      {default_adapter(i_IdletimeLogicalToPhysicalSweepGrantCtlArray_[4], chipIndex,pipeIndex,mauIndex,4,[this](uint32_t i){this->not_implemented("IdletimeLogicalToPhysicalSweepGrantCtlArray");} )},
      {default_adapter(i_IdletimeLogicalToPhysicalSweepGrantCtlArray_[5], chipIndex,pipeIndex,mauIndex,5,[this](uint32_t i){this->not_implemented("IdletimeLogicalToPhysicalSweepGrantCtlArray");} )},
      {default_adapter(i_IdletimeLogicalToPhysicalSweepGrantCtlArray_[6], chipIndex,pipeIndex,mauIndex,6,[this](uint32_t i){this->not_implemented("IdletimeLogicalToPhysicalSweepGrantCtlArray");} )},
      {default_adapter(i_IdletimeLogicalToPhysicalSweepGrantCtlArray_[7], chipIndex,pipeIndex,mauIndex,7,[this](uint32_t i){this->not_implemented("IdletimeLogicalToPhysicalSweepGrantCtlArray");} )} } },
  i_IdletimePhysicalToLogicalReqIncCtlArray_  { {
      {default_adapter(i_IdletimePhysicalToLogicalReqIncCtlArray_[0], chipIndex,pipeIndex,mauIndex,0,[this](uint32_t i){this->not_implemented("IdletimePhysicalToLogicalReqIncCtlArray");} )},
      {default_adapter(i_IdletimePhysicalToLogicalReqIncCtlArray_[1], chipIndex,pipeIndex,mauIndex,1,[this](uint32_t i){this->not_implemented("IdletimePhysicalToLogicalReqIncCtlArray");} )},
      {default_adapter(i_IdletimePhysicalToLogicalReqIncCtlArray_[2], chipIndex,pipeIndex,mauIndex,2,[this](uint32_t i){this->not_implemented("IdletimePhysicalToLogicalReqIncCtlArray");} )},
      {default_adapter(i_IdletimePhysicalToLogicalReqIncCtlArray_[3], chipIndex,pipeIndex,mauIndex,3,[this](uint32_t i){this->not_implemented("IdletimePhysicalToLogicalReqIncCtlArray");} )},
      {default_adapter(i_IdletimePhysicalToLogicalReqIncCtlArray_[4], chipIndex,pipeIndex,mauIndex,4,[this](uint32_t i){this->not_implemented("IdletimePhysicalToLogicalReqIncCtlArray");} )},
      {default_adapter(i_IdletimePhysicalToLogicalReqIncCtlArray_[5], chipIndex,pipeIndex,mauIndex,5,[this](uint32_t i){this->not_implemented("IdletimePhysicalToLogicalReqIncCtlArray");} )},
      {default_adapter(i_IdletimePhysicalToLogicalReqIncCtlArray_[6], chipIndex,pipeIndex,mauIndex,6,[this](uint32_t i){this->not_implemented("IdletimePhysicalToLogicalReqIncCtlArray");} )},
      {default_adapter(i_IdletimePhysicalToLogicalReqIncCtlArray_[7], chipIndex,pipeIndex,mauIndex,7,[this](uint32_t i){this->not_implemented("IdletimePhysicalToLogicalReqIncCtlArray");} )} } },
  i_MapramMbeErrlogArray_  { {
      {default_adapter(i_MapramMbeErrlogArray_[0], chipIndex,pipeIndex,mauIndex,0,[this](uint32_t i){this->not_implemented("MapramMbeErrlogArray");} )},
      {default_adapter(i_MapramMbeErrlogArray_[1], chipIndex,pipeIndex,mauIndex,1,[this](uint32_t i){this->not_implemented("MapramMbeErrlogArray");} )},
      {default_adapter(i_MapramMbeErrlogArray_[2], chipIndex,pipeIndex,mauIndex,2,[this](uint32_t i){this->not_implemented("MapramMbeErrlogArray");} )},
      {default_adapter(i_MapramMbeErrlogArray_[3], chipIndex,pipeIndex,mauIndex,3,[this](uint32_t i){this->not_implemented("MapramMbeErrlogArray");} )},
      {default_adapter(i_MapramMbeErrlogArray_[4], chipIndex,pipeIndex,mauIndex,4,[this](uint32_t i){this->not_implemented("MapramMbeErrlogArray");} )},
      {default_adapter(i_MapramMbeErrlogArray_[5], chipIndex,pipeIndex,mauIndex,5,[this](uint32_t i){this->not_implemented("MapramMbeErrlogArray");} )},
      {default_adapter(i_MapramMbeErrlogArray_[6], chipIndex,pipeIndex,mauIndex,6,[this](uint32_t i){this->not_implemented("MapramMbeErrlogArray");} )},
      {default_adapter(i_MapramMbeErrlogArray_[7], chipIndex,pipeIndex,mauIndex,7,[this](uint32_t i){this->not_implemented("MapramMbeErrlogArray");} )} } },
  i_MapramSbeErrlogArray_  { {
      {default_adapter(i_MapramSbeErrlogArray_[0], chipIndex,pipeIndex,mauIndex,0,[this](uint32_t i){this->not_implemented("MapramSbeErrlogArray");} )},
      {default_adapter(i_MapramSbeErrlogArray_[1], chipIndex,pipeIndex,mauIndex,1,[this](uint32_t i){this->not_implemented("MapramSbeErrlogArray");} )},
      {default_adapter(i_MapramSbeErrlogArray_[2], chipIndex,pipeIndex,mauIndex,2,[this](uint32_t i){this->not_implemented("MapramSbeErrlogArray");} )},
      {default_adapter(i_MapramSbeErrlogArray_[3], chipIndex,pipeIndex,mauIndex,3,[this](uint32_t i){this->not_implemented("MapramSbeErrlogArray");} )},
      {default_adapter(i_MapramSbeErrlogArray_[4], chipIndex,pipeIndex,mauIndex,4,[this](uint32_t i){this->not_implemented("MapramSbeErrlogArray");} )},
      {default_adapter(i_MapramSbeErrlogArray_[5], chipIndex,pipeIndex,mauIndex,5,[this](uint32_t i){this->not_implemented("MapramSbeErrlogArray");} )},
      {default_adapter(i_MapramSbeErrlogArray_[6], chipIndex,pipeIndex,mauIndex,6,[this](uint32_t i){this->not_implemented("MapramSbeErrlogArray");} )},
      {default_adapter(i_MapramSbeErrlogArray_[7], chipIndex,pipeIndex,mauIndex,7,[this](uint32_t i){this->not_implemented("MapramSbeErrlogArray");} )} } },
  i_AdrmuxRowMemSlowMode_  { {
      {default_adapter(i_AdrmuxRowMemSlowMode_[0], chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("AdrmuxRowMemSlowMode");} )},
      {default_adapter(i_AdrmuxRowMemSlowMode_[1], chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("AdrmuxRowMemSlowMode");} )},
      {default_adapter(i_AdrmuxRowMemSlowMode_[2], chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("AdrmuxRowMemSlowMode");} )},
      {default_adapter(i_AdrmuxRowMemSlowMode_[3], chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("AdrmuxRowMemSlowMode");} )},
      {default_adapter(i_AdrmuxRowMemSlowMode_[4], chipIndex,pipeIndex,mauIndex,4,[this](){this->not_implemented("AdrmuxRowMemSlowMode");} )},
      {default_adapter(i_AdrmuxRowMemSlowMode_[5], chipIndex,pipeIndex,mauIndex,5,[this](){this->not_implemented("AdrmuxRowMemSlowMode");} )},
      {default_adapter(i_AdrmuxRowMemSlowMode_[6], chipIndex,pipeIndex,mauIndex,6,[this](){this->not_implemented("AdrmuxRowMemSlowMode");} )},
      {default_adapter(i_AdrmuxRowMemSlowMode_[7], chipIndex,pipeIndex,mauIndex,7,[this](){this->not_implemented("AdrmuxRowMemSlowMode");} )} } },
  i_IntrEnable0MauAdrmuxRow_  { {
      {default_adapter(i_IntrEnable0MauAdrmuxRow_[0], chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("IntrEnable0MauAdrmuxRow");} )},
      {default_adapter(i_IntrEnable0MauAdrmuxRow_[1], chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("IntrEnable0MauAdrmuxRow");} )},
      {default_adapter(i_IntrEnable0MauAdrmuxRow_[2], chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("IntrEnable0MauAdrmuxRow");} )},
      {default_adapter(i_IntrEnable0MauAdrmuxRow_[3], chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("IntrEnable0MauAdrmuxRow");} )},
      {default_adapter(i_IntrEnable0MauAdrmuxRow_[4], chipIndex,pipeIndex,mauIndex,4,[this](){this->not_implemented("IntrEnable0MauAdrmuxRow");} )},
      {default_adapter(i_IntrEnable0MauAdrmuxRow_[5], chipIndex,pipeIndex,mauIndex,5,[this](){this->not_implemented("IntrEnable0MauAdrmuxRow");} )},
      {default_adapter(i_IntrEnable0MauAdrmuxRow_[6], chipIndex,pipeIndex,mauIndex,6,[this](){this->not_implemented("IntrEnable0MauAdrmuxRow");} )},
      {default_adapter(i_IntrEnable0MauAdrmuxRow_[7], chipIndex,pipeIndex,mauIndex,7,[this](){this->not_implemented("IntrEnable0MauAdrmuxRow");} )} } },
  i_IntrEnable0MauSynth2port_  { {
      {default_adapter(i_IntrEnable0MauSynth2port_[0], chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("IntrEnable0MauSynth2port");} )},
      {default_adapter(i_IntrEnable0MauSynth2port_[1], chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("IntrEnable0MauSynth2port");} )},
      {default_adapter(i_IntrEnable0MauSynth2port_[2], chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("IntrEnable0MauSynth2port");} )},
      {default_adapter(i_IntrEnable0MauSynth2port_[3], chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("IntrEnable0MauSynth2port");} )},
      {default_adapter(i_IntrEnable0MauSynth2port_[4], chipIndex,pipeIndex,mauIndex,4,[this](){this->not_implemented("IntrEnable0MauSynth2port");} )},
      {default_adapter(i_IntrEnable0MauSynth2port_[5], chipIndex,pipeIndex,mauIndex,5,[this](){this->not_implemented("IntrEnable0MauSynth2port");} )},
      {default_adapter(i_IntrEnable0MauSynth2port_[6], chipIndex,pipeIndex,mauIndex,6,[this](){this->not_implemented("IntrEnable0MauSynth2port");} )},
      {default_adapter(i_IntrEnable0MauSynth2port_[7], chipIndex,pipeIndex,mauIndex,7,[this](){this->not_implemented("IntrEnable0MauSynth2port");} )} } },
  i_IntrEnable1MauAdrmuxRow_  { {
      {default_adapter(i_IntrEnable1MauAdrmuxRow_[0], chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("IntrEnable1MauAdrmuxRow");} )},
      {default_adapter(i_IntrEnable1MauAdrmuxRow_[1], chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("IntrEnable1MauAdrmuxRow");} )},
      {default_adapter(i_IntrEnable1MauAdrmuxRow_[2], chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("IntrEnable1MauAdrmuxRow");} )},
      {default_adapter(i_IntrEnable1MauAdrmuxRow_[3], chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("IntrEnable1MauAdrmuxRow");} )},
      {default_adapter(i_IntrEnable1MauAdrmuxRow_[4], chipIndex,pipeIndex,mauIndex,4,[this](){this->not_implemented("IntrEnable1MauAdrmuxRow");} )},
      {default_adapter(i_IntrEnable1MauAdrmuxRow_[5], chipIndex,pipeIndex,mauIndex,5,[this](){this->not_implemented("IntrEnable1MauAdrmuxRow");} )},
      {default_adapter(i_IntrEnable1MauAdrmuxRow_[6], chipIndex,pipeIndex,mauIndex,6,[this](){this->not_implemented("IntrEnable1MauAdrmuxRow");} )},
      {default_adapter(i_IntrEnable1MauAdrmuxRow_[7], chipIndex,pipeIndex,mauIndex,7,[this](){this->not_implemented("IntrEnable1MauAdrmuxRow");} )} } },
  i_IntrEnable1MauSynth2port_  { {
      {default_adapter(i_IntrEnable1MauSynth2port_[0], chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("IntrEnable1MauSynth2port");} )},
      {default_adapter(i_IntrEnable1MauSynth2port_[1], chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("IntrEnable1MauSynth2port");} )},
      {default_adapter(i_IntrEnable1MauSynth2port_[2], chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("IntrEnable1MauSynth2port");} )},
      {default_adapter(i_IntrEnable1MauSynth2port_[3], chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("IntrEnable1MauSynth2port");} )},
      {default_adapter(i_IntrEnable1MauSynth2port_[4], chipIndex,pipeIndex,mauIndex,4,[this](){this->not_implemented("IntrEnable1MauSynth2port");} )},
      {default_adapter(i_IntrEnable1MauSynth2port_[5], chipIndex,pipeIndex,mauIndex,5,[this](){this->not_implemented("IntrEnable1MauSynth2port");} )},
      {default_adapter(i_IntrEnable1MauSynth2port_[6], chipIndex,pipeIndex,mauIndex,6,[this](){this->not_implemented("IntrEnable1MauSynth2port");} )},
      {default_adapter(i_IntrEnable1MauSynth2port_[7], chipIndex,pipeIndex,mauIndex,7,[this](){this->not_implemented("IntrEnable1MauSynth2port");} )} } },
  i_IntrFreezeEnableMauAdrmuxRow_  { {
      {default_adapter(i_IntrFreezeEnableMauAdrmuxRow_[0], chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("IntrFreezeEnableMauAdrmuxRow");} )},
      {default_adapter(i_IntrFreezeEnableMauAdrmuxRow_[1], chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("IntrFreezeEnableMauAdrmuxRow");} )},
      {default_adapter(i_IntrFreezeEnableMauAdrmuxRow_[2], chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("IntrFreezeEnableMauAdrmuxRow");} )},
      {default_adapter(i_IntrFreezeEnableMauAdrmuxRow_[3], chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("IntrFreezeEnableMauAdrmuxRow");} )},
      {default_adapter(i_IntrFreezeEnableMauAdrmuxRow_[4], chipIndex,pipeIndex,mauIndex,4,[this](){this->not_implemented("IntrFreezeEnableMauAdrmuxRow");} )},
      {default_adapter(i_IntrFreezeEnableMauAdrmuxRow_[5], chipIndex,pipeIndex,mauIndex,5,[this](){this->not_implemented("IntrFreezeEnableMauAdrmuxRow");} )},
      {default_adapter(i_IntrFreezeEnableMauAdrmuxRow_[6], chipIndex,pipeIndex,mauIndex,6,[this](){this->not_implemented("IntrFreezeEnableMauAdrmuxRow");} )},
      {default_adapter(i_IntrFreezeEnableMauAdrmuxRow_[7], chipIndex,pipeIndex,mauIndex,7,[this](){this->not_implemented("IntrFreezeEnableMauAdrmuxRow");} )} } },
  i_IntrInjectMauAdrmuxRow_  { {
      {default_adapter(i_IntrInjectMauAdrmuxRow_[0], chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("IntrInjectMauAdrmuxRow");} )},
      {default_adapter(i_IntrInjectMauAdrmuxRow_[1], chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("IntrInjectMauAdrmuxRow");} )},
      {default_adapter(i_IntrInjectMauAdrmuxRow_[2], chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("IntrInjectMauAdrmuxRow");} )},
      {default_adapter(i_IntrInjectMauAdrmuxRow_[3], chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("IntrInjectMauAdrmuxRow");} )},
      {default_adapter(i_IntrInjectMauAdrmuxRow_[4], chipIndex,pipeIndex,mauIndex,4,[this](){this->not_implemented("IntrInjectMauAdrmuxRow");} )},
      {default_adapter(i_IntrInjectMauAdrmuxRow_[5], chipIndex,pipeIndex,mauIndex,5,[this](){this->not_implemented("IntrInjectMauAdrmuxRow");} )},
      {default_adapter(i_IntrInjectMauAdrmuxRow_[6], chipIndex,pipeIndex,mauIndex,6,[this](){this->not_implemented("IntrInjectMauAdrmuxRow");} )},
      {default_adapter(i_IntrInjectMauAdrmuxRow_[7], chipIndex,pipeIndex,mauIndex,7,[this](){this->not_implemented("IntrInjectMauAdrmuxRow");} )} } },
  i_IntrInjectMauSynth2port_  { {
      {default_adapter(i_IntrInjectMauSynth2port_[0], chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("IntrInjectMauSynth2port");} )},
      {default_adapter(i_IntrInjectMauSynth2port_[1], chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("IntrInjectMauSynth2port");} )},
      {default_adapter(i_IntrInjectMauSynth2port_[2], chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("IntrInjectMauSynth2port");} )},
      {default_adapter(i_IntrInjectMauSynth2port_[3], chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("IntrInjectMauSynth2port");} )},
      {default_adapter(i_IntrInjectMauSynth2port_[4], chipIndex,pipeIndex,mauIndex,4,[this](){this->not_implemented("IntrInjectMauSynth2port");} )},
      {default_adapter(i_IntrInjectMauSynth2port_[5], chipIndex,pipeIndex,mauIndex,5,[this](){this->not_implemented("IntrInjectMauSynth2port");} )},
      {default_adapter(i_IntrInjectMauSynth2port_[6], chipIndex,pipeIndex,mauIndex,6,[this](){this->not_implemented("IntrInjectMauSynth2port");} )},
      {default_adapter(i_IntrInjectMauSynth2port_[7], chipIndex,pipeIndex,mauIndex,7,[this](){this->not_implemented("IntrInjectMauSynth2port");} )} } },
  i_IntrStatusMauAdrmuxRow_  { {
      {default_adapter(i_IntrStatusMauAdrmuxRow_[0], chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("IntrStatusMauAdrmuxRow");} )},
      {default_adapter(i_IntrStatusMauAdrmuxRow_[1], chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("IntrStatusMauAdrmuxRow");} )},
      {default_adapter(i_IntrStatusMauAdrmuxRow_[2], chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("IntrStatusMauAdrmuxRow");} )},
      {default_adapter(i_IntrStatusMauAdrmuxRow_[3], chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("IntrStatusMauAdrmuxRow");} )},
      {default_adapter(i_IntrStatusMauAdrmuxRow_[4], chipIndex,pipeIndex,mauIndex,4,[this](){this->not_implemented("IntrStatusMauAdrmuxRow");} )},
      {default_adapter(i_IntrStatusMauAdrmuxRow_[5], chipIndex,pipeIndex,mauIndex,5,[this](){this->not_implemented("IntrStatusMauAdrmuxRow");} )},
      {default_adapter(i_IntrStatusMauAdrmuxRow_[6], chipIndex,pipeIndex,mauIndex,6,[this](){this->not_implemented("IntrStatusMauAdrmuxRow");} )},
      {default_adapter(i_IntrStatusMauAdrmuxRow_[7], chipIndex,pipeIndex,mauIndex,7,[this](){this->not_implemented("IntrStatusMauAdrmuxRow");} )} } },
  i_IntrStatusMauSynth2port_  { {
      {default_adapter(i_IntrStatusMauSynth2port_[0], chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("IntrStatusMauSynth2port");} )},
      {default_adapter(i_IntrStatusMauSynth2port_[1], chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("IntrStatusMauSynth2port");} )},
      {default_adapter(i_IntrStatusMauSynth2port_[2], chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("IntrStatusMauSynth2port");} )},
      {default_adapter(i_IntrStatusMauSynth2port_[3], chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("IntrStatusMauSynth2port");} )},
      {default_adapter(i_IntrStatusMauSynth2port_[4], chipIndex,pipeIndex,mauIndex,4,[this](){this->not_implemented("IntrStatusMauSynth2port");} )},
      {default_adapter(i_IntrStatusMauSynth2port_[5], chipIndex,pipeIndex,mauIndex,5,[this](){this->not_implemented("IntrStatusMauSynth2port");} )},
      {default_adapter(i_IntrStatusMauSynth2port_[6], chipIndex,pipeIndex,mauIndex,6,[this](){this->not_implemented("IntrStatusMauSynth2port");} )},
      {default_adapter(i_IntrStatusMauSynth2port_[7], chipIndex,pipeIndex,mauIndex,7,[this](){this->not_implemented("IntrStatusMauSynth2port");} )} } },
  i_MapramMbeInj_  { {
      {default_adapter(i_MapramMbeInj_[0], chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("MapramMbeInj");} )},
      {default_adapter(i_MapramMbeInj_[1], chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("MapramMbeInj");} )},
      {default_adapter(i_MapramMbeInj_[2], chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("MapramMbeInj");} )},
      {default_adapter(i_MapramMbeInj_[3], chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("MapramMbeInj");} )},
      {default_adapter(i_MapramMbeInj_[4], chipIndex,pipeIndex,mauIndex,4,[this](){this->not_implemented("MapramMbeInj");} )},
      {default_adapter(i_MapramMbeInj_[5], chipIndex,pipeIndex,mauIndex,5,[this](){this->not_implemented("MapramMbeInj");} )},
      {default_adapter(i_MapramMbeInj_[6], chipIndex,pipeIndex,mauIndex,6,[this](){this->not_implemented("MapramMbeInj");} )},
      {default_adapter(i_MapramMbeInj_[7], chipIndex,pipeIndex,mauIndex,7,[this](){this->not_implemented("MapramMbeInj");} )} } },
  i_MapramSbeInj_  { {
      {default_adapter(i_MapramSbeInj_[0], chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("MapramSbeInj");} )},
      {default_adapter(i_MapramSbeInj_[1], chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("MapramSbeInj");} )},
      {default_adapter(i_MapramSbeInj_[2], chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("MapramSbeInj");} )},
      {default_adapter(i_MapramSbeInj_[3], chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("MapramSbeInj");} )},
      {default_adapter(i_MapramSbeInj_[4], chipIndex,pipeIndex,mauIndex,4,[this](){this->not_implemented("MapramSbeInj");} )},
      {default_adapter(i_MapramSbeInj_[5], chipIndex,pipeIndex,mauIndex,5,[this](){this->not_implemented("MapramSbeInj");} )},
      {default_adapter(i_MapramSbeInj_[6], chipIndex,pipeIndex,mauIndex,6,[this](){this->not_implemented("MapramSbeInj");} )},
      {default_adapter(i_MapramSbeInj_[7], chipIndex,pipeIndex,mauIndex,7,[this](){this->not_implemented("MapramSbeInj");} )} } },
  i_MauSynth2portErrlog_  { {
      {default_adapter(i_MauSynth2portErrlog_[0], chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("MauSynth2portErrlog");} )},
      {default_adapter(i_MauSynth2portErrlog_[1], chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("MauSynth2portErrlog");} )},
      {default_adapter(i_MauSynth2portErrlog_[2], chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("MauSynth2portErrlog");} )},
      {default_adapter(i_MauSynth2portErrlog_[3], chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("MauSynth2portErrlog");} )},
      {default_adapter(i_MauSynth2portErrlog_[4], chipIndex,pipeIndex,mauIndex,4,[this](){this->not_implemented("MauSynth2portErrlog");} )},
      {default_adapter(i_MauSynth2portErrlog_[5], chipIndex,pipeIndex,mauIndex,5,[this](){this->not_implemented("MauSynth2portErrlog");} )},
      {default_adapter(i_MauSynth2portErrlog_[6], chipIndex,pipeIndex,mauIndex,6,[this](){this->not_implemented("MauSynth2portErrlog");} )},
      {default_adapter(i_MauSynth2portErrlog_[7], chipIndex,pipeIndex,mauIndex,7,[this](){this->not_implemented("MauSynth2portErrlog");} )} } },
  i_MauSynth2portErrorCtl_  { {
      {default_adapter(i_MauSynth2portErrorCtl_[0], chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("MauSynth2portErrorCtl");} )},
      {default_adapter(i_MauSynth2portErrorCtl_[1], chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("MauSynth2portErrorCtl");} )},
      {default_adapter(i_MauSynth2portErrorCtl_[2], chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("MauSynth2portErrorCtl");} )},
      {default_adapter(i_MauSynth2portErrorCtl_[3], chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("MauSynth2portErrorCtl");} )},
      {default_adapter(i_MauSynth2portErrorCtl_[4], chipIndex,pipeIndex,mauIndex,4,[this](){this->not_implemented("MauSynth2portErrorCtl");} )},
      {default_adapter(i_MauSynth2portErrorCtl_[5], chipIndex,pipeIndex,mauIndex,5,[this](){this->not_implemented("MauSynth2portErrorCtl");} )},
      {default_adapter(i_MauSynth2portErrorCtl_[6], chipIndex,pipeIndex,mauIndex,6,[this](){this->not_implemented("MauSynth2portErrorCtl");} )},
      {default_adapter(i_MauSynth2portErrorCtl_[7], chipIndex,pipeIndex,mauIndex,7,[this](){this->not_implemented("MauSynth2portErrorCtl");} )} } },
  i_IntrEnable0MauSelectorAlu_  { {
      {default_adapter(i_IntrEnable0MauSelectorAlu_[0], chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("IntrEnable0MauSelectorAlu");} )},
      {default_adapter(i_IntrEnable0MauSelectorAlu_[1], chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("IntrEnable0MauSelectorAlu");} )},
      {default_adapter(i_IntrEnable0MauSelectorAlu_[2], chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("IntrEnable0MauSelectorAlu");} )},
      {default_adapter(i_IntrEnable0MauSelectorAlu_[3], chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("IntrEnable0MauSelectorAlu");} )} } },
  i_IntrEnable1MauSelectorAlu_  { {
      {default_adapter(i_IntrEnable1MauSelectorAlu_[0], chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("IntrEnable1MauSelectorAlu");} )},
      {default_adapter(i_IntrEnable1MauSelectorAlu_[1], chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("IntrEnable1MauSelectorAlu");} )},
      {default_adapter(i_IntrEnable1MauSelectorAlu_[2], chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("IntrEnable1MauSelectorAlu");} )},
      {default_adapter(i_IntrEnable1MauSelectorAlu_[3], chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("IntrEnable1MauSelectorAlu");} )} } },
  i_IntrInjectMauSelectorAlu_  { {
      {default_adapter(i_IntrInjectMauSelectorAlu_[0], chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("IntrInjectMauSelectorAlu");} )},
      {default_adapter(i_IntrInjectMauSelectorAlu_[1], chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("IntrInjectMauSelectorAlu");} )},
      {default_adapter(i_IntrInjectMauSelectorAlu_[2], chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("IntrInjectMauSelectorAlu");} )},
      {default_adapter(i_IntrInjectMauSelectorAlu_[3], chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("IntrInjectMauSelectorAlu");} )} } },
  i_IntrStatusMauSelectorAlu_  { {
      {default_adapter(i_IntrStatusMauSelectorAlu_[0], chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("IntrStatusMauSelectorAlu");} )},
      {default_adapter(i_IntrStatusMauSelectorAlu_[1], chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("IntrStatusMauSelectorAlu");} )},
      {default_adapter(i_IntrStatusMauSelectorAlu_[2], chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("IntrStatusMauSelectorAlu");} )},
      {default_adapter(i_IntrStatusMauSelectorAlu_[3], chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("IntrStatusMauSelectorAlu");} )} } },
  i_MauDiagMeterAluGroup_  { {
      {default_adapter(i_MauDiagMeterAluGroup_[0], chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("MauDiagMeterAluGroup");} )},
      {default_adapter(i_MauDiagMeterAluGroup_[1], chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("MauDiagMeterAluGroup");} )},
      {default_adapter(i_MauDiagMeterAluGroup_[2], chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("MauDiagMeterAluGroup");} )},
      {default_adapter(i_MauDiagMeterAluGroup_[3], chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("MauDiagMeterAluGroup");} )} } },
  i_MauSelectorAluErrlog_  { {
      {default_adapter(i_MauSelectorAluErrlog_[0], chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("MauSelectorAluErrlog");} )},
      {default_adapter(i_MauSelectorAluErrlog_[1], chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("MauSelectorAluErrlog");} )},
      {default_adapter(i_MauSelectorAluErrlog_[2], chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("MauSelectorAluErrlog");} )},
      {default_adapter(i_MauSelectorAluErrlog_[3], chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("MauSelectorAluErrlog");} )} } },
  i_IntrEnable0MauStatsAlu_  { {
      {default_adapter(i_IntrEnable0MauStatsAlu_[0], chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("IntrEnable0MauStatsAlu");} )},
      {default_adapter(i_IntrEnable0MauStatsAlu_[1], chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("IntrEnable0MauStatsAlu");} )},
      {default_adapter(i_IntrEnable0MauStatsAlu_[2], chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("IntrEnable0MauStatsAlu");} )},
      {default_adapter(i_IntrEnable0MauStatsAlu_[3], chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("IntrEnable0MauStatsAlu");} )} } },
  i_IntrEnable1MauStatsAlu_  { {
      {default_adapter(i_IntrEnable1MauStatsAlu_[0], chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("IntrEnable1MauStatsAlu");} )},
      {default_adapter(i_IntrEnable1MauStatsAlu_[1], chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("IntrEnable1MauStatsAlu");} )},
      {default_adapter(i_IntrEnable1MauStatsAlu_[2], chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("IntrEnable1MauStatsAlu");} )},
      {default_adapter(i_IntrEnable1MauStatsAlu_[3], chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("IntrEnable1MauStatsAlu");} )} } },
  i_IntrInjectMauStatsAlu_  { {
      {default_adapter(i_IntrInjectMauStatsAlu_[0], chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("IntrInjectMauStatsAlu");} )},
      {default_adapter(i_IntrInjectMauStatsAlu_[1], chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("IntrInjectMauStatsAlu");} )},
      {default_adapter(i_IntrInjectMauStatsAlu_[2], chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("IntrInjectMauStatsAlu");} )},
      {default_adapter(i_IntrInjectMauStatsAlu_[3], chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("IntrInjectMauStatsAlu");} )} } },
  i_IntrStatusMauStatsAlu_  { {
      {default_adapter(i_IntrStatusMauStatsAlu_[0], chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("IntrStatusMauStatsAlu");} )},
      {default_adapter(i_IntrStatusMauStatsAlu_[1], chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("IntrStatusMauStatsAlu");} )},
      {default_adapter(i_IntrStatusMauStatsAlu_[2], chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("IntrStatusMauStatsAlu");} )},
      {default_adapter(i_IntrStatusMauStatsAlu_[3], chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("IntrStatusMauStatsAlu");} )} } },
  i_MauDiagStatsAlu_  { {
      {default_adapter(i_MauDiagStatsAlu_[0], chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("MauDiagStatsAlu");} )},
      {default_adapter(i_MauDiagStatsAlu_[1], chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("MauDiagStatsAlu");} )},
      {default_adapter(i_MauDiagStatsAlu_[2], chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("MauDiagStatsAlu");} )},
      {default_adapter(i_MauDiagStatsAlu_[3], chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("MauDiagStatsAlu");} )} } },
  i_UnitRamEcc_  { {
      {default_adapter(i_UnitRamEcc_[0], chipIndex,pipeIndex,mauIndex,0,0,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[1], chipIndex,pipeIndex,mauIndex,0,1,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[2], chipIndex,pipeIndex,mauIndex,0,2,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[3], chipIndex,pipeIndex,mauIndex,0,3,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[4], chipIndex,pipeIndex,mauIndex,0,4,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[5], chipIndex,pipeIndex,mauIndex,0,5,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[6], chipIndex,pipeIndex,mauIndex,0,6,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[7], chipIndex,pipeIndex,mauIndex,0,7,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[8], chipIndex,pipeIndex,mauIndex,0,8,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[9], chipIndex,pipeIndex,mauIndex,0,9,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[10], chipIndex,pipeIndex,mauIndex,0,10,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[11], chipIndex,pipeIndex,mauIndex,0,11,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[12], chipIndex,pipeIndex,mauIndex,1,0,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[13], chipIndex,pipeIndex,mauIndex,1,1,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[14], chipIndex,pipeIndex,mauIndex,1,2,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[15], chipIndex,pipeIndex,mauIndex,1,3,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[16], chipIndex,pipeIndex,mauIndex,1,4,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[17], chipIndex,pipeIndex,mauIndex,1,5,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[18], chipIndex,pipeIndex,mauIndex,1,6,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[19], chipIndex,pipeIndex,mauIndex,1,7,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[20], chipIndex,pipeIndex,mauIndex,1,8,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[21], chipIndex,pipeIndex,mauIndex,1,9,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[22], chipIndex,pipeIndex,mauIndex,1,10,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[23], chipIndex,pipeIndex,mauIndex,1,11,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[24], chipIndex,pipeIndex,mauIndex,2,0,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[25], chipIndex,pipeIndex,mauIndex,2,1,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[26], chipIndex,pipeIndex,mauIndex,2,2,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[27], chipIndex,pipeIndex,mauIndex,2,3,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[28], chipIndex,pipeIndex,mauIndex,2,4,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[29], chipIndex,pipeIndex,mauIndex,2,5,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[30], chipIndex,pipeIndex,mauIndex,2,6,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[31], chipIndex,pipeIndex,mauIndex,2,7,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[32], chipIndex,pipeIndex,mauIndex,2,8,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[33], chipIndex,pipeIndex,mauIndex,2,9,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[34], chipIndex,pipeIndex,mauIndex,2,10,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[35], chipIndex,pipeIndex,mauIndex,2,11,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[36], chipIndex,pipeIndex,mauIndex,3,0,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[37], chipIndex,pipeIndex,mauIndex,3,1,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[38], chipIndex,pipeIndex,mauIndex,3,2,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[39], chipIndex,pipeIndex,mauIndex,3,3,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[40], chipIndex,pipeIndex,mauIndex,3,4,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[41], chipIndex,pipeIndex,mauIndex,3,5,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[42], chipIndex,pipeIndex,mauIndex,3,6,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[43], chipIndex,pipeIndex,mauIndex,3,7,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[44], chipIndex,pipeIndex,mauIndex,3,8,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[45], chipIndex,pipeIndex,mauIndex,3,9,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[46], chipIndex,pipeIndex,mauIndex,3,10,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[47], chipIndex,pipeIndex,mauIndex,3,11,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[48], chipIndex,pipeIndex,mauIndex,4,0,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[49], chipIndex,pipeIndex,mauIndex,4,1,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[50], chipIndex,pipeIndex,mauIndex,4,2,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[51], chipIndex,pipeIndex,mauIndex,4,3,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[52], chipIndex,pipeIndex,mauIndex,4,4,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[53], chipIndex,pipeIndex,mauIndex,4,5,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[54], chipIndex,pipeIndex,mauIndex,4,6,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[55], chipIndex,pipeIndex,mauIndex,4,7,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[56], chipIndex,pipeIndex,mauIndex,4,8,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[57], chipIndex,pipeIndex,mauIndex,4,9,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[58], chipIndex,pipeIndex,mauIndex,4,10,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[59], chipIndex,pipeIndex,mauIndex,4,11,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[60], chipIndex,pipeIndex,mauIndex,5,0,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[61], chipIndex,pipeIndex,mauIndex,5,1,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[62], chipIndex,pipeIndex,mauIndex,5,2,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[63], chipIndex,pipeIndex,mauIndex,5,3,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[64], chipIndex,pipeIndex,mauIndex,5,4,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[65], chipIndex,pipeIndex,mauIndex,5,5,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[66], chipIndex,pipeIndex,mauIndex,5,6,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[67], chipIndex,pipeIndex,mauIndex,5,7,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[68], chipIndex,pipeIndex,mauIndex,5,8,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[69], chipIndex,pipeIndex,mauIndex,5,9,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[70], chipIndex,pipeIndex,mauIndex,5,10,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[71], chipIndex,pipeIndex,mauIndex,5,11,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[72], chipIndex,pipeIndex,mauIndex,6,0,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[73], chipIndex,pipeIndex,mauIndex,6,1,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[74], chipIndex,pipeIndex,mauIndex,6,2,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[75], chipIndex,pipeIndex,mauIndex,6,3,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[76], chipIndex,pipeIndex,mauIndex,6,4,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[77], chipIndex,pipeIndex,mauIndex,6,5,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[78], chipIndex,pipeIndex,mauIndex,6,6,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[79], chipIndex,pipeIndex,mauIndex,6,7,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[80], chipIndex,pipeIndex,mauIndex,6,8,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[81], chipIndex,pipeIndex,mauIndex,6,9,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[82], chipIndex,pipeIndex,mauIndex,6,10,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[83], chipIndex,pipeIndex,mauIndex,6,11,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[84], chipIndex,pipeIndex,mauIndex,7,0,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[85], chipIndex,pipeIndex,mauIndex,7,1,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[86], chipIndex,pipeIndex,mauIndex,7,2,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[87], chipIndex,pipeIndex,mauIndex,7,3,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[88], chipIndex,pipeIndex,mauIndex,7,4,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[89], chipIndex,pipeIndex,mauIndex,7,5,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[90], chipIndex,pipeIndex,mauIndex,7,6,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[91], chipIndex,pipeIndex,mauIndex,7,7,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[92], chipIndex,pipeIndex,mauIndex,7,8,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[93], chipIndex,pipeIndex,mauIndex,7,9,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[94], chipIndex,pipeIndex,mauIndex,7,10,[this](){this->not_implemented("UnitRamEcc");} )},
      {default_adapter(i_UnitRamEcc_[95], chipIndex,pipeIndex,mauIndex,7,11,[this](){this->not_implemented("UnitRamEcc");} )} } },
  i_UnitRamMbeErrlog_  { {
      {default_adapter(i_UnitRamMbeErrlog_[0], chipIndex,pipeIndex,mauIndex,0,0,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[1], chipIndex,pipeIndex,mauIndex,0,1,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[2], chipIndex,pipeIndex,mauIndex,0,2,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[3], chipIndex,pipeIndex,mauIndex,0,3,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[4], chipIndex,pipeIndex,mauIndex,0,4,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[5], chipIndex,pipeIndex,mauIndex,0,5,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[6], chipIndex,pipeIndex,mauIndex,0,6,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[7], chipIndex,pipeIndex,mauIndex,0,7,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[8], chipIndex,pipeIndex,mauIndex,0,8,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[9], chipIndex,pipeIndex,mauIndex,0,9,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[10], chipIndex,pipeIndex,mauIndex,0,10,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[11], chipIndex,pipeIndex,mauIndex,0,11,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[12], chipIndex,pipeIndex,mauIndex,1,0,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[13], chipIndex,pipeIndex,mauIndex,1,1,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[14], chipIndex,pipeIndex,mauIndex,1,2,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[15], chipIndex,pipeIndex,mauIndex,1,3,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[16], chipIndex,pipeIndex,mauIndex,1,4,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[17], chipIndex,pipeIndex,mauIndex,1,5,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[18], chipIndex,pipeIndex,mauIndex,1,6,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[19], chipIndex,pipeIndex,mauIndex,1,7,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[20], chipIndex,pipeIndex,mauIndex,1,8,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[21], chipIndex,pipeIndex,mauIndex,1,9,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[22], chipIndex,pipeIndex,mauIndex,1,10,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[23], chipIndex,pipeIndex,mauIndex,1,11,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[24], chipIndex,pipeIndex,mauIndex,2,0,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[25], chipIndex,pipeIndex,mauIndex,2,1,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[26], chipIndex,pipeIndex,mauIndex,2,2,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[27], chipIndex,pipeIndex,mauIndex,2,3,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[28], chipIndex,pipeIndex,mauIndex,2,4,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[29], chipIndex,pipeIndex,mauIndex,2,5,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[30], chipIndex,pipeIndex,mauIndex,2,6,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[31], chipIndex,pipeIndex,mauIndex,2,7,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[32], chipIndex,pipeIndex,mauIndex,2,8,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[33], chipIndex,pipeIndex,mauIndex,2,9,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[34], chipIndex,pipeIndex,mauIndex,2,10,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[35], chipIndex,pipeIndex,mauIndex,2,11,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[36], chipIndex,pipeIndex,mauIndex,3,0,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[37], chipIndex,pipeIndex,mauIndex,3,1,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[38], chipIndex,pipeIndex,mauIndex,3,2,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[39], chipIndex,pipeIndex,mauIndex,3,3,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[40], chipIndex,pipeIndex,mauIndex,3,4,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[41], chipIndex,pipeIndex,mauIndex,3,5,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[42], chipIndex,pipeIndex,mauIndex,3,6,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[43], chipIndex,pipeIndex,mauIndex,3,7,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[44], chipIndex,pipeIndex,mauIndex,3,8,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[45], chipIndex,pipeIndex,mauIndex,3,9,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[46], chipIndex,pipeIndex,mauIndex,3,10,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[47], chipIndex,pipeIndex,mauIndex,3,11,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[48], chipIndex,pipeIndex,mauIndex,4,0,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[49], chipIndex,pipeIndex,mauIndex,4,1,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[50], chipIndex,pipeIndex,mauIndex,4,2,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[51], chipIndex,pipeIndex,mauIndex,4,3,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[52], chipIndex,pipeIndex,mauIndex,4,4,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[53], chipIndex,pipeIndex,mauIndex,4,5,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[54], chipIndex,pipeIndex,mauIndex,4,6,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[55], chipIndex,pipeIndex,mauIndex,4,7,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[56], chipIndex,pipeIndex,mauIndex,4,8,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[57], chipIndex,pipeIndex,mauIndex,4,9,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[58], chipIndex,pipeIndex,mauIndex,4,10,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[59], chipIndex,pipeIndex,mauIndex,4,11,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[60], chipIndex,pipeIndex,mauIndex,5,0,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[61], chipIndex,pipeIndex,mauIndex,5,1,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[62], chipIndex,pipeIndex,mauIndex,5,2,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[63], chipIndex,pipeIndex,mauIndex,5,3,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[64], chipIndex,pipeIndex,mauIndex,5,4,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[65], chipIndex,pipeIndex,mauIndex,5,5,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[66], chipIndex,pipeIndex,mauIndex,5,6,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[67], chipIndex,pipeIndex,mauIndex,5,7,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[68], chipIndex,pipeIndex,mauIndex,5,8,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[69], chipIndex,pipeIndex,mauIndex,5,9,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[70], chipIndex,pipeIndex,mauIndex,5,10,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[71], chipIndex,pipeIndex,mauIndex,5,11,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[72], chipIndex,pipeIndex,mauIndex,6,0,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[73], chipIndex,pipeIndex,mauIndex,6,1,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[74], chipIndex,pipeIndex,mauIndex,6,2,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[75], chipIndex,pipeIndex,mauIndex,6,3,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[76], chipIndex,pipeIndex,mauIndex,6,4,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[77], chipIndex,pipeIndex,mauIndex,6,5,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[78], chipIndex,pipeIndex,mauIndex,6,6,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[79], chipIndex,pipeIndex,mauIndex,6,7,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[80], chipIndex,pipeIndex,mauIndex,6,8,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[81], chipIndex,pipeIndex,mauIndex,6,9,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[82], chipIndex,pipeIndex,mauIndex,6,10,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[83], chipIndex,pipeIndex,mauIndex,6,11,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[84], chipIndex,pipeIndex,mauIndex,7,0,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[85], chipIndex,pipeIndex,mauIndex,7,1,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[86], chipIndex,pipeIndex,mauIndex,7,2,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[87], chipIndex,pipeIndex,mauIndex,7,3,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[88], chipIndex,pipeIndex,mauIndex,7,4,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[89], chipIndex,pipeIndex,mauIndex,7,5,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[90], chipIndex,pipeIndex,mauIndex,7,6,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[91], chipIndex,pipeIndex,mauIndex,7,7,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[92], chipIndex,pipeIndex,mauIndex,7,8,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[93], chipIndex,pipeIndex,mauIndex,7,9,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[94], chipIndex,pipeIndex,mauIndex,7,10,[this](){this->not_implemented("UnitRamMbeErrlog");} )},
      {default_adapter(i_UnitRamMbeErrlog_[95], chipIndex,pipeIndex,mauIndex,7,11,[this](){this->not_implemented("UnitRamMbeErrlog");} )} } },
  i_UnitRamSbeErrlog_  { {
      {default_adapter(i_UnitRamSbeErrlog_[0], chipIndex,pipeIndex,mauIndex,0,0,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[1], chipIndex,pipeIndex,mauIndex,0,1,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[2], chipIndex,pipeIndex,mauIndex,0,2,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[3], chipIndex,pipeIndex,mauIndex,0,3,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[4], chipIndex,pipeIndex,mauIndex,0,4,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[5], chipIndex,pipeIndex,mauIndex,0,5,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[6], chipIndex,pipeIndex,mauIndex,0,6,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[7], chipIndex,pipeIndex,mauIndex,0,7,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[8], chipIndex,pipeIndex,mauIndex,0,8,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[9], chipIndex,pipeIndex,mauIndex,0,9,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[10], chipIndex,pipeIndex,mauIndex,0,10,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[11], chipIndex,pipeIndex,mauIndex,0,11,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[12], chipIndex,pipeIndex,mauIndex,1,0,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[13], chipIndex,pipeIndex,mauIndex,1,1,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[14], chipIndex,pipeIndex,mauIndex,1,2,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[15], chipIndex,pipeIndex,mauIndex,1,3,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[16], chipIndex,pipeIndex,mauIndex,1,4,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[17], chipIndex,pipeIndex,mauIndex,1,5,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[18], chipIndex,pipeIndex,mauIndex,1,6,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[19], chipIndex,pipeIndex,mauIndex,1,7,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[20], chipIndex,pipeIndex,mauIndex,1,8,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[21], chipIndex,pipeIndex,mauIndex,1,9,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[22], chipIndex,pipeIndex,mauIndex,1,10,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[23], chipIndex,pipeIndex,mauIndex,1,11,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[24], chipIndex,pipeIndex,mauIndex,2,0,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[25], chipIndex,pipeIndex,mauIndex,2,1,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[26], chipIndex,pipeIndex,mauIndex,2,2,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[27], chipIndex,pipeIndex,mauIndex,2,3,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[28], chipIndex,pipeIndex,mauIndex,2,4,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[29], chipIndex,pipeIndex,mauIndex,2,5,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[30], chipIndex,pipeIndex,mauIndex,2,6,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[31], chipIndex,pipeIndex,mauIndex,2,7,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[32], chipIndex,pipeIndex,mauIndex,2,8,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[33], chipIndex,pipeIndex,mauIndex,2,9,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[34], chipIndex,pipeIndex,mauIndex,2,10,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[35], chipIndex,pipeIndex,mauIndex,2,11,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[36], chipIndex,pipeIndex,mauIndex,3,0,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[37], chipIndex,pipeIndex,mauIndex,3,1,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[38], chipIndex,pipeIndex,mauIndex,3,2,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[39], chipIndex,pipeIndex,mauIndex,3,3,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[40], chipIndex,pipeIndex,mauIndex,3,4,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[41], chipIndex,pipeIndex,mauIndex,3,5,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[42], chipIndex,pipeIndex,mauIndex,3,6,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[43], chipIndex,pipeIndex,mauIndex,3,7,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[44], chipIndex,pipeIndex,mauIndex,3,8,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[45], chipIndex,pipeIndex,mauIndex,3,9,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[46], chipIndex,pipeIndex,mauIndex,3,10,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[47], chipIndex,pipeIndex,mauIndex,3,11,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[48], chipIndex,pipeIndex,mauIndex,4,0,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[49], chipIndex,pipeIndex,mauIndex,4,1,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[50], chipIndex,pipeIndex,mauIndex,4,2,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[51], chipIndex,pipeIndex,mauIndex,4,3,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[52], chipIndex,pipeIndex,mauIndex,4,4,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[53], chipIndex,pipeIndex,mauIndex,4,5,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[54], chipIndex,pipeIndex,mauIndex,4,6,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[55], chipIndex,pipeIndex,mauIndex,4,7,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[56], chipIndex,pipeIndex,mauIndex,4,8,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[57], chipIndex,pipeIndex,mauIndex,4,9,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[58], chipIndex,pipeIndex,mauIndex,4,10,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[59], chipIndex,pipeIndex,mauIndex,4,11,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[60], chipIndex,pipeIndex,mauIndex,5,0,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[61], chipIndex,pipeIndex,mauIndex,5,1,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[62], chipIndex,pipeIndex,mauIndex,5,2,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[63], chipIndex,pipeIndex,mauIndex,5,3,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[64], chipIndex,pipeIndex,mauIndex,5,4,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[65], chipIndex,pipeIndex,mauIndex,5,5,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[66], chipIndex,pipeIndex,mauIndex,5,6,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[67], chipIndex,pipeIndex,mauIndex,5,7,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[68], chipIndex,pipeIndex,mauIndex,5,8,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[69], chipIndex,pipeIndex,mauIndex,5,9,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[70], chipIndex,pipeIndex,mauIndex,5,10,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[71], chipIndex,pipeIndex,mauIndex,5,11,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[72], chipIndex,pipeIndex,mauIndex,6,0,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[73], chipIndex,pipeIndex,mauIndex,6,1,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[74], chipIndex,pipeIndex,mauIndex,6,2,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[75], chipIndex,pipeIndex,mauIndex,6,3,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[76], chipIndex,pipeIndex,mauIndex,6,4,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[77], chipIndex,pipeIndex,mauIndex,6,5,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[78], chipIndex,pipeIndex,mauIndex,6,6,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[79], chipIndex,pipeIndex,mauIndex,6,7,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[80], chipIndex,pipeIndex,mauIndex,6,8,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[81], chipIndex,pipeIndex,mauIndex,6,9,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[82], chipIndex,pipeIndex,mauIndex,6,10,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[83], chipIndex,pipeIndex,mauIndex,6,11,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[84], chipIndex,pipeIndex,mauIndex,7,0,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[85], chipIndex,pipeIndex,mauIndex,7,1,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[86], chipIndex,pipeIndex,mauIndex,7,2,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[87], chipIndex,pipeIndex,mauIndex,7,3,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[88], chipIndex,pipeIndex,mauIndex,7,4,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[89], chipIndex,pipeIndex,mauIndex,7,5,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[90], chipIndex,pipeIndex,mauIndex,7,6,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[91], chipIndex,pipeIndex,mauIndex,7,7,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[92], chipIndex,pipeIndex,mauIndex,7,8,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[93], chipIndex,pipeIndex,mauIndex,7,9,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[94], chipIndex,pipeIndex,mauIndex,7,10,[this](){this->not_implemented("UnitRamSbeErrlog");} )},
      {default_adapter(i_UnitRamSbeErrlog_[95], chipIndex,pipeIndex,mauIndex,7,11,[this](){this->not_implemented("UnitRamSbeErrlog");} )} } },
  i_ActiondataErrorUramCtlArray_  { {
      {default_adapter(i_ActiondataErrorUramCtlArray_[0], chipIndex,pipeIndex,mauIndex,0,[this](uint32_t i){this->not_implemented("ActiondataErrorUramCtlArray");} )},
      {default_adapter(i_ActiondataErrorUramCtlArray_[1], chipIndex,pipeIndex,mauIndex,1,[this](uint32_t i){this->not_implemented("ActiondataErrorUramCtlArray");} )},
      {default_adapter(i_ActiondataErrorUramCtlArray_[2], chipIndex,pipeIndex,mauIndex,2,[this](uint32_t i){this->not_implemented("ActiondataErrorUramCtlArray");} )},
      {default_adapter(i_ActiondataErrorUramCtlArray_[3], chipIndex,pipeIndex,mauIndex,3,[this](uint32_t i){this->not_implemented("ActiondataErrorUramCtlArray");} )},
      {default_adapter(i_ActiondataErrorUramCtlArray_[4], chipIndex,pipeIndex,mauIndex,4,[this](uint32_t i){this->not_implemented("ActiondataErrorUramCtlArray");} )},
      {default_adapter(i_ActiondataErrorUramCtlArray_[5], chipIndex,pipeIndex,mauIndex,5,[this](uint32_t i){this->not_implemented("ActiondataErrorUramCtlArray");} )},
      {default_adapter(i_ActiondataErrorUramCtlArray_[6], chipIndex,pipeIndex,mauIndex,6,[this](uint32_t i){this->not_implemented("ActiondataErrorUramCtlArray");} )},
      {default_adapter(i_ActiondataErrorUramCtlArray_[7], chipIndex,pipeIndex,mauIndex,7,[this](uint32_t i){this->not_implemented("ActiondataErrorUramCtlArray");} )} } },
  i_EmmEccErrorUramCtlArray_  { {
      {default_adapter(i_EmmEccErrorUramCtlArray_[0], chipIndex,pipeIndex,mauIndex,0,[this](uint32_t i){this->not_implemented("EmmEccErrorUramCtlArray");} )},
      {default_adapter(i_EmmEccErrorUramCtlArray_[1], chipIndex,pipeIndex,mauIndex,1,[this](uint32_t i){this->not_implemented("EmmEccErrorUramCtlArray");} )},
      {default_adapter(i_EmmEccErrorUramCtlArray_[2], chipIndex,pipeIndex,mauIndex,2,[this](uint32_t i){this->not_implemented("EmmEccErrorUramCtlArray");} )},
      {default_adapter(i_EmmEccErrorUramCtlArray_[3], chipIndex,pipeIndex,mauIndex,3,[this](uint32_t i){this->not_implemented("EmmEccErrorUramCtlArray");} )},
      {default_adapter(i_EmmEccErrorUramCtlArray_[4], chipIndex,pipeIndex,mauIndex,4,[this](uint32_t i){this->not_implemented("EmmEccErrorUramCtlArray");} )},
      {default_adapter(i_EmmEccErrorUramCtlArray_[5], chipIndex,pipeIndex,mauIndex,5,[this](uint32_t i){this->not_implemented("EmmEccErrorUramCtlArray");} )},
      {default_adapter(i_EmmEccErrorUramCtlArray_[6], chipIndex,pipeIndex,mauIndex,6,[this](uint32_t i){this->not_implemented("EmmEccErrorUramCtlArray");} )},
      {default_adapter(i_EmmEccErrorUramCtlArray_[7], chipIndex,pipeIndex,mauIndex,7,[this](uint32_t i){this->not_implemented("EmmEccErrorUramCtlArray");} )} } },
  i_TindEccErrorUramCtlArray_  { {
      {default_adapter(i_TindEccErrorUramCtlArray_[0], chipIndex,pipeIndex,mauIndex,0,[this](uint32_t i){this->not_implemented("TindEccErrorUramCtlArray");} )},
      {default_adapter(i_TindEccErrorUramCtlArray_[1], chipIndex,pipeIndex,mauIndex,1,[this](uint32_t i){this->not_implemented("TindEccErrorUramCtlArray");} )},
      {default_adapter(i_TindEccErrorUramCtlArray_[2], chipIndex,pipeIndex,mauIndex,2,[this](uint32_t i){this->not_implemented("TindEccErrorUramCtlArray");} )},
      {default_adapter(i_TindEccErrorUramCtlArray_[3], chipIndex,pipeIndex,mauIndex,3,[this](uint32_t i){this->not_implemented("TindEccErrorUramCtlArray");} )},
      {default_adapter(i_TindEccErrorUramCtlArray_[4], chipIndex,pipeIndex,mauIndex,4,[this](uint32_t i){this->not_implemented("TindEccErrorUramCtlArray");} )},
      {default_adapter(i_TindEccErrorUramCtlArray_[5], chipIndex,pipeIndex,mauIndex,5,[this](uint32_t i){this->not_implemented("TindEccErrorUramCtlArray");} )},
      {default_adapter(i_TindEccErrorUramCtlArray_[6], chipIndex,pipeIndex,mauIndex,6,[this](uint32_t i){this->not_implemented("TindEccErrorUramCtlArray");} )},
      {default_adapter(i_TindEccErrorUramCtlArray_[7], chipIndex,pipeIndex,mauIndex,7,[this](uint32_t i){this->not_implemented("TindEccErrorUramCtlArray");} )} } },
  i_IntrEnable0MauUnitRamRow_  { {
      {default_adapter(i_IntrEnable0MauUnitRamRow_[0], chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("IntrEnable0MauUnitRamRow");} )},
      {default_adapter(i_IntrEnable0MauUnitRamRow_[1], chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("IntrEnable0MauUnitRamRow");} )},
      {default_adapter(i_IntrEnable0MauUnitRamRow_[2], chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("IntrEnable0MauUnitRamRow");} )},
      {default_adapter(i_IntrEnable0MauUnitRamRow_[3], chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("IntrEnable0MauUnitRamRow");} )},
      {default_adapter(i_IntrEnable0MauUnitRamRow_[4], chipIndex,pipeIndex,mauIndex,4,[this](){this->not_implemented("IntrEnable0MauUnitRamRow");} )},
      {default_adapter(i_IntrEnable0MauUnitRamRow_[5], chipIndex,pipeIndex,mauIndex,5,[this](){this->not_implemented("IntrEnable0MauUnitRamRow");} )},
      {default_adapter(i_IntrEnable0MauUnitRamRow_[6], chipIndex,pipeIndex,mauIndex,6,[this](){this->not_implemented("IntrEnable0MauUnitRamRow");} )},
      {default_adapter(i_IntrEnable0MauUnitRamRow_[7], chipIndex,pipeIndex,mauIndex,7,[this](){this->not_implemented("IntrEnable0MauUnitRamRow");} )} } },
  i_IntrEnable1MauUnitRamRow_  { {
      {default_adapter(i_IntrEnable1MauUnitRamRow_[0], chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("IntrEnable1MauUnitRamRow");} )},
      {default_adapter(i_IntrEnable1MauUnitRamRow_[1], chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("IntrEnable1MauUnitRamRow");} )},
      {default_adapter(i_IntrEnable1MauUnitRamRow_[2], chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("IntrEnable1MauUnitRamRow");} )},
      {default_adapter(i_IntrEnable1MauUnitRamRow_[3], chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("IntrEnable1MauUnitRamRow");} )},
      {default_adapter(i_IntrEnable1MauUnitRamRow_[4], chipIndex,pipeIndex,mauIndex,4,[this](){this->not_implemented("IntrEnable1MauUnitRamRow");} )},
      {default_adapter(i_IntrEnable1MauUnitRamRow_[5], chipIndex,pipeIndex,mauIndex,5,[this](){this->not_implemented("IntrEnable1MauUnitRamRow");} )},
      {default_adapter(i_IntrEnable1MauUnitRamRow_[6], chipIndex,pipeIndex,mauIndex,6,[this](){this->not_implemented("IntrEnable1MauUnitRamRow");} )},
      {default_adapter(i_IntrEnable1MauUnitRamRow_[7], chipIndex,pipeIndex,mauIndex,7,[this](){this->not_implemented("IntrEnable1MauUnitRamRow");} )} } },
  i_IntrFreezeEnableMauUnitRamRow_  { {
      {default_adapter(i_IntrFreezeEnableMauUnitRamRow_[0], chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("IntrFreezeEnableMauUnitRamRow");} )},
      {default_adapter(i_IntrFreezeEnableMauUnitRamRow_[1], chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("IntrFreezeEnableMauUnitRamRow");} )},
      {default_adapter(i_IntrFreezeEnableMauUnitRamRow_[2], chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("IntrFreezeEnableMauUnitRamRow");} )},
      {default_adapter(i_IntrFreezeEnableMauUnitRamRow_[3], chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("IntrFreezeEnableMauUnitRamRow");} )},
      {default_adapter(i_IntrFreezeEnableMauUnitRamRow_[4], chipIndex,pipeIndex,mauIndex,4,[this](){this->not_implemented("IntrFreezeEnableMauUnitRamRow");} )},
      {default_adapter(i_IntrFreezeEnableMauUnitRamRow_[5], chipIndex,pipeIndex,mauIndex,5,[this](){this->not_implemented("IntrFreezeEnableMauUnitRamRow");} )},
      {default_adapter(i_IntrFreezeEnableMauUnitRamRow_[6], chipIndex,pipeIndex,mauIndex,6,[this](){this->not_implemented("IntrFreezeEnableMauUnitRamRow");} )},
      {default_adapter(i_IntrFreezeEnableMauUnitRamRow_[7], chipIndex,pipeIndex,mauIndex,7,[this](){this->not_implemented("IntrFreezeEnableMauUnitRamRow");} )} } },
  i_IntrInjectMauUnitRamRow_  { {
      {default_adapter(i_IntrInjectMauUnitRamRow_[0], chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("IntrInjectMauUnitRamRow");} )},
      {default_adapter(i_IntrInjectMauUnitRamRow_[1], chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("IntrInjectMauUnitRamRow");} )},
      {default_adapter(i_IntrInjectMauUnitRamRow_[2], chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("IntrInjectMauUnitRamRow");} )},
      {default_adapter(i_IntrInjectMauUnitRamRow_[3], chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("IntrInjectMauUnitRamRow");} )},
      {default_adapter(i_IntrInjectMauUnitRamRow_[4], chipIndex,pipeIndex,mauIndex,4,[this](){this->not_implemented("IntrInjectMauUnitRamRow");} )},
      {default_adapter(i_IntrInjectMauUnitRamRow_[5], chipIndex,pipeIndex,mauIndex,5,[this](){this->not_implemented("IntrInjectMauUnitRamRow");} )},
      {default_adapter(i_IntrInjectMauUnitRamRow_[6], chipIndex,pipeIndex,mauIndex,6,[this](){this->not_implemented("IntrInjectMauUnitRamRow");} )},
      {default_adapter(i_IntrInjectMauUnitRamRow_[7], chipIndex,pipeIndex,mauIndex,7,[this](){this->not_implemented("IntrInjectMauUnitRamRow");} )} } },
  i_IntrStatusMauUnitRamRow_  { {
      {default_adapter(i_IntrStatusMauUnitRamRow_[0], chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("IntrStatusMauUnitRamRow");} )},
      {default_adapter(i_IntrStatusMauUnitRamRow_[1], chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("IntrStatusMauUnitRamRow");} )},
      {default_adapter(i_IntrStatusMauUnitRamRow_[2], chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("IntrStatusMauUnitRamRow");} )},
      {default_adapter(i_IntrStatusMauUnitRamRow_[3], chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("IntrStatusMauUnitRamRow");} )},
      {default_adapter(i_IntrStatusMauUnitRamRow_[4], chipIndex,pipeIndex,mauIndex,4,[this](){this->not_implemented("IntrStatusMauUnitRamRow");} )},
      {default_adapter(i_IntrStatusMauUnitRamRow_[5], chipIndex,pipeIndex,mauIndex,5,[this](){this->not_implemented("IntrStatusMauUnitRamRow");} )},
      {default_adapter(i_IntrStatusMauUnitRamRow_[6], chipIndex,pipeIndex,mauIndex,6,[this](){this->not_implemented("IntrStatusMauUnitRamRow");} )},
      {default_adapter(i_IntrStatusMauUnitRamRow_[7], chipIndex,pipeIndex,mauIndex,7,[this](){this->not_implemented("IntrStatusMauUnitRamRow");} )} } },
  i_MauDiagRowAdbClkEnable_  { {
      {default_adapter(i_MauDiagRowAdbClkEnable_[0], chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("MauDiagRowAdbClkEnable");} )},
      {default_adapter(i_MauDiagRowAdbClkEnable_[1], chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("MauDiagRowAdbClkEnable");} )},
      {default_adapter(i_MauDiagRowAdbClkEnable_[2], chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("MauDiagRowAdbClkEnable");} )},
      {default_adapter(i_MauDiagRowAdbClkEnable_[3], chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("MauDiagRowAdbClkEnable");} )},
      {default_adapter(i_MauDiagRowAdbClkEnable_[4], chipIndex,pipeIndex,mauIndex,4,[this](){this->not_implemented("MauDiagRowAdbClkEnable");} )},
      {default_adapter(i_MauDiagRowAdbClkEnable_[5], chipIndex,pipeIndex,mauIndex,5,[this](){this->not_implemented("MauDiagRowAdbClkEnable");} )},
      {default_adapter(i_MauDiagRowAdbClkEnable_[6], chipIndex,pipeIndex,mauIndex,6,[this](){this->not_implemented("MauDiagRowAdbClkEnable");} )},
      {default_adapter(i_MauDiagRowAdbClkEnable_[7], chipIndex,pipeIndex,mauIndex,7,[this](){this->not_implemented("MauDiagRowAdbClkEnable");} )} } },
  i_BubbleReqCtlArray_(default_adapter(i_BubbleReqCtlArray_,chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("BubbleReqCtlArray");})),
  i_DeferredMeterParityControlArray_(default_adapter(i_DeferredMeterParityControlArray_,chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("DeferredMeterParityControlArray");})),
  i_DeferredStatsParityControlArray_(default_adapter(i_DeferredStatsParityControlArray_,chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("DeferredStatsParityControlArray");})),
  i_DeferredStatsParityErrlogArray_(default_adapter(i_DeferredStatsParityErrlogArray_,chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("DeferredStatsParityErrlogArray");})),
  i_DefMeterSbeErrlogArray_(default_adapter(i_DefMeterSbeErrlogArray_,chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("DefMeterSbeErrlogArray");})),
  i_EmmEccErrorCtlArray_(default_adapter(i_EmmEccErrorCtlArray_,chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("EmmEccErrorCtlArray");})),
  i_ErrIdataOvrCtlArray_(default_adapter(i_ErrIdataOvrCtlArray_,chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("ErrIdataOvrCtlArray");})),
  i_ErrIdataOvrFifoCtlArray_(default_adapter(i_ErrIdataOvrFifoCtlArray_,chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("ErrIdataOvrFifoCtlArray");})),
  i_GfmParityErrorCtlArray_(default_adapter(i_GfmParityErrorCtlArray_,chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("GfmParityErrorCtlArray");})),
  i_IdleBubbleReqArray_(default_adapter(i_IdleBubbleReqArray_,chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("IdleBubbleReqArray");})),
  i_IdletimeSlipArray_(default_adapter(i_IdletimeSlipArray_,chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("IdletimeSlipArray");})),
  i_IdletimeSlipIntrCtlArray_(default_adapter(i_IdletimeSlipIntrCtlArray_,chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("IdletimeSlipIntrCtlArray");})),
  i_IntrMauDecodeMemoryCoreArray_(default_adapter(i_IntrMauDecodeMemoryCoreArray_,chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("IntrMauDecodeMemoryCoreArray");})),
  i_MauCfgMramThreadArray_(default_adapter(i_MauCfgMramThreadArray_,chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("MauCfgMramThreadArray");})),
  i_MauCfgUramThreadArray_(default_adapter(i_MauCfgUramThreadArray_,chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("MauCfgUramThreadArray");})),
  i_MauMatchInputXbarExactMatchEnableArray_(default_adapter(i_MauMatchInputXbarExactMatchEnableArray_,chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("MauMatchInputXbarExactMatchEnableArray");})),
  i_MauMatchInputXbarTernaryMatchEnableArray_(default_adapter(i_MauMatchInputXbarTernaryMatchEnableArray_,chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("MauMatchInputXbarTernaryMatchEnableArray");})),
  i_MauSnapshotCaptureDatapathErrorArray_(default_adapter(i_MauSnapshotCaptureDatapathErrorArray_,chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("MauSnapshotCaptureDatapathErrorArray");})),
  i_MeterAluGroupErrorCtlArray_(default_adapter(i_MeterAluGroupErrorCtlArray_,chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("MeterAluGroupErrorCtlArray");})),
  i_MeterBubbleReqArray_(default_adapter(i_MeterBubbleReqArray_,chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("MeterBubbleReqArray");})),
  i_OErrorFifoCtlArray_(default_adapter(i_OErrorFifoCtlArray_,chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("OErrorFifoCtlArray");})),
  i_PbsCreqErrlogArray_(default_adapter(i_PbsCreqErrlogArray_,chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("PbsCreqErrlogArray");})),
  i_PbsCrespErrlogArray_(default_adapter(i_PbsCrespErrlogArray_,chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("PbsCrespErrlogArray");})),
  i_PbsSreqErrlogArray_(default_adapter(i_PbsSreqErrlogArray_,chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("PbsSreqErrlogArray");})),
  i_PrevErrorCtlArray_(default_adapter(i_PrevErrorCtlArray_,chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("PrevErrorCtlArray");})),
  i_S2pMeterErrorCtlArray_(default_adapter(i_S2pMeterErrorCtlArray_,chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("S2pMeterErrorCtlArray");})),
  i_S2pStatsErrorCtlArray_(default_adapter(i_S2pStatsErrorCtlArray_,chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("S2pStatsErrorCtlArray");})),
  i_StatsBubbleReqArray_(default_adapter(i_StatsBubbleReqArray_,chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("StatsBubbleReqArray");})),
  i_StatsLrtFsmSweepOffsetArray_(default_adapter(i_StatsLrtFsmSweepOffsetArray_,chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("StatsLrtFsmSweepOffsetArray");})),
  i_StatsLrtSweepAdrArray_(default_adapter(i_StatsLrtSweepAdrArray_,chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("StatsLrtSweepAdrArray");})),
  i_TcamLogicalChannelErrlogHiArray_(default_adapter(i_TcamLogicalChannelErrlogHiArray_,chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("TcamLogicalChannelErrlogHiArray");})),
  i_TcamLogicalChannelErrlogLoArray_(default_adapter(i_TcamLogicalChannelErrlogLoArray_,chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("TcamLogicalChannelErrlogLoArray");})),
  i_TcamMatchErrorCtlArray_(default_adapter(i_TcamMatchErrorCtlArray_,chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("TcamMatchErrorCtlArray");})),
  i_TcamOutputTableThreadArray_(default_adapter(i_TcamOutputTableThreadArray_,chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("TcamOutputTableThreadArray");})),
  i_TcamParityControlArray_(default_adapter(i_TcamParityControlArray_,chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("TcamParityControlArray");})),
  i_TcamSbeErrlogArray_(default_adapter(i_TcamSbeErrlogArray_,chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("TcamSbeErrlogArray");})),
  i_TcamTablePropArray_(default_adapter(i_TcamTablePropArray_,chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("TcamTablePropArray");})),
  i_TindEccErrorCtlArray_(default_adapter(i_TindEccErrorCtlArray_,chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("TindEccErrorCtlArray");})),
  i_ActiondataErrorCtl_(default_adapter(i_ActiondataErrorCtl_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("ActiondataErrorCtl");})),
  i_AdrDistMemSlowMode_(default_adapter(i_AdrDistMemSlowMode_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("AdrDistMemSlowMode");})),
  i_HashoutCtl_(default_adapter(i_HashoutCtl_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("HashoutCtl");})),
  i_IdletimeSlipErrlog_(default_adapter(i_IdletimeSlipErrlog_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("IdletimeSlipErrlog");})),
  i_ImemParityErrorCtl_(default_adapter(i_ImemParityErrorCtl_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("ImemParityErrorCtl");})),
  i_ImemSbeErrlog_(default_adapter(i_ImemSbeErrlog_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("ImemSbeErrlog");})),
  i_IntrDecodeTop_(default_adapter(i_IntrDecodeTop_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("IntrDecodeTop");})),
  i_IntrEnable0MauAd_(default_adapter(i_IntrEnable0MauAd_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("IntrEnable0MauAd");})),
  i_IntrEnable0MauCfg_(default_adapter(i_IntrEnable0MauCfg_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("IntrEnable0MauCfg");})),
  i_IntrEnable0MauGfmHash_(default_adapter(i_IntrEnable0MauGfmHash_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("IntrEnable0MauGfmHash");})),
  i_IntrEnable0MauImem_(default_adapter(i_IntrEnable0MauImem_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("IntrEnable0MauImem");})),
  i_IntrEnable0MauSnapshot_(default_adapter(i_IntrEnable0MauSnapshot_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("IntrEnable0MauSnapshot");})),
  i_IntrEnable1MauAd_(default_adapter(i_IntrEnable1MauAd_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("IntrEnable1MauAd");})),
  i_IntrEnable1MauCfg_(default_adapter(i_IntrEnable1MauCfg_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("IntrEnable1MauCfg");})),
  i_IntrEnable1MauGfmHash_(default_adapter(i_IntrEnable1MauGfmHash_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("IntrEnable1MauGfmHash");})),
  i_IntrEnable1MauImem_(default_adapter(i_IntrEnable1MauImem_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("IntrEnable1MauImem");})),
  i_IntrEnable1MauSnapshot_(default_adapter(i_IntrEnable1MauSnapshot_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("IntrEnable1MauSnapshot");})),
  i_IntrFreezeEnableMauAd_(default_adapter(i_IntrFreezeEnableMauAd_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("IntrFreezeEnableMauAd");})),
  i_IntrFreezeEnableMauCfg_(default_adapter(i_IntrFreezeEnableMauCfg_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("IntrFreezeEnableMauCfg");})),
  i_IntrFreezeEnableMauGfmHash_(default_adapter(i_IntrFreezeEnableMauGfmHash_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("IntrFreezeEnableMauGfmHash");})),
  i_IntrFreezeEnableMauImem_(default_adapter(i_IntrFreezeEnableMauImem_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("IntrFreezeEnableMauImem");})),
  i_IntrFreezeEnableMauSnapshot_(default_adapter(i_IntrFreezeEnableMauSnapshot_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("IntrFreezeEnableMauSnapshot");})),
  i_IntrInjectMauAd_(default_adapter(i_IntrInjectMauAd_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("IntrInjectMauAd");})),
  i_IntrInjectMauCfg_(default_adapter(i_IntrInjectMauCfg_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("IntrInjectMauCfg");})),
  i_IntrInjectMauGfmHash_(default_adapter(i_IntrInjectMauGfmHash_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("IntrInjectMauGfmHash");})),
  i_IntrInjectMauImem_(default_adapter(i_IntrInjectMauImem_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("IntrInjectMauImem");})),
  i_IntrInjectMauSnapshot_(default_adapter(i_IntrInjectMauSnapshot_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("IntrInjectMauSnapshot");})),
  i_IntrStatusMauCfg_(default_adapter(i_IntrStatusMauCfg_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("IntrStatusMauCfg");})),
  i_IntrStatusMauGfmHash_(default_adapter(i_IntrStatusMauGfmHash_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("IntrStatusMauGfmHash");})),
  i_IntrStatusMauImem_(default_adapter(i_IntrStatusMauImem_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("IntrStatusMauImem");})),
  i_MauCfgDramThread_(default_adapter(i_MauCfgDramThread_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("MauCfgDramThread");})),
  i_MauCfgImemBubbleReq_(default_adapter(i_MauCfgImemBubbleReq_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("MauCfgImemBubbleReq");})),
  i_MauCfgLtThread_(default_adapter(i_MauCfgLtThread_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("MauCfgLtThread");})),
  i_MauCfgMemSlowMode_(default_adapter(i_MauCfgMemSlowMode_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("MauCfgMemSlowMode");})),
  i_MauDiag_32bOxbarCtl_(default_adapter(i_MauDiag_32bOxbarCtl_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("MauDiag_32bOxbarCtl");})),
  i_MauDiag_32bOxbarPremuxCtl_(default_adapter(i_MauDiag_32bOxbarPremuxCtl_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("MauDiag_32bOxbarPremuxCtl");})),
  i_MauDiag_8bOxbarCtl_(default_adapter(i_MauDiag_8bOxbarCtl_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("MauDiag_8bOxbarCtl");})),
  i_MauDiagAdbCtl_(default_adapter(i_MauDiagAdbCtl_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("MauDiagAdbCtl");})),
  i_MauDiagAdbMap_(default_adapter(i_MauDiagAdbMap_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("MauDiagAdbMap");})),
  i_MauDiagAdrDistIdletimeAdrOxbarCtl_(default_adapter(i_MauDiagAdrDistIdletimeAdrOxbarCtl_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("MauDiagAdrDistIdletimeAdrOxbarCtl");})),
  i_MauDiagCfgCtl_(default_adapter(i_MauDiagCfgCtl_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("MauDiagCfgCtl");})),
  i_MauDiagEopVldXport_(default_adapter(i_MauDiagEopVldXport_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("MauDiagEopVldXport");})),
  i_MauDiagMeterAdrSel_(default_adapter(i_MauDiagMeterAdrSel_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("MauDiagMeterAdrSel");})),
  i_MauDiagPbusEnable_(default_adapter(i_MauDiagPbusEnable_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("MauDiagPbusEnable");})),
  i_MauDiagStatsAdrSel_(default_adapter(i_MauDiagStatsAdrSel_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("MauDiagStatsAdrSel");})),
  i_MauDiagTcamClkEn_(default_adapter(i_MauDiagTcamClkEn_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("MauDiagTcamClkEn");})),
  i_MauDiagTcamHitXbarCtl_(default_adapter(i_MauDiagTcamHitXbarCtl_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("MauDiagTcamHitXbarCtl");})),
  i_MauDiagValidCtl_(default_adapter(i_MauDiagValidCtl_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("MauDiagValidCtl");})),
  i_MeterSweepErrlog_(default_adapter(i_MeterSweepErrlog_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("MeterSweepErrlog");})),
  i_PbsCreqEcc_(default_adapter(i_PbsCreqEcc_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("PbsCreqEcc");})),
  i_PbsCrespEcc_(default_adapter(i_PbsCrespEcc_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("PbsCrespEcc");})),
  i_PbsSreqEcc_(default_adapter(i_PbsSreqEcc_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("PbsSreqEcc");})),
  i_QHoleAccErrlogHi_(default_adapter(i_QHoleAccErrlogHi_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("QHoleAccErrlogHi");})),
  i_QHoleAccErrlogLo_(default_adapter(i_QHoleAccErrlogLo_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("QHoleAccErrlogLo");})),
  i_TcamPiped_(default_adapter(i_TcamPiped_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("TcamPiped");})),
  i_TcamScrubCtl_(default_adapter(i_TcamScrubCtl_,chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("TcamScrubCtl");}))
  {
  reset();
  }
void MauIgnoredRegs::reset() {
  for (int i = 0; i < 8; i++) {
    i_IdletimeLogicalToPhysicalSweepGrantCtlArray_[i].reset();
    i_IdletimePhysicalToLogicalReqIncCtlArray_[i].reset();
    i_MapramMbeErrlogArray_[i].reset();
    i_MapramSbeErrlogArray_[i].reset();
    i_AdrmuxRowMemSlowMode_[i].reset();
    i_IntrEnable0MauAdrmuxRow_[i].reset();
    i_IntrEnable0MauSynth2port_[i].reset();
    i_IntrEnable1MauAdrmuxRow_[i].reset();
    i_IntrEnable1MauSynth2port_[i].reset();
    i_IntrFreezeEnableMauAdrmuxRow_[i].reset();
    i_IntrInjectMauAdrmuxRow_[i].reset();
    i_IntrInjectMauSynth2port_[i].reset();
    i_IntrStatusMauAdrmuxRow_[i].reset();
    i_IntrStatusMauSynth2port_[i].reset();
    i_MapramMbeInj_[i].reset();
    i_MapramSbeInj_[i].reset();
    i_MauSynth2portErrlog_[i].reset();
    i_MauSynth2portErrorCtl_[i].reset();

    i_ActiondataErrorUramCtlArray_[i].reset();
    i_EmmEccErrorUramCtlArray_[i].reset();
    i_TindEccErrorUramCtlArray_[i].reset();
    i_IntrEnable0MauUnitRamRow_[i].reset();
    i_IntrEnable1MauUnitRamRow_[i].reset();
    i_IntrFreezeEnableMauUnitRamRow_[i].reset();
    i_IntrInjectMauUnitRamRow_[i].reset();
    i_IntrStatusMauUnitRamRow_[i].reset();
    i_MauDiagRowAdbClkEnable_[i].reset();
  }

  for (int i = 0; i < 4; i++) {
    i_IntrEnable0MauSelectorAlu_[i].reset();
    i_IntrEnable1MauSelectorAlu_[i].reset();
    i_IntrInjectMauSelectorAlu_[i].reset();
    i_IntrStatusMauSelectorAlu_[i].reset();
    i_MauDiagMeterAluGroup_[i].reset();
    i_MauSelectorAluErrlog_[i].reset();
    i_IntrEnable0MauStatsAlu_[i].reset();
    i_IntrEnable1MauStatsAlu_[i].reset();
    i_IntrInjectMauStatsAlu_[i].reset();
    i_IntrStatusMauStatsAlu_[i].reset();
    i_MauDiagStatsAlu_[i].reset();
  }

  for (int i = 0; i < 96; i++) {
    i_UnitRamEcc_[i].reset();
    i_UnitRamMbeErrlog_[i].reset();
    i_UnitRamSbeErrlog_[i].reset();
  }

  i_BubbleReqCtlArray_.reset();
  i_DeferredMeterParityControlArray_.reset();
  i_DeferredStatsParityControlArray_.reset();
  i_DeferredStatsParityErrlogArray_.reset();
  i_DefMeterSbeErrlogArray_.reset();
  i_EmmEccErrorCtlArray_.reset();
  i_ErrIdataOvrCtlArray_.reset();
  i_ErrIdataOvrFifoCtlArray_.reset();
  i_GfmParityErrorCtlArray_.reset();
  i_IdleBubbleReqArray_.reset();
  i_IdletimeSlipArray_.reset();
  i_IdletimeSlipIntrCtlArray_.reset();
  i_IntrMauDecodeMemoryCoreArray_.reset();
  i_MauCfgMramThreadArray_.reset();
  i_MauCfgUramThreadArray_.reset();
  i_MauMatchInputXbarExactMatchEnableArray_.reset();
  i_MauMatchInputXbarTernaryMatchEnableArray_.reset();
  i_MauSnapshotCaptureDatapathErrorArray_.reset();
  i_MeterAluGroupErrorCtlArray_.reset();
  i_MeterBubbleReqArray_.reset();
  i_OErrorFifoCtlArray_.reset();
  i_PbsCreqErrlogArray_.reset();
  i_PbsCrespErrlogArray_.reset();
  i_PbsSreqErrlogArray_.reset();
  i_PrevErrorCtlArray_.reset();
  i_S2pMeterErrorCtlArray_.reset();
  i_S2pStatsErrorCtlArray_.reset();
  i_StatsBubbleReqArray_.reset();
  i_StatsLrtFsmSweepOffsetArray_.reset();
  i_StatsLrtSweepAdrArray_.reset();
  i_TcamLogicalChannelErrlogHiArray_.reset();
  i_TcamLogicalChannelErrlogLoArray_.reset();
  i_TcamMatchErrorCtlArray_.reset();
  i_TcamOutputTableThreadArray_.reset();
  i_TcamParityControlArray_.reset();
  i_TcamSbeErrlogArray_.reset();
  i_TcamTablePropArray_.reset();
  i_TindEccErrorCtlArray_.reset();
  i_ActiondataErrorCtl_.reset();
  i_AdrDistMemSlowMode_.reset();
  i_HashoutCtl_.reset();
  i_IdletimeSlipErrlog_.reset();
  i_ImemParityErrorCtl_.reset();
  i_ImemSbeErrlog_.reset();
  i_IntrDecodeTop_.reset();
  i_IntrEnable0MauAd_.reset();
  i_IntrEnable0MauCfg_.reset();
  i_IntrEnable0MauGfmHash_.reset();
  i_IntrEnable0MauImem_.reset();
  i_IntrEnable0MauSnapshot_.reset();
  i_IntrEnable1MauAd_.reset();
  i_IntrEnable1MauCfg_.reset();
  i_IntrEnable1MauGfmHash_.reset();
  i_IntrEnable1MauImem_.reset();
  i_IntrEnable1MauSnapshot_.reset();
  i_IntrFreezeEnableMauAd_.reset();
  i_IntrFreezeEnableMauCfg_.reset();
  i_IntrFreezeEnableMauGfmHash_.reset();
  i_IntrFreezeEnableMauImem_.reset();
  i_IntrFreezeEnableMauSnapshot_.reset();
  i_IntrInjectMauAd_.reset();
  i_IntrInjectMauCfg_.reset();
  i_IntrInjectMauGfmHash_.reset();
  i_IntrInjectMauImem_.reset();
  i_IntrInjectMauSnapshot_.reset();
  i_IntrStatusMauCfg_.reset();
  i_IntrStatusMauGfmHash_.reset();
  i_IntrStatusMauImem_.reset();
  i_MauCfgDramThread_.reset();
  i_MauCfgImemBubbleReq_.reset();
  i_MauCfgLtThread_.reset();
  i_MauCfgMemSlowMode_.reset();
  i_MauDiag_32bOxbarCtl_.reset();
  i_MauDiag_32bOxbarPremuxCtl_.reset();
  i_MauDiag_8bOxbarCtl_.reset();
  i_MauDiagAdbCtl_.reset();
  i_MauDiagAdbMap_.reset();
  i_MauDiagAdrDistIdletimeAdrOxbarCtl_.reset();
  i_MauDiagCfgCtl_.reset();
  i_MauDiagEopVldXport_.reset();
  i_MauDiagMeterAdrSel_.reset();
  i_MauDiagPbusEnable_.reset();
  i_MauDiagStatsAdrSel_.reset();
  i_MauDiagTcamClkEn_.reset();
  i_MauDiagTcamHitXbarCtl_.reset();
  i_MauDiagValidCtl_.reset();
  i_MeterSweepErrlog_.reset();
  i_PbsCreqEcc_.reset();
  i_PbsCrespEcc_.reset();
  i_PbsSreqEcc_.reset();
  i_QHoleAccErrlogHi_.reset();
  i_QHoleAccErrlogLo_.reset();
  i_TcamPiped_.reset();
  i_TcamScrubCtl_.reset();
}
}
