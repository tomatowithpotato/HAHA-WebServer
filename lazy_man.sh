#!/bin/bash

#shell脚本不能有空格，卧槽

CUR_DIR=${pwd}

./build.sh

cd "${CUR_DIR}"

./run.sh