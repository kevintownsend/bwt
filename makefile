all : example

example : example.cpp bwt.hpp
	g++ -std=gnu++0x -o example example.cpp

check : example
	example gataca
