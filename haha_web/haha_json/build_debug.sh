#!/bin/bash

# cd ./tests
# make httpServer

if [ ! -d "build" ]; then
        mkdir -p build
fi

cd ./build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make