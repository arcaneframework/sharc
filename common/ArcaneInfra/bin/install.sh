#!/bin/sh

copy_dependencies(){
# $1: source dependency
# $2: install dir
# $3: dependency filter (if exists)
  for i in $(ldd "${1}" | grep -E "${3}" | awk '{ print $3 }')
  do
    target_file=${2}/$(basename ${i})
    if [ "$i" -nt "${target_file}" ] # source is more recent than target or target does no exist
    then
      #echo "copy "$i" to ${target_file}"
      cp "$i" "${target_file}"
    fi
  done
}

exitusage() {
  if [ -n "$1" ]; then
    echo "Error: $1" 2>&1
  fi
  echo "Usage: $0 [-i] [-o] [-s] [-r] [-p] [-t] [-h] [-k] [-n] build_directory install_path"
  echo "Options:"
  echo "  -i : installs IFP libs"
  echo "  -o : installs exotic IFP libs dependencies"
  echo "  -s : strips binaries (remove debug symbols)"
  echo "  -r : copy mpi runtime (from current environment)"
  echo "  -p : check prohibited libraries to redistribute (aka Metis)"
  echo "  -t : install testing infrastructure"
  echo "  -k : disable old launcher generation"
  echo "  -n : new toolchain"
  echo "  -h : this help page"
  exit 1
}

OPTIONS=$@
set -- $(getopt iosrpthkn "$@")
while [ $# -gt 0 ]
do
    case "$1" in
    (-i) NEED_INFRADEV=yes;;
    (-s) NEED_STRIP=yes;;
    (-r) MPIRT=yes;;
    (-o) NEED_OTHER=yes;;
    (-p) CHECK_PROHIBITED=yes;;
    (-t) INSTALL_TESTING=yes;;
    (-k) OLD_LAUNCHER=no;;
    (-n) NEW_TOOLCHAIN=yes;;
    (-h) exitusage;;
    (--) shift; break;;
    (-*) echo "$0: internal error - unrecognized option $1" 1>&2; exit 1;;
    (*)  break;;
    esac
    shift
done

BUILD_DIR=$1
INSTALL_DIR=$2
BUILD_DIR=`/bin/sh -c "cd ${BUILD_DIR} ; pwd"`

if [ -z "${INSTALL_DIR}" ]; then
  exitusage "bad syntax"
fi

CACHE_FILE=${BUILD_DIR}/CMakeCache.txt
if [ ! -e $CACHE_FILE ]; then
  echo "Error: bad directory"
  echo "Current directory does not seem like an Arcane project directory"
  exit 1
fi

PROJECT_NAME=`awk -F= '/CMAKE_PROJECT_NAME:STATIC/ { print $2 }' ${CACHE_FILE}`
echo "${PROJECT_NAME}"
PROJECT_ROOT=`awk -F= '/PROJECT_ROOT:PATH/ { print $2 }' ${CACHE_FILE}`
if [ -z "$PROJECT_ROOT" ]; then
  PROJECT_ROOT=`awk -F= '/^CMAKE_HOME_DIRECTORY:INTERNAL/ { print $2 }' ${CACHE_FILE}`
  if [ -z "$PROJECT_ROOT" ]; then
    echo "Project root not found; aborting installation"
    exit 1
  fi
fi

## Your configuration is necessary ##
CONFIG_DIR=src/${PROJECT_NAME} # relative path for your config
CONFIG=${PROJECT_NAME}.config  # name of your config
EXE=${PROJECT_NAME}.exe
LDCFG=/lib64/ld-linux-x86-64.so.2
CHRPATH=/home/irsrvshare1/R11/arcuser/softs/chrpath/bin/chrpath

PROJECT_DIR=`pwd`
BIN_DIR=${BUILD_DIR}/bin
LIB_DIR=${BUILD_DIR}/lib
ARCANE_DIR=`awk -F= '/^ARCANE_PATH:PATH/ { print $2 }' ${CACHE_FILE}`
if [ -z "$ARCANE_DIR" ]; then
  ARCANE_DIR=`awk -F= '/^Arcane_LIBDIR:INTERNAL/ { print $2 }' ${CACHE_FILE}`
  if [ -z "$ARCANE_DIR" ]; then
    echo "Arcane installation not found; aborting installation"
    exit 1
  fi
  ARCANE_DIR=$ARCANE_DIR/..
fi

ALIEN_DIR=`awk -F= '/^Alien_PREFIX:INTERNAL/ { print $2 }' ${CACHE_FILE}`

CMDSH=${INSTALL_DIR}/${PROJECT_NAME}.sh
REPORT=${INSTALL_DIR}/${PROJECT_NAME}.log
echo "Detected config:"
echo "   Project root      : "${PROJECT_ROOT}
echo "   Install directory : "${INSTALL_DIR}
echo "   Binary directory  : "${BIN_DIR}
echo "   Library directory : "${LIB_DIR}
echo "   Arcane directory  : "${ARCANE_DIR}
echo "   Alien directory   : "${ALIEN_DIR}
echo "   Exec name         : "${EXE}

if ( ldd ${BIN_DIR}/${EXE} | grep -c "not found" 2>&1 >/dev/null );
then
  echo "Cannot install incompletly defined executable ${BIN_DIR}/${EXE}"
  exit 1
fi

if [ -f ${INSTALL_DIR}/bin ] ; then
  echo ${INSTALL_DIR}/bin "is already existing"
else
  echo "Creating directory "${INSTALL_DIR}/bin
  mkdir -p ${INSTALL_DIR}/bin
fi

if [ -f ${INSTALL_DIR}/lib ] ; then
  echo ${INSTALL_DIR}/bin "is already existing"
else
  echo "Creating directory "${INSTALL_DIR}/lib
  mkdir -p ${INSTALL_DIR}/lib
fi

echo "Copying project files"
cp ${BIN_DIR}/${EXE} ${INSTALL_DIR}/bin/
cp ${PROJECT_DIR}/${CONFIG_DIR}/${CONFIG} ${INSTALL_DIR}/bin/
cp ${LIB_DIR}/*.so ${INSTALL_DIR}/lib/
echo "Copying arcane libs"
cp ${ARCANE_DIR}/lib/*.so ${INSTALL_DIR}/lib/
if [ -n "$ALIEN_DIR" ]; then
  echo "Copying alien libs"
  cp ${ALIEN_DIR}/lib/*.so ${INSTALL_DIR}/lib/
fi

if [ -n "$NEW_TOOLCHAIN" ]; then
  INDEX_OF_BASE_PATH=`which gcc | awk '{ print index($0,"/Core") }'`
  BASE_PATH=`which gcc | cut -c1-$INDEX_OF_BASE_PATH`
  #ldd ${BIN_DIR}/${EXE} | egrep "$BASE_PATH" | awk '{ print $3 }' | xargs --no-run-if-empty cp -f --target-directory=${INSTALL_DIR}/lib/
  copy_dependencies ${BIN_DIR}/${EXE} "${INSTALL_DIR}/lib" "$BASE_PATH"
fi

if [ -n "$NEED_INFRADEV" ]; then
  echo "Copying extra IFP libs"
  #ldd ${BIN_DIR}/${EXE} | egrep "commonlib/ifpen|commonlib/eb|infradev/softs|arcuser/softs|R11/X_HPC| /soft/" | awk '{ print $3 }' | xargs --no-run-if-empty cp -f --target-directory=${INSTALL_DIR}/lib/
  copy_dependencies ${BIN_DIR}/${EXE} "${INSTALL_DIR}/lib" "commonlib/ifpen|commonlib/eb|infradev/softs|arcuser/softs|R11/X_HPC| /soft/"
  echo "Copying indirect libs"
  for lib in ${INSTALL_DIR}/lib/*.so*
  do
    chmod u+rx $lib # for ldd lib must be executable
    copy_dependencies $lib "${INSTALL_DIR}/lib" "commonlib/ifpen|commonlib/eb|infradev/softs|arcuser/softs|R11/X_HPC| /soft/"
    #ldd $lib | egrep "commonlib/ifpen|commonlib/eb|infradev/softs|arcuser/softs|R11/X_HPC| /soft/" | awk '{ print $3 }' | xargs --no-run-if-empty cp -f --target-directory=${INSTALL_DIR}/lib/
  done



  if [ -n "$NEED_OTHER" ]; then
    echo "Copy exotic IFP lib dependencies"
    if [ -z "$NEW_TOOLCHAIN" ]; then
      for lib in \
	/usr/lib64/libgfortran.so.1 \
	/soft/irsrvsoft1/expl/Intel_12.1.3/composer_xe_2011_sp1.9.293/mkl/lib/intel64/libmkl_core.so \
	/soft/irsrvsoft1/expl/Intel_12.1.3/composer_xe_2011_sp1.9.293/mkl/lib/intel64/libmkl_def.so \
	/soft/irsrvsoft1/expl/Intel_12.1.3/composer_xe_2011_sp1.9.293/mkl/lib/intel64/libmkl_intel_lp64.so \
	/soft/irsrvsoft1/expl/Intel_12.1.3/composer_xe_2011_sp1.9.293/mkl/lib/intel64/libmkl_mc.so \
	/soft/irsrvsoft1/expl/Intel_12.1.3/composer_xe_2011_sp1.9.293/mkl/lib/intel64/libmkl_sequential.so
      do
        echo "    $lib"
        cp -f --target-directory=${INSTALL_DIR}/lib/ $lib
      done
    else # new toolchain
      MKL_LIBDIR=`awk -F= '/^MKL_INCLUDE_DIR:PATH/ { print $2 }' ${CACHE_FILE}`/../lib/intel64
      for lib in \
	$MKL_LIBDIR/libmkl_def.so
      do
        echo "    $lib"
        cp -f --target-directory=${INSTALL_DIR}/lib/ $lib
      done
    fi
    echo "Copy libexpat dependencies" 
    for lib in ${INSTALL_DIR}/lib/*.so*
    do
      chmod u+rx $lib # for ldd lib must be executable
      ldd $lib | egrep "libexpat.so" | awk '{ print $3 }' | xargs --no-run-if-empty cp -f --target-directory=${INSTALL_DIR}/lib/
    done
  fi
 
  if [ -z "${MPIRT}" ]; then
    echo "Removing MPI libs"
    rm -f ${INSTALL_DIR}/lib/libmpi*.so*
  fi
  
  chmod u+w -R ${INSTALL_DIR}/lib
  echo  "Removing default rpath from libraries and binary"
  $CHRPATH -d ${INSTALL_DIR}/bin/${EXE}
  for file in ${INSTALL_DIR}/lib/*
  do
    $CHRPATH -d $file
  done
else
  chmod u+w -R ${INSTALL_DIR}/lib
  echo  "Removing project rpath from binary"
  RPATH0=`$CHRPATH -l ${INSTALL_DIR}/bin/${EXE}`
  RPATH0=${RPATH0/*RPATH=/}
  RPATH0=${RPATH0//${LIB_DIR}/}
  $CHRPATH -r "$RPATH0" ${INSTALL_DIR}/bin/${EXE} >/dev/null
fi

if [ -n "$CHECK_PROHIBITED" ]; then
  echo "Checking prohibited dependencies"
  for lib in ${INSTALL_DIR}/lib/* ${INSTALL_DIR}/bin/${EXE}
  do
    # echo "Checking $lib"
    if ( nm $lib 2>&1 | grep -i metis_part > /dev/null ); then
      echo "## WARNING ## suspect reference to metis found in $lib"
    fi
  done
fi

if [ -n "$NEED_STRIP" ]; then
  echo "Striping binaries"
  strip ${INSTALL_DIR}/lib/lib*.so
  strip ${INSTALL_DIR}/bin/${EXE}
fi

if [ -n "$MPIRT" ]; then
  if [ -n "${NEW_TOOLCHAIN}" ]; then # new toolchain used
    if [ -z "${OLD_LAUNCHER}" ]; then
      echo "new toolchain install not compatible with old launcher generation"
      exit 1
    fi
    for INTELMPI_RUNTIME_VERSION in 2019.7.217 2018.3.222; do
      echo "Installing IntelMPI runtime version ${INTELMPI_RUNTIME_VERSION}"
      tar -zxf ${PROJECT_DIR}/common/ArcaneInfra/IntelMPI/IntelMPI_${INTELMPI_RUNTIME_VERSION}.tar.gz -C ${INSTALL_DIR}
    done
  else
    echo "Copying intelmpi runtime"
    MPIRUNPATH=`which mpirun`
    if [ -z "$MPIRUNPATH" ]; then
      echo "Cannot find intelmpi runtime" 1>&2
    else
      MPIRTDIR=`dirname $MPIRUNPATH`
      mkdir -p ${INSTALL_DIR}/bin/mpirt
      # mpi runtine list from list /soft/irsrvsoft1/expl/IntelMPI_4.0.2/impi_4.0.2.003/redist-rt.txt
      for i in              \
          mpd.py            \
          mpdallexit.py     \
          mpdboot.py        \
          mpdcheck.py       \
          mpdchkpyver.py    \
          mpdcleanup.py     \
          mpdexit.py        \
          mpdgdbdrv.py      \
          mpdhelp.py        \
          mpdkilljob.py     \
          mpdlib.py         \
          mpdlistjobs.py    \
          mpdman.py         \
          mpdringtest.py    \
          mpdrun.py         \
          mpdsigjob.py      \
          mpdtrace.py       \
          mpiexec.py        \
          ptp_impi_proxy.py \
          cpuinfo           \
          IMB-MPI1          \
          mpd               \
          mpdallexit        \
          mpdboot           \
          mpdcheck          \
          mpdcleanup        \
          mpdexit           \
          mpdhelp           \
          mpdkilljob        \
          mpdlistjobs       \
          mpdringtest       \
          mpdroot           \
          mpdrun            \
          mpdsigjob         \
          mpdtrace          \
          mpiexec           \
          mpiexec.hydra     \
          mpirun            \
          mpitune           \
          pmi_proxy
      do
        cp -af ${MPIRTDIR}/$i ${INSTALL_DIR}/bin/mpirt
      done
      MPIRTBINDIR='${ROOT_DIR}/bin/mpirt'
    fi
  fi
fi

if [ -z "$OLD_LAUNCHER" ]; then
echo "Creating ${CMDSH} launcher script"
cat > ${CMDSH} << EOT
#!/bin/sh
ROOT_DIR=\$(dirname \$0)
IS_ABSOLUTE_ROOT_DIR=\$(echo \$ROOT_DIR|cut -c1)

if [ ! "\$IS_ABSOLUTE_ROOT_DIR" = "/" ] ; then
{ ROOT_DIR=\$(cd "\$ROOT_DIR" && pwd) ; }
fi ;
#ROOT_DIR=${INSTALL_DIR}

# environment variables
LD_LIBRARY_PATH=\${ROOT_DIR}/lib:\$LD_LIBRARY_PATH
STDENV_PATH_SHR=\${ROOT_DIR}/bin
export LD_LIBRARY_PATH STDENV_PATH_SHR

# set unlimited stack size for big static array from fortran (IFPSolver)
ulimit -s unlimited

exitusage()
{
  if [ -n "\$1" ]; then
    echo "Error: \$1" 2>&1
  fi
  cat <<EOF
Usage: \$0 [options] [--] executable parameters
options:
-n N : start parallel run with N procs
-c   : check mode 
-V   : print version
-h   : this help page
EOF
  exit 1
}

checkinteger()
{
  echo \$1 | grep '^[1-9][0-9]*\$' > /dev/null
  return \$?
}

NPROC=1
set -- \$(getopt n:hVc "\$@")
while [ \$# -gt 0 ]
do
    case "\$1" in
    (-n) NPROC=\$2; checkinteger \$NPROC || exitusage "N=\$NPROC must be an integer"; shift;;
    (-c) INTERNAL_CHECK=1;;
    (-h) exitusage;;
    (-V) \${ROOT_DIR}/bin/${EXE} --version ; exit 0;;
    (--) shift; break;;
    (-*) exitusage "internal error - unrecognized option \$1";;
    (*)  break;;
    esac
    shift
done

test -n "\$1" || exitusage "executable parameters requested"

LAUNCHER=
if [ -n "\${ARCANE_PARALLEL_SERVICE}" ]; then
  case "\${ARCANE_PARALLEL_SERVICE}" in
    (Mpi) LAUNCHER="${MPIRTBINDIR}/mpirun -n \$NPROC"
          if [ -n "${MPIRTBINDIR}" ]; then
            export I_MPI_ROOT="${MPIRTBINDIR}"
            export PATH=${MPIRTBINDIR}:\$PATH
          fi;;
    (*)   echo "Parallel error: unrecognized parallel service <\${ARCANE_PARALLEL_SERVICE}>" 1>&2; exit 1;;
  esac
else
  if [ \$NPROC != 1 ]; then
    echo "Parallel error: ARCANE_PARALLEL_SERVICE not set for a parallel run" 1>&2; exit 1;
  fi
fi

if [ -n "\${INTERNAL_CHECK}" ]; then
  echo "=== Locally binded libraries ==="
  ldd \${ROOT_DIR}/bin/${EXE} | grep "=> \${ROOT_DIR}/lib"
  echo "=== Non locally binded libraries ==="
  ldd \${ROOT_DIR}/bin/${EXE} | grep -v "=> \${ROOT_DIR}/lib"
  echo "=== Runtime command ==="
  echo "        \$LAUNCHER \${ROOT_DIR}/bin/${EXE} \$@"
else
  \$LAUNCHER \${ROOT_DIR}/bin/${EXE} \$@
fi
EOT
chmod +x ${CMDSH}
fi

if [ -n "$INSTALL_TESTING" ]; then
  BASE=`dirname $0` 
  INSTALL_FILES=$BASE/install_files
  TESTING=${INSTALL_DIR}/testing
  echo "Install testing infrastructure into $TESTING"
  echo "  Read README.txt to complete testing procedure"
  mkdir -p ${TESTING}
  mkdir -p ${TESTING}/data
  mkdir -p ${TESTING}/test
  mkdir -p ${TESTING}/bin/common/ArcaneInfra/bin
  CMAKE_ROOT=/soft/irsrvsoft1/expl/Cmake_2.8.7
  cp -a $CMAKE_ROOT ${TESTING}/bin
  ln -sf `basename $CMAKE_ROOT`/bin/cmake ${TESTING}/bin
  ln -sf `basename $CMAKE_ROOT`/bin/ctest ${TESTING}/bin
  for file in ArcaneTest.sh convergence.pl convergenceTest.sh run.pl ; do
    cp -f $BASE/$file ${TESTING}/bin/common/ArcaneInfra/bin
  done
  for file in FindTestAuthor.pl ; do
    cp -f ${INSTALL_FILES}/$file ${TESTING}/bin/common/ArcaneInfra/bin
  done
  for file in README.txt CMakeLists.txt ; do
    cp -f ${INSTALL_FILES}/$file ${TESTING}
  done
  for file in Macros.txt ; do
    cp -f ${BASE}/../CMake/$file ${TESTING}
  done
  perl -i -pe "s/%PROJECT_NAME%/${PROJECT_NAME}/g" ${TESTING}/CMakeLists.txt
fi

# Reporting d'installation
echo "################## Install report ##################"               >  ${REPORT}
echo "Date                  : "`date`                   >> ${REPORT}
echo "Hostname              : "`hostname`               >> ${REPORT}
echo "Host OS version       : "`cat /etc/redhat-release`>> ${REPORT}
echo "Host Kernel version   : "`uname -r`               >> ${REPORT}
echo "User                  : "`whoami`                 >> ${REPORT}
echo "Working directory     : ${PWD}"                   >> ${REPORT}
echo "Project root          : ${PROJECT_ROOT}"          >> ${REPORT}
echo "Build directory       : ${BUILD_DIR}"             >> ${REPORT}
echo "Compilation command   :"                          >> ${REPORT}
if [[ -a ${BUILD_DIR}/reconfigure ]]; then
  awk '{ print "\t"$_ }' ${BUILD_DIR}/reconfigure         >> ${REPORT}
fi
echo "Binary directory      : ${BIN_DIR}"               >> ${REPORT}
echo "Library directory     : ${LIB_DIR}"               >> ${REPORT}
echo "Binary stats          :"                          >> ${REPORT}
stat ${BIN_DIR}/${EXE}|awk '{ print "\t"$_ }'           >> ${REPORT}
echo "Install command       : $0 ${OPTIONS}"            >> ${REPORT}
echo "Arcane directory      : ${ARCANE_DIR}"            >> ${REPORT}
awk '{ print "\t"$_ }' ${ARCANE_DIR}/env                >> ${REPORT}
echo "Subversion infos      :"                          >> ${REPORT}
svn info ${PROJECT_ROOT}                           \
         ${PROJECT_ROOT}/common/ArcGeoSim          \
         ${PROJECT_ROOT}/common/ArcaneInfra        \
   | awk '{ print "\t"$_ }'                             >> ${REPORT}  
echo "Subversions stats     :"                          >> ${REPORT}
svn st ${PROJECT_ROOT}/common/ArcaneInfra          \
       ${PROJECT_ROOT}/common/ArcGeoSim/src        \
       ${PROJECT_ROOT}/src                         \
       ${PROJECT_ROOT}/CMakeLists.txt              \
       ${PROJECT_ROOT}/*.xml                       \
   | awk '{ print "\t"$_ }'                             >> ${REPORT}  
echo "Subversions diffs     :"                          >> ${REPORT}
svn diff ${PROJECT_ROOT}/common/ArcaneInfra        \
         ${PROJECT_ROOT}/common/ArcGeoSim/src      \
         ${PROJECT_ROOT}/src                       \
         ${PROJECT_ROOT}/CMakeLists.txt            \
         ${PROJECT_ROOT}/*.xml                     \
   | awk '{ print "\t"$_ }'                             >> ${REPORT}
echo "Environment variables :"                          >> ${REPORT}
PROJECT_CAP_NAME=`echo ${PROJECT_NAME} | tr '[:lower:]' '[:upper:]'` 
perl -e "BEGIN{\$NAME=${PROJECT_CAP_NAME}}"'while(my ($k,$v) = each %ENV) { if ($k =~ /(^ARCANE|^ARCGEOSIM|^$NAME|PATH|INTEL|MKL|USER|TYPE|LC_|LICENSE)/) { print "\t$k=$v\n" } }' >> ${REPORT}

echo "Install log written in ${REPORT}"
echo "Installation completed"
