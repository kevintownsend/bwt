all : example

example : example.cpp bwt.hpp
	g++ -g -o example example.cpp

check : example
	example
