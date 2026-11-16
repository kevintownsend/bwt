all : example

install :
	cp bwt.hpp ${HOME}/include/.

example : example.cpp bwt.hpp
	g++ -O3 -std=c++17 -o example example.cpp

check : example
	example gataca

check2 : example
	example -f dna

clean :
	rm -rf *.o *.exe example *.stackdump
