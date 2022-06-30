CODENAME=ApplicationLauncher
# CODENAME may be overriden using CODENAME=YourCodeName in make command line

# Configure default build type and build type flags
BUILD_TYPE=Release
#BUILD_TYPE=Debug

ifeq ($(MONO_PATH),)
  MONO_PATH=Application/bin/$(BUILD_TYPE)
endif
EXTPATH := $(MONO_PATH)

SRCS= Launcher.cs Main.cs Mono_Options.cs AssemblyInfo.cs Env.cs ExecTools.cs Logger.cs Application.cs ApplicationConfiguration.cs ConfigurationReader.cs AssemblyResolver.cs
RESX= Resources/Banners.resx
LIBS := mscorlib System System.Xml System.Core
EXTLIBS := Application
MYFLAGS:= /platform:x86 

Debug_FLAGS := -debug+ /debug:full "/define:DEBUG"
Release_FLAGS := -optimize-

MCS=mcs
RESGEN=resgen
dirsep:=/
ressep:=.
MONO_ROOT_DIR=$(shell which $(MCS) | xargs dirname)/..
FRAMEWORK_RPATH=$(MONO_ROOT_DIR)/lib/mono/4.5

RESOURCES= $(RESX:.resx=.resources) ApplicationConfiguration.xsd

DISTRIB_SRC_PATH = .

FULL_SRCS = $(SRCS:%=${DISTRIB_SRC_PATH}/%)

TARGET_DIR=bin/$(BUILD_TYPE)
TARGET= ${TARGET_DIR}/$(CODENAME)

cs: ${TARGET}.exe ${TARGET}.dll

all: ${TARGET}.exe ${TARGET}

MCS_FLAGS := $($(BUILD_TYPE)_FLAGS) $(MYFLAGS) /noconfig -codepage:utf8 /nologo /warn:4 /t:exe -nostdlib 

${TARGET}.exe : $(FULL_SRCS) $(RESOURCES) Makefile $(foreach r,$(LIBS),$(FRAMEWORK_RPATH)/$r.dll) 
	${MCS} $(FULL_SRCS) -out:$@ ${MCS_FLAGS} $(foreach r,$(RESOURCES),/res:$r,$(subst $(dirsep),$(ressep),$(CODENAME).$r)) $(foreach r,$(LIBS),/r:$(FRAMEWORK_RPATH)/$r.dll) 

${TARGET}.dll : ${TARGET}.exe
	cp ${TARGET}.exe ${TARGET}.dll

#${TARGET}: ${TARGET}.exe $(foreach r,$(EXTLIBS),$(EXTPATH)/$r.dll)
${TARGET}: ${TARGET}.exe ${TARGET}.dll 
	PKG_CONFIG_PATH=$(MONO_ROOT_DIR)/lib/pkgconfig mkbundle --simple -L ${TARGET_DIR}   -z ${TARGET}.exe $(foreach r,$(EXTLIBS),$(EXTPATH)/$r.dll) -o ${TARGET}

%.resources : %.resx
	${RESGEN} /compile $<,$@

native: ${TARGET} 

clean_Debug:
	-rm -f ${TARGET}.exe.mdb ${TARGET}.mdb 

clean_Release:

clean: clean_$(BUILD_TYPE)
	-rm -f ${TARGET}.exe 
	-rm -f $(RESX:.resx=.resources)
	-rm -f ${TARGET}
