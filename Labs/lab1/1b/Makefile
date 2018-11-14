#NAME: Lauren Fromm
#EMAIL: laurenfromm@gmail.com
#ID: 404751250

default: lab1b-client lab1b-server

lab1b-client: lab1b-client.o
	gcc -g lab1b-client.o -o lab1b-client -Wall -Wextra -lz

lab1b-server: lab1b-server.o
	gcc -g lab1b-server.o -o lab1b-server -Wall -Wextra -lz

clean:
	rm -f lab1b-client lab1b-server *.o *.txt *.tar.gz

dist:
	tar -czvf lab1b-404751250.tar.gz README Makefile lab1b-server.c lab1b-client.c
