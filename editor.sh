#! /bin/bash
mkdir -p code
mkdir -p build
pushd code > /dev/null
emacs -q -l ../misc/init.el & > /dev/null
popd > /dev/null
cd build
