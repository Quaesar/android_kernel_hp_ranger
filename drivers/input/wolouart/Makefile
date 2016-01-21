#
# Makefile for the input core drivers.
#

# Each configuration option enables a list of files.

ifeq ($(EN_WOLONG_SC),1)
ccflags-y := -DBUILD_WOLONG_SOURCE_KERNEL
obj-$(CONFIG_SERIO_WOLOUART)	+= wolouartdrv.o
else
obj-$(CONFIG_SERIO_WOLOUART) += wolouartdrv_pre.o
endif

obj-$(CONFIG_SERIO_WOLOUART)	+= wolouartdrv_ctrl.o

