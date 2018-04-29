
hs=$(shell ls *.h)

all: a.out

blowfish.o: blowfish.cpp
	g++ -c blowfish.cpp

blowfishStream.o: blowfishStream.cpp
	g++ -c blowfishStream.cpp

a.out: blowfish.o blowfishStream.o main.cpp $(hs)
	g++ -lncurses blowfish.o blowfishStream.o main.cpp
