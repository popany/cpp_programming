#!/bin/bash

readonly SCRIPT_PATH=$(realpath "$0")
readonly SCRIPT_DIR=$(dirname "${SCRIPT_PATH}")
readonly PROJECT_DIR=${SCRIPT_DIR}

target_dirs=('core' 'jupyter')
for dir in ${target_dirs[@]}; do
    find "${PROJECT_DIR}/${dir}" \( -name *.h -o -name *.cc \) -exec clang-format -i {} \;
done
