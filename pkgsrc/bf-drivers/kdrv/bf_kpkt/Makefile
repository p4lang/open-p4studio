###########################################################################
#
# Local source generation targets.
#
###############################################################################

krn = bf_kpkt

ifneq ($(KERNELRELEASE),)

obj-m := $(krn).o
cmn_obj := ../../src/lld/lld_map.o ../../src/lld/lld.o ../../src/lld/lld_dev.o ../../src/lld/lld_dev_lock.o ../../src/lld/lld_dr.o ../../src/lld/lld_reg_if.o ../../src/lld/lld_subdev_reg_if.o ../../src/lld/lld_dr_if.o ../../src/lld/bf_dma_if.o ../../src/lld/bf_lld_if.o ../../src/lld/bf_int_if.o ../../src/lld/lld_interrupt.o ../../src/lld/lld_sku.o ../../src/lld/lld_efuse.o  ../../src/lld/lld_dr_regs.o
tf1_obj := ../../src/lld/lld_dev_tof.o  ../../src/lld/lld_dr_tof.o  ../../src/lld/lld_dr_regs_tof.o  ../../src/lld/lld_tof_interrupt.o  ../../src/lld/lld_efuse_tof.o
tf2_obj := ../../src/lld/lld_dev_tof2.o ../../src/lld/lld_dr_tof2.o ../../src/lld/lld_dr_regs_tof2.o ../../src/lld/lld_tof2_interrupt.o ../../src/lld/lld_efuse_tof2.o
tf3_obj := ../../src/lld/lld_dev_tof3.o ../../src/lld/lld_dr_tof3.o ../../src/lld/lld_dr_regs_tof3.o ../../src/lld/lld_tof3_interrupt.o ../../src/lld/lld_efuse_tof3.o


$(krn)-y := $(cmn_obj)
$(krn)-y += $(tf1_obj)
$(krn)-y += $(tf2_obj)
$(krn)-y += $(tf3_obj)

$(krn)-y +=  bf_kpkt_net.o bf_kpkt_txrx.o bf_kpkt_sysfs.o bf_kpkt_timer.o bf_kpkt_stubs.o ../bf_kdrv/bf_kdrv.o

ccflags-y := -I$(src)
ccflags-y += -I$(src)/../../include
ccflags-y += -I$(inc)
ccflags-y += -DBF_INCLUDE_KPKT -Werror
ccflags-y += $(call cc-option,-Wimplicit-fallthrough=2)

else

KDIR ?= /lib/modules/$$(uname -r)/build

all: default

default:
	            $(MAKE) -C $(KDIR) M=$$BUILD_DIR src=$$PWD inc=$$INCLUDE modules

clean:
	            $(MAKE) -C $(KDIR) M=$$BUILD_DIR src=$$PWD inc=$$include clean

.PHONY : clean
endif
