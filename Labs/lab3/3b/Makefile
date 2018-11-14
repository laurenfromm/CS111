#NAME: Lauren Fromm
#EMAIL: laurenfromm@gmail.com
#ID: 404751250

default: lab3b

lab3b:
	cp lab3b.py lab3b
	chmod +x lab3b lab3b.py

clean:
	rm -f *.tar.gz lab3b *.o

dist: clean default
	tar -czvf lab3b-404751250.tar.gz lab3b.py Makefile README
