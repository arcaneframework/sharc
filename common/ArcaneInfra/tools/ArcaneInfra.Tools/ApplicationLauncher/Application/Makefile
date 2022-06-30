CODENAME=Application

# Configure default build type and build type flags
BUILD_TYPE=Release
#BUILD_TYPE=Debug

ifeq ($(MONO_PATH),)
  MONO_PATH=../bin/$(BUILD_TYPE)
endif
EXTPATH := $(MONO_PATH)

SRCS= Loader.cs AssemblyInfo.cs
RESX= 
LIBS := mscorlib System 
EXTEXES := ApplicationLauncher
MYFLAGS:= /platform:x86 

Debug_FLAGS := -debug+ /debug:full "/define:DEBUG"
Release_FLAGS := -optimize-

MCS=mcs
dirsep:=/
ressep:=.
MONO_ROOT_DIR=$(shell which $(MCS) | xargs dirname)/..
FRAMEWORK_RPATH=$(MONO_ROOT_DIR)/lib/mono/4.5

RESOURCES= Application.xml

DISTRIB_SRC_PATH = .

FULL_SRCS = $(SRCS:%=${DISTRIB_SRC_PATH}/%)

TARGET= bin/$(BUILD_TYPE)/$(CODENAME)

cs: ${TARGET}.dll

all: ${TARGET}.dll

MCS_FLAGS := $($(BUILD_TYPE)_FLAGS) $(MYFLAGS) /noconfig -codepage:utf8 /nologo /warn:4 /t:library -nostdlib

${TARGET}.dll : $(FULL_SRCS) $(RESOURCES) Makefile $(foreach r,$(LIBS),$(FRAMEWORK_RPATH)/$r.dll) $(foreach r,$(EXTEXES),$(EXTPATH)/$r.exe) 
	${MCS} $(FULL_SRCS) -out:$@ ${MCS_FLAGS} $(foreach r,$(RESOURCES),/res:$r,$(subst $(dirsep),$(ressep),$(CODENAME).$r)) $(foreach r,$(LIBS),/r:$(FRAMEWORK_RPATH)/$r.dll) $(foreach r,$(EXTEXES),/r:$(EXTPATH)/$r.exe)

clean_Debug:
	-rm -f ${TARGET}.dll.mdb ${TARGET}.mdb 

clean_Release:

clean: clean_$(BUILD_TYPE)
	-rm -f ${TARGET}.dll
	-rm -f ${TARGET}
