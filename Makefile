all: rshell
	mkdir bin
	mv ./rshell ./bin
rshell:
	g++ -Wall -Werror -ansi -pedantic -o rshell ./src/rshell.cpp

