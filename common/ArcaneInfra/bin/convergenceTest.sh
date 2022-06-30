#!/bin/sh
TEST_NAME=$1
FULL_TEST_NAME=$2
ROOT_DIR=$3
BUILD_DIR=$4
EXE=$5
CODE_NAME=$6
CONFIG_FILE=$7
TEST_DIR_XML=$8
TEST=$9
OUTPUT_DIR=${10}
NUM_PROC=${11}

LD_LIBRARY_PATH=${BUILD_DIR}/lib:$LD_LIBRARY_PATH

# same policy than in ArcaneTest.sh
if [ "${CONFIG_FILE}" == "CooresArcane" ]; then
  STDENV_PATH_SHR=${ROOT_DIR}/src/Coores
elif [ "${CONFIG_FILE}" == "ArcTem" ]; then
  # STDENV_PATH_SHR=${PWD}
  unset STDENV_PATH_SHR # embarqué dans le code main.cc
elif [ "${CONFIG_FILE}" == "Local" ]; then
  STDENV_PATH_SHR=${PWD}
elif [ "${CONFIG_FILE}" == "ArcGeoSim" ]; then
  STDENV_PATH_SHR=${ROOT_DIR}/common/ArcGeoSim/test/ServiceTests
elif [ "${CONFIG_FILE}" == "ArcaneDemo" ]; then
  STDENV_PATH_SHR=${PWD}
elif [ "${CONFIG_FILE}" == "ServiceTest" ]; then
  STDENV_PATH_SHR=${ROOT_DIR}/common/ArcGeoSim/test/ServiceTests
elif [ "${CONFIG_FILE}" == "ArcEOR" ]; then
  STDENV_PATH_SHR=${ROOT_DIR}/src/ArcEOR
else
  STDENV_PATH_SHR=${ROOT_DIR}/${CONFIG_FILE}
fi
export LD_LIBRARY_PATH STDENV_PATH_SHR 
ARCANE_PARALLEL_SERVICE=Mpi
export ARCANE_PARALLEL_SERVICE

SCRIPT=${ROOT_DIR}/common/ArcaneInfra/bin/convergence.pl
OUTFILE=${OUTPUT_DIR}/${FULL_TEST_NAME}

${SCRIPT} ${TEST_DIR_XML} ${TEST} --project-dir=${ROOT_DIR} --executable=${EXE} --num-proc=${NUM_PROC} --codename=${CODE_NAME} --output-dir=. > ${OUTFILE}
