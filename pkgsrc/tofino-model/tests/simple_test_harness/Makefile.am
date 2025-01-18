
SHARED_SOURCES  = bfrt.cpp
SHARED_SOURCES += bson.cpp
SHARED_SOURCES += input.cpp
SHARED_SOURCES += json.cpp
SHARED_SOURCES += log.cpp
SHARED_SOURCES += main.cpp
SHARED_SOURCES += p4name.cpp
SHARED_SOURCES += packet.cpp
SHARED_SOURCES += script.cpp
SHARED_SOURCES += sleep.cpp
SHARED_SOURCES += table.cpp
SHARED_SOURCES += table_config.cpp
SHARED_SOURCES += write.cpp

# Need to configure and link with all libs for all configured chips, even though
# the harness will only run with one.
LIBS_NEEDED  = $(top_builddir)/libmodelcore.a
if MODEL_TOFINO
LIBS_NEEDED += $(top_builddir)/libtofino.a
endif
if MODEL_TOFINOB0
LIBS_NEEDED += $(top_builddir)/libtofinoB0.a
endif
if MODEL_JBAY
LIBS_NEEDED += $(top_builddir)/libjbay.a
endif
if MODEL_JBAYB0
LIBS_NEEDED += $(top_builddir)/libjbayB0.a
endif
LIBS_NEEDED += $(top_builddir)/libmodelcore.a

TEST_HARNESSES =
if MODEL_TOFINO
if MODEL_TOFINOB0
# If both TOFINO and TOFINOB0 are enabled, we build tofino as tofinoA0
tofinoA0_test_harness_SOURCES = $(SHARED_SOURCES)
tofinoA0_test_harness_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/include/tofino \
	-I$(top_srcdir)/include/tofino/dummy_registers \
	-I$(top_srcdir)/include/tofinoXX \
	-I$(top_srcdir)/include/tofinoXX_jbay \
	-I$(top_srcdir)/include/shared \
	-I$(top_srcdir)/include/tofino/register_includes \
	-I$(top_srcdir)/include/model_core \
	$(AM_CPPFLAGS) \
	-DMODEL_CHIP_NAMESPACE=tofino
tofinoA0_test_harness_LDADD = $(LIBS_NEEDED) -ldl $(SIMPLE_TEST_HARNESS_LDADD)
TEST_HARNESSES += tofinoA0_test_harness
else
tofino_test_harness_SOURCES = $(SHARED_SOURCES)
tofino_test_harness_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/include/tofino \
	-I$(top_srcdir)/include/tofino/dummy_registers \
	-I$(top_srcdir)/include/tofinoXX \
	-I$(top_srcdir)/include/tofinoXX_jbay \
	-I$(top_srcdir)/include/shared \
	-I$(top_srcdir)/include/tofino/register_includes \
	-I$(top_srcdir)/include/model_core \
	$(AM_CPPFLAGS) \
	-DMODEL_CHIP_NAMESPACE=tofino
tofino_test_harness_LDADD = $(LIBS_NEEDED) -ldl $(SIMPLE_TEST_HARNESS_LDADD)
TEST_HARNESSES += tofino_test_harness
endif
endif

if MODEL_TOFINOB0
tofino_test_harness_SOURCES = $(SHARED_SOURCES)
tofino_test_harness_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/include/tofinoB0 \
	-I$(top_srcdir)/include/tofinoB0/dummy_registers \
	-I$(top_srcdir)/include/tofinoXX \
	-I$(top_srcdir)/include/tofinoXX_jbay \
	-I$(top_srcdir)/include/shared \
	-I$(top_srcdir)/include/tofinoB0/register_includes \
	-I$(top_srcdir)/include/model_core \
	$(AM_CPPFLAGS) \
	-DMODEL_CHIP_NAMESPACE=tofinoB0
tofino_test_harness_LDADD = $(LIBS_NEEDED) -ldl $(SIMPLE_TEST_HARNESS_LDADD)
TEST_HARNESSES += tofino_test_harness
endif

if MODEL_JBAY
if MODEL_JBAYB0
# If both JBAY and JBAYB0 are enabled, we build jbay as jbayA0
jbayA0_test_harness_SOURCES = $(SHARED_SOURCES)
jbayA0_test_harness_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/include/jbay \
	-I$(top_srcdir)/include/jbay/dummy_registers \
	-I$(top_srcdir)/include/jbayXX \
	-I$(top_srcdir)/include/jbay_shared \
	-I$(top_srcdir)/include/tofinoXX_jbay \
	-I$(top_srcdir)/include/shared \
	-I$(top_srcdir)/include/jbay/register_includes \
	-I$(top_srcdir)/include/model_core \
	$(AM_CPPFLAGS) \
	-DMODEL_CHIP_NAMESPACE=jbay
jbayA0_test_harness_LDADD = $(LIBS_NEEDED) -ldl $(SIMPLE_TEST_HARNESS_LDADD)
TEST_HARNESSES += jbayA0_test_harness
else
jbay_test_harness_SOURCES = $(SHARED_SOURCES)
jbay_test_harness_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/include/jbay \
	-I$(top_srcdir)/include/jbay/dummy_registers \
	-I$(top_srcdir)/include/jbayXX \
	-I$(top_srcdir)/include/jbay_shared \
	-I$(top_srcdir)/include/tofinoXX_jbay \
	-I$(top_srcdir)/include/shared \
	-I$(top_srcdir)/include/jbay/register_includes \
	-I$(top_srcdir)/include/model_core \
	$(AM_CPPFLAGS) \
	-DMODEL_CHIP_NAMESPACE=jbay
jbay_test_harness_LDADD = $(LIBS_NEEDED) -ldl $(SIMPLE_TEST_HARNESS_LDADD)
TEST_HARNESSES += jbay_test_harness
endif
endif

if MODEL_JBAYB0
jbay_test_harness_SOURCES = $(SHARED_SOURCES)
jbay_test_harness_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/include/jbayB0 \
	-I$(top_srcdir)/include/jbayB0/dummy_registers \
	-I$(top_srcdir)/include/jbayXX \
	-I$(top_srcdir)/include/jbay_shared \
	-I$(top_srcdir)/include/tofinoXX_jbay \
	-I$(top_srcdir)/include/shared \
	-I$(top_srcdir)/include/jbayB0/register_includes \
	-I$(top_srcdir)/include/model_core \
	$(AM_CPPFLAGS) \
	-DMODEL_CHIP_NAMESPACE=jbayB0
jbay_test_harness_LDADD = $(LIBS_NEEDED) -ldl $(SIMPLE_TEST_HARNESS_LDADD)
TEST_HARNESSES += jbay_test_harness
endif


bin_PROGRAMS = $(TEST_HARNESSES)
