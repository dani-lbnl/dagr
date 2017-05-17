#!/bin/bash
set -v

# setup repo with recent gcc versions
sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
sudo add-apt-repository -y ppa:teward/swig3.0
#sudo add-apt-repository -y ppa:george-edison55/cmake-3.x

# suck in package lists
sudo apt-get update -qq

# install deps
# use PIP for Python packages
sudo apt-get install -qq -y gcc-5 g++-5 gfortran-5 swig3.0 \
    libmpich-dev libboost-program-options-dev python-dev subversion

pip install --user numpy mpi4py matplotlib

# install cmake manually because repo/ppa versions are too old
wget https://cmake.org/files/v3.5/cmake-3.5.2-Linux-x86_64.tar.gz
sudo tar -C /usr -x -z -f cmake-3.5.2-Linux-x86_64.tar.gz --strip-components=1

# install data files.
#svn co svn://missmarple.lbl.gov/work3/dagr/DAGR_data DAGR_data
