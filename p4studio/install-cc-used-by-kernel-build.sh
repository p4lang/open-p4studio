#! /bin/bash

# Install the package containing the version of gcc used to build
# the kernel.

# On some Ubuntu 22.04 systems, its kernel was built using a different
# version of gcc than the one installed by the package named "gcc".
# If the version of gcc used to build the kernel is not installed, building
# several kernel-related components in bf-driver will fail.

KVER=`uname -r`
KDIR="/lib/modules/${KVER}/build"
if [ ! -d "${KDIR}" ]
then
    1>&2 echo "No directory ${KDIR}.  Aborting."
    exit 1
fi

cd "${KDIR}"
eval `make --dry-run --print-data-base 2>/dev/null | grep '^HOSTCC' | head -n 1 | sed 's/ //g'`
sudo apt-get install -y ${HOSTCC}
