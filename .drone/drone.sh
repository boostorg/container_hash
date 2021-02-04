#!/bin/bash

# Copyright 2020 Rene Rivera, Sam Darwin
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.txt or copy at http://boost.org/LICENSE_1_0.txt)

set -e
export TRAVIS_BUILD_DIR=$(pwd)
export DRONE_BUILD_DIR=$(pwd)
export TRAVIS_BRANCH=$DRONE_BRANCH
export VCS_COMMIT_ID=$DRONE_COMMIT
export GIT_COMMIT=$DRONE_COMMIT
export REPO_NAME=$DRONE_REPO
export USER=$(whoami)
export CC=${CC:-gcc}
export PATH=~/.local/bin:/usr/local/bin:$PATH

if [ "$DRONE_JOB_BUILDTYPE" == "boost" ]; then

echo '==================================> INSTALL'

export BOOST_VERSION=1.67.0
export BOOST_FILENAME=boost_1_67_0
export BOOST_ROOT=${HOME}/boost
cd ${TRAVIS_BUILD_DIR}
touch Jamroot.jam
cd $HOME
echo $USER_CONFIG > ~/user-config.jam
cat ~/user-config.jam
mkdir $HOME/download
mkdir $HOME/extract
cd $HOME/download
if [ "$TRAVIS_EVENT_TYPE" == "cron" ]
then
    if [ "$TRAVIS_BRANCH" == "master" ]
    then
        snapshot_branch=master
    else
        snapshot_branch=develop
    fi
    download_url=$(curl https://api.bintray.com/packages/boostorg/$snapshot_branch/snapshot/files |
            python -c "import os.path, sys, json; x = json.load(sys.stdin); print '\n'.join(a['path'] for a in x if os.path.splitext(a['path'])[1] == '.bz2')" |
            head -n 1 |
            sed "s/^/http:\/\/dl.bintray.com\/boostorg\/$snapshot_branch\//")
else
    download_url=https://sourceforge.net/projects/boost/files/boost/${BOOST_VERSION}/${BOOST_FILENAME}.tar.bz2/download
fi
echo "Downloading ${download_url}"
wget -O boost.tar.bz2 $download_url
cd $HOME/extract
tar -xjf $HOME/download/boost.tar.bz2
mv * ${BOOST_ROOT}

rm -r ${BOOST_ROOT}/boost/functional
cd ${BOOST_ROOT}/tools/build
mkdir ${HOME}/opt
./bootstrap.sh
./b2 install --prefix=$HOME/opt

echo '==================================> SCRIPT'

cd ${TRAVIS_BUILD_DIR}/test
${HOME}/opt/bin/b2 --verbose-test -j 3 cxxstd=$CXXSTD -q ${BJAM_TOOLSET} include=${BOOST_ROOT} include=${TRAVIS_BUILD_DIR}/include hash_info
${HOME}/opt/bin/b2 -j 3 cxxstd=$CXXSTD -q ${BJAM_TOOLSET} include=${BOOST_ROOT} include=${TRAVIS_BUILD_DIR}/include

fi
