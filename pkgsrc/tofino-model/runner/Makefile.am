ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS} -I m4
bin_PROGRAMS = tofino-model.debug tofino-model

tofino_model_debug_SOURCES = \
bmi_int.h \
bmi_interface.c \
bmi_interface.h \
bmi_port.c \
bmi_port.h \
cli.cpp \
cli.h \
lld_intf.c \
main.cpp \
os_privs.cpp \
os_privs.h \
portmanager.c \
portmanager.h \
simulator_intf.c \
simulator_intf.h

tofino_model_debug_CPPFLAGS  = -I$(srcdir)/../include
tofino_model_debug_CPPFLAGS += -I$(srcdir)/../include/shared
tofino_model_debug_CPPFLAGS += -I$(srcdir)/../include/model_core
tofino_model_debug_LDFLAGS =
tofino_model_debug_LDADD = $(builddir)/../libmodelcore.a

# Hardening as requested in SDLe
tofino_model_debug_CPPFLAGS += -D_FORTIFY_SOURCE=2 -fstack-protector-strong -Wl,-z,relro -Wl,-z,now

if MODEL_TOFINOB0
tofino_model_debug_CPPFLAGS += -DMODEL_TOFINOB0
tofino_model_debug_LDADD += $(builddir)/../libtofinoB0.a
endif
if MODEL_TOFINO
tofino_model_debug_CPPFLAGS += -DMODEL_TOFINO
tofino_model_debug_LDADD += $(builddir)/../libtofino.a
endif
if MODEL_JBAYB0
tofino_model_debug_CPPFLAGS += -DMODEL_JBAYB0
tofino_model_debug_LDADD += $(builddir)/../libjbayB0.a
endif
if MODEL_JBAY
tofino_model_debug_CPPFLAGS += -DMODEL_JBAY
tofino_model_debug_LDADD += $(builddir)/../libjbay.a
endif

tofino_model_debug_CPPFLAGS += -DLIBPCAP_USE_FIX
tofino_model_debug_CPPFLAGS += $(AM_CPPFLAGS)
tofino_model_debug_LDFLAGS += $(RUNNER_LDFLAGS)
tofino_model_debug_LDADD += $(builddir)/../libmodelcore.a
# libmodel is provided by bf-drivers
tofino_model_debug_LDADD += -lmodel $(RUNNER_LDADD)

# Once tofino-model.debug is built, use objcopy tool to
# create a stripped binary called tofino-model.
# (This stripped binary will be the *only* binary supplied to customers)
#
# Stripping process involves:
# 1. Stripping all debug tokens,
# 2. Linking stripped binary to tofino-model.debug binary so debugging still possible
#    (on systems where tofino-model.debug binary made available),
# 3. Stripping tokens listed in strip-symbols.txt file (typically CSR tokens).
#
tofino-model$(EXEEXT): tofino-model.debug$(EXEEXT) $(srcdir)/strip-symbols.txt
	objcopy --strip-debug --add-gnu-debuglink=$< --wildcard --strip-symbols=$(srcdir)/strip-symbols.txt $< $@
