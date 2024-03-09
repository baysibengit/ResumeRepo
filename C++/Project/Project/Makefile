#Name: Ben Bayinger Class: CS6015
#Makefile for CS6015 

#Target: all checks if prereq executable is up to date
#
#Target: run checks if the executable file named executable is up to date
	#runs the executable file
#
#Target: clean
	#removes all .o file and the executable file named executable
#
#Target: .PHONY explicitely tells make that these targets are not associated with files
#
#Target: make checks if the .cpp and .h files are up to date
	#Creates .o files and executable 
#
#Target: printM
	#prints out values of macros
#
#Target: doc
	#uses doxygen to create documented outline of project
#
#Target: pdf
	#turns document to pdf 

	

CXX = c++
CFLAGS = -std=c++11
CXXSOURCE = cmdline.cpp main.cpp  Expr.cpp parse.cpp
HEADERS = cmdline.hpp catch.hpp Expr.hpp parse.hpp
CXXOBJECT = cmdline.o main.o Expr.o parse.o
DOC = Document
DOX_CONFIG = Doxyfile

.SILENT:

.PHONY: all run test clean pdf doc

all: msdscript

msdscript: $(CXXSOURCE) $(HEADERS)
	$(CXX) $(CFLAGS) -c $(CXXSOURCE)
	$(CXX) $(CFLAGS) -o msdscript $(CXXOBJECT)

run: msdscript
	./msdscript

test: msdscript
	./msdscript --test
	
clean:
	rm -rf *.o msdscript test_msdscript

printM:
	echo $(CXXSOURCE) $(CXX) $(CFLAGS) $(HEADERS) $(CXXOBJECT)
		
pdf: doc
	cd Documentation/latex \
	&& make pdf > /dev/null \
	&& cp refman.pdf ../$(DOC).pdf
	
doc:
	cd Documentation/ && doxygen $(DOX_CONFIG) > /dev/null
	
test_msdscript: test_msdscript.cpp exec.cpp exec.hpp
	$(CXX) $(CFLAGS) test_msdscript.cpp exec.cpp -o test_msdscript

