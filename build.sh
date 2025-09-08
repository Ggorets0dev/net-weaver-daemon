#!/bin/bash

build_dir="build_proj"

if [ -d $build_dir ]; then
	rm -rf build_proj
fi

mkdir $build_dir
cd $build_dir
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
make install
