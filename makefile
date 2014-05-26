all : example

install :
	cp bwt.hpp ${HOME}/include/.

example : example.cpp bwt.hpp
	g++ -O3 -std=gnu++0x -o example example.cpp

install :
	cp bwt.hpp ${HOME}/include/.

check : example
	example gataca

check2 : example
	example -f dna

clean :
	rm -rf *.o *.exe example *.stackdump
