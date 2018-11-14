#NAME: Lauren Fromm
#EMAIL: laurenfromm@gmail.com
#ID: 404751250


default: lab4c_tcp lab4c_tls

lab4c_tcp:
	gcc -lmraa -lm -Wall -Wextra -g lab4c_tcp.c -o lab4c_tcp

lab4c_tls:
	gcc -lmraa -lm -lssl -lcrypto -Wall -Wextra -g lab4c_tls.c -o lab4c_tls

dist:
	tar -czvf lab4c-404751250.tar.gz lab4c_tls.c lab4c_tcp.c Makefile README

clean: 
	rm -f lab4c_tcp lab4c_tls  *.o *.txt *.tar.gz
