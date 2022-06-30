#!/bin/sh
if [ "x$1" = "x" ]; then
echo Usage : "`basename $0`" ARCANE_DIR APPLI_DIR DOXYFILE AXLFILE 1>&2
echo to generate doxygen .dox files 1>&2
echo where : 1>&2
echo ARCANE_DIR : Arcane installation path 1>&2
echo APPLI_DIR  : Appli installation path 1>&2
echo DOXYFILE   : Appli doxygen config file name 1>&2
echo AXLFILES   : list of axl files to parse to generate doc 1>&2
exit 0
fi
ARCANE_DIR=$1
APPLI_DIR=$2
DOXYFILE=$3
AXLFILES=$4
cd ${APPLI_DIR}
${APPLI_DIR}/common/ArcaneInfra/bin/axlDbBuilder `cat ${AXLFILES}` > ${APPLI_DIR}/DOdoc/appli_db_file

# For generating doxygen .dox files, you have to call:
${ARCANE_DIR}/bin/axldoc -o ${APPLI_DIR}/DOdoc/src -a ${APPLI_DIR}/DOdoc/appli_db_file `cat ${AXLFILES}`
#Then copy default axldoc.doxyfile configuration file and run
cd ${APPLI_DIR}/DOdoc ; doxygen ${DOXYFILE}

