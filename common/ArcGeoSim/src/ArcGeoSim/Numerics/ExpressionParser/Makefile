BISON=bison
LEX=flex
INCLS=-I${INFRADEV}/softs/boost_1_34_0
LDFLAGS=
CC=g++
ERASE= main ${wildcard *.o} expression_parser.cc expression_parser.hh expression_scanner.cc location.hh position.hh stack.hh
CPPFLAGS= ${INCLS}

.PHONY : all clean

all: expression_parser.cc expression_scanner.cc

%.cc: %.yy
	@echo "-- Bison compiling " $<; \
	${BISON} -d -p "ep" -ra -S "/home/irsrvhome1/R11/dipietrd/Software/share/bison/lalr1.cc" -o $@ $<; \
	./modifyBisonHeader.pl `ls $@ | sed "s/\.cc/\.hh/g"`	; \
	./modifyBisonHeader.pl location.hh; \
	./modifyBisonHeader.pl position.hh; \
	./modifyBisonHeader.pl stack.hh; \
	\rm *.old;

%.cc: %.ll
	@echo "-- Lex compiling " $<; \
	${LEX} $<; \
	mv lex.ep.c $@

clean:
	@echo "-- Cleaning all"; \
	for dustfile in ${ERASE}; do \
	!( \test -e $$dustfile ) || \rm $$dustfile; \
	done; \
