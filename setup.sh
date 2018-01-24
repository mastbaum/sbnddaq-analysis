#! /bin/sh

source /grid/fermiapp/products/larsoft/setup
source /grid/fermiapp/products/artdaq/setup

export LARSOFTOBJ_TARGET_VERSION=v1_26_00
export LARSOFTOBJ_TARGET_QUALS=e14:prof

echo "Setting up larsoftobj ${LARSOFTOBJ_TARGET_VERSION} -q ${LARSOFTOBJ_TARGET_QUALS}"

setup larsoftobj ${LARSOFTOBJ_TARGET_VERSION} -q ${LARSOFTOBJ_TARGET_QUALS}
ups active | grep "larsoftobj "

export SBNDDAQ_DATATYPES_TARGET_VERSION=v00_01_00
export SBNDDAQ_DATATYPES_TARGET_QUALS=e14:prof:s50
export SBNDDAQ_DATATYPES_LOCATION=~/sbnddaq/dev

echo "Setting up sbnddaq_datatypes ${SBNDDAQ_DATATYPES_TARGET_VERSION} -q ${SBNDDAQ_DATATYPES_TARGET_QUALS}"

echo "Setting up local area (with mrb)"
setup mrb
export MRB_PROJECT=artdaq
source ${SBNDDAQ_DATATYPES_LOCATION}/localProducts_artdaq_*/setup
setup sbnddaq_datatypes ${SBNDDAQ_DATATYPES_TARGET_VERSION} -q ${SBNDDAQ_DATATYPES_TARGET_QUALS}

export TRACE_MSGMAX=0
export TRACE_FILE=/run/user/${UID}/tbuffer
