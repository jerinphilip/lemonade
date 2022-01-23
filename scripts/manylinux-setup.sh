#!/bin/bash

set -eo pipefail;
set -x;

yum install python*-devel -y

yum-config-manager -y --add-repo https://yum.repos.intel.com/mkl/setup/intel-mkl.repo
yum install -y intel-mkl



