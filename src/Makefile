LEX     = flex
YACC    = bison -y
YFLAGS  = -d

objects = PSFormulaMassParser.o SIDimensionalityParser.tab.o SIDimensionalityScanner.o SIDimensionality.o \
	SIUnitParser.tab.o SIUnitScanner.o SIUnit.o 

libSITypes.a : $(objects)
	ar rcs libSITypes.a $(objects)

PSFormulaMassParser.c : PSFormulaMassParser.l

SIDimensionalityScanner.c : SIDimensionalityScanner.l

SIDimensionalityParser.tab.c + SIDimensionalityParser.tab.h : SIDimensionalityParser.y
	yacc -d SIDimensionalityParser.y
	mv y.tab.c SIDimensionalityParser.tab.c
	mv y.tab.h SIDimensionalityParser.tab.h

SIDimensionality.o : SIDimensionality.h SILibrary.h
	cc -c -O3 SIDimensionality.c

SIUnitScanner.c : SIUnitScanner.l

SIUnitParser.tab.c + SIUnitParser.tab.h : SIUnitParser.y
	yacc -d SIUnitParser.y
	mv y.tab.c SIUnitParser.tab.c
	mv y.tab.h SIUnitParser.tab.h

SIUnit.o : SIUnit.h SILibrary.h
	cc -c -O3 SIUnit.c


clean :
	rm $(objects) libSITypes.a PSFormulaMassParser.c SIDimensionalityScanner.c SIDimensionalityParser.tab.c \
		SIUnitScanner.c SIUnitParser.tab.c
