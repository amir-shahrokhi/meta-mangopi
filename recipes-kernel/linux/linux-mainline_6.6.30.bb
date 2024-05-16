DESCRIPTION = "Linux Kernel from Tarball"
SECTION = "kernel"
LICENSE = "GPLv2"

inherit kernel
require recipes-kernel/linux/linux-yocto.inc

LIC_FILES_CHKSUM = "file://COPYING;md5=6bc538ed5bd9a7fc9398086aedcd7e46"

# file://002-add-mangopi-dual-dtb.patch
SRC_URI = "\
    https://cdn.kernel.org/pub/linux/kernel/v6.x/linux-${PV}.tar.xz \
    file://001-second_core_support_in_platsmp.patch \
    file://002-add-mangopi-dual-dtb-6.6.patch \
    file://defconfig \
    "

SRC_URI[sha256sum] = "b66a5b863b0f8669448b74ca83bd641a856f164b29956e539bbcb5fdeeab9cc6"

LINUX_VERSION ?= "${PV}"
LINUX_VERSION_EXTENSION_append = "-custom"

S = "${WORKDIR}/linux-${PV}"
COMPATIBLE_MACHINE = "sun8i"
