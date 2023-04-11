#!/bin/bash
set -e

readonly SCRIPT_PATH=$(realpath "$0")
readonly SCRIPT_DIR=$(dirname "${SCRIPT_PATH}")
export readonly PROJECT_DIR=$(dirname "${SCRIPT_DIR}")

cd ${SCRIPT_DIR}

export PYTHONPATH=${PROJECT_DIR}/build/jupyter/cpp/
export PATH=${HOME}/.local/bin:${PATH}
if ! command -v jupyter &>/dev/null; then
    echo "jupyter not installed"
    python3 -m pip install jupyter
fi

function check_pip_install {
    package_name=$1
    if ! python3 -m pip list 2>/dev/null | grep ${package_name} >/dev/null 2>&1; then
        echo "${package_name} not installed"
        python3 -m pip install ${package_name}
    fi
}

check_pip_install bokeh
check_pip_install jupyter-server-proxy

# https://stackoverflow.com/a/72747002
# May need to Downgrade markupsafe to 2.0.1
# pip install markupsafe==2.0.1
jupyter notebook --port 52001 .
