TESTS = utests/utests

# Generic tests that do not need to be repeated per-chip...
GENERIC_UTEST_FILES = \
utests/test_asserts.cpp \
utests/test_file_logger.cpp \
utests/test_json_loader.cpp \
utests/test_log_buffer.cpp \
utests/test_port_json_loader.cpp \
@top_srcdir@/runner/tests/utests/test_runner.cpp \
@top_srcdir@/runner/os_privs.cpp \
@top_srcdir@/runner/os_privs.h


# Common supporting files required for unit tests.
# Tests should not be added to this list.
# TODO: are some of these only required for dv tests?
COMMON_REQUIRED_UTEST_FILES = \
utests/action_hv_translator.cpp \
utests/reader_actions.cpp \
utests/tcam_row_vh_util.cpp \
utests/tcam_row_vh_util.h \
utests/test_config.cpp \
utests/test_reader.cpp \
utests/test_util.cpp \
@top_srcdir@/include/utests/test_reader.h \
@top_srcdir@/include/utests/test_reader_grammar.h \
@top_srcdir@/include/utests/test_util.h \
@top_srcdir@/include/utests/action_hv_translator.h \
@top_srcdir@/include/utests/reader_actions.h \
@top_srcdir@/include/utests/register_mapper.h \
@top_srcdir@/include/utests/test_config.h \
@top_srcdir@/include/utests/test_namespace.h

# A small set of tests for reduced testing, common to all chips.
# New tests should probably NOT be added to this list.
COMMON_SUBSET_UTEST_FILES = \
utests/test_chip.cpp \
utests/test_overlaps.cpp \
utests/test_wrapper.cpp

# Remaining tests that are common to all chips.
# Add new tests here or in a chip specific list.
COMMON_UTEST_FILES = \
utests/test_action_output_hv_xbar.cpp \
utests/test_actions.cpp \
utests/input_xbar_util.cpp \
utests/input_xbar_util.h \
utests/cntrstack_util.cpp \
utests/cntrstack_util.h \
utests/lpf_util.cpp \
utests/lpf_util.h \
utests/meter_util.cpp \
utests/meter_util.h \
utests/tcam_util.cpp \
utests/tcam_util.h \
utests/test_bitvector.cpp \
utests/test_caching_interval_map.cpp \
utests/test_capture.cpp \
utests/test_cksum.cpp \
utests/test_crafter1.cpp \
utests/test_idle.cpp \
utests/test_instr.cpp \
utests/test_ipb_common.cpp \
utests/test_lfltr.cpp \
utests/test_lpf.cpp \
utests/test_ltcam.cpp \
utests/test_mauhash.cpp \
utests/test_mau.cpp \
utests/test_meters.cpp \
utests/test_meter_sweep.cpp \
utests/test_mirror.cpp \
utests/test_model.cpp \
utests/test_p4_name_lookup.cpp \
utests/test_packet.cpp \
utests/test_parse.cpp \
utests/test_phv.cpp \
utests/test_pipe_object.cpp \
utests/test_port.cpp \
utests/test_queueing.cpp \
utests/test_registers.cpp \
utests/test_rmt_debug.cpp \
utests/test_rmt_log.cpp \
utests/test_rmt_util.cpp \
utests/test_salu_extracted.cpp \
utests/test_snapshot.cpp \
utests/test_sps.cpp \
utests/test_stateful_alu.cpp \
utests/test_stats.cpp \
utests/test_tcam2.cpp \
utests/test_tcam3.cpp \
utests/test_tcam.cpp \
utests/test_tcam_row_vh.cpp \
utests/test_timer.cpp \
utests/test_virt.cpp \
utests/test_dv999.cpp

# TofinoXX specific DV unit test files
TOFINO_DV_UTEST_FILES = \
utests/test_dvXX.cpp \
utests/test_dv11.cpp \
utests/test_dv13.cpp \
utests/test_dv14.cpp \
utests/test_dv16.cpp \
utests/test_dv19.cpp \
utests/test_dv22.cpp \
utests/test_dv27.cpp \
utests/test_dv29.cpp \
utests/test_dv33.cpp \
utests/test_dv35.cpp \
utests/test_dv37.cpp \
utests/test_dv40.cpp \
utests/test_dv41.cpp \
utests/test_dv49.cpp \
utests/test_dv56.cpp \
utests/test_dv60.cpp \
utests/test_dv62.cpp \
utests/test_dv67.cpp \
utests/test_dv69.cpp \
utests/test_dv71.cpp \
utests/test_dv72.cpp \
utests/test_dv74.cpp \
utests/test_dv76.cpp \
utests/test_dv77.cpp \
utests/test_dv79.cpp \
utests/test_dv80.cpp \
utests/test_dv81.cpp \
utests/test_dv82.cpp \
utests/test_dv83.cpp \
utests/test_dv84.cpp \
utests/test_dv85.cpp \
utests/test_dv86.cpp \
utests/test_dv88.cpp \
utests/test_dv90.cpp \
utests/test_dv92.cpp \
utests/test_dv94.cpp \
utests/test_dv96.cpp \
utests/test_dv102.cpp \
utests/test_dv108.cpp \
utests/test_dv110.cpp \
utests/test_dv112.cpp \
utests/test_dv120.cpp \
utests/test_dv124.cpp \
utests/test_dv126.cpp \
utests/test_dv128.cpp \
utests/test_dv142.cpp \
utests/test_dv146.cpp \
utests/test_dv150.cpp \
utests/test_dv152.cpp \
utests/test_dv156.cpp \
utests/test_dv158.cpp \
utests/test_dv164.cpp \
utests/test_dv166.cpp \
utests/test_dv168.cpp \
utests/test_dv170.cpp \
utests/test_dv172.cpp \
utests/test_dv176.cpp \
utests/test_dv178.cpp \
utests/test_dv180.cpp \
utests/test_dv182.cpp \
utests/test_dv184.cpp \
utests/test_dv198.cpp \
utests/test_dv204.cpp \
utests/test_dv206.cpp \
utests/test_dv212.cpp \
utests/test_dv214.cpp

# FIXME: Failing tests.
# utests/test_recipes.cpp
# utests/test_dv3.cpp
# utests/test_dv17.cpp
# utests/test_dv18.cpp
# utests/test_dv20.cpp
# utests/test_dv23.cpp
# utests/test_dv26.cpp
# utests/test_dv34.cpp
# utests/test_dv38.cpp
# utests/test_dv42.cpp
# utests/test_dv44.cpp
# utests/test_dv48.cpp
# utests/test_dv52.cpp
# utests/test_dv54.cpp
# utests/test_dv55.cpp
# utests/test_dv58.cpp
# utests/test_dv64.cpp
# utests/test_dv68.cpp
# utests/test_dv70.cpp

# BROKEN_BY_TCAM_CRB_CHANGE
#utests/test_test_dv3.cpp
#utests/test_test_dv17.cpp
#utests/test_test_dv18.cpp
#utests/test_test_dv20.cpp
#utests/test_test_dv23.cpp
#utests/test_test_dv26.cpp
#utests/test_test_dv38.cpp
#utests/test_test_dv44.cpp
#utests/test_test_dv52.cpp
#utests/test_test_dv54.cpp
#utests/test_test_dv58.cpp
#utests/test_test_dv68.cpp
#utests/test_test_dv70.cpp

# BROKEN_BY_SRAM_COL0_REMOVAL
#utests/test_test_dv34.cpp
#utests/test_test_dv42.cpp
#utests/test_test_dv48.cpp

# BROKEN_BY_HYPERDEV_TRANSITION
#utests/test_test_dv55.cpp
#utests/test_test_dv64.cpp

# not called/failing
#utests/test_dv31.cpp \
#utests/test_dv32.cpp \
#utests/test_dv224.cpp \
#utests/test_dv234.cpp

# TofinoXX specific supporting files required for unit tests.
# Tests should not be added to this list.
TOFINO_REQUIRED_UTEST_FILES = \
utests/test_util_tofino.cpp

# TofinoXX specific unit test files
TOFINO_UTEST_FILES = \
utests/test_counters.cpp \
utests/test_deparse.cpp \
utests/test_event.cpp \
utests/test_input_xbar.cpp \
utests/test_ipb_tofino.cpp \
utests/test_epb_tofino.cpp \
utests/test_parser_arbiter_tofino.cpp \
utests/test_parse_tofino.cpp \
utests/test_pktgen.cpp \
utests/test_rmt_packet_coordinator_tofinoxx.cpp \
utests/test_stash.cpp

# TofinoXX and JBay specific unit test files
TOFINOXX_JBAY_UTEST_FILES = \
utests/test_mac_counters.cpp

# JBay specific supporting files required for unit tests.
# Tests should not be added to this list.
JBAY_REQUIRED_UTEST_FILES = \
utests/test_util_jbay_shared.cpp

# JBay specific unit test files
JBAY_UTEST_FILES = \
utests/pktgen_util.cpp \
utests/pktgen_util.h \
utests/test_cksum_jbay.cpp \
utests/test_jbay_color_switchbox.cpp \
utests/test_jbay_counters.cpp \
utests/test_jbay_deparse.cpp \
utests/test_jbay_extracted.cpp \
utests/test_jbay_homerow_color.cpp \
utests/test_jbay_pktgen.cpp \
utests/test_epb_jbay.cpp \
utests/test_ipb_jbay.cpp \
utests/test_parser_arbiter_jbay.cpp \
utests/test_parse_jbay.cpp \
utests/test_pkt_q.h \
utests/test_s2p_p2s.cpp \
utests/test_tm_sch_jbay.cpp \
utests/test_wrapper_jbay.cpp

# previously missing on Jbay w.r.t. tofino...fail and/or ifdef'd out for jbay
#utests/test_event.cpp \
#utests/test_input_xbar.cpp \
#utests/test_stash.cpp

# CB specific unit test files
CB_UTEST_FILES = \
utests/test_cb_deparse.cpp \
utests/test_mac_counters.cpp

###############################################################################
#
# TofinoA0
#
###############################################################################
# Final specification of all built TofinoA0 utest sources
TOFINOA0_UTEST_SOURCES = \
$(TOFINO_BUILT_SOURCES) \
$(COMMON_REQUIRED_UTEST_FILES) \
$(TOFINO_REQUIRED_UTEST_FILES) \
$(COMMON_SUBSET_UTEST_FILES)

if MODEL_FULL_UTESTS_ENABLED
TOFINOA0_UTEST_SOURCES += $(COMMON_UTEST_FILES)
TOFINOA0_UTEST_SOURCES += $(TOFINO_UTEST_FILES)
TOFINOA0_UTEST_SOURCES += $(TOFINOXX_JBAY_UTEST_FILES)
if MODEL_DV_UTESTS_ENABLED
TOFINOA0_UTEST_SOURCES += $(TOFINO_DV_UTEST_FILES)
endif
endif

###############################################################################
#
# TofinoB0
#
###############################################################################
# Only build these lex/yacc files for TofinoB0
TOFINOB0_EXTRA_FILES = \
utests/lex.meter.c \
utests/meterparseryacc.c \
utests/meterparseryacc.h \
utests/lex.lpf.c \
utests/lpfparseryacc.c \
utests/lpfparseryacc.h

# Final specification of all built TofinB0 utest sources
TOFINOB0_UTEST_SOURCES = \
utests/test_parse_tofinoxx_jbay.cpp \
$(TOFINOB0_BUILT_SOURCES) \
$(COMMON_REQUIRED_UTEST_FILES) \
$(TOFINO_REQUIRED_UTEST_FILES) \
$(COMMON_SUBSET_UTEST_FILES)

if MODEL_FULL_UTESTS_ENABLED
TOFINOB0_UTEST_SOURCES += $(TOFINOB0_EXTRA_FILES)
TOFINOB0_UTEST_SOURCES += $(COMMON_UTEST_FILES)
TOFINOB0_UTEST_SOURCES += $(TOFINO_UTEST_FILES)
TOFINOB0_UTEST_SOURCES += $(TOFINOXX_JBAY_UTEST_FILES)
if MODEL_DV_UTESTS_ENABLED
TOFINOB0_UTEST_SOURCES += $(TOFINO_DV_UTEST_FILES)
endif
endif

###############################################################################
#
# JBay
#
###############################################################################
# Final specification of all built JBay utest sources
JBAY_UTEST_SOURCES = \
utests/test_egress_buf_jbay.cpp \
utests/test_parse_tofinoxx_jbay.cpp \
utests/test_rmt_packet_coordinator_jbay.cpp \
utests/test_s2p_p2s_jbay.cpp \
utests/test_util_jbay.cpp \
$(JBAY_BUILT_SOURCES) \
$(COMMON_REQUIRED_UTEST_FILES) \
$(JBAY_REQUIRED_UTEST_FILES) \
$(COMMON_SUBSET_UTEST_FILES)

if MODEL_FULL_UTESTS_ENABLED
JBAY_UTEST_SOURCES += $(COMMON_UTEST_FILES)
JBAY_UTEST_SOURCES += $(JBAY_UTEST_FILES)
JBAY_UTEST_SOURCES += $(TOFINOXX_JBAY_UTEST_FILES)
endif

###############################################################################
#
# JBayB0
#
###############################################################################
# Final specification of all built JBay utest sources
JBAYB0_UTEST_SOURCES = \
utests/test_egress_buf_jbay.cpp \
utests/test_parse_tofinoxx_jbay.cpp \
utests/test_rmt_packet_coordinator_jbay.cpp \
utests/test_s2p_p2s_jbay.cpp \
utests/test_util_jbay.cpp \
$(JBAYB0_BUILT_SOURCES) \
$(COMMON_REQUIRED_UTEST_FILES) \
$(JBAY_REQUIRED_UTEST_FILES) \
$(COMMON_SUBSET_UTEST_FILES)

if MODEL_FULL_UTESTS_ENABLED
JBAYB0_UTEST_SOURCES += $(COMMON_UTEST_FILES)
JBAYB0_UTEST_SOURCES += $(JBAY_UTEST_FILES)
JBAYB0_UTEST_SOURCES += $(TOFINOXX_JBAY_UTEST_FILES)
endif

###############################################################################
#
# CB
#
###############################################################################
# Final specification of all built CB utest sources
CB_UTEST_SOURCES = \
utests/cntrstack_wrap_cb.cpp \
utests/test_parse_cb.cpp \
utests/test_rmt_packet_coordinator_cb.cpp \
utests/test_util_cb.cpp \
$(CB_BUILT_SOURCES) \
$(COMMON_REQUIRED_UTEST_FILES) \
$(JBAY_REQUIRED_UTEST_FILES) \
$(COMMON_SUBSET_UTEST_FILES)

if MODEL_FULL_UTESTS_ENABLED
CB_UTEST_SOURCES += $(COMMON_UTEST_FILES)
CB_UTEST_SOURCES += $(JBAY_UTEST_FILES)
CB_UTEST_SOURCES += $(CB_UTEST_FILES)
endif



###############################################################################
#
# UTESTS
#
###############################################################################
OTHER_SOURCES = \
utests/gmock-gtest-all.cpp \
utests/utest.cpp \
utests/cmp_helper.cpp \
utests/filesystem_helper.cpp \
utests/main.cpp
utests_utests_SOURCES = $(OTHER_SOURCES)
utests_utests_SOURCES += $(GENERIC_UTEST_FILES)
utests_utests_LDADD = $(builddir)/../libmodelcore.a
utests_utests_LDFLAGS =
utests_utests_CPPFLAGS = $(UTESTS_CPPFLAGS) -I$(srcdir)/../include -I$(srcdir)/../include/shared
utests_utests_CPPFLAGS += -I$(srcdir)/../runner
utests_utests_CPPFLAGS += -I$(srcdir)/utests

BUILT_SOURCES =
lib_LIBRARIES =
check_LIBRARIES =
TEST_READER_OBJECTS =
TEST_UTIL_CPPFLAGS = $(AM_CPPFLAGS) $(CPPFLAGS) $(UTESTS_CPPFLAGS)

## CHIP SPECIFIC CONTENT for utests ##
#### tofino ####
TOFINO_CPPFLAGS  = -I$(srcdir)/../include
TOFINO_CPPFLAGS += -I$(srcdir)/../include/wrapper
TOFINO_CPPFLAGS += -I$(srcdir)/../include/tofino
TOFINO_CPPFLAGS += -I$(srcdir)/../include/tofino/dummy_registers
TOFINO_CPPFLAGS += -I$(srcdir)/../include/tofinoXX
TOFINO_CPPFLAGS += -I$(srcdir)/../include/tofinoXX_jbay
TOFINO_CPPFLAGS += -I$(srcdir)/../include/shared
TOFINO_CPPFLAGS += -I$(srcdir)/../include/model_core
TOFINO_CPPFLAGS += $(AM_CPPFLAGS)
TOFINO_CPPFLAGS += -DMODEL_TOFINO
TOFINO_CPPFLAGS += -DMODEL_CHIP_NAMESPACE=tofino

libtofinotest_a_CPPFLAGS = -I@pchdir@  -I@pchdir@/tofino
libtofinotest_a_CPPFLAGS += $(TOFINO_CPPFLAGS)
libtofinotest_a_CPPFLAGS += $(UTESTS_CPPFLAGS)
libtofinotest_a_SOURCES   = $(TOFINOA0_UTEST_SOURCES)

# Note: for each chip type, the chip specific tests lib is added to
# utests_utests_LDADD to ensure dependency/ordering. We also want to force the
# whole lib archive to be linked so that gtests actually finds test classes,
# but the forced loading flag(s) cannot be used in LDADD. We therefore add the
# lib again, with forced loading flags, in LDFLAGS.
# For each chip generate pre-compiled headers for test_util.h.
# Dependency file is generated by -MMD flag to C++ compiler.
if MODEL_TOFINO
tofino_precompiled_headers = \
@pchdir@/tofino/test_util.h.gch \
@pchdir@/tofino/test_util.h

-include @pchdir@/tofino/test_util.h.d
@pchdir@/tofino/test_util.h.gch : @top_srcdir@/include/utests/test_util.h Makefile
	mkdir -p @pchdir@/tofino
	$(CXX) -MMD -x c++-header $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(TEST_UTIL_CPPFLAGS) $(CXXFLAGS) $(libtofinotest_a_CPPFLAGS) $(extra_CFLAGS_utests_utests) -c $< -o $@
@pchdir@/tofino/test_util.h : Makefile @pchdir@/tofino/test_util.h.gch
	printf '#error "Precompiled header for %s not found"' $@ > $@

TOFINO_BUILT_SOURCES = $(tofino_precompiled_headers)

BUILT_SOURCES += $(TOFINO_BUILT_SOURCES)
check_LIBRARIES += libtofinotest.a
check_LIBRARIES += wrapper/libtofinowrapper.a
utests_utests_CPPFLAGS += -DMODEL_TOFINO
utests_utests_LDADD += libtofinotest.a
utests_utests_LDFLAGS += -Wl,${FORCE_LOAD},libtofinotest.a,${NO_FORCE_LOAD}
utests_utests_LDADD += $(builddir)/../libtofino.a
utests_utests_LDADD += wrapper/libtofinowrapper.a
TEST_READER_OBJECTS += utests/libtofinotest_a-test_reader.o
endif

#### tofinoB0 ####
TOFINOB0_CPPFLAGS  = -I$(srcdir)/../include
TOFINOB0_CPPFLAGS += -I$(srcdir)/../include/wrapper
TOFINOB0_CPPFLAGS += -I$(srcdir)/../include/tofinoB0
TOFINOB0_CPPFLAGS += -I$(srcdir)/../include/tofinoB0/dummy_registers
TOFINOB0_CPPFLAGS += -I$(srcdir)/../include/tofinoXX
TOFINOB0_CPPFLAGS += -I$(srcdir)/../include/tofinoXX_jbay
TOFINOB0_CPPFLAGS += -I$(srcdir)/../include/shared
TOFINOB0_CPPFLAGS += -I$(srcdir)/../include/model_core
TOFINOB0_CPPFLAGS += $(AM_CPPFLAGS)
TOFINOB0_CPPFLAGS += -DMODEL_TOFINOB0
TOFINOB0_CPPFLAGS += -DMODEL_CHIP_NAMESPACE=tofinoB0

libtofinoB0test_a_CPPFLAGS = -I@pchdir@  -I@pchdir@/tofinoB0
libtofinoB0test_a_CPPFLAGS += $(TOFINOB0_CPPFLAGS)
libtofinoB0test_a_CPPFLAGS += $(UTESTS_CPPFLAGS)
libtofinoB0test_a_SOURCES = $(TOFINOB0_UTEST_SOURCES)

if MODEL_TOFINOB0
tofinoB0_precompiled_headers = \
@pchdir@/tofinoB0/test_util.h.gch \
@pchdir@/tofinoB0/test_util.h

-include @pchdir@/tofinoB0/test_util.h.d
@pchdir@/tofinoB0/test_util.h.gch : @top_srcdir@/include/utests/test_util.h Makefile
	mkdir -p @pchdir@/tofinoB0
	$(CXX) -MMD -x c++-header $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(TEST_UTIL_CPPFLAGS) $(CXXFLAGS) $(libtofinoB0test_a_CPPFLAGS) $(extra_CFLAGS_utests_utests) -c $< -o $@
@pchdir@/tofinoB0/test_util.h : Makefile @pchdir@/tofinoB0/test_util.h.gch
	printf '#error "Precompiled header for %s not found"' $@ > $@

TOFINOB0_BUILT_SOURCES = $(tofinoB0_precompiled_headers)

BUILT_SOURCES += $(EXTRA_BUILT_SOURCES)
BUILT_SOURCES += $(TOFINOB0_BUILT_SOURCES)
check_LIBRARIES += libtofinoB0test.a
check_LIBRARIES += wrapper/libtofinoB0wrapper.a
utests_utests_CPPFLAGS += -DMODEL_TOFINOB0
utests_utests_LDADD += libtofinoB0test.a
utests_utests_LDFLAGS += -Wl,${FORCE_LOAD},libtofinoB0test.a,${NO_FORCE_LOAD}
utests_utests_LDADD += $(builddir)/../libtofinoB0.a
utests_utests_LDADD += wrapper/libtofinoB0wrapper.a
TEST_READER_OBJECTS += utests/libtofinoB0test_a-test_reader.o
endif

#### jbay ####
JBAY_CPPFLAGS  = -I$(srcdir)/../include
JBAY_CPPFLAGS += -I$(srcdir)/../include/wrapper
JBAY_CPPFLAGS += -I$(srcdir)/../include/jbay
JBAY_CPPFLAGS += -I$(srcdir)/../include/jbay/dummy_registers
JBAY_CPPFLAGS += -I$(srcdir)/../include/jbayXX
JBAY_CPPFLAGS += -I$(srcdir)/../include/jbay_shared
JBAY_CPPFLAGS += -I$(srcdir)/../include/tofinoXX_jbay
JBAY_CPPFLAGS += -I$(srcdir)/../include/shared
JBAY_CPPFLAGS += -I$(srcdir)/../include/model_core
JBAY_CPPFLAGS += $(AM_CPPFLAGS)
JBAY_CPPFLAGS += -DMODEL_JBAY
JBAY_CPPFLAGS += -DMODEL_CHIP_NAMESPACE=jbay

libjbaytest_a_CPPFLAGS = -I@pchdir@  -I@pchdir@/jbay
libjbaytest_a_CPPFLAGS += $(JBAY_CPPFLAGS)
libjbaytest_a_CPPFLAGS += $(UTESTS_CPPFLAGS)
libjbaytest_a_SOURCES = $(JBAY_UTEST_SOURCES)

if MODEL_JBAY
jbay_precompiled_headers = \
@pchdir@/jbay/test_util.h.gch \
@pchdir@/jbay/test_util.h

-include @pchdir@/jbay/test_util.h.d
@pchdir@/jbay/test_util.h.gch : @top_srcdir@/include/utests/test_util.h Makefile
	mkdir -p @pchdir@/jbay
	$(CXX) -MMD -x c++-header $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(TEST_UTIL_CPPFLAGS) $(CXXFLAGS) $(libjbaytest_a_CPPFLAGS) $(extra_CFLAGS_utests_utests) -c $< -o $@
@pchdir@/jbay/test_util.h : Makefile @pchdir@/jbay/test_util.h.gch
	printf '#error "Precompiled header for %s not found"' $@ > $@

JBAY_BUILT_SOURCES = $(jbay_precompiled_headers)

BUILT_SOURCES += $(JBAY_BUILT_SOURCES)
check_LIBRARIES += libjbaytest.a
check_LIBRARIES += wrapper/libjbaywrapper.a
utests_utests_CPPFLAGS += -DMODEL_JBAY
utests_utests_LDADD += libjbaytest.a
utests_utests_LDFLAGS += -Wl,${FORCE_LOAD},libjbaytest.a,${NO_FORCE_LOAD}
utests_utests_LDADD += $(builddir)/../libjbay.a
utests_utests_LDADD += wrapper/libjbaywrapper.a
TEST_READER_OBJECTS += utests/libjbaytest_a-test_reader.o
endif

#### jbayB0 ####
JBAYB0_CPPFLAGS  = -I$(srcdir)/../include
JBAYB0_CPPFLAGS += -I$(srcdir)/../include/wrapper
JBAYB0_CPPFLAGS += -I$(srcdir)/../include/jbayB0
JBAYB0_CPPFLAGS += -I$(srcdir)/../include/jbayB0/dummy_registers
JBAYB0_CPPFLAGS += -I$(srcdir)/../include/jbayXX
JBAYB0_CPPFLAGS += -I$(srcdir)/../include/jbay_shared
JBAYB0_CPPFLAGS += -I$(srcdir)/../include/tofinoXX_jbay
JBAYB0_CPPFLAGS += -I$(srcdir)/../include/shared
JBAYB0_CPPFLAGS += -I$(srcdir)/../include/model_core
JBAYB0_CPPFLAGS += $(AM_CPPFLAGS)
JBAYB0_CPPFLAGS += -DMODEL_JBAYB0
JBAYB0_CPPFLAGS += -DMODEL_CHIP_NAMESPACE=jbayB0

libjbayB0test_a_CPPFLAGS = -I@pchdir@  -I@pchdir@/jbayB0
libjbayB0test_a_CPPFLAGS += $(JBAYB0_CPPFLAGS)
libjbayB0test_a_CPPFLAGS += $(UTESTS_CPPFLAGS)
libjbayB0test_a_SOURCES = $(JBAYB0_UTEST_SOURCES)

if MODEL_JBAYB0
jbayB0_precompiled_headers = \
@pchdir@/jbayB0/test_util.h.gch \
@pchdir@/jbayB0/test_util.h

-include @pchdir@/jbayB0/test_util.h.d
@pchdir@/jbayB0/test_util.h.gch : @top_srcdir@/include/utests/test_util.h Makefile
	mkdir -p @pchdir@/jbayB0
	$(CXX) -MMD -x c++-header $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(TEST_UTIL_CPPFLAGS) $(CXXFLAGS) $(libjbayB0test_a_CPPFLAGS) $(extra_CFLAGS_utests_utests) -c $< -o $@
@pchdir@/jbayB0/test_util.h : Makefile @pchdir@/jbayB0/test_util.h.gch
	printf '#error "Precompiled header for %s not found"' $@ > $@

JBAYB0_BUILT_SOURCES = $(jbayB0_precompiled_headers)

BUILT_SOURCES += $(JBAYB0_BUILT_SOURCES)
check_LIBRARIES += libjbayB0test.a
check_LIBRARIES += wrapper/libjbayB0wrapper.a
utests_utests_CPPFLAGS += -DMODEL_JBAYB0
utests_utests_LDADD += libjbayB0test.a
utests_utests_LDFLAGS += -Wl,${FORCE_LOAD},libjbayB0test.a,${NO_FORCE_LOAD}
utests_utests_LDADD += $(builddir)/../libjbayB0.a
utests_utests_LDADD += wrapper/libjbayB0wrapper.a
TEST_READER_OBJECTS += utests/libjbayB0test_a-test_reader.o
endif


## CHIP SPECIFIC CONTENT for utests ##

utests_utests_LDADD += $(builddir)/../libmodelcore.a
utests_utests_LDADD += $(BOOST_LDFLAGS) $(BOOST_FILESYSTEM_LIB) $(BOOST_SYSTEM_LIB)
utests_utests_LDADD += $(UTESTS_LDADD)

# Rebuild utest.o whenever config.status changes as could be
# a new chip has been added via configure
@srcdir@/utests/utest.cpp: @top_builddir@/config.status
	touch $@

#### test_reader ####
utests_test_test_reader_CPPFLAGS  = -O3  $(AM_CPPFLAGS)
utests_test_test_reader_CPPFLAGS += -I @top_srcdir@/include/shared
utests_test_test_reader_CPPFLAGS += -I @top_srcdir@/include/utests
#utests_test_test_reader_CPPFLAGS += -ftemplate-depth=1000
utests_test_test_reader_LDADD    = $(BOOST_LDFLAGS) $(BOOST_FILESYSTEM_LIB) $(BOOST_SYSTEM_LIB)
utests_test_test_reader_SOURCES     = utests/test_test_reader.cpp

# target to build only the test_reader.cpp for each configured chip using -O3
test_reader_O3: $(TEST_READER_OBJECTS)
test_reader_O3: CXXFLAGS = -O3 -std=c++11

check_PROGRAMS = utests/utests utests/test_test_reader


###############################################################################
#
# Wrapper
#
###############################################################################

WRAPPER_COMMON_SOURCES  = wrapper/dprsr_meta_data_enum.hpp
WRAPPER_COMMON_SOURCES += wrapper/main.cpp
WRAPPER_COMMON_SOURCES += wrapper/ref_model_dpi.cpp
WRAPPER_COMMON_SOURCES += wrapper/ref_model_dpi.hpp
WRAPPER_COMMON_SOURCES += wrapper/ref_model_wrapper.cpp
WRAPPER_COMMON_SOURCES += wrapper/ref_model_wrapper.hpp
WRAPPER_COMMON_SOURCES += ../include/wrapper/sknobs.h

## CHIP SPECIFIC CONTENT for wrapper ##
#### tofino ####
wrapper_libtofinowrapper_a_SOURCES  = $(WRAPPER_COMMON_SOURCES)
wrapper_libtofinowrapper_a_CPPFLAGS = $(TOFINO_CPPFLAGS)
wrapper_libtofinowrapper_a_CPPFLAGS += -I$(srcdir)/utests
wrapper_libtofinowrapper_a_CPPFLAGS += -DINCLUDED_FROM_TEST_WRAPPER

#### tofinoB0 ####
wrapper_libtofinoB0wrapper_a_SOURCES = $(WRAPPER_COMMON_SOURCES)
wrapper_libtofinoB0wrapper_a_CPPFLAGS  = $(TOFINOB0_CPPFLAGS)
wrapper_libtofinoB0wrapper_a_CPPFLAGS += -I$(srcdir)/utests
wrapper_libtofinoB0wrapper_a_CPPFLAGS += -DINCLUDED_FROM_TEST_WRAPPER

#### jbay ####
wrapper_libjbaywrapper_a_SOURCES = $(WRAPPER_COMMON_SOURCES)
wrapper_libjbaywrapper_a_CPPFLAGS  = $(JBAY_CPPFLAGS)
wrapper_libjbaywrapper_a_CPPFLAGS += -I$(srcdir)/utests
wrapper_libjbaywrapper_a_CPPFLAGS += -DINCLUDED_FROM_TEST_WRAPPER

#### jbayB0 ####
wrapper_libjbayB0wrapper_a_SOURCES = $(WRAPPER_COMMON_SOURCES)
wrapper_libjbayB0wrapper_a_CPPFLAGS  = $(JBAYB0_CPPFLAGS)
wrapper_libjbayB0wrapper_a_CPPFLAGS += -I$(srcdir)/utests
wrapper_libjbayB0wrapper_a_CPPFLAGS += -DINCLUDED_FROM_TEST_WRAPPER


## END OF CHIP SPECIFIC CONTENT for wrapper ##


#### lex, yacc ####
utests/lex.meter.c : $(srcdir)/utests/meterparser.lex @builddir@/utests/meterparseryacc.h
	lex --outfile=$@ $?
utests/lex.lpf.c : $(srcdir)/utests/lpfparser.lex @builddir@/utests/lpfparseryacc.h
	lex --outfile=$@ $?
utests/meterparseryacc.c : $(srcdir)/utests/meterparser.y
	yacc -d -p meter -o $@ $(srcdir)/utests/meterparser.y
utests/lpfparseryacc.c : $(srcdir)/utests/lpfparser.y
	yacc -d -p lpf -o $@ $(srcdir)/utests/lpfparser.y
# The two rules below are needed for to force serialization of the .c and .h
# files. Otherwise, builds with "-jN" option will call yacc multiple times.
@builddir@/utests/meterparseryacc.h : @builddir@/utests/meterparseryacc.c
@builddir@/utests/lpfparseryacc.h : @builddir@/utests/lpfparseryacc.c



CLEANFILES = $(EXTRA_BUILT_SOURCES)

clean-local:
	-rm -rf @pchdir@
