all : example

install :
	mkdir -p ${HOME}/include
	cp bwt.hpp ${HOME}/include/.

example : examples/bwt_cli.cc bwt.hpp
	g++ -O3 -std=c++17 -Wall -Wextra -Wpedantic -I. -o example examples/bwt_cli.cc

check : example
	./example gataca

check2 : example
	./example -f examples/data/dna.txt

clean :
	rm -rf *.o *.exe example *.stackdump
