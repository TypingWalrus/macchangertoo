macchangertoo: setup main.o
	g++ main.o -o macchangertoo

main.o: main.cpp mac_vendors.hpp defaults.hpp
	g++ -c main.cpp -o main.o

defaults.hpp: setup
	./setup

setup: setup.cpp
	g++ setup.cpp -o setup
