all : example

example : example.cpp bwt.hpp
	g++ -std=gnu++0x -o example example.cpp

check : example
	example gataca

check2 : example
	example -f dna

clean :
	rm -rf *.o *.exe example *.stackdump
