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

// Ingress meta data
enum ing_meta_data_e {
    ING_PKT_VER                  ,
    ING_CT_MC                    ,
    ING_CT_DIS                   ,
    ING_ICOS                     ,
    ING_INPORT_PORT              ,
    ING_INPORT_PORT_2L_PIPE_ID   ,
    ING_INPORT_PORT_2L_P_PORT_ID ,
    ING_EPIPE_PORT_VLD           ,
    ING_EPIPE_PORT               ,
    ING_EPORT_QID                ,
    ING_COLOR                    ,
    ING_PIPE_VEC                 ,
    ING_HASH1                    ,
    ING_HASH2                    ,
    ING_MCID1_VLD                ,
    ING_MCID1_ID                 ,
    ING_MCID2_VLD                ,
    ING_MCID2_ID                 ,
    ING_XID                      ,
    ING_YID                      ,
    ING_RID                      ,
    ING_EGRESS_BYPASS            ,
    ING_TABLEID                  ,
    ING_C2C_VLD                  ,
    ING_C2C_COS                  ,
    ING_DEF_ON_DROP              ,
    MIRROR_ID                    ,
    MIRROR_VERSION               ,
    MIRROR_COAL                  ,
    ING_TM_VEC                   ,
    ING_AFC_PORT_ID              ,
    ING_AFC_QID                  ,
    ING_AFC_CREDIT               ,
    ING_AFC_ADVQFC               ,
    ING_META_FIELDS
};

// Egress meta data
enum egr_meta_data_e {
  EGR_EPIPE_PORT
};


