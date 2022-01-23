#!/bin/bash

yum-config-manager -y --add-repo https://yum.repos.intel.com/mkl/setup/intel-mkl.repo
yum install -y intel-mkl

VERSION=$(python3 -c "import platform; print(platform.python_version().replace('.', '')[:-1])")
yum install python${VERSION}-devel


