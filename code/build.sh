#!/bin/bash
mkdir -p ../build
pushd ../build
CommonCompilerFlags=(-Wall -Werror -Wl,-rpath,'$ORIGIN'
		     -Wno-unused-function -Wno-write-strings -Wno-unused-variable -g -Wno-null-dereference
		    -Wno-unused-but-set-variable)
CommonLinkerFlags=()

gcc ${CommonCompilerFlags[*]} ../code/db.c -o db ${CommonLinkerFlags[*]}

popd

