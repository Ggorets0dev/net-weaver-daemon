#!/bin/bash

build_dir="build_proj"

if [ ! -d $build_dir ]; then
	mkdir $build_dir
fi

cd $build_dir
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
make install
