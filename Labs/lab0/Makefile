# NAME: Lauren Fromm
# Email: laurenfromm@gmail.com
# ID: 404751250

default: lab0.o
	gcc -g lab0.o -o lab0 -Wall -Wextra

clean:
	rm -f lab0 *.o *.txt *.tar.gz

check: 
	touch input.txt; \
	touch output.txt; \
	echo "TEST" > input.txt; \
	echo "Remove this" > output.txt; \
	./lab0 --input=input.txt --output=output.txt; \
	if [ $$? -ne 0 ]; \
	then \
		echo "Program did not finish successfully. Finished with return value $$?"; \
		exit 1; \
	fi ; \
	./lab0 --hi; \
	if [ $$? -ne 1 ]; \
	then \
		echo "Program did not exit with code 1. Finished with return value $$?"; \
		exit 1; \
	fi ; \
	chmod a-w output.txt ; \
	./lab0 --input=input.txt --output=output.txt ; \
	if [ $$? -ne 3 ] ; \
	then \
		echo "Program did not exit with code 3. Finished with return value $$?"; \
		exit 1;\
	fi;\
	rm -f output.txt; \
	chmod a-r input.txt; \
	./lab0 --input=input.txt --output=ouput.txt; \
	if [ $$? -ne 2 ]; \
	then \
		echo "Program did not exit with code 2. Finished with return value $$?"; \
		exit 1;\
	fi;
	./lab0 --input=nonexistant.txt --output=output.txt; \
	if [ $$? -ne 2 ] ; \
	then \
		echo "Program did not exit with code 3. Finished with return value $$?"; \
	fi;
	rm -f output.txt input.txt;
dist:
	tar -czvf lab0-404751250.tar.gz lab0.c Makefile README backtrace.png breakpoint.png
