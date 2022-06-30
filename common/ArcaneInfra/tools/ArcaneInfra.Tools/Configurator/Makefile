CODENAME=Configurator

SRCS = \
    CommonTools.cs \
    Configurator.cs \
    Mono_Options.cs \
    PkglistParser.cs \
    Program.cs \
    Properties/AssemblyInfo.cs \
    VendorChecker.cs

LIBS := mscorlib System System.Xml System.Core System.Xml.Linq
RESX=
MYFLAGS:=

# Configure default build type and build type flags
BUILD_TYPE=Debug
Debug_FLAGS := -debug+ /debug:full "/define:DEBUG"

#BUILD_TYPE=Debug
Release_FLAGS := -optimize-

MCS=mcs
RESGEN=resgen
dirsep:=/
ressep:=.
MONO_ROOT_DIR=$(shell which $(MCS) | xargs dirname)/..
FRAMEWORK_RPATH=$(MONO_ROOT_DIR)/lib/mono/4.5

DISTRIB_SRC_PATH = .

FULL_SRCS = $(SRCS:%=${DISTRIB_SRC_PATH}/%)

TARGET= bin/$(BUILD_TYPE)/$(CODENAME)

cs: ${TARGET}.exe

all: ${TARGET}.exe ${TARGET}

MCS_FLAGS := $($(BUILD_TYPE)_FLAGS) $(MYFLAGS) /noconfig -codepage:utf8 /nologo /warn:4 /t:exe -nostdlib $(foreach r,$(LIBS),/r:$(FRAMEWORK_RPATH)/$r.dll)

${TARGET}.exe : $(FULL_SRCS) Makefile
	${MCS} $(FULL_SRCS) -out:$@ ${MCS_FLAGS} 

${TARGET}: ${TARGET}.exe 
	PKG_CONFIG_PATH=$(MONO_ROOT_DIR)/lib/pkgconfig mkbundle --simple -z ${TARGET}.exe -o ${TARGET}

%.resources : %.resx
	${RESGEN} /compile $<,$@

native: ${TARGET}

clean_Debug:
	-rm -f ${TARGET}.exe.mdb ${TARGET}.mdb 

clean_Release:

clean: clean_$(BUILD_TYPE)
	-rm -f ${TARGET}.exe 
	-rm -f ${TARGET}
