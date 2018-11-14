#NAME: Lauren Fromm
#EMAIL: laurenfromm@gmail.com
#ID: 404751250

default: lab4b

lab4b:
	gcc -lmraa -lm -g -Wall -Wextra lab4b.c -o lab4b
clean:
	rm -f lab4b *.o *.txt *.tar.gz

check:
	./lab4b --bogus; \
	if [ $$? -ne 1 ]; \
	then \
		echo "Program did not return 1. Finished with return value $$?"; \
		exit 1; \
	fi ; \
	{ echo "START"; sleep 2; echo "STOP"; sleep 2; echo "OFF"; } | ./lab4b --period=2 --scale="C" --log=test.txt; \
	if [ $$? -ne 0 ]; \
	then \
		echo "Program did not exit with code 0. Finished with return value $$?"; \
		exit 1; \
	fi ; \

dist:
	tar -czvf lab4b-404751250.tar.gz lab4b.c README Makefile
