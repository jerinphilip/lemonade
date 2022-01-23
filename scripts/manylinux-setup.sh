#!/bin/bash

set -eo pipefail

PACKAGES=$(echo ${PY_VERSIONS} | grep "[0-9]*" -o | sort | uniq | xargs -I% echo python%-devel)
yum install ${PACKAGES}

yum-config-manager -y --add-repo https://yum.repos.intel.com/mkl/setup/intel-mkl.repo
yum install -y intel-mkl



