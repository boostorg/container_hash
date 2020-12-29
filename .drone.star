# Use, modification, and distribution are
# subject to the Boost Software License, Version 1.0. (See accompanying
# file LICENSE.txt)
#
# Copyright Rene Rivera 2020.

# For Drone CI we use the Starlark scripting language to reduce duplication.
# As the yaml syntax for Drone CI is rather limited.
#
#
globalenv={}
linuxglobalimage="cppalliance/droneubuntu1404:1"
windowsglobalimage="cppalliance/dronevs2019"

def main(ctx):
  return [
  linux_cxx("USER_CONFIG=using gcc : : g++-4.8 ; CXXSTD= Job 0", "g++", packages="", buildtype="boost", buildscript="drone", image=linuxglobalimage, environment={'USER_CONFIG': 'using gcc : : g++-4.8 ;', 'CXXSTD': '03,11', 'DRONE_JOB_UUID': 'b6589fc6ab'}, globalenv=globalenv),
  linux_cxx("USER_CONFIG=using gcc : : g++-7 ; CXXSTD=11 Job 1", "g++-7", packages="g++-7", buildtype="boost", buildscript="drone", image=linuxglobalimage, environment={'USER_CONFIG': 'using gcc : : g++-7 ;', 'CXXSTD': '11,14,17', 'DRONE_JOB_UUID': '356a192b79'}, globalenv=globalenv),
  linux_cxx("USER_CONFIG=using clang : : clang++ ; CXXST Job 2", "clang++", packages="", buildtype="boost", buildscript="drone", image=linuxglobalimage, environment={'USER_CONFIG': 'using clang : : clang++ ;', 'CXXSTD': '03,11', 'DRONE_JOB_UUID': 'da4b9237ba'}, globalenv=globalenv),
  linux_cxx("USER_CONFIG=using clang : : clang++ -D_HAS_A Job 3", "clang++", packages="", buildtype="boost", buildscript="drone", image=linuxglobalimage, environment={'USER_CONFIG': 'using clang : : clang++ -D_HAS_AUTO_PTR_ETC=0 ;', 'CXXSTD': '11', 'DRONE_JOB_UUID': '77de68daec'}, globalenv=globalenv),
    ]

# from https://github.com/boostorg/boost-ci
load("@boost_ci//ci/drone/:functions.star", "linux_cxx","windows_cxx","osx_cxx","freebsd_cxx")
