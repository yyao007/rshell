all: bin/rshell bin/ls

bin/rshell:	src/rshell.cpp src/ls.cpp | bin
	g++ -Wall -Werror -ansi -pedantic src/rshell.cpp -o bin/rshell
bin/ls: src/ls.cpp | bin
	g++ -Wall -Werror -ansi -pedantic -std=c++11 src/ls.cpp -o bin/ls
bin:
	mkdir bin
clean:
	rm -rf bin
