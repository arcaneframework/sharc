#!/bin/sh

function check_parameters {
    echo "Check parameters:"
    echo "     test name = ${TEST_NAME}"
    echo "full test name = ${FULL_TEST_NAME}"
    echo "      root dir = ${ROOT_DIR}"
    echo "     build dir = ${BUILD_DIR}"
    echo "           exe = ${EXE}"
    echo "     code name = ${CODE_NAME}"
    echo "   config file = ${CONFIG_FILE}"
    echo "          case = ${CASE}"
    echo "    output dir = ${OUTPUT_DIR}"
    echo "           mpi = ${MPI} [description: ${MPI_DESCRIPTION}]"
    echo "     num procs = ${NUM_PROC}"
    echo "       restart = ${RESTART}"
}

TEST_NAME=$1
FULL_TEST_NAME=$2
ROOT_DIR=$3
BUILD_DIR=$4
EXE=$5
CODE_NAME=$6
CONFIG_FILE=$7
CASE=$8
OUTPUT_DIR=$9
MPI=${10}
NUM_PROC=${11}
RESTART=${12}

case "$MPI" in
    "Mpi")
        MPI_DESCRIPTION="Parallel Mpi mode"
        ARCANE_PARALLEL_SERVICE=${MPI}
        export ARCANE_PARALLEL_SERVICE
        ;;
    "Seq")
        MPI_DESCRIPTION="Sequential mode"
        ;;
    "Embedded")
        MPI_DESCRIPTION="External launcher"
        ;;
    *)
        echo "-------------------------------------------------------------"
        >&2 echo "Bad MPI mode with value ${MPI} [expected: Mpi | Seq | Embbeded]"
        exit 3
        ;;       
esac

#check_parameters

# Supports a ARCGEOSIM_CTEST_OUTPUT_MODE environment variable where
# - not defined : is equivalent to value FILE
# - FILE        : code output are only in a test file
# - TEE         : code output are in a test file and stdout

export COVERITY_TEST_NAME=$FULL_TEST_NAME
export COVERITY_SUITE_NAME=${CODE_NAME}

if [ "$OS" == "Windows_NT" ]; then
  #echo "ArcaneTest.sh launched from cygwin sh"
  #check_parameters
    export PATH=/bin:/usr/bin:/usr/local/bin:$PATH
fi

if [ "${MPI}" != "Embedded" ]; then
    # same policy than in ConvergenceTest.sh
    if [ "${CONFIG_FILE}" == "CooresArcane" ]; then
        STDENV_PATH_SHR=${ROOT_DIR}/src/Coores
    elif [ "${CONFIG_FILE}" == "ArcaneDemo" ]; then
        STDENV_PATH_SHR=${PWD}
    elif [ "${CONFIG_FILE}" == "ArcEOR" ]; then
        STDENV_PATH_SHR=${ROOT_DIR}/src/ArcEOR
    elif [ "${CONFIG_FILE}" == "Local" ]; then
        STDENV_PATH_SHR=${PWD}
    elif [ "${CONFIG_FILE}" == "ArcGeoSim" ]; then
        STDENV_PATH_SHR=${ROOT_DIR}/common/ArcGeoSim/test/ServiceTests
    elif [ "${CONFIG_FILE}" == "ArcGeoPhy" ]; then
        STDENV_PATH_SHR=${ROOT_DIR}/common/ArcGeoPhy/test/ServiceTests
     elif [ "${CONFIG_FILE}" == "SharedUtils" ]; then
        STDENV_PATH_SHR=${ROOT_DIR}/common/SharedUtils/test/ServiceTests    
    elif [ "${CONFIG_FILE}" == "ServiceTest" ]; then
        STDENV_PATH_SHR=${ROOT_DIR}/common/ArcGeoSim/test/ServiceTests
    else # ArcaDES, ArcTem, FastArcaneTest, path embedded in main.cc
        unset STDENV_PATH_SHR
    fi

    export STDENV_PATH_SHR

    if [ "OS" != "Windows_NT" ]; then
        LD_LIBRARY_PATH=${BUILD_DIR}/lib:$LD_LIBRARY_PATH
        export LD_LIBRARY_PATH
    fi
fi

OUTFILE=${OUTPUT_DIR}/${FULL_TEST_NAME}
if [ "$OS" == "Windows_NT" ]; then
  #echo "OUTFILE=${OUTFILE}"
    OUTFILE=`cygpath $OUTFILE`
  #echo "OUTFILE=${OUTFILE}"
  #echo "ROOT_DIR=${ROOT_DIR}"
    ROOT_DIR=`cygpath ${ROOT_DIR}`
  #echo "ROOT_DIR=${ROOT_DIR}"
  #echo "BUILD_DIR=${BUILD_DIR}"
    BUILD_DIR=`cygpath ${BUILD_DIR}`
  #echo "BUILD_DIR=${BUILD_DIR}"
fi

# Empty log file
echo -n > ${OUTFILE}

# Default redirection
REDIRECT="cat >>${OUTFILE}"
if [ -n "${ARCGEOSIM_CTEST_OUTPUT_MODE}" ]; then
    case "${ARCGEOSIM_CTEST_OUTPUT_MODE}" in
        TEE)
            REDIRECT="tee -a ${OUTFILE}"
            ;;
        FILE)
            REDIRECT="cat >>${OUTFILE}"
            ;;
        URL=*)
        _URL=`echo ${ARCGEOSIM_CTEST_OUTPUT_MODE} | sed "s,^URL=\(.*\),\1,"`  
        REDIRECT="tee -a ${OUTFILE}"
        echo  "Full Output is available at url ${_URL}/${FULL_TEST_NAME}/*view*/"
        ;;
        *)
            echo "-------------------------------------------------------------"
            >&2 echo "Bad ARCGEOSIM_CTEST_OUTPUT_MODE with value ${ARCGEOSIM_CTEST_OUTPUT_MODE}"
            exit 3
            ;;       
    esac
fi

echo "Environment infos:"
echo "  STDENV_PATH_SHR=${STDENV_PATH_SHR}"
echo "  OUTFILE=${OUTFILE}"
echo "  ROOT_DIR=${ROOT_DIR}"
echo "  BUILD_DIR=${BUILD_DIR}"
echo "  PWD="`pwd`

# Opening log file
date 2>&1 | eval $REDIRECT

AUTHOR_COMMAND="${ROOT_DIR}/common/ArcaneInfra/bin/FindTestAuthor.pl ${ROOT_DIR} ${TEST_NAME} 2>&1"
#echo "Author Command : ${AUTHOR_COMMAND}" 2>&1 | eval $REDIRECT
AUTHOR=`${AUTHOR_COMMAND}`
AUTHORCODE=$?
if [ "$AUTHORCODE" == "0" ]; then
    echo "Author of test ${TEST_NAME} is $AUTHOR" 2>&1 | eval $REDIRECT
elif [ "$AUTHORCODE" == "2" ]; then 
    echo "Illegal test name ${TEST_NAME}" 2>&1 | eval $REDIRECT
    echo "It must be shorter than 50 chars" 2>&1 | eval $REDIRECT
    exit 2
elif [ "$AUTHORCODE" == "1" ]; then 
    echo "Cannot find author for test ${TEST_NAME} : test failed" 2>&1 | eval $REDIRECT
    echo "Authors must be registered in Authors.txt file" 2>&1 | eval $REDIRECT
    exit 1
else
    echo "Unknown error [$AUTHORCODE] while processing ${TEST_NAME} test author check" 2>&1 | eval $REDIRECT
    exit 1
fi

# appel du script de lancement
echo "Launching with ${NUM_PROC} proc(s) [${MPI_DESCRIPTION}]"
echo "-------------------------------------------------------------"

if [ "${RESTART}" != "0" ]; then
    echo "Mode restart : Checkpoint pass with ${RESTART} iteration(s)"
    echo "-------------------------------------------------------------"
    
    ${ROOT_DIR}/common/ArcaneInfra/bin/run.pl ${CASE} --executable "${EXE}" --num-proc ${NUM_PROC} --parallel "${MPI}" --codename ${CODE_NAME} --options " -arcane_opt max_iteration ${RESTART}" 2>&1 | eval $REDIRECT

    echo "Mode restart : Restart pass"
    echo "-------------------------------------------------------------"
    
    ${ROOT_DIR}/common/ArcaneInfra/bin/run.pl ${CASE} --executable "${EXE}" --num-proc ${NUM_PROC} --parallel "${MPI}" --codename ${CODE_NAME} --options " -arcane_opt continue" 2>&1 | eval $REDIRECT
    
else
    ${ROOT_DIR}/common/ArcaneInfra/bin/run.pl ${CASE} --executable "${EXE}" --num-proc ${NUM_PROC} --parallel "${MPI}" --codename ${CODE_NAME} 2>&1 | eval $REDIRECT
fi

# nettoyage attention convention partage avec run.pl
TMP_CASE_FILE=$(sed 's/.arc/_test.arc/g' <<< "$CASE")
if [ -f "${PWD}/${TMP_CASE_FILE}" ]
then 
    rm "${PWD}/${TMP_CASE_FILE}"
fi 

# nettoyage fatal_4 en cas d echec du test
if [ -f "${PWD}/fatal_4" ]
then
    rm "${PWD}/fatal_4"
fi

echo "-------------------------------------------------------------"

if [ "$?" = "0" ] ; then
    if ( 1>/dev/null 2>/dev/null grep "STOP PERFECT" ${OUTFILE} ) ; then
        export COVERITY_TEST_STATUS=pass
        echo "'STOP PERFECT' found"
        exit 0
    else
        export COVERITY_TEST_STATUS=fail
        echo "'STOP PERFECT' not found"
		if [ "$OSTYPE" = "cygwin" ] ; then
		  taskkill /IM "${EXE}" /T /F
		fi
        exit 1
    fi
else
    if [ "$OSTYPE" = "cygwin" ] ; then
	  taskkill /IM "${EXE}" /T /F
	fi
    exit 2
fi
