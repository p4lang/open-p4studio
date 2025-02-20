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
      { chipIndex,pipeIndex,mauIndex,0,[this](uint32_t i){this->not_implemented("IdletimeLogicalToPhysicalSweepGrantCtlArray");} },
      { chipIndex,pipeIndex,mauIndex,1,[this](uint32_t i){this->not_implemented("IdletimeLogicalToPhysicalSweepGrantCtlArray");} },
      { chipIndex,pipeIndex,mauIndex,2,[this](uint32_t i){this->not_implemented("IdletimeLogicalToPhysicalSweepGrantCtlArray");} },
      { chipIndex,pipeIndex,mauIndex,3,[this](uint32_t i){this->not_implemented("IdletimeLogicalToPhysicalSweepGrantCtlArray");} },
      { chipIndex,pipeIndex,mauIndex,4,[this](uint32_t i){this->not_implemented("IdletimeLogicalToPhysicalSweepGrantCtlArray");} },
      { chipIndex,pipeIndex,mauIndex,5,[this](uint32_t i){this->not_implemented("IdletimeLogicalToPhysicalSweepGrantCtlArray");} },
      { chipIndex,pipeIndex,mauIndex,6,[this](uint32_t i){this->not_implemented("IdletimeLogicalToPhysicalSweepGrantCtlArray");} },
      { chipIndex,pipeIndex,mauIndex,7,[this](uint32_t i){this->not_implemented("IdletimeLogicalToPhysicalSweepGrantCtlArray");} } } },
  i_IdletimePhysicalToLogicalReqIncCtlArray_  { {
      { chipIndex,pipeIndex,mauIndex,0,[this](uint32_t i){this->not_implemented("IdletimePhysicalToLogicalReqIncCtlArray");} },
      { chipIndex,pipeIndex,mauIndex,1,[this](uint32_t i){this->not_implemented("IdletimePhysicalToLogicalReqIncCtlArray");} },
      { chipIndex,pipeIndex,mauIndex,2,[this](uint32_t i){this->not_implemented("IdletimePhysicalToLogicalReqIncCtlArray");} },
      { chipIndex,pipeIndex,mauIndex,3,[this](uint32_t i){this->not_implemented("IdletimePhysicalToLogicalReqIncCtlArray");} },
      { chipIndex,pipeIndex,mauIndex,4,[this](uint32_t i){this->not_implemented("IdletimePhysicalToLogicalReqIncCtlArray");} },
      { chipIndex,pipeIndex,mauIndex,5,[this](uint32_t i){this->not_implemented("IdletimePhysicalToLogicalReqIncCtlArray");} },
      { chipIndex,pipeIndex,mauIndex,6,[this](uint32_t i){this->not_implemented("IdletimePhysicalToLogicalReqIncCtlArray");} },
      { chipIndex,pipeIndex,mauIndex,7,[this](uint32_t i){this->not_implemented("IdletimePhysicalToLogicalReqIncCtlArray");} } } },
  i_MapramMbeErrlogArray_  { {
      { chipIndex,pipeIndex,mauIndex,0,[this](uint32_t i){this->not_implemented("MapramMbeErrlogArray");} },
      { chipIndex,pipeIndex,mauIndex,1,[this](uint32_t i){this->not_implemented("MapramMbeErrlogArray");} },
      { chipIndex,pipeIndex,mauIndex,2,[this](uint32_t i){this->not_implemented("MapramMbeErrlogArray");} },
      { chipIndex,pipeIndex,mauIndex,3,[this](uint32_t i){this->not_implemented("MapramMbeErrlogArray");} },
      { chipIndex,pipeIndex,mauIndex,4,[this](uint32_t i){this->not_implemented("MapramMbeErrlogArray");} },
      { chipIndex,pipeIndex,mauIndex,5,[this](uint32_t i){this->not_implemented("MapramMbeErrlogArray");} },
      { chipIndex,pipeIndex,mauIndex,6,[this](uint32_t i){this->not_implemented("MapramMbeErrlogArray");} },
      { chipIndex,pipeIndex,mauIndex,7,[this](uint32_t i){this->not_implemented("MapramMbeErrlogArray");} } } },
  i_MapramSbeErrlogArray_  { {
      { chipIndex,pipeIndex,mauIndex,0,[this](uint32_t i){this->not_implemented("MapramSbeErrlogArray");} },
      { chipIndex,pipeIndex,mauIndex,1,[this](uint32_t i){this->not_implemented("MapramSbeErrlogArray");} },
      { chipIndex,pipeIndex,mauIndex,2,[this](uint32_t i){this->not_implemented("MapramSbeErrlogArray");} },
      { chipIndex,pipeIndex,mauIndex,3,[this](uint32_t i){this->not_implemented("MapramSbeErrlogArray");} },
      { chipIndex,pipeIndex,mauIndex,4,[this](uint32_t i){this->not_implemented("MapramSbeErrlogArray");} },
      { chipIndex,pipeIndex,mauIndex,5,[this](uint32_t i){this->not_implemented("MapramSbeErrlogArray");} },
      { chipIndex,pipeIndex,mauIndex,6,[this](uint32_t i){this->not_implemented("MapramSbeErrlogArray");} },
      { chipIndex,pipeIndex,mauIndex,7,[this](uint32_t i){this->not_implemented("MapramSbeErrlogArray");} } } },
  i_AdrmuxRowMemSlowMode_  { {
      { chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("AdrmuxRowMemSlowMode");} },
      { chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("AdrmuxRowMemSlowMode");} },
      { chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("AdrmuxRowMemSlowMode");} },
      { chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("AdrmuxRowMemSlowMode");} },
      { chipIndex,pipeIndex,mauIndex,4,[this](){this->not_implemented("AdrmuxRowMemSlowMode");} },
      { chipIndex,pipeIndex,mauIndex,5,[this](){this->not_implemented("AdrmuxRowMemSlowMode");} },
      { chipIndex,pipeIndex,mauIndex,6,[this](){this->not_implemented("AdrmuxRowMemSlowMode");} },
      { chipIndex,pipeIndex,mauIndex,7,[this](){this->not_implemented("AdrmuxRowMemSlowMode");} } } },
  i_IntrEnable0MauAdrmuxRow_  { {
      { chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("IntrEnable0MauAdrmuxRow");} },
      { chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("IntrEnable0MauAdrmuxRow");} },
      { chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("IntrEnable0MauAdrmuxRow");} },
      { chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("IntrEnable0MauAdrmuxRow");} },
      { chipIndex,pipeIndex,mauIndex,4,[this](){this->not_implemented("IntrEnable0MauAdrmuxRow");} },
      { chipIndex,pipeIndex,mauIndex,5,[this](){this->not_implemented("IntrEnable0MauAdrmuxRow");} },
      { chipIndex,pipeIndex,mauIndex,6,[this](){this->not_implemented("IntrEnable0MauAdrmuxRow");} },
      { chipIndex,pipeIndex,mauIndex,7,[this](){this->not_implemented("IntrEnable0MauAdrmuxRow");} } } },
  i_IntrEnable0MauSynth2port_  { {
      { chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("IntrEnable0MauSynth2port");} },
      { chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("IntrEnable0MauSynth2port");} },
      { chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("IntrEnable0MauSynth2port");} },
      { chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("IntrEnable0MauSynth2port");} },
      { chipIndex,pipeIndex,mauIndex,4,[this](){this->not_implemented("IntrEnable0MauSynth2port");} },
      { chipIndex,pipeIndex,mauIndex,5,[this](){this->not_implemented("IntrEnable0MauSynth2port");} },
      { chipIndex,pipeIndex,mauIndex,6,[this](){this->not_implemented("IntrEnable0MauSynth2port");} },
      { chipIndex,pipeIndex,mauIndex,7,[this](){this->not_implemented("IntrEnable0MauSynth2port");} } } },
  i_IntrEnable1MauAdrmuxRow_  { {
      { chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("IntrEnable1MauAdrmuxRow");} },
      { chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("IntrEnable1MauAdrmuxRow");} },
      { chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("IntrEnable1MauAdrmuxRow");} },
      { chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("IntrEnable1MauAdrmuxRow");} },
      { chipIndex,pipeIndex,mauIndex,4,[this](){this->not_implemented("IntrEnable1MauAdrmuxRow");} },
      { chipIndex,pipeIndex,mauIndex,5,[this](){this->not_implemented("IntrEnable1MauAdrmuxRow");} },
      { chipIndex,pipeIndex,mauIndex,6,[this](){this->not_implemented("IntrEnable1MauAdrmuxRow");} },
      { chipIndex,pipeIndex,mauIndex,7,[this](){this->not_implemented("IntrEnable1MauAdrmuxRow");} } } },
  i_IntrEnable1MauSynth2port_  { {
      { chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("IntrEnable1MauSynth2port");} },
      { chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("IntrEnable1MauSynth2port");} },
      { chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("IntrEnable1MauSynth2port");} },
      { chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("IntrEnable1MauSynth2port");} },
      { chipIndex,pipeIndex,mauIndex,4,[this](){this->not_implemented("IntrEnable1MauSynth2port");} },
      { chipIndex,pipeIndex,mauIndex,5,[this](){this->not_implemented("IntrEnable1MauSynth2port");} },
      { chipIndex,pipeIndex,mauIndex,6,[this](){this->not_implemented("IntrEnable1MauSynth2port");} },
      { chipIndex,pipeIndex,mauIndex,7,[this](){this->not_implemented("IntrEnable1MauSynth2port");} } } },
  i_IntrFreezeEnableMauAdrmuxRow_  { {
      { chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("IntrFreezeEnableMauAdrmuxRow");} },
      { chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("IntrFreezeEnableMauAdrmuxRow");} },
      { chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("IntrFreezeEnableMauAdrmuxRow");} },
      { chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("IntrFreezeEnableMauAdrmuxRow");} },
      { chipIndex,pipeIndex,mauIndex,4,[this](){this->not_implemented("IntrFreezeEnableMauAdrmuxRow");} },
      { chipIndex,pipeIndex,mauIndex,5,[this](){this->not_implemented("IntrFreezeEnableMauAdrmuxRow");} },
      { chipIndex,pipeIndex,mauIndex,6,[this](){this->not_implemented("IntrFreezeEnableMauAdrmuxRow");} },
      { chipIndex,pipeIndex,mauIndex,7,[this](){this->not_implemented("IntrFreezeEnableMauAdrmuxRow");} } } },
  i_IntrInjectMauAdrmuxRow_  { {
      { chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("IntrInjectMauAdrmuxRow");} },
      { chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("IntrInjectMauAdrmuxRow");} },
      { chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("IntrInjectMauAdrmuxRow");} },
      { chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("IntrInjectMauAdrmuxRow");} },
      { chipIndex,pipeIndex,mauIndex,4,[this](){this->not_implemented("IntrInjectMauAdrmuxRow");} },
      { chipIndex,pipeIndex,mauIndex,5,[this](){this->not_implemented("IntrInjectMauAdrmuxRow");} },
      { chipIndex,pipeIndex,mauIndex,6,[this](){this->not_implemented("IntrInjectMauAdrmuxRow");} },
      { chipIndex,pipeIndex,mauIndex,7,[this](){this->not_implemented("IntrInjectMauAdrmuxRow");} } } },
  i_IntrInjectMauSynth2port_  { {
      { chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("IntrInjectMauSynth2port");} },
      { chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("IntrInjectMauSynth2port");} },
      { chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("IntrInjectMauSynth2port");} },
      { chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("IntrInjectMauSynth2port");} },
      { chipIndex,pipeIndex,mauIndex,4,[this](){this->not_implemented("IntrInjectMauSynth2port");} },
      { chipIndex,pipeIndex,mauIndex,5,[this](){this->not_implemented("IntrInjectMauSynth2port");} },
      { chipIndex,pipeIndex,mauIndex,6,[this](){this->not_implemented("IntrInjectMauSynth2port");} },
      { chipIndex,pipeIndex,mauIndex,7,[this](){this->not_implemented("IntrInjectMauSynth2port");} } } },
  i_IntrStatusMauAdrmuxRow_  { {
      { chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("IntrStatusMauAdrmuxRow");} },
      { chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("IntrStatusMauAdrmuxRow");} },
      { chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("IntrStatusMauAdrmuxRow");} },
      { chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("IntrStatusMauAdrmuxRow");} },
      { chipIndex,pipeIndex,mauIndex,4,[this](){this->not_implemented("IntrStatusMauAdrmuxRow");} },
      { chipIndex,pipeIndex,mauIndex,5,[this](){this->not_implemented("IntrStatusMauAdrmuxRow");} },
      { chipIndex,pipeIndex,mauIndex,6,[this](){this->not_implemented("IntrStatusMauAdrmuxRow");} },
      { chipIndex,pipeIndex,mauIndex,7,[this](){this->not_implemented("IntrStatusMauAdrmuxRow");} } } },
  i_IntrStatusMauSynth2port_  { {
      { chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("IntrStatusMauSynth2port");} },
      { chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("IntrStatusMauSynth2port");} },
      { chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("IntrStatusMauSynth2port");} },
      { chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("IntrStatusMauSynth2port");} },
      { chipIndex,pipeIndex,mauIndex,4,[this](){this->not_implemented("IntrStatusMauSynth2port");} },
      { chipIndex,pipeIndex,mauIndex,5,[this](){this->not_implemented("IntrStatusMauSynth2port");} },
      { chipIndex,pipeIndex,mauIndex,6,[this](){this->not_implemented("IntrStatusMauSynth2port");} },
      { chipIndex,pipeIndex,mauIndex,7,[this](){this->not_implemented("IntrStatusMauSynth2port");} } } },
  i_MapramMbeInj_  { {
      { chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("MapramMbeInj");} },
      { chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("MapramMbeInj");} },
      { chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("MapramMbeInj");} },
      { chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("MapramMbeInj");} },
      { chipIndex,pipeIndex,mauIndex,4,[this](){this->not_implemented("MapramMbeInj");} },
      { chipIndex,pipeIndex,mauIndex,5,[this](){this->not_implemented("MapramMbeInj");} },
      { chipIndex,pipeIndex,mauIndex,6,[this](){this->not_implemented("MapramMbeInj");} },
      { chipIndex,pipeIndex,mauIndex,7,[this](){this->not_implemented("MapramMbeInj");} } } },
  i_MapramSbeInj_  { {
      { chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("MapramSbeInj");} },
      { chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("MapramSbeInj");} },
      { chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("MapramSbeInj");} },
      { chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("MapramSbeInj");} },
      { chipIndex,pipeIndex,mauIndex,4,[this](){this->not_implemented("MapramSbeInj");} },
      { chipIndex,pipeIndex,mauIndex,5,[this](){this->not_implemented("MapramSbeInj");} },
      { chipIndex,pipeIndex,mauIndex,6,[this](){this->not_implemented("MapramSbeInj");} },
      { chipIndex,pipeIndex,mauIndex,7,[this](){this->not_implemented("MapramSbeInj");} } } },
  i_MauSynth2portErrlog_  { {
      { chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("MauSynth2portErrlog");} },
      { chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("MauSynth2portErrlog");} },
      { chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("MauSynth2portErrlog");} },
      { chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("MauSynth2portErrlog");} },
      { chipIndex,pipeIndex,mauIndex,4,[this](){this->not_implemented("MauSynth2portErrlog");} },
      { chipIndex,pipeIndex,mauIndex,5,[this](){this->not_implemented("MauSynth2portErrlog");} },
      { chipIndex,pipeIndex,mauIndex,6,[this](){this->not_implemented("MauSynth2portErrlog");} },
      { chipIndex,pipeIndex,mauIndex,7,[this](){this->not_implemented("MauSynth2portErrlog");} } } },
  i_MauSynth2portErrorCtl_  { {
      { chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("MauSynth2portErrorCtl");} },
      { chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("MauSynth2portErrorCtl");} },
      { chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("MauSynth2portErrorCtl");} },
      { chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("MauSynth2portErrorCtl");} },
      { chipIndex,pipeIndex,mauIndex,4,[this](){this->not_implemented("MauSynth2portErrorCtl");} },
      { chipIndex,pipeIndex,mauIndex,5,[this](){this->not_implemented("MauSynth2portErrorCtl");} },
      { chipIndex,pipeIndex,mauIndex,6,[this](){this->not_implemented("MauSynth2portErrorCtl");} },
      { chipIndex,pipeIndex,mauIndex,7,[this](){this->not_implemented("MauSynth2portErrorCtl");} } } },
  i_IntrEnable0MauSelectorAlu_  { {
      { chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("IntrEnable0MauSelectorAlu");} },
      { chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("IntrEnable0MauSelectorAlu");} },
      { chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("IntrEnable0MauSelectorAlu");} },
      { chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("IntrEnable0MauSelectorAlu");} } } },
  i_IntrEnable1MauSelectorAlu_  { {
      { chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("IntrEnable1MauSelectorAlu");} },
      { chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("IntrEnable1MauSelectorAlu");} },
      { chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("IntrEnable1MauSelectorAlu");} },
      { chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("IntrEnable1MauSelectorAlu");} } } },
  i_IntrInjectMauSelectorAlu_  { {
      { chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("IntrInjectMauSelectorAlu");} },
      { chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("IntrInjectMauSelectorAlu");} },
      { chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("IntrInjectMauSelectorAlu");} },
      { chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("IntrInjectMauSelectorAlu");} } } },
  i_IntrStatusMauSelectorAlu_  { {
      { chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("IntrStatusMauSelectorAlu");} },
      { chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("IntrStatusMauSelectorAlu");} },
      { chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("IntrStatusMauSelectorAlu");} },
      { chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("IntrStatusMauSelectorAlu");} } } },
  i_MauDiagMeterAluGroup_  { {
      { chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("MauDiagMeterAluGroup");} },
      { chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("MauDiagMeterAluGroup");} },
      { chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("MauDiagMeterAluGroup");} },
      { chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("MauDiagMeterAluGroup");} } } },
  i_MauSelectorAluErrlog_  { {
      { chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("MauSelectorAluErrlog");} },
      { chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("MauSelectorAluErrlog");} },
      { chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("MauSelectorAluErrlog");} },
      { chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("MauSelectorAluErrlog");} } } },
  i_IntrEnable0MauStatsAlu_  { {
      { chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("IntrEnable0MauStatsAlu");} },
      { chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("IntrEnable0MauStatsAlu");} },
      { chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("IntrEnable0MauStatsAlu");} },
      { chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("IntrEnable0MauStatsAlu");} } } },
  i_IntrEnable1MauStatsAlu_  { {
      { chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("IntrEnable1MauStatsAlu");} },
      { chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("IntrEnable1MauStatsAlu");} },
      { chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("IntrEnable1MauStatsAlu");} },
      { chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("IntrEnable1MauStatsAlu");} } } },
  i_IntrInjectMauStatsAlu_  { {
      { chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("IntrInjectMauStatsAlu");} },
      { chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("IntrInjectMauStatsAlu");} },
      { chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("IntrInjectMauStatsAlu");} },
      { chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("IntrInjectMauStatsAlu");} } } },
  i_IntrStatusMauStatsAlu_  { {
      { chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("IntrStatusMauStatsAlu");} },
      { chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("IntrStatusMauStatsAlu");} },
      { chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("IntrStatusMauStatsAlu");} },
      { chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("IntrStatusMauStatsAlu");} } } },
  i_MauDiagStatsAlu_  { {
      { chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("MauDiagStatsAlu");} },
      { chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("MauDiagStatsAlu");} },
      { chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("MauDiagStatsAlu");} },
      { chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("MauDiagStatsAlu");} } } },
  i_UnitRamEcc_  { {
      { chipIndex,pipeIndex,mauIndex,0,0,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,0,1,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,0,2,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,0,3,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,0,4,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,0,5,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,0,6,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,0,7,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,0,8,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,0,9,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,0,10,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,0,11,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,1,0,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,1,1,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,1,2,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,1,3,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,1,4,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,1,5,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,1,6,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,1,7,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,1,8,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,1,9,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,1,10,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,1,11,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,2,0,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,2,1,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,2,2,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,2,3,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,2,4,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,2,5,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,2,6,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,2,7,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,2,8,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,2,9,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,2,10,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,2,11,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,3,0,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,3,1,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,3,2,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,3,3,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,3,4,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,3,5,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,3,6,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,3,7,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,3,8,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,3,9,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,3,10,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,3,11,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,4,0,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,4,1,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,4,2,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,4,3,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,4,4,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,4,5,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,4,6,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,4,7,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,4,8,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,4,9,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,4,10,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,4,11,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,5,0,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,5,1,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,5,2,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,5,3,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,5,4,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,5,5,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,5,6,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,5,7,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,5,8,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,5,9,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,5,10,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,5,11,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,6,0,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,6,1,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,6,2,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,6,3,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,6,4,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,6,5,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,6,6,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,6,7,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,6,8,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,6,9,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,6,10,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,6,11,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,7,0,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,7,1,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,7,2,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,7,3,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,7,4,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,7,5,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,7,6,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,7,7,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,7,8,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,7,9,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,7,10,[this](){this->not_implemented("UnitRamEcc");} },
      { chipIndex,pipeIndex,mauIndex,7,11,[this](){this->not_implemented("UnitRamEcc");} } } },
  i_UnitRamMbeErrlog_  { {
      { chipIndex,pipeIndex,mauIndex,0,0,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,0,1,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,0,2,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,0,3,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,0,4,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,0,5,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,0,6,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,0,7,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,0,8,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,0,9,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,0,10,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,0,11,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,1,0,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,1,1,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,1,2,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,1,3,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,1,4,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,1,5,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,1,6,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,1,7,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,1,8,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,1,9,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,1,10,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,1,11,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,2,0,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,2,1,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,2,2,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,2,3,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,2,4,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,2,5,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,2,6,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,2,7,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,2,8,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,2,9,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,2,10,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,2,11,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,3,0,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,3,1,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,3,2,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,3,3,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,3,4,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,3,5,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,3,6,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,3,7,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,3,8,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,3,9,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,3,10,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,3,11,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,4,0,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,4,1,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,4,2,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,4,3,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,4,4,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,4,5,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,4,6,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,4,7,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,4,8,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,4,9,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,4,10,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,4,11,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,5,0,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,5,1,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,5,2,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,5,3,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,5,4,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,5,5,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,5,6,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,5,7,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,5,8,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,5,9,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,5,10,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,5,11,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,6,0,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,6,1,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,6,2,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,6,3,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,6,4,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,6,5,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,6,6,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,6,7,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,6,8,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,6,9,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,6,10,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,6,11,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,7,0,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,7,1,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,7,2,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,7,3,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,7,4,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,7,5,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,7,6,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,7,7,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,7,8,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,7,9,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,7,10,[this](){this->not_implemented("UnitRamMbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,7,11,[this](){this->not_implemented("UnitRamMbeErrlog");} } } },
  i_UnitRamSbeErrlog_  { {
      { chipIndex,pipeIndex,mauIndex,0,0,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,0,1,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,0,2,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,0,3,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,0,4,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,0,5,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,0,6,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,0,7,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,0,8,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,0,9,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,0,10,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,0,11,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,1,0,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,1,1,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,1,2,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,1,3,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,1,4,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,1,5,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,1,6,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,1,7,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,1,8,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,1,9,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,1,10,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,1,11,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,2,0,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,2,1,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,2,2,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,2,3,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,2,4,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,2,5,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,2,6,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,2,7,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,2,8,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,2,9,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,2,10,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,2,11,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,3,0,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,3,1,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,3,2,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,3,3,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,3,4,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,3,5,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,3,6,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,3,7,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,3,8,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,3,9,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,3,10,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,3,11,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,4,0,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,4,1,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,4,2,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,4,3,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,4,4,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,4,5,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,4,6,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,4,7,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,4,8,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,4,9,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,4,10,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,4,11,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,5,0,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,5,1,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,5,2,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,5,3,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,5,4,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,5,5,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,5,6,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,5,7,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,5,8,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,5,9,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,5,10,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,5,11,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,6,0,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,6,1,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,6,2,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,6,3,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,6,4,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,6,5,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,6,6,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,6,7,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,6,8,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,6,9,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,6,10,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,6,11,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,7,0,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,7,1,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,7,2,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,7,3,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,7,4,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,7,5,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,7,6,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,7,7,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,7,8,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,7,9,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,7,10,[this](){this->not_implemented("UnitRamSbeErrlog");} },
      { chipIndex,pipeIndex,mauIndex,7,11,[this](){this->not_implemented("UnitRamSbeErrlog");} } } },
  i_ActiondataErrorUramCtlArray_  { {
      { chipIndex,pipeIndex,mauIndex,0,[this](uint32_t i){this->not_implemented("ActiondataErrorUramCtlArray");} },
      { chipIndex,pipeIndex,mauIndex,1,[this](uint32_t i){this->not_implemented("ActiondataErrorUramCtlArray");} },
      { chipIndex,pipeIndex,mauIndex,2,[this](uint32_t i){this->not_implemented("ActiondataErrorUramCtlArray");} },
      { chipIndex,pipeIndex,mauIndex,3,[this](uint32_t i){this->not_implemented("ActiondataErrorUramCtlArray");} },
      { chipIndex,pipeIndex,mauIndex,4,[this](uint32_t i){this->not_implemented("ActiondataErrorUramCtlArray");} },
      { chipIndex,pipeIndex,mauIndex,5,[this](uint32_t i){this->not_implemented("ActiondataErrorUramCtlArray");} },
      { chipIndex,pipeIndex,mauIndex,6,[this](uint32_t i){this->not_implemented("ActiondataErrorUramCtlArray");} },
      { chipIndex,pipeIndex,mauIndex,7,[this](uint32_t i){this->not_implemented("ActiondataErrorUramCtlArray");} } } },
  i_EmmEccErrorUramCtlArray_  { {
      { chipIndex,pipeIndex,mauIndex,0,[this](uint32_t i){this->not_implemented("EmmEccErrorUramCtlArray");} },
      { chipIndex,pipeIndex,mauIndex,1,[this](uint32_t i){this->not_implemented("EmmEccErrorUramCtlArray");} },
      { chipIndex,pipeIndex,mauIndex,2,[this](uint32_t i){this->not_implemented("EmmEccErrorUramCtlArray");} },
      { chipIndex,pipeIndex,mauIndex,3,[this](uint32_t i){this->not_implemented("EmmEccErrorUramCtlArray");} },
      { chipIndex,pipeIndex,mauIndex,4,[this](uint32_t i){this->not_implemented("EmmEccErrorUramCtlArray");} },
      { chipIndex,pipeIndex,mauIndex,5,[this](uint32_t i){this->not_implemented("EmmEccErrorUramCtlArray");} },
      { chipIndex,pipeIndex,mauIndex,6,[this](uint32_t i){this->not_implemented("EmmEccErrorUramCtlArray");} },
      { chipIndex,pipeIndex,mauIndex,7,[this](uint32_t i){this->not_implemented("EmmEccErrorUramCtlArray");} } } },
  i_TindEccErrorUramCtlArray_  { {
      { chipIndex,pipeIndex,mauIndex,0,[this](uint32_t i){this->not_implemented("TindEccErrorUramCtlArray");} },
      { chipIndex,pipeIndex,mauIndex,1,[this](uint32_t i){this->not_implemented("TindEccErrorUramCtlArray");} },
      { chipIndex,pipeIndex,mauIndex,2,[this](uint32_t i){this->not_implemented("TindEccErrorUramCtlArray");} },
      { chipIndex,pipeIndex,mauIndex,3,[this](uint32_t i){this->not_implemented("TindEccErrorUramCtlArray");} },
      { chipIndex,pipeIndex,mauIndex,4,[this](uint32_t i){this->not_implemented("TindEccErrorUramCtlArray");} },
      { chipIndex,pipeIndex,mauIndex,5,[this](uint32_t i){this->not_implemented("TindEccErrorUramCtlArray");} },
      { chipIndex,pipeIndex,mauIndex,6,[this](uint32_t i){this->not_implemented("TindEccErrorUramCtlArray");} },
      { chipIndex,pipeIndex,mauIndex,7,[this](uint32_t i){this->not_implemented("TindEccErrorUramCtlArray");} } } },
  i_IntrEnable0MauUnitRamRow_  { {
      { chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("IntrEnable0MauUnitRamRow");} },
      { chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("IntrEnable0MauUnitRamRow");} },
      { chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("IntrEnable0MauUnitRamRow");} },
      { chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("IntrEnable0MauUnitRamRow");} },
      { chipIndex,pipeIndex,mauIndex,4,[this](){this->not_implemented("IntrEnable0MauUnitRamRow");} },
      { chipIndex,pipeIndex,mauIndex,5,[this](){this->not_implemented("IntrEnable0MauUnitRamRow");} },
      { chipIndex,pipeIndex,mauIndex,6,[this](){this->not_implemented("IntrEnable0MauUnitRamRow");} },
      { chipIndex,pipeIndex,mauIndex,7,[this](){this->not_implemented("IntrEnable0MauUnitRamRow");} } } },
  i_IntrEnable1MauUnitRamRow_  { {
      { chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("IntrEnable1MauUnitRamRow");} },
      { chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("IntrEnable1MauUnitRamRow");} },
      { chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("IntrEnable1MauUnitRamRow");} },
      { chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("IntrEnable1MauUnitRamRow");} },
      { chipIndex,pipeIndex,mauIndex,4,[this](){this->not_implemented("IntrEnable1MauUnitRamRow");} },
      { chipIndex,pipeIndex,mauIndex,5,[this](){this->not_implemented("IntrEnable1MauUnitRamRow");} },
      { chipIndex,pipeIndex,mauIndex,6,[this](){this->not_implemented("IntrEnable1MauUnitRamRow");} },
      { chipIndex,pipeIndex,mauIndex,7,[this](){this->not_implemented("IntrEnable1MauUnitRamRow");} } } },
  i_IntrFreezeEnableMauUnitRamRow_  { {
      { chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("IntrFreezeEnableMauUnitRamRow");} },
      { chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("IntrFreezeEnableMauUnitRamRow");} },
      { chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("IntrFreezeEnableMauUnitRamRow");} },
      { chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("IntrFreezeEnableMauUnitRamRow");} },
      { chipIndex,pipeIndex,mauIndex,4,[this](){this->not_implemented("IntrFreezeEnableMauUnitRamRow");} },
      { chipIndex,pipeIndex,mauIndex,5,[this](){this->not_implemented("IntrFreezeEnableMauUnitRamRow");} },
      { chipIndex,pipeIndex,mauIndex,6,[this](){this->not_implemented("IntrFreezeEnableMauUnitRamRow");} },
      { chipIndex,pipeIndex,mauIndex,7,[this](){this->not_implemented("IntrFreezeEnableMauUnitRamRow");} } } },
  i_IntrInjectMauUnitRamRow_  { {
      { chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("IntrInjectMauUnitRamRow");} },
      { chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("IntrInjectMauUnitRamRow");} },
      { chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("IntrInjectMauUnitRamRow");} },
      { chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("IntrInjectMauUnitRamRow");} },
      { chipIndex,pipeIndex,mauIndex,4,[this](){this->not_implemented("IntrInjectMauUnitRamRow");} },
      { chipIndex,pipeIndex,mauIndex,5,[this](){this->not_implemented("IntrInjectMauUnitRamRow");} },
      { chipIndex,pipeIndex,mauIndex,6,[this](){this->not_implemented("IntrInjectMauUnitRamRow");} },
      { chipIndex,pipeIndex,mauIndex,7,[this](){this->not_implemented("IntrInjectMauUnitRamRow");} } } },
  i_IntrStatusMauUnitRamRow_  { {
      { chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("IntrStatusMauUnitRamRow");} },
      { chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("IntrStatusMauUnitRamRow");} },
      { chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("IntrStatusMauUnitRamRow");} },
      { chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("IntrStatusMauUnitRamRow");} },
      { chipIndex,pipeIndex,mauIndex,4,[this](){this->not_implemented("IntrStatusMauUnitRamRow");} },
      { chipIndex,pipeIndex,mauIndex,5,[this](){this->not_implemented("IntrStatusMauUnitRamRow");} },
      { chipIndex,pipeIndex,mauIndex,6,[this](){this->not_implemented("IntrStatusMauUnitRamRow");} },
      { chipIndex,pipeIndex,mauIndex,7,[this](){this->not_implemented("IntrStatusMauUnitRamRow");} } } },
  i_MauDiagRowAdbClkEnable_  { {
      { chipIndex,pipeIndex,mauIndex,0,[this](){this->not_implemented("MauDiagRowAdbClkEnable");} },
      { chipIndex,pipeIndex,mauIndex,1,[this](){this->not_implemented("MauDiagRowAdbClkEnable");} },
      { chipIndex,pipeIndex,mauIndex,2,[this](){this->not_implemented("MauDiagRowAdbClkEnable");} },
      { chipIndex,pipeIndex,mauIndex,3,[this](){this->not_implemented("MauDiagRowAdbClkEnable");} },
      { chipIndex,pipeIndex,mauIndex,4,[this](){this->not_implemented("MauDiagRowAdbClkEnable");} },
      { chipIndex,pipeIndex,mauIndex,5,[this](){this->not_implemented("MauDiagRowAdbClkEnable");} },
      { chipIndex,pipeIndex,mauIndex,6,[this](){this->not_implemented("MauDiagRowAdbClkEnable");} },
      { chipIndex,pipeIndex,mauIndex,7,[this](){this->not_implemented("MauDiagRowAdbClkEnable");} } } },
  i_BubbleReqCtlArray_(chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("BubbleReqCtlArray");}),
  i_DeferredMeterParityControlArray_(chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("DeferredMeterParityControlArray");}),
  i_DeferredStatsParityControlArray_(chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("DeferredStatsParityControlArray");}),
  i_DeferredStatsParityErrlogArray_(chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("DeferredStatsParityErrlogArray");}),
  i_DefMeterSbeErrlogArray_(chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("DefMeterSbeErrlogArray");}),
  i_EmmEccErrorCtlArray_(chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("EmmEccErrorCtlArray");}),
  i_ErrIdataOvrCtlArray_(chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("ErrIdataOvrCtlArray");}),
  i_ErrIdataOvrFifoCtlArray_(chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("ErrIdataOvrFifoCtlArray");}),
  i_ExitGatewayCtlArray_(chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("ExitGatewayCtlArray");}),
  i_GfmParityErrorCtlArray_(chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("GfmParityErrorCtlArray");}),
  i_IdleBubbleReqArray_(chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("IdleBubbleReqArray");}),
  i_IdletimeSlipArray_(chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("IdletimeSlipArray");}),
  i_IdletimeSlipIntrCtlArray_(chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("IdletimeSlipIntrCtlArray");}),
  i_IntrMauDecodeMemoryCoreArray_(chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("IntrMauDecodeMemoryCoreArray");}),
  i_MauCfgMramThreadArray_(chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("MauCfgMramThreadArray");}),
  i_MauCfgSreqTimeoutArray_(chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("MauCfgSreqTimeoutArray");}),
  i_MauCfgUramThreadArray_(chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("MauCfgUramThreadArray");}),
  i_MauDiagCfgOxbarCtlArray_(chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("MauDiagCfgOxbarCtlArray");}),
  i_MauMatchInputXbarExactMatchEnableArray_(chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("MauMatchInputXbarExactMatchEnableArray");}),
  i_MauMatchInputXbarTernaryMatchEnableArray_(chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("MauMatchInputXbarTernaryMatchEnableArray");}),
  i_MauSnapshotCaptureDatapathErrorArray_(chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("MauSnapshotCaptureDatapathErrorArray");}),
  i_MeterAluGroupErrorCtlArray_(chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("MeterAluGroupErrorCtlArray");}),
  i_MeterBubbleReqArray_(chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("MeterBubbleReqArray");}),
  i_OErrorFifoCtlArray_(chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("OErrorFifoCtlArray");}),
  i_PbsCreqErrlogArray_(chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("PbsCreqErrlogArray");}),
  i_PbsCrespErrlogArray_(chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("PbsCrespErrlogArray");}),
  i_PbsSreqErrlogArray_(chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("PbsSreqErrlogArray");}),
  i_PrevErrorCtlArray_(chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("PrevErrorCtlArray");}),
  i_S2pMeterErrorCtlArray_(chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("S2pMeterErrorCtlArray");}),
  i_S2pStatsErrorCtlArray_(chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("S2pStatsErrorCtlArray");}),
  i_StatsBubbleReqArray_(chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("StatsBubbleReqArray");}),
  i_StatsLrtFsmSweepOffsetArray_(chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("StatsLrtFsmSweepOffsetArray");}),
  i_StatsLrtSweepAdrArray_(chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("StatsLrtSweepAdrArray");}),
  i_TcamLogicalChannelErrlogHiArray_(chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("TcamLogicalChannelErrlogHiArray");}),
  i_TcamLogicalChannelErrlogLoArray_(chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("TcamLogicalChannelErrlogLoArray");}),
  i_TcamMatchErrorCtlArray_(chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("TcamMatchErrorCtlArray");}),
  i_TcamOutputTableThreadArray_(chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("TcamOutputTableThreadArray");}),
  i_TcamParityControlArray_(chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("TcamParityControlArray");}),
  i_TcamSbeErrlogArray_(chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("TcamSbeErrlogArray");}),
  i_TcamTablePropArray_(chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("TcamTablePropArray");}),
  i_TindEccErrorCtlArray_(chipIndex,pipeIndex,mauIndex,[this](uint32_t i){this->not_implemented("TindEccErrorCtlArray");}),
  i_ActiondataErrorCtl_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("ActiondataErrorCtl");}),
  i_AdrDistMemSlowMode_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("AdrDistMemSlowMode");}),
  i_HashoutCtl_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("HashoutCtl");}),
  i_IdletimeSlipErrlog_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("IdletimeSlipErrlog");}),
  i_ImemParityErrorCtl_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("ImemParityErrorCtl");}),
  i_ImemSbeErrlog_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("ImemSbeErrlog");}),
  i_IntrDecodeTop_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("IntrDecodeTop");}),
  i_IntrEnable0MauAd_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("IntrEnable0MauAd");}),
  i_IntrEnable0MauCfg_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("IntrEnable0MauCfg");}),
  i_IntrEnable0MauGfmHash_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("IntrEnable0MauGfmHash");}),
  i_IntrEnable0MauImem_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("IntrEnable0MauImem");}),
  i_IntrEnable0MauSnapshot_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("IntrEnable0MauSnapshot");}),
  i_IntrEnable1MauAd_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("IntrEnable1MauAd");}),
  i_IntrEnable1MauCfg_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("IntrEnable1MauCfg");}),
  i_IntrEnable1MauGfmHash_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("IntrEnable1MauGfmHash");}),
  i_IntrEnable1MauImem_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("IntrEnable1MauImem");}),
  i_IntrEnable1MauSnapshot_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("IntrEnable1MauSnapshot");}),
  i_IntrFreezeEnableMauAd_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("IntrFreezeEnableMauAd");}),
  i_IntrFreezeEnableMauCfg_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("IntrFreezeEnableMauCfg");}),
  i_IntrFreezeEnableMauGfmHash_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("IntrFreezeEnableMauGfmHash");}),
  i_IntrFreezeEnableMauImem_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("IntrFreezeEnableMauImem");}),
  i_IntrFreezeEnableMauSnapshot_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("IntrFreezeEnableMauSnapshot");}),
  i_IntrInjectMauAd_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("IntrInjectMauAd");}),
  i_IntrInjectMauCfg_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("IntrInjectMauCfg");}),
  i_IntrInjectMauGfmHash_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("IntrInjectMauGfmHash");}),
  i_IntrInjectMauImem_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("IntrInjectMauImem");}),
  i_IntrInjectMauSnapshot_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("IntrInjectMauSnapshot");}),
  i_IntrStatusMauCfg_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("IntrStatusMauCfg");}),
  i_IntrStatusMauGfmHash_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("IntrStatusMauGfmHash");}),
  i_IntrStatusMauImem_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("IntrStatusMauImem");}),
  i_MauCfgDramThread_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("MauCfgDramThread");}),
  i_MauCfgImemBubbleReq_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("MauCfgImemBubbleReq");}),
  i_MauCfgLtThread_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("MauCfgLtThread");}),
  i_MauCfgMemSlowMode_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("MauCfgMemSlowMode");}),
  i_MauDiag_32bOxbarCtl_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("MauDiag_32bOxbarCtl");}),
  i_MauDiag_32bOxbarPremuxCtl_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("MauDiag_32bOxbarPremuxCtl");}),
  i_MauDiag_8bOxbarCtl_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("MauDiag_8bOxbarCtl");}),
  i_MauDiagAdbCtl_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("MauDiagAdbCtl");}),
  i_MauDiagAdbMap_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("MauDiagAdbMap");}),
  i_MauDiagAdrDistIdletimeAdrOxbarCtl_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("MauDiagAdrDistIdletimeAdrOxbarCtl");}),
  i_MauDiagCfgCtl_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("MauDiagCfgCtl");}),
  i_MauDiagEopVldXport_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("MauDiagEopVldXport");}),
  i_MauDiagMeterAdrSel_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("MauDiagMeterAdrSel");}),
  i_MauDiagPbusEnable_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("MauDiagPbusEnable");}),
  i_MauDiagStatsAdrSel_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("MauDiagStatsAdrSel");}),
  i_MauDiagTcamClkEn_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("MauDiagTcamClkEn");}),
  i_MauDiagTcamHitXbarCtl_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("MauDiagTcamHitXbarCtl");}),
  i_MauDiagValidCtl_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("MauDiagValidCtl");}),
  i_MeterSweepErrlog_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("MeterSweepErrlog");}),
  i_PbsCreqEcc_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("PbsCreqEcc");}),
  i_PbsCrespEcc_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("PbsCrespEcc");}),
  i_PbsSreqEcc_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("PbsSreqEcc");}),
  i_QHoleAccErrlogHi_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("QHoleAccErrlogHi");}),
  i_QHoleAccErrlogLo_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("QHoleAccErrlogLo");}),
  i_SreqIdleTimeoutErrlog_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("SreqIdleTimeoutErrlog");}),
  i_SreqStatsTimeoutErrlog_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("SreqStatsTimeoutErrlog");}),
  i_TcamPiped_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("TcamPiped");}),
  i_TcamScrubCtl_(chipIndex,pipeIndex,mauIndex,[this](){this->not_implemented("TcamScrubCtl");})
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

  i_ExitGatewayCtlArray_.reset();
  i_MauCfgSreqTimeoutArray_.reset();
  i_MauDiagCfgOxbarCtlArray_.reset();
  i_SreqIdleTimeoutErrlog_.reset();
  i_SreqStatsTimeoutErrlog_.reset();
  i_BubbleReqCtlArray_.reset();
  i_DeferredMeterParityControlArray_.reset();
  i_DeferredStatsParityControlArray_.reset();
  i_DeferredStatsParityErrlogArray_.reset();
  i_DefMeterSbeErrlogArray_.reset();
  i_EmmEccErrorCtlArray_.reset();
  i_ErrIdataOvrCtlArray_.reset();
  i_ErrIdataOvrFifoCtlArray_.reset();
  i_IdleBubbleReqArray_.reset();
  i_IdletimeSlipArray_.reset();
  i_IdletimeSlipIntrCtlArray_.reset();
  i_IntrMauDecodeMemoryCoreArray_.reset();
  i_MauCfgMramThreadArray_.reset();
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
  i_TcamScrubCtl_.reset();
}
}
