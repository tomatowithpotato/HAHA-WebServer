#!/bin/bash

# cd ./tests
# make httpServer

cd ./build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make