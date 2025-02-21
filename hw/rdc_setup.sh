#!/bin/bash

# TODO: Fill out the path to the bf-drivers directory in the RDC and open source repositories, respectively 

RDC_BFD="/nfs/site/home/vtipired/bf-sde-9.13.3/bf-drivers-9.13.3"
OS_BFD="/nfs/site/home/vtipired/opensource/pkgsrc/bf-drivers"

replace(){
    local SUBDIR="$1"
    local IS_DIR="$2"
    shift 2
    local FILES=("${@}")
    for FILE in "${FILES[@]}"
    do
        echo "Copying $SUBDIR/$FILE"
        if [ "$IS_DIR" -eq 1 ]; then
            cp -r $RDC_BFD/$SUBDIR/$FILE $OS_BFD/$SUBDIR/$FILE
        else
            cp $RDC_BFD/$SUBDIR/$FILE $OS_BFD/$SUBDIR/$FILE
        fi
    done
}

rdc_setup(){
    
    if [ -z "$RDC_BFD" ] || [ -z "$OS_BFD" ]; then
        echo "Missing path(s) to open source repository and RDC repository. Please Modify the RDC_BFD and OS_BFD path variables in the script before calling for the RDC file transfer."
        return
    fi

    SUBDIR="bf_switchd"
    cp $RDC_BFD/$SUBDIR/CMakeLists.txt $OS_BFD/$SUBDIR/CMakeLists.txt

    SUBDIR="src"
    DIRS=("alphawave" "credo" "avago" "microp")
    replace "$SUBDIR" 1 "${DIRS[@]}" 
    cp $RDC_BFD/$SUBDIR/CMakeLists.txt $OS_BFD/$SUBDIR/CMakeLists.txt

    cp $RDC_BFD/libavago* $OS_BFD

    SUBDIR="include/avago"
    FILES=("aapl.h" "avago_aapl.h" "avago_dox.h")
    replace "$SUBDIR" 0 "${FILES[@]}"

    SUBDIR="src/port_mgr"
    # If port_mgr_dev.c or port_mgr_physical_dev.c are modified by the open source community, those changes must be merged manually by the user
    FILES=("CMakeLists.txt" "bf_ll_umac3_if.c" "bf_ll_umac4_if.c" "port_mgr_dev.c" "port_mgr_physical_dev.h" "port_mgr_umac_access.c")
    DIRS=("csr" "crdo" "aw-gen" "t3-csr")
    replace "$SUBDIR" 1 "${DIRS[@]}"
    replace "$SUBDIR" 0 "${FILES[@]}"

    SUBDIR="src/port_mgr/port_mgr_tof1"
    FILES=("bf_serdes_if.c" "comira_reg_access_autogen.c" "comira_reg_access_autogen.h" "comira_reg_def_autogen.h" "comira_reg_strs.h" "port_mgr_av_sd.c" "port_mgr_av_sd.h" "port_mgr_av_sd_an.c" "port_mgr_av_sd_an.h" "port_mgr_mac.c" "port_mgr_port_diag.c" "port_mgr_serdes.c" "port_mgr_serdes_diag.c" "port_mgr_serdes_sbus_map.c" "port_mgr_ucli.c" "bf_fsm_hdlrs.c" )
    replace "$SUBDIR" 0 "${FILES[@]}"

    SUBDIR="src/port_mgr/port_mgr_tof2"
    FILES=("autogen-required-headers.h" "bf_ll_eth100g_reg_rspec_if.c" "bf_ll_eth400g_mac_rspec_if.c" "bf_ll_eth400g_pcs_rspec_if.c" "bf_ll_serdes_if.c" "bf_tof2_serdes_if.c" "credo_sd_access.c" "credo_sd_access.h" "eth100g_reg_rspec_access.c" "eth400g_mac_rspec_access.c" "eth400g_pcs_rspec_access.c" "port_mgr_tof2_bandgap.c" "port_mgr_tof2_gpio.c" "port_mgr_tof2_microp.c" "port_mgr_tof2_serdes.c" "port_mgr_tof2_umac.c" "port_mgr_tof2_umac3.c" "port_mgr_tof2_umac4.c" "umac3c4_access.c" "umac3c4_fld_access.c" "umac4_ctrs.c" "umac4_ctrs_str.c" "umac4c8_access.c" "umac4c8_fld_access.c")
    replace "$SUBDIR" 0 "${FILES[@]}"

    SUBDIR="src/port_mgr/port_mgr_tof3"
    # If aw_if.h, aw_mss.h, or bf_tof3_serdes_utils.h are modified by the open source community, those changes must be merged manually by the user
    FILES=("aw_driver_sim.c" "aw_driver_sim.h" "aw_if.h" "aw_io.c" "aw_io.h" "aw_mss.h" "aw_reg_dbg.c" "aw_reg_dbg.h" "aw_types.h" "aw_vector_types.h" "bf_aw_pmd.c" "bf_aw_vfld_pmd.c" "bf_ll_tof3_eth400g_app_rspec_if.c" "bf_ll_tof3_eth400g_app_rspec_if.h" "bf_ll_tof3_eth400g_mac_rspec_if.c" "bf_ll_tof3_eth400g_mac_rspec_if.h" "bf_ll_tof3_eth400g_sys_rspec_if.c" "bf_ll_tof3_eth400g_sys_rspec_if.h" "bf_tof3_serdes_if.c" "bf_tof3_serdes_utils.c" "bf_tof3_serdes_utils.h" "port_mgr_tof3.c" "port_mgr_tof3_dev.c" "port_mgr_tof3_map.c" "port_mgr_tof3_microp.c" "port_mgr_tof3_port.c" "port_mgr_tof3_serdes.c" "port_mgr_tof3_serdes_map.c" "port_mgr_tof3_tmac.c" "svdpi.c" "svdpi.h" "tmac_access.c" "tmac_access.h" "tof3-autogen-required-headers.h" "tof3_eth400g_app_rspec_access.c" "tof3_eth400g_app_rspec_access.h" "tof3_eth400g_mac_rspec_access.c" "tof3_eth400g_mac_rspec_access.h" "tof3_eth400g_sys_rspec_access.c" "tof3_eth400g_sys_rspec_access.h" "vfld_vec_name.h" "vfld_vec_type.h")
    DIRS=("aw-reg-gen" "aw_16ln" "aw_4ln")
    replace "$SUBDIR" 1 "${DIRS[@]}"
    replace "$SUBDIR" 0 "${FILES[@]}"
}
