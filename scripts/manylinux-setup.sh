#!/bin/bash

set -eo pipefail;
set -x;

yum install python*-devel -y
yum install -y pybind11-devel

yum-config-manager -y --add-repo https://yum.repos.intel.com/mkl/setup/intel-mkl.repo
yum install -y intel-mkl



