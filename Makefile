all: bin/rshell bin/ls

bin/rshell:	src/rshell.cpp | bin
	g++ -g -Wall -Werror -ansi -pedantic -std=c++11 src/rshell.cpp -o bin/rshell
bin/ls: src/ls.cpp | bin
	g++ -g -Wall -Werror -ansi -pedantic -std=c++11 src/ls.cpp -ltermcap -o bin/ls
bin:
	mkdir bin
clean:
	rm -rf bin
