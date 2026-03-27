#!/bin/bash

export MDC_SDK=/usr/local/mdc_sdk_llvm/dp_gea/mdc_cross_compiler
export PATH=$MDC_SDK/bin:$PATH
export CC=clang     CXX=clang++  
export TARGET=$(clang -dumpmachine)           
export SYSROOT=$MDC_SDK/sysroot               
export CFLAGS="--sysroot=$SYSROOT -O2 -g"
export CXXFLAGS="$CFLAGS"
export LDFLAGS="--sysroot=$SYSROOT"


function ClangCMake() {
  
  mkdir build && cd build
  cmake -DCMAKE_BUILD_TYPE=Release \
      -DBUILD_SHARED_LIBS=OFF \
      -DBUILD_TESTING=OFF \
      -DWITH_GFLAGS=OFF \
      -DCMAKE_INSTALL_PREFIX=$PWD/install \
      -DCMAKE_C_COMPILER=$CC \
      -DCMAKE_CXX_COMPILER=$CXX \
      -DCMAKE_C_FLAGS="$CFLAGS" \
      -DCMAKE_CXX_FLAGS="$CXXFLAGS" \
      -DCMAKE_EXE_LINKER_FLAGS="$LDFLAGS" \
     ..
}


function ClangMake() {
  make -j8 && make install
}


