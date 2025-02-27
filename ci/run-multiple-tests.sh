#! /bin/bash

# Copyright 2025 Andy Fingerhut
# SPDX-License-Identifier: Apache-2.0
#
# Run all Tofino1 and Tofino2 tests except these:
#
# * skip ones expected to fail or hang
# * skip ones expected to take more than 5 minutes
#
# This script only runs the tests one at a time.  It is likely to be
# possible to run multiple tests in parallel on a single system, but
# each such parallel test requires about another 2 GBytes of RAM, and
# it is significantly more involved to get all of the setup details
# right to enable the tests to run in parallel.  The goal here is to
# be very simple and straightforward.  This can be useful for test
# runs scheduled nightly or weeekly, for example.

THIS_SCRIPT_FILE_MAYBE_RELATIVE="$0"
THIS_SCRIPT_DIR_MAYBE_RELATIVE="${THIS_SCRIPT_FILE_MAYBE_RELATIVE%/*}"
THIS_SCRIPT_DIR_ABSOLUTE=`readlink -f "${THIS_SCRIPT_DIR_MAYBE_RELATIVE}"`

ARCH="tofino"
for ARCH in tofino tofino2
do
    case ${ARCH} in
    tofino)
        tests_dir="install/share/tofinopd"
        ;;
    tofino2)
        tests_dir="install/share/tofino2pd"
        ;;
    esac
    for dir in $tests_dir/*
    do
	# All of the estimated durations below were measured on a
	# personal x86_64 system circa 2024 that cost $600.  They are
	# only here to give a rough idea of how much time we expect
	# the test to take.
        basedir=$(basename $dir)
        P4_NAME="${basedir}"
        estimated_duration_sec="(unknown)"
        expected_result="pass"
        p4_variant="p4_16"
        case ${basedir} in
            alpm_test)
                estimated_duration_sec="5739"
                p4_variant="p4_14"
                ;;
            basic_switching)
                estimated_duration_sec="12"
                p4_variant="p4_14"
                ;;
            bri_grpc_error)
                estimated_duration_sec="917"
                ;;
            bri_handle)
                estimated_duration_sec="17"
                ;;
            bri_with_pdfixed_thrift)
                estimated_duration_sec="14"
                ;;
            chksum)
                estimated_duration_sec="12"
                p4_variant="p4_14"
                ;;
            default_entry)
                estimated_duration_sec="16"
                p4_variant="p4_14"
                ;;
            deparse_zero)
                estimated_duration_sec="17"
                p4_variant="p4_14"
                ;;
            drivers_test)
                estimated_duration_sec="12"
                p4_variant="p4_14"
                expected_result="fail"
		fail_notes="No file named drivers-test-info.json"
                ;;
            fast_reconfig)
                estimated_duration_sec="31"
                p4_variant="p4_14"
                ;;
            ha)
                estimated_duration_sec="46"
                p4_variant="p4_14"
                ;;
            hash_driven)
                estimated_duration_sec="19"
                p4_variant="p4_14"
                ;;
            iterator)
                estimated_duration_sec="32"
                p4_variant="p4_14"
                ;;
            knet_mgr_test)
                estimated_duration_sec="12"
                p4_variant="p4_14"
                expected_result="fail"
		fail_notes="Failed to find a class named 'Packet' in Python ptf test file"
                ;;
            meters)
                estimated_duration_sec="193"
                p4_variant="p4_14"
                ;;
            mirror_test)
                estimated_duration_sec="36"
                p4_variant="p4_14"
                ;;
            multicast_test)
                estimated_duration_sec="271"
                p4_variant="p4_14"
                ;;
            parser_error)
                estimated_duration_sec="13"
                p4_variant="p4_14"
                ;;
            parser_intr_md)
                estimated_duration_sec="12"
                p4_variant="p4_14"
                ;;
            pcie_pkt_test)
                estimated_duration_sec="59"
                p4_variant="p4_14"
                ;;
            perf_test)
                estimated_duration_sec="54"
                p4_variant="p4_14"
                ;;
            perf_test_alpm)
                estimated_duration_sec="1033"
                p4_variant="p4_14"
                ;;
            pgrs)
                estimated_duration_sec="252"
                p4_variant="p4_14"
                ;;
            pvs)
                estimated_duration_sec="65"
                p4_variant="p4_14"
                ;;
            resubmit)
                estimated_duration_sec="13"
                p4_variant="p4_14"
                ;;
            selector_resize)
                estimated_duration_sec="2148"
                ;;
            smoke_large_tbls)
                estimated_duration_sec="1235"
                p4_variant="p4_14"
                ;;
            stful)
                estimated_duration_sec="(hangs)"
                p4_variant="p4_14"
                expected_result="fail"
		fail_notes="No file named stful.json"
                ;;
	    t2na_counter_true_egress_accounting)
		case ${ARCH} in
		    tofino)
			# Test only intended to run with Tofino2 model
                        P4_NAME="(none)"
		        ;;
		    tofino2)
			estimated_duration_sec="11"
			;;
		esac
                ;;
            tna_32q_2pipe)
                estimated_duration_sec="43"
                ;;
            tna_32q_multiprogram_a)
                P4_NAME="tna_32q_multiprogram"
                estimated_duration_sec="28"
                ;;
            tna_32q_multiprogram_b)
                P4_NAME="(none)"
                estimated_duration_sec=""
                ;;
            tna_action_profile)
                estimated_duration_sec="19"
                ;;
            tna_action_selector)
                estimated_duration_sec="2285"
                ;;
            tna_alpm)
                estimated_duration_sec="19"
                ;;
            tna_bridged_md)
                estimated_duration_sec="13"
                ;;
            tna_checksum)
                estimated_duration_sec="16"
                ;;
            tna_counter)
                estimated_duration_sec="40"
                ;;
            tna_custom_hash)
                estimated_duration_sec="12"
                ;;
            tna_digest)
                estimated_duration_sec="14"
                ;;
            tna_dkm)
                estimated_duration_sec="13"
                ;;
            tna_dyn_hashing)
                estimated_duration_sec="15"
                ;;
            tna_exact_match)
                estimated_duration_sec="87"
                ;;
            tna_field_slice)
                estimated_duration_sec="201"
                ;;
            tna_idletimeout)
                estimated_duration_sec="211"
                ;;
            tna_lpm_match)
                estimated_duration_sec="82"
                ;;
            tna_meter_bytecount_adjust)
                estimated_duration_sec="13"
                ;;
            tna_meter_lpf_wred)
                estimated_duration_sec="39"
                ;;
            tna_mirror)
                estimated_duration_sec="47"
                ;;
            tna_multicast)
                estimated_duration_sec="100"
                ;;
            tna_operations)
                estimated_duration_sec="29"
                ;;
            tna_pktgen)
                estimated_duration_sec="198"
                ;;
            tna_port_metadata)
                estimated_duration_sec="18"
                ;;
            tna_port_metadata_extern)
                estimated_duration_sec="22"
                ;;
            tna_ports)
                estimated_duration_sec="12"
                ;;
            tna_proxy_hash)
                estimated_duration_sec="13"
                ;;
            tna_pvs)
                estimated_duration_sec="13"
                ;;
            tna_random)
                estimated_duration_sec="16"
                ;;
            tna_range_match)
                estimated_duration_sec="15"
                ;;
            tna_register)
                estimated_duration_sec="65"
                ;;
            tna_resubmit)
                estimated_duration_sec="13"
                ;;
            tna_snapshot)
                estimated_duration_sec="18"
                ;;
            tna_symmetric_hash)
                estimated_duration_sec="12"
                ;;
            tna_ternary_match)
                estimated_duration_sec="46"
                ;;
            tna_timestamp)
                estimated_duration_sec="12"
                ;;
        esac
	if [ "${basedir}" == "${P4_NAME}" ]
	then
	    comment=""
	else
	    comment="  -- basedir was ${basedir}"
	fi
	if [ ${P4_NAME} == "(none)" ]
	then
	    continue
	fi
	echo "------------------------------------------------------------"
        echo ${ARCH} ${expected_result} ${p4_variant} ${estimated_duration_sec} ${P4_NAME} ${comment}
	if [ ${expected_result} == "fail" ]
	then
	    continue
	fi
	if [ ${estimated_duration_sec} == "(hangs)" ]
	then
	    continue
	elif [ ${estimated_duration_sec} -le 60 ]
	then
	    time_category="short"
	elif [ ${estimated_duration_sec} -le 300 ]
	then
	    time_category="medium"
	else
	    time_category="long"
	fi
	if [ ${time_category} == "long" ]
	then
	    continue
	fi
	time ${THIS_SCRIPT_DIR_ABSOLUTE}/run-one-test.sh -p ${P4_NAME} --arch ${ARCH}
	test_exit_status=$?
        echo "Test ARCH=${ARCH} exit_status=${test_exit_status} ${p4_variant} ${P4_NAME}"
    done
done
