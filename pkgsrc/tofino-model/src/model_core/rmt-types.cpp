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

#include <model_core/rmt-types.h>

#include <algorithm>
#include <string>
#include <cctype>

namespace model_core {

  constexpr std::array<const int,195> RmtTypes::PrintDepth;
  constexpr std::array<const RmtTypes::kRmtTypeEntry, 195> RmtTypes::kRmtTypeArray;

  const char* RmtTypes::toString(const int rmt_type) {
    if (rmt_type < 0 || rmt_type > kRmtTypeArray.size() - 1)
      return "undef";
    return kRmtTypeArray[rmt_type].name;
  }

  int RmtTypes::toInt(const char* find) {
    auto s = std::string{find};
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);

    auto found = kRmtTypeMap.find(s);

    if (found == kRmtTypeMap.end())
      return 0;
    return found->second;
  }
  // to fix const initalizer
  const std::unordered_map<std::string, int> RmtTypes::kRmtTypeMap{
    {"undef",                           kRmtTypeUndefined},
    {"checksumengine",                  kRmtTypeChecksumEngine},
    {"deparser",                        kRmtTypeDeparser},
    {"deparserblock",                   kRmtTypeDeparserBlock},
    {"epb",                             kRmtTypeEpb},
    {"hashaddressvhxbar",               kRmtTypeHashAddressVhXbar},
    {"model",                           kRmtTypeModel},
    {"inputcontrolledxbar",             kRmtTypeInputControlledXbar},
    {"instr",                           kRmtTypeInstr},
    {"matchdatainputvhxbar",            kRmtTypeMatchDataInputVhXbar},
    {"mau",                             kRmtTypeMau},
    {"mauaddrdist",                     kRmtTypeMauAddrDist},
    {"maudependencies",                 kRmtTypeMauDependencies},
    {"maugatewaypayload",               kRmtTypeMauGatewayPayload},
    {"maugatewaypayloadreg",            kRmtTypeMauGatewayPayloadReg},
    {"maugatewaytable",                 kRmtTypeMauGatewayTable},
    {"maugatewaytablereg",              kRmtTypeMauGatewayTableReg},
    {"mauhashgenerator",                kRmtTypeMauHashGenerator},
    {"mauhashgeneratorwithreg",         kRmtTypeMauHashGeneratorWithReg},
    {"mauinput",                        kRmtTypeMauInput},
    {"mauinputxbar",                    kRmtTypeMauInputXbar},
    {"mauinstrstore",                   kRmtTypeMauInstrStore},
    {"maulogicalrow",                   kRmtTypeMauLogicalRow},
    {"maulogicalrowreg",                kRmtTypeMauLogicalRowReg},
    {"maulogicaltable",                 kRmtTypeMauLogicalTable},
    {"maulogicaltablereg",              kRmtTypeMauLogicalTableReg},
    {"maulogicaltcam",                  kRmtTypeMauLogicalTcam},
    {"maulogicaltcamreg",               kRmtTypeMauLogicalTcamReg},
    {"maulookupresult",                 kRmtTypeMauLookupResult},
    {"maumapram",                       kRmtTypeMauMapram},
    {"maumemory",                       kRmtTypeMauMemory},
    {"mauresultbus",                    kRmtTypeMauResultBus},
    {"mausram",                         kRmtTypeMauSram},
    {"mausramcolumn",                   kRmtTypeMauSramColumn},
    {"mausramcolumnreg",                kRmtTypeMauSramColumnReg},
    {"mausramreg",                      kRmtTypeMauSramReg},
    {"mausramrow",                      kRmtTypeMauSramRow},
    {"maustash",                        kRmtTypeMauStash},
    {"maustashcolumn",                  kRmtTypeMauStashColumn},
    {"maustatsalu",                     kRmtTypeMauStatsAlu},
    {"mautcam",                         kRmtTypeMauTcam},
    {"mautcamrow",                      kRmtTypeMauTcamRow},
    {"packet",                          kRmtTypePacket},
    {"parser",                          kRmtTypeParser},
    {"parserblock",                     kRmtTypeParserBlock},
    {"phv",                             kRmtTypePhv},
    {"phvfactory",                      kRmtTypePhvFactory},
    {"pipe",                            kRmtTypePipe},
    {"port",                            kRmtTypePort},
    {"queueing",                        kRmtTypeQueueing},
    {"registerblock",                   kRmtTypeRegisterBlock},
    {"registerblockindirect",           kRmtTypeRegisterBlockIndirect},
    {"rmtobjectmanager",                kRmtTypeRmtObjectManager},
    {"rmtophandler",                    kRmtTypeRmtOpHandler},
    {"sram",                            kRmtTypeSram},
    {"tcam3",                           kRmtTypeTcam3},
    {"maumeteralu",                     kRmtTypeMauMeterAlu},
    {"rmtsweeper",                      kRmtTypeRmtSweeper},
    {"maumeter",                        kRmtTypeMauMeter},
    {"maulpfmeter",                     kRmtTypeMauLpfMeter},
    {"statefulalu",                     kRmtTypeStatefulAlu},
    {"maumapramreg",                    kRmtTypeMauMapramReg},
    {"mausramrowreg",                   kRmtTypeMauSramRowReg},
    {"reserved1",                       kRmtTypeReserved1},
    {"reserved2",                       kRmtTypeReserved2},
    {"mautcamreg",                      kRmtTypeMauTcamReg},
    {"selectoralu",                     kRmtTypeSelectorAlu},
    {"maumoveregs",                     kRmtTypeMauMoveregs},
    {"mautablecounters",                kRmtTypeMauTableCounters},
    {"mausnapshot",                     kRmtTypeMauSnapshot},
    {"bitvector",                       kRmtTypeBitVector},
    {"rmtpacketcoordinator",            kRmtTypeRmtPacketCoordinator},
    {"indirectaccessblock",             kRmtTypeIndirectAccessBlock},
    {"maulogicaltcamcol",               kRmtTypeMauLogicalTcamCol},
    {"mauophandler",                    kRmtTypeMauOpHandler},
    {"packetreplicationengine",         kRmtTypePacketReplicationEngine},
    {"packetreplicationenginereg",      kRmtTypePacketReplicationEngineReg},
    {"actionoutputhvxbar",              kRmtTypeActionOutputHvXbar},
    {"hashaddressvhxbarwithreg",        kRmtTypeHashAddressVhXbarWithReg},
    {"matchdatainputvhxbarwithreg",     kRmtTypeMatchDataInputVhXbarWithReg},
    {"mauinputxbarwithreg",             kRmtTypeMauInputXbarWithReg},
    {"mauobject",                       kRmtTypeMauObject},
    {"maunotused",                      kRmtTypeMauNotUsed},
    {"packetbuffer",                    kRmtTypePacketBuffer},
    {"pipeobject",                      kRmtTypePipeObject},
    {"rmtdefs",                         kRmtTypeRmtDefs},
    {"rmtobject",                       kRmtTypeRmtObject},
    {"rmttypes",                        kRmtTypeRmtTypes},
    {"tcamrowvhwithreg",                kRmtTypeTcamRowVhWithReg},
    {"cacheid",                         kRmtTypeCacheId},
    {"pktgen",                          kRmtTypePktGen},
    {"mauio",                           kRmtTypeMauIO},
    {"maucolorswitchbox",               kRmtTypeMauColorSwitchbox},
    {"rmtstringmap",                    kRmtTypeRmtStringMap},
    {"chip",                            kRmtTypeChip},
    {"ipb",                             kRmtTypeIpb},
    {"maupredication",                  kRmtTypeMauPredication},
    {"mauteop",                         kRmtTypeMauTeop},
    {"tm",                              kRmtTypeTm},
    {"reserved9",                       kRmtTypeReserved9},
    {"unittests",                       kRmtTypeMimicUnittests},
    {"framework",                       kRmtTypeMimicFramework},
    {"emerge",                          kRmtTypeMimicEmerge},
    {"extractor",                       kRmtTypeMimicExtractor},
    {"packer",                          kRmtTypeMimicPacker},
    {"pparser",                         kRmtTypeMimicPparser},
    {"ppuactionbus",                    kRmtTypeMimicPpuActionbus},
    {"ppuactionphv",                    kRmtTypeMimicPpuActionphv},
    {"ppuactionram",                    kRmtTypeMimicPpuActionram},
    {"ppubasicaction",                  kRmtTypeMimicPpuBasicaction},
    {"ppuealu",                         kRmtTypeMimicPpuEalu},
    {"ppuexactmatch",                   kRmtTypeMimicPpuExactmatch},
    {"ppuidletimeaction",               kRmtTypeMimicPpuIdletimeaction},
    {"ppumatchinput",                   kRmtTypeMimicPpuMatchinput},
    {"ppumatchresdist",                 kRmtTypeMimicPpuMatchresdist},
    {"ppumeteraction",                  kRmtTypeMimicPpuMeteraction},
    {"ppuoperandfifo",                  kRmtTypeMimicPpuOperandfifo},
    {"ppuphvwrite",                     kRmtTypeMimicPpuPhvwrite},
    {"ppusparsetrie",                   kRmtTypeMimicPpuSparsetrie},
    {"ppustatefulaction",               kRmtTypeMimicPpuStatefulaction},
    {"ppustatsaction",                  kRmtTypeMimicPpuStatsaction},
    {"pputernaryind",                   kRmtTypeMimicPpuTernaryind},
    {"ppuxcmpaction",                   kRmtTypeMimicPpuXcmpaction},
    {"scm",                             kRmtTypeMimicScm},
    {"stm",                             kRmtTypeMimicStm},
    {"tcams",                           kRmtTypeMimicTcams},
    {"phvaction",                       kRmtTypeMimicPhvAction},
    {"delaychecks",                     kRmtTypeMimicDelayChecks},
    {"legacycode",                      kRmtTypeMimicLegacyCode},
    {"emac",                            kRmtTypeMimicEmac},
    {"etm",                             kRmtTypeMimicEtm},
    {"imac",                            kRmtTypeMimicImac},
    {"itm",                             kRmtTypeMimicItm},
  };

  // to fix linker errors
  constexpr int RmtTypes::kRmtTypeUndefined;
  constexpr int RmtTypes::kRmtTypeChecksumEngine;
  constexpr int RmtTypes::kRmtTypeDeparser;
  constexpr int RmtTypes::kRmtTypeDeparserBlock;
  constexpr int RmtTypes::kRmtTypeEpb;
  constexpr int RmtTypes::kRmtTypeHashAddressVhXbar;
  constexpr int RmtTypes::kRmtTypeModel;
  constexpr int RmtTypes::kRmtTypeInputControlledXbar;
  constexpr int RmtTypes::kRmtTypeInstr;
  constexpr int RmtTypes::kRmtTypeMatchDataInputVhXbar;
  constexpr int RmtTypes::kRmtTypeMau;
  constexpr int RmtTypes::kRmtTypeMauAddrDist;
  constexpr int RmtTypes::kRmtTypeMauDependencies;
  constexpr int RmtTypes::kRmtTypeMauGatewayPayload;
  constexpr int RmtTypes::kRmtTypeMauGatewayPayloadReg;
  constexpr int RmtTypes::kRmtTypeMauGatewayTable;
  constexpr int RmtTypes::kRmtTypeMauGatewayTableReg;
  constexpr int RmtTypes::kRmtTypeMauHashGenerator;
  constexpr int RmtTypes::kRmtTypeMauHashGeneratorWithReg;
  constexpr int RmtTypes::kRmtTypeMauInput;
  constexpr int RmtTypes::kRmtTypeMauInputXbar;
  constexpr int RmtTypes::kRmtTypeMauInstrStore;
  constexpr int RmtTypes::kRmtTypeMauLogicalRow;
  constexpr int RmtTypes::kRmtTypeMauLogicalRowReg;
  constexpr int RmtTypes::kRmtTypeMauLogicalTable;
  constexpr int RmtTypes::kRmtTypeMauLogicalTableReg;
  constexpr int RmtTypes::kRmtTypeMauLogicalTcam;
  constexpr int RmtTypes::kRmtTypeMauLogicalTcamReg;
  constexpr int RmtTypes::kRmtTypeMauLookupResult;
  constexpr int RmtTypes::kRmtTypeMauMapram;
  constexpr int RmtTypes::kRmtTypeMauMemory;
  constexpr int RmtTypes::kRmtTypeMauResultBus;
  constexpr int RmtTypes::kRmtTypeMauSram;
  constexpr int RmtTypes::kRmtTypeMauSramColumn;
  constexpr int RmtTypes::kRmtTypeMauSramColumnReg;
  constexpr int RmtTypes::kRmtTypeMauSramReg;
  constexpr int RmtTypes::kRmtTypeMauSramRow;
  constexpr int RmtTypes::kRmtTypeMauStash;
  constexpr int RmtTypes::kRmtTypeMauStashColumn;
  constexpr int RmtTypes::kRmtTypeMauStatsAlu;
  constexpr int RmtTypes::kRmtTypeMauTcam;
  constexpr int RmtTypes::kRmtTypeMauTcamRow;
  constexpr int RmtTypes::kRmtTypePacket;
  constexpr int RmtTypes::kRmtTypeParser;
  constexpr int RmtTypes::kRmtTypeParserBlock;
  constexpr int RmtTypes::kRmtTypePhv;
  constexpr int RmtTypes::kRmtTypePhvFactory;
  constexpr int RmtTypes::kRmtTypePipe;
  constexpr int RmtTypes::kRmtTypePort;
  constexpr int RmtTypes::kRmtTypeQueueing;
  constexpr int RmtTypes::kRmtTypeRegisterBlock;
  constexpr int RmtTypes::kRmtTypeRegisterBlockIndirect;
  constexpr int RmtTypes::kRmtTypeRmtObjectManager;
  constexpr int RmtTypes::kRmtTypeRmtOpHandler;
  constexpr int RmtTypes::kRmtTypeSram;
  constexpr int RmtTypes::kRmtTypeTcam3;
  constexpr int RmtTypes::kRmtTypeMauMeterAlu;
  constexpr int RmtTypes::kRmtTypeRmtSweeper;
  constexpr int RmtTypes::kRmtTypeMauMeter;
  constexpr int RmtTypes::kRmtTypeMauLpfMeter;
  constexpr int RmtTypes::kRmtTypeStatefulAlu;
  constexpr int RmtTypes::kRmtTypeMauMapramReg;
  constexpr int RmtTypes::kRmtTypeMauSramRowReg;
  constexpr int RmtTypes::kRmtTypeReserved1;
  constexpr int RmtTypes::kRmtTypeReserved2;
  constexpr int RmtTypes::kRmtTypeMauTcamReg;
  constexpr int RmtTypes::kRmtTypeSelectorAlu;
  constexpr int RmtTypes::kRmtTypeMauMoveregs;
  constexpr int RmtTypes::kRmtTypeMauTableCounters;
  constexpr int RmtTypes::kRmtTypeMauSnapshot;
  constexpr int RmtTypes::kRmtTypeBitVector;
  constexpr int RmtTypes::kRmtTypeRmtPacketCoordinator;
  constexpr int RmtTypes::kRmtTypeIndirectAccessBlock;
  constexpr int RmtTypes::kRmtTypeMauLogicalTcamCol;
  constexpr int RmtTypes::kRmtTypeMauOpHandler;
  constexpr int RmtTypes::kRmtTypePacketReplicationEngine;
  constexpr int RmtTypes::kRmtTypePacketReplicationEngineReg;
  constexpr int RmtTypes::kRmtTypeActionOutputHvXbar;
  constexpr int RmtTypes::kRmtTypeDeparserReg;
  constexpr int RmtTypes::kRmtTypeHashAddressVhXbarWithReg;
  constexpr int RmtTypes::kRmtTypeMatchDataInputVhXbarWithReg;
  constexpr int RmtTypes::kRmtTypeMauInputXbarWithReg;
  constexpr int RmtTypes::kRmtTypeMauObject;
  constexpr int RmtTypes::kRmtTypeMauNotUsed;
  constexpr int RmtTypes::kRmtTypePacketBuffer;
  constexpr int RmtTypes::kRmtTypePipeObject;
  constexpr int RmtTypes::kRmtTypeRmtDefs;
  constexpr int RmtTypes::kRmtTypeRmtObject;
  constexpr int RmtTypes::kRmtTypeRmtTypes;
  constexpr int RmtTypes::kRmtTypeTcamRowVhWithReg;
  constexpr int RmtTypes::kRmtTypeCacheId;
  constexpr int RmtTypes::kRmtTypePktGen;
  constexpr int RmtTypes::kRmtTypeMauIO;
  constexpr int RmtTypes::kRmtTypeMauColorSwitchbox;
  constexpr int RmtTypes::kRmtTypeRmtStringMap;
  constexpr int RmtTypes::kRmtTypeChip;
  constexpr int RmtTypes::kRmtTypeIpb;
  constexpr int RmtTypes::kRmtTypeMauPredication;
  constexpr int RmtTypes::kRmtTypeMauTeop;
  constexpr int RmtTypes::kRmtTypeTm;
  constexpr int RmtTypes::kRmtTypeReserved9;
  constexpr int RmtTypes::kRmtTypeMimicUnittests;
  constexpr int RmtTypes::kRmtTypeMimicFramework;
  constexpr int RmtTypes::kRmtTypeMimicEmerge;
  constexpr int RmtTypes::kRmtTypeMimicExtractor;
  constexpr int RmtTypes::kRmtTypeMimicPacker;
  constexpr int RmtTypes::kRmtTypeMimicPparser;
  constexpr int RmtTypes::kRmtTypeMimicPpuActionbus;
  constexpr int RmtTypes::kRmtTypeMimicPpuActionphv;
  constexpr int RmtTypes::kRmtTypeMimicPpuActionram;
  constexpr int RmtTypes::kRmtTypeMimicPpuBasicaction;
  constexpr int RmtTypes::kRmtTypeMimicPpuEalu;
  constexpr int RmtTypes::kRmtTypeMimicPpuExactmatch;
  constexpr int RmtTypes::kRmtTypeMimicPpuIdletimeaction;
  constexpr int RmtTypes::kRmtTypeMimicPpuMatchinput;
  constexpr int RmtTypes::kRmtTypeMimicPpuMatchresdist;
  constexpr int RmtTypes::kRmtTypeMimicPpuMeteraction;
  constexpr int RmtTypes::kRmtTypeMimicPpuOperandfifo;
  constexpr int RmtTypes::kRmtTypeMimicPpuPhvwrite;
  constexpr int RmtTypes::kRmtTypeMimicPpuSparsetrie;
  constexpr int RmtTypes::kRmtTypeMimicPpuStatefulaction;
  constexpr int RmtTypes::kRmtTypeMimicPpuStatsaction;
  constexpr int RmtTypes::kRmtTypeMimicPpuTernaryind;
  constexpr int RmtTypes::kRmtTypeMimicPpuXcmpaction;
  constexpr int RmtTypes::kRmtTypeMimicScm;
  constexpr int RmtTypes::kRmtTypeMimicStm;
  constexpr int RmtTypes::kRmtTypeMimicTcams;
  constexpr int RmtTypes::kRmtTypeMimicPhvAction;
  constexpr int RmtTypes::kRmtTypeMimicDelayChecks;
  constexpr int RmtTypes::kRmtTypeMimicLegacyCode;
  constexpr int RmtTypes::kRmtTypeMimicEmac;
  constexpr int RmtTypes::kRmtTypeMimicEtm;
  constexpr int RmtTypes::kRmtTypeMimicImac;
  constexpr int RmtTypes::kRmtTypeMimicItm;
} // namespace model_core
