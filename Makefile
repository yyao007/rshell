all: bin/rshell bin/ls

bin/rshell:	src/rshell.cpp | bin
	g++ -Wall -Werror -ansi -pedantic src/rshell.cpp -o bin/rshell
bin/ls: src/ls.cpp | bin
	g++ -g -Wall -Werror -ansi -pedantic -ltermcap src/ls.cpp -o bin/ls
bin:
	mkdir bin
clean:
	rm -rf bin
