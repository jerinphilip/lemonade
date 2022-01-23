#!/bin/bash

sudo yum-config-manager -y --add-repo https://yum.repos.intel.com/mkl/setup/intel-mkl.repo
sudo yum install -y intel-mkl


