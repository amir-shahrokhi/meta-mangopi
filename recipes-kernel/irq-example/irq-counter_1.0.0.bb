SUMMARY = "Simple IRQ handle which counts GPIO interrupts"
DESCRIPTION = "${SUMMARY}"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

inherit module

SRC_URI = " \
        file://irq-counter.c \
        file://Makefile \
"

S = "${WORKDIR}"

RPROVIDES_${PN} += "kernel-module-irq-counter"

