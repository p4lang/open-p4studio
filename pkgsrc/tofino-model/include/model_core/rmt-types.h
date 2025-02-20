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

#ifndef _MODEL_CORE_RMT_TYPES_
#define _MODEL_CORE_RMT_TYPES_

#include <array>
#include <unordered_map>
#include <string>

namespace model_core {

  class RmtTypes {

 public:
    RmtTypes()  { }
    ~RmtTypes() { }
    // ? use enum instead?
    // Note RmtObjectManager::update_log_flags uses 64-bit bitmasks
    // for switching on type debug and there are more than 64 types
    // so bit X will switch on debug for any type == X mod 64
    //
    // First block contains LOG/RMT_LOG statements (18/11/2015)
    static constexpr int kRmtTypeUndefined                   =  0;
    static constexpr int kRmtTypeChecksumEngine              =  1;
    static constexpr int kRmtTypeDeparser                    =  2;
    static constexpr int kRmtTypeDeparserBlock               =  3;
    static constexpr int kRmtTypeEpb                         =  4;
    static constexpr int kRmtTypeHashAddressVhXbar           =  5;
    static constexpr int kRmtTypeModel                       =  6;
    static constexpr int kRmtTypeInputControlledXbar         =  7;
    static constexpr int kRmtTypeInstr                       =  8;
    static constexpr int kRmtTypeMatchDataInputVhXbar        =  9;
    static constexpr int kRmtTypeMau                         = 10;
    static constexpr int kRmtTypeMauAddrDist                 = 11;
    static constexpr int kRmtTypeMauDependencies             = 12;
    static constexpr int kRmtTypeMauGatewayPayload           = 13;
    static constexpr int kRmtTypeMauGatewayPayloadReg        = 14;
    static constexpr int kRmtTypeMauGatewayTable             = 15;
    static constexpr int kRmtTypeMauGatewayTableReg          = 16;
    static constexpr int kRmtTypeMauHashGenerator            = 17;
    static constexpr int kRmtTypeMauHashGeneratorWithReg     = 18;
    static constexpr int kRmtTypeMauInput                    = 19;
    static constexpr int kRmtTypeMauInputXbar                = 20;
    static constexpr int kRmtTypeMauInstrStore               = 21;
    static constexpr int kRmtTypeMauLogicalRow               = 22;
    static constexpr int kRmtTypeMauLogicalRowReg            = 23;
    static constexpr int kRmtTypeMauLogicalTable             = 24;
    static constexpr int kRmtTypeMauLogicalTableReg          = 25;
    static constexpr int kRmtTypeMauLogicalTcam              = 26;
    static constexpr int kRmtTypeMauLogicalTcamReg           = 27;
    static constexpr int kRmtTypeMauLookupResult             = 28;
    static constexpr int kRmtTypeMauMapram                   = 29;
    static constexpr int kRmtTypeMauMemory                   = 30;
    static constexpr int kRmtTypeMauResultBus                = 31;
    static constexpr int kRmtTypeMauSram                     = 32;
    static constexpr int kRmtTypeMauSramColumn               = 33;
    static constexpr int kRmtTypeMauSramColumnReg            = 34;
    static constexpr int kRmtTypeMauSramReg                  = 35;
    static constexpr int kRmtTypeMauSramRow                  = 36;
    static constexpr int kRmtTypeMauStash                    = 37;
    static constexpr int kRmtTypeMauStashColumn              = 38;
    static constexpr int kRmtTypeMauStatsAlu                 = 39;
    static constexpr int kRmtTypeMauTcam                     = 40;
    static constexpr int kRmtTypeMauTcamRow                  = 41;
    static constexpr int kRmtTypePacket                      = 42;
    static constexpr int kRmtTypeParser                      = 43;
    static constexpr int kRmtTypeParserBlock                 = 44;
    static constexpr int kRmtTypePhv                         = 45;
    static constexpr int kRmtTypePhvFactory                  = 46;
    static constexpr int kRmtTypePipe                        = 47;
    static constexpr int kRmtTypePort                        = 48;
    static constexpr int kRmtTypeQueueing                    = 49;
    static constexpr int kRmtTypeRegisterBlock               = 50;
    static constexpr int kRmtTypeRegisterBlockIndirect       = 51;
    static constexpr int kRmtTypeRmtObjectManager            = 52;
    static constexpr int kRmtTypeRmtOpHandler                = 53;
    static constexpr int kRmtTypeSram                        = 54;
    static constexpr int kRmtTypeTcam3                       = 55;
    static constexpr int kRmtTypeMauMeterAlu                 = 56;
    static constexpr int kRmtTypeRmtSweeper                  = 57;
    static constexpr int kRmtTypeMauMeter                    = 58;
    static constexpr int kRmtTypeMauLpfMeter                 = 59;
    static constexpr int kRmtTypeStatefulAlu                 = 60;
    static constexpr int kRmtTypeMauMapramReg                = 61;
    static constexpr int kRmtTypeMauSramRowReg               = 62;
    static constexpr int kRmtTypeReserved1                   = 63;
    static constexpr int kRmtTypeReserved2                   = 64;
    static constexpr int kRmtTypeMauTcamReg                  = 65;
    static constexpr int kRmtTypeSelectorAlu                 = 66;
    static constexpr int kRmtTypeMauMoveregs                 = 67;
    static constexpr int kRmtTypeMauTableCounters            = 68;
    static constexpr int kRmtTypeMauSnapshot                 = 69;

    // This block doesn't (as at 18/11/2015) but probably will
    static constexpr int kRmtTypeBitVector                   = 70;
    static constexpr int kRmtTypeRmtPacketCoordinator        = 71;
    static constexpr int kRmtTypeIndirectAccessBlock         = 72;
    static constexpr int kRmtTypeMauLogicalTcamCol           = 73;
    static constexpr int kRmtTypeMauOpHandler                = 74;
    static constexpr int kRmtTypePacketReplicationEngine     = 75;
    static constexpr int kRmtTypePacketReplicationEngineReg  = 76;
    // This block might never
    static constexpr int kRmtTypeActionOutputHvXbar          = 77;
    static constexpr int kRmtTypeDeparserReg                 = 78;
    static constexpr int kRmtTypeHashAddressVhXbarWithReg    = 79;
    static constexpr int kRmtTypeMatchDataInputVhXbarWithReg = 80;
    static constexpr int kRmtTypeMauInputXbarWithReg         = 81;
    static constexpr int kRmtTypeMauObject                   = 82;
    static constexpr int kRmtTypeMauNotUsed                  = 83;
    static constexpr int kRmtTypePacketBuffer                = 84;
    static constexpr int kRmtTypePipeObject                  = 85;
    static constexpr int kRmtTypeRmtDefs                     = 86;
    static constexpr int kRmtTypeRmtObject                   = 87;
    static constexpr int kRmtTypeRmtTypes                    = 88;
    static constexpr int kRmtTypeTcamRowVhWithReg            = 89;
    static constexpr int kRmtTypeCacheId                     = 90;
    static constexpr int kRmtTypePktGen                      = 91;
    static constexpr int kRmtTypeMauIO                       = 92;
    static constexpr int kRmtTypeMauColorSwitchbox           = 93;

    static constexpr int kRmtTypeRmtStringMap                = 94;
    static constexpr int kRmtTypeChip                        = 95;
    static constexpr int kRmtTypeIpb                         = 96;

    // Just placeholders for new types (array sized for 100)
    static constexpr int kRmtTypeMauPredication              = 97;
    static constexpr int kRmtTypeMauTeop                     = 98;
    static constexpr int kRmtTypeTm                          = 99;
    static constexpr int kRmtTypeReserved9                   = 100;

    // Mimic report::ModuleId tags - note lots of 'holes' to make numbers
    // line up mod64 with corresponding module in old code
    static constexpr int kRmtTypeMimicUnittests              = 128;
    static constexpr int kRmtTypeMimicFramework              = 129;
//    static constexpr int kRmtTypeMimicDeparser               = 130; // 2:kRmtTypeDeparser
    static constexpr int kRmtTypeMimicEmerge                 = 131;
    static constexpr int kRmtTypeMimicExtractor              = 132;
    static constexpr int kRmtTypeMimicPacker                 = 133;
    static constexpr int kRmtTypeMimicPparser                = 134;

    static constexpr int kRmtTypeMimicPpuEalu                = 136; // 8:kRmtTypeInstr
    static constexpr int kRmtTypeMimicLegacyCode             = 137;

    static constexpr int kRmtTypeMimicPpuMatchresdist        = 139; // 11:kRmtTypeMauAddrDist
    static constexpr int kRmtTypeMimicDelayChecks            = 140; // 12:MauDependencies
    static constexpr int kRmtTypeMimicPpuActionbus           = 141;
    static constexpr int kRmtTypeMimicPpuActionphv           = 142;
    static constexpr int kRmtTypeMimicPpuActionram           = 143;
    static constexpr int kRmtTypeMimicPpuBasicaction         = 144;
    static constexpr int kRmtTypeMimicPpuExactmatch          = 145;
    static constexpr int kRmtTypeMimicPpuIdletimeaction      = 146;
    static constexpr int kRmtTypeMimicPpuMatchinput          = 147;

    static constexpr int kRmtTypeMimicEmac                   = 149;
    static constexpr int kRmtTypeMimicEtm                    = 150;
    static constexpr int kRmtTypeMimicImac                   = 151;
    static constexpr int kRmtTypeMimicItm                    = 152;

    static constexpr int kRmtTypeMimicStm                    = 160; // 32:kRmtTypeMauSram

    static constexpr int kRmtTypeMimicPpuStatsaction         = 167; // 39:kRmtTypeMauStatsAlu
    static constexpr int kRmtTypeMimicScm                    = 168; // 40:kRmtTypeMauTcam

//    static constexpr int kRmtTypeMimicParser                 = 171; // 43:kRmtTypeParser

    static constexpr int kRmtTypeMimicPpuOperandfifo         = 174;
    static constexpr int kRmtTypeMimicPpuPhvwrite            = 175;
    static constexpr int kRmtTypeMimicPpuSparsetrie          = 176;
    static constexpr int kRmtTypeMimicPpuTernaryind          = 177;
    static constexpr int kRmtTypeMimicPhvAction              = 178;

    static constexpr int kRmtTypeMimicTcams                  = 183; // 55:kRmtTypeTcam3
    
    static constexpr int kRmtTypeMimicPpuMeteraction         = 186; // 58:kRmtTypeMauMeter
    
    static constexpr int kRmtTypeMimicPpuStatefulaction      = 188; // 60:kRmtTypeStatefulAlu
    
    static constexpr int kRmtTypeMimicPpuXcmpaction          = 194; // 66: SelectorAlu


    // for debug: defines how many identifiers (pipe,stage,row/table,column) to print
    static constexpr std::array<const int,195> PrintDepth = {
      0, // kRmtTypeUndefined    0
      2, // kRmtTypeChecksumEngine    1
      2, // kRmtTypeDeparser    2
      2, // kRmtTypeDeparserBlock    3
      0, // kRmtTypeEpb    4
      0, // kRmtTypeHashAddressVhXbar    5
      0, // kRmtTypeModel    6
      0, // kRmtTypeInputControlledXbar  7
      2, // kRmtTypeInstr  8
      2, // kRmtTypeMatchDataInputVhXbar  9
      2, // kRmtTypeMau  10
      2, // kRmtTypeMauAddrDist  11
      2, // kRmtTypeMauDependencies  12
      3, // kRmtTypeMauGatewayPayload  13
      3, // kRmtTypeMauGatewayPayloadReg  14
      4, // kRmtTypeMauGatewayTable  15
      4, // kRmtTypeMauGatewayTableReg  16
      2, // kRmtTypeMauHashGenerator  17
      2, // kRmtTypeMauHashGeneratorWithReg  18
      2, // kRmtTypeMauInput  19
      2, // kRmtTypeMauInputXbar  20
      2, // kRmtTypeMauInstrStore  21
      3, // kRmtTypeMauLogicalRow  22
      3, // kRmtTypeMauLogicalRowReg  23
      3, // kRmtTypeMauLogicalTable  24
      3, // kRmtTypeMauLogicalTableReg  25
      3, // kRmtTypeMauLogicalTcam  26
      3, // kRmtTypeMauLogicalTcamReg  27
      2, // kRmtTypeMauLookupResult  28
      4, // kRmtTypeMauMapram  29
      4, // kRmtTypeMauMemory  30
      0, // kRmtTypeMauResultBus  31
      4, // kRmtTypeMauSram  32
      4, // kRmtTypeMauSramColumn  33
      4, // kRmtTypeMauSramColumnReg  34
      4, // kRmtTypeMauSramReg  35
      3, // kRmtTypeMauSramRow  36
      3, // kRmtTypeMauStash  37
      3, // kRmtTypeMauStashColumn  38
      3, // kRmtTypeMauStatsAlu  39
      4, // kRmtTypeMauTcam  40
      3, // kRmtTypeMauTcamRow  41
      0, // kRmtTypePacket  42
      2, // kRmtTypeParser  43
      2, // kRmtTypeParserBlock  44
      1, // kRmtTypePhv  45
      0, // kRmtTypePhvFactory  46
      1, // kRmtTypePipe  47
      0, // kRmtTypePort  48
      0, // kRmtTypeQueueing  49
      0, // kRmtTypeRegisterBlock  50
      0, // kRmtTypeRegisterBlockIndirect  51
      0, // kRmtTypeRmtObjectManager  52
      0, // kRmtTypeRmtOpHandler  53
      4, // kRmtTypeSram  54
      4, // kRmtTypeTcam3  55
      3, // kRmtTypeMauMeterAlu  56
      0, // kRmtTypeRmtSweeper  57
      3, // kRmtTypeMauMeter 58
      3, // kRmtTypeMauLpfMeter   59
      3, // kRmtTypeStatefulAlu   60
      4, // kRmtTypeMauMapramReg  61
      4, // kRmtTypeMauSramRowReg  62
      0, // kRmtTypeReserved1    63
      0, // kRmtTypeReserved2    64
      4, // kRmtTypeMauTcamReg  65
      3, // kRmtTypeMauSelectorAlu  66
      2, // kRmtTypeMauMoveregs 67
      2, // kRmtTypeMauTableCounters 68
      2, // kRmtTypeMauSnapshot 69
      0, // kRmtTypeBitVector    70
      0, // kRmtTypeRmtPacketCoordinator    71
      0, // kRmtTypeIndirectAccessBlock  72
      4, // kRmtTypeMauLogicalTcamCol  73
      0, // kRmtTypeMauOpHandler  74
      0, // kRmtTypePacketReplicationEngine  75
      0, // kRmtTypePacketReplicationEngineReg  76
      0, // kRmtTypeActionOutputHvXbar v77
      2, // kRmtTypeDeparserReg  78
      0, // kRmtTypeHashAddressVhXbarWithReg   79
      2, // kRmtTypeMatchDataInputVhXbarWithReg  80
      2, // kRmtTypeMauInputXbarWithReg  81
      2, // kRmtTypeMauObject 82
      0, // kRmtTypeMauNotUsed  83
      0, // kRmtTypePacketBuffer  84
      1, // kRmtTypePipeObject 85
      0, // kRmtTypeRmtDefs 86
      0, // kRmtTypeRmtObject 87
      0, // kRmtTypeRmtTypes 88
      4, // kRmtTypeTcamRowVhWithReg 89
      0, // kRmtTypeCacheId 90
      2, // kRmtTypePktGen 91
      2, // kRmtTypeMauIO 92
      3, // kRmtTypeMauColorSwitchbox  93
      0, // kRmtTypeRmtStringMap  94
      0, // kRmtTypeChip  95
      0, // kRmtTypeReserved6  96
      0, // kRmtTypeMauPredication  97
      0, // kRmtTypeMauTeop  98
      0, // kRmtTypeTm 99
      0  // kRmtTypeReserved9  100
    };
    
    // can't be constexpr
    static const std::unordered_map<std::string, int> kRmtTypeMap;

    struct kRmtTypeEntry
    {
      const char* name;
      const short depth;
    };
    // ! change to map to reduce bug chance!
    // idx%64 = bit position in mask
    static constexpr std::array<const kRmtTypeEntry, 195> kRmtTypeArray = {
      // struct      name                            depth
      kRmtTypeEntry{"undef",                           0},  // 0
      kRmtTypeEntry{"checksumengine",                  2},
      kRmtTypeEntry{"deparser",                        2},
      kRmtTypeEntry{"deparserblock",                   2},
      kRmtTypeEntry{"epb",                             0},
      kRmtTypeEntry{"hashaddressvhxbar",               0},  // 5
      kRmtTypeEntry{"model",                           0},
      kRmtTypeEntry{"inputcontrolledxbar",             0},
      kRmtTypeEntry{"instr",                           2},
      kRmtTypeEntry{"matchdatainputvhxbar",            2},
      kRmtTypeEntry{"mau",                             2},  // 10
      kRmtTypeEntry{"mauaddrdist",                     2},
      kRmtTypeEntry{"maudependencies",                 2},
      kRmtTypeEntry{"maugatewaypayload",               3},
      kRmtTypeEntry{"maugatewaypayloadreg",            3},
      kRmtTypeEntry{"maugatewaytable",                 4},  // 15
      kRmtTypeEntry{"maugatewaytablereg",              4},
      kRmtTypeEntry{"mauhashgenerator",                2},
      kRmtTypeEntry{"mauhashgeneratorwithreg",         2},
      kRmtTypeEntry{"mauinput",                        2},
      kRmtTypeEntry{"mauinputxbar",                    2},  // 20
      kRmtTypeEntry{"mauinstrstore",                   2},
      kRmtTypeEntry{"maulogicalrow",                   3},
      kRmtTypeEntry{"maulogicalrowreg",                3},
      kRmtTypeEntry{"maulogicaltable",                 3},
      kRmtTypeEntry{"maulogicaltablereg",              3},  // 25
      kRmtTypeEntry{"maulogicaltcam",                  3},
      kRmtTypeEntry{"maulogicaltcamreg",               3},
      kRmtTypeEntry{"maulookupresult",                 2},
      kRmtTypeEntry{"maumapram",                       4},
      kRmtTypeEntry{"maumemory",                       4},  // 30
      kRmtTypeEntry{"mauresultbus",                    0},
      kRmtTypeEntry{"mausram",                         4},
      kRmtTypeEntry{"mausramcolumn",                   4},
      kRmtTypeEntry{"mausramcolumnreg",                4},
      kRmtTypeEntry{"mausramreg",                      4},  // 35
      kRmtTypeEntry{"mausramrow",                      3},
      kRmtTypeEntry{"maustash",                        3},
      kRmtTypeEntry{"maustashcolumn",                  3},
      kRmtTypeEntry{"maustatsalu",                     3},
      kRmtTypeEntry{"mautcam",                         4},  // 40
      kRmtTypeEntry{"mautcamrow",                      3},
      kRmtTypeEntry{"packet",                          0},
      kRmtTypeEntry{"parser",                          2},
      kRmtTypeEntry{"parserblock",                     2},
      kRmtTypeEntry{"phv",                             1},  // 45
      kRmtTypeEntry{"phvfactory",                      0},
      kRmtTypeEntry{"pipe",                            1},
      kRmtTypeEntry{"port",                            0},
      kRmtTypeEntry{"queueing",                        0},
      kRmtTypeEntry{"registerblock",                   0},  // 50
      kRmtTypeEntry{"registerblockindirect",           0},
      kRmtTypeEntry{"rmtobjectmanager",                0},
      kRmtTypeEntry{"rmtophandler",                    0},
      kRmtTypeEntry{"sram",                            4},
      kRmtTypeEntry{"tcam3",                           4},  // 55
      kRmtTypeEntry{"maumeteralu",                     3},
      kRmtTypeEntry{"rmtsweeper",                      0},
      kRmtTypeEntry{"maumeter",                        3},
      kRmtTypeEntry{"maulpfmeter",                     3},
      kRmtTypeEntry{"statefulalu",                     3},  // 60
      kRmtTypeEntry{"maumapramreg",                    4},
      kRmtTypeEntry{"mausramrowreg",                   4},
      kRmtTypeEntry{"reserved1",                       0},
      kRmtTypeEntry{"reserved2",                       0},
      kRmtTypeEntry{"mautcamreg",                      4},  // 65
      kRmtTypeEntry{"selectoralu",                     3},
      kRmtTypeEntry{"maumoveregs",                     2},
      kRmtTypeEntry{"mautablecounters",                2},
      kRmtTypeEntry{"mausnapshot",                     2},
      kRmtTypeEntry{"bitvector",                       0},  // 70
      kRmtTypeEntry{"rmtpacketcoordinator",            0},
      kRmtTypeEntry{"indirectaccessblock",             0},
      kRmtTypeEntry{"maulogicaltcamcol",               4},
      kRmtTypeEntry{"mauophandler",                    0},
      kRmtTypeEntry{"packetreplicationengine",         0},  // 75
      kRmtTypeEntry{"packetreplicationenginereg",      0},
      kRmtTypeEntry{"actionoutputhvxbar",              0},
      kRmtTypeEntry{"deparserreg",                     2},
      kRmtTypeEntry{"hashaddressvhxbarwithreg",        0},
      kRmtTypeEntry{"matchdatainputvhxbarwithreg",     2},  // 80
      kRmtTypeEntry{"mauinputxbarwithreg",             2},
      kRmtTypeEntry{"mauobject",                       2},
      kRmtTypeEntry{"maunotused",                      0},
      kRmtTypeEntry{"packetbuffer",                    0},
      kRmtTypeEntry{"pipeobject",                      1},  // 85
      kRmtTypeEntry{"rmtdefs",                         0},
      kRmtTypeEntry{"rmtobject",                       0},
      kRmtTypeEntry{"rmttypes",                        0},
      kRmtTypeEntry{"tcamrowvhwithreg",                4},
      kRmtTypeEntry{"cacheid",                         0},  // 90
      kRmtTypeEntry{"pktgen",                          2},
      kRmtTypeEntry{"mauio",                           2},
      kRmtTypeEntry{"maucolorswitchbox",               3},
      kRmtTypeEntry{"rmtstringmap",                    0},
      kRmtTypeEntry{"chip",                            0},  // 95
      kRmtTypeEntry{"ipb",                             0},
      kRmtTypeEntry{"maupredication",                  0},
      kRmtTypeEntry{"mauteop",                         0},
      kRmtTypeEntry{"tm",                              0},
      kRmtTypeEntry{"reserved9",                       0},  // 100
      kRmtTypeEntry{"undef",                           0},
      kRmtTypeEntry{"undef",                           0},
      kRmtTypeEntry{"undef",                           0},
      kRmtTypeEntry{"undef",                           0},
      kRmtTypeEntry{"undef",                           0},  // 105
      kRmtTypeEntry{"undef",                           0},
      kRmtTypeEntry{"undef",                           0},
      kRmtTypeEntry{"undef",                           0},
      kRmtTypeEntry{"undef",                           0},
      kRmtTypeEntry{"undef",                           0},  // 110
      kRmtTypeEntry{"undef",                           0},
      kRmtTypeEntry{"undef",                           0},
      kRmtTypeEntry{"undef",                           0},
      kRmtTypeEntry{"undef",                           0},
      kRmtTypeEntry{"undef",                           0},  // 115
      kRmtTypeEntry{"undef",                           0},
      kRmtTypeEntry{"undef",                           0},
      kRmtTypeEntry{"undef",                           0},
      kRmtTypeEntry{"undef",                           0},
      kRmtTypeEntry{"undef",                           0},  // 120
      kRmtTypeEntry{"undef",                           0},
      kRmtTypeEntry{"undef",                           0},
      kRmtTypeEntry{"undef",                           0},
      kRmtTypeEntry{"undef",                           0},
      kRmtTypeEntry{"undef",                           0},  // 125
      kRmtTypeEntry{"undef",                           0},
      kRmtTypeEntry{"undef",                           0},
      kRmtTypeEntry{"unittests",                       0},  // TODO: what depth should mimic have?
      kRmtTypeEntry{"framework",                       0},
      kRmtTypeEntry{"undef",                           0},  // 130
      kRmtTypeEntry{"emerge",                          0},
      kRmtTypeEntry{"extractor",                       0},
      kRmtTypeEntry{"packer",                          0},
      kRmtTypeEntry{"pparser",                         0},
      kRmtTypeEntry{"undef",                           0},  // 135
      kRmtTypeEntry{"ppuealu",                         0},
      kRmtTypeEntry{"legacycode",                      0},
      kRmtTypeEntry{"undef",                           0},
      kRmtTypeEntry{"ppumatchresdist",                 0},
      kRmtTypeEntry{"delaychecks",                     0},  // 140
      kRmtTypeEntry{"ppuactionbus",                    0},
      kRmtTypeEntry{"ppuactionphv",                    0},
      kRmtTypeEntry{"ppuactionram",                    0},
      kRmtTypeEntry{"ppubasicaction",                  0},
      kRmtTypeEntry{"ppuexactmatch",                   0},  // 145
      kRmtTypeEntry{"ppuidletimeaction",               0},
      kRmtTypeEntry{"ppumatchinput",                   0},
      kRmtTypeEntry{"undef",                           0},
      kRmtTypeEntry{"emac",                            0},
      kRmtTypeEntry{"etm",                             0},  // 150
      kRmtTypeEntry{"imac",                            0},
      kRmtTypeEntry{"itm",                             0},
      kRmtTypeEntry{"undef",                           0},
      kRmtTypeEntry{"undef",                           0},
      kRmtTypeEntry{"undef",                           0},  // 155
      kRmtTypeEntry{"undef",                           0},
      kRmtTypeEntry{"undef",                           0},
      kRmtTypeEntry{"undef",                           0},
      kRmtTypeEntry{"undef",                           0},
      kRmtTypeEntry{"stm",                             0},  // 160
      kRmtTypeEntry{"undef",                           0},
      kRmtTypeEntry{"undef",                           0},
      kRmtTypeEntry{"undef",                           0},
      kRmtTypeEntry{"undef",                           0},
      kRmtTypeEntry{"undef",                           0},  // 165
      kRmtTypeEntry{"undef",                           0},
      kRmtTypeEntry{"ppustatsaction",                  0},
      kRmtTypeEntry{"scm",                             0},
      kRmtTypeEntry{"undef",                           0},
      kRmtTypeEntry{"undef",                           0},  // 170
      kRmtTypeEntry{"undef",                           0},
      kRmtTypeEntry{"undef",                           0},
      kRmtTypeEntry{"undef",                           0},
      kRmtTypeEntry{"ppuoperandfifo",                  0},
      kRmtTypeEntry{"ppuphvwrite",                     0},  // 175
      kRmtTypeEntry{"ppusparsetrie",                   0},
      kRmtTypeEntry{"pputernaryind",                   0},
      kRmtTypeEntry{"phvaction",                       0},
      kRmtTypeEntry{"undef",                           0},
      kRmtTypeEntry{"undef",                           0},  // 180
      kRmtTypeEntry{"undef",                           0},
      kRmtTypeEntry{"undef",                           0},
      kRmtTypeEntry{"tcams",                           0},
      kRmtTypeEntry{"undef",                           0},
      kRmtTypeEntry{"undef",                           0},  // 185
      kRmtTypeEntry{"ppumeteraction",                  0},
      kRmtTypeEntry{"undef",                           0},
      kRmtTypeEntry{"ppustatefulaction",               0},
      kRmtTypeEntry{"undef",                           0},
      kRmtTypeEntry{"undef",                           0},  // 190
      kRmtTypeEntry{"undef",                           0},
      kRmtTypeEntry{"undef",                           0},
      kRmtTypeEntry{"undef",                           0},
      kRmtTypeEntry{"ppuxcmpaction",                   0},  // 194
    };

    // return type string consistent with event JSON schema
    static const char* toString(const int);
    static int toInt(const char*);
  };

}

#endif // _MODEL_CORE_RMT_TYPES_
