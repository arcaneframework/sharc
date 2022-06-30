CODENAME=XmlToCMake

SRCS = 
LIBS := 
RESX=
MYFLAGS:=

# Configure default build type and build type flags
BUILD_TYPE=Release
Debug_FLAGS := /p:Configuration=Debug

BUILD_TYPE=Debug
Release_FLAGS := /p:Configuration=Release

BUILD=xbuild


TARGET= bin/$(BUILD_TYPE)/$(CODENAME)

cs: ${TARGET}.exe

all: ${TARGET}.exe ${TARGET}

${TARGET}.exe : Makefile
	${BUILD}  $($(BUILD_TYPE)_FLAGS) ${CODENAME}.csproj

${TARGET}: ${TARGET}.exe 
	

native: ${TARGET}

clean_Debug:
	-rm -f ${TARGET}.exe.mdb ${TARGET}.mdb 

clean_Release:

clean: clean_$(BUILD_TYPE)
	-rm -f ${TARGET}.exe 
	-rm -f ${TARGET}
