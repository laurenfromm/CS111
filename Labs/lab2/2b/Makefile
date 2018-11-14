#NAME: Lauren Fromm
#EMAIL: laurenfromm@gmail.com
#ID: 404751250

default: lab2_list

lab2_list: lab2_list.o
	gcc -lpthread -Wall -Wextra -g SortedList.c lab2_list.o -o lab2_list

dist: graphs
	tar -czvf lab2b-404751250.tar.gz SortedList.h SortedList.c lab2_list.c Makefile lab2b_list.csv lab2b_1.png lab2b_2.png lab2b_3.png lab2b_4.png lab2b_5.png lab2_list.gp README profile.out

clean: 
	rm -f lab2_list *.o lab2b-404751250.tar.gz

tests: clean lab2_list
	rm -f lab2b_list.csv
	./lab2_list --threads=1   --iterations=1000 --sync=s >> lab2b_list.csv; \
	./lab2_list --threads=2   --iterations=1000 --sync=s >> lab2b_list.csv; \
	./lab2_list --threads=4	  --iterations=1000 --sync=s >> lab2b_list.csv; \
	./lab2_list --threads=8	  --iterations=1000 --sync=s >> lab2b_list.csv; \
	./lab2_list --threads=12  --iterations=1000 --sync=s >> lab2b_list.csv; \
	./lab2_list --threads=16  --iterations=1000 --sync=s >> lab2b_list.csv; \
	./lab2_list --threads=24  --iterations=1000 --sync=s >> lab2b_list.csv; \
	./lab2_list --threads=1	  --iterations=1000 --sync=m >> lab2b_list.csv; \
	./lab2_list --threads=2   --iterations=1000 --sync=m >> lab2b_list.csv; \
	./lab2_list --threads=4   --iterations=1000 --sync=m >> lab2b_list.csv; \
	./lab2_list --threads=8   --iterations=1000 --sync=m >> lab2b_list.csv; \
	./lab2_list --threads=12  --iterations=1000 --sync=m >> lab2b_list.csv; \
	./lab2_list --threads=16  --iterations=1000 --sync=m >> lab2b_list.csv; \
	./lab2_list --threads=24  --iterations=1000 --sync=m >> lab2b_list.csv; \

	#./lab2_list --threads=1   --iterations=1000 --sync=m >> lab2b_list.csv; \
	#./lab2_list --threads=2   --iterations=1000 --sync=m >> lab2b_list.csv; \
	#./lab2_list --threads=4   --iterations=1000 --sync=m >> lab2b_list.csv; \
	#./lab2_list --threads=8   --iterations=1000 --sync=m >> lab2b_list.csv; \
	#./lab2_list --threads=12  --iterations=1000 --sync=m >> lab2b_list.csv; \
	#./lab2_list --threads=16  --iterations=1000 --sync=m >> lab2b_list.csv; \
	#./lab2_list --threads=24  --iterations=1000 --sync=m >> lab2b_list.csv; 

	#./lab2_list --threads=1   --iterations=1 --lists=4  --yield=id >> lab2b_list.csv
	./lab2_list --threads=4   --iterations=1 --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=8   --iterations=1 --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=12   --iterations=1 --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=16   --iterations=1 --lists=4  --yield=id >> lab2b_list.csv; \
	#./lab2_list --threads=1   --iterations=2 --lists=4  --yield=id >> lab2b_list.csv
	./lab2_list --threads=4   --iterations=2 --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=8   --iterations=2 --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=12   --iterations=2 --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=16   --iterations=2 --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=1   --iterations=4 --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=4   --iterations=4 --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=8   --iterations=4 --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=12   --iterations=4 --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=16   --iterations=4 --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=1   --iterations=8 --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=4   --iterations=8 --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=8   --iterations=8 --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=12   --iterations=8 --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=16   --iterations=8 --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=1   --iterations=16 --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=4   --iterations=16 --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=8   --iterations=16 --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=12   --iterations=16 --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=16   --iterations=16 --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=1   --iterations=10 --sync=s --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=4   --iterations=10 --sync=s --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=8   --iterations=10 --sync=s --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=12   --iterations=10 --sync=s --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=16   --iterations=10 --sync=s --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=1   --iterations=20 --sync=s --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=4   --iterations=20 --sync=s --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=8   --iterations=20 --sync=s --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=12   --iterations=20 --sync=s --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=16   --iterations=20 --sync=s --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=1   --iterations=40 --sync=s --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=4   --iterations=40 --sync=s --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=8   --iterations=40 --sync=s  --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=12   --iterations=40 --sync=s  --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=16   --iterations=40 --sync=s  --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=1   --iterations=80 --sync=s --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=4   --iterations=80 --sync=s  --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=8   --iterations=80 --sync=s  --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=12   --iterations=80 --sync=s --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=16   --iterations=80 --sync=s --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=1   --iterations=10 --sync=m --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=4   --iterations=10 --sync=m --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=8   --iterations=10 --sync=m --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=12   --iterations=10 --sync=m --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=16   --iterations=10 --sync=m --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=1   --iterations=20 --sync=m --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=4   --iterations=20 --sync=m --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=8   --iterations=20 --sync=m --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=12   --iterations=20 --sync=m --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=16   --iterations=20 --sync=m --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=1   --iterations=40 --sync=m --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=4   --iterations=40 --sync=m --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=8   --iterations=40 --sync=m  --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=12   --iterations=40 --sync=m  --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=16   --iterations=40 --sync=m  --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=1   --iterations=80 --sync=m --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=4   --iterations=80 --sync=m  --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=8   --iterations=80 --sync=m  --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=12   --iterations=80 --sync=m --lists=4  --yield=id >> lab2b_list.csv; \
	./lab2_list --threads=16   --iterations=80 --sync=m --lists=4  --yield=id >> lab2b_list.csv; \

	#./lab2_list --threads=1   --iterations=1000 --lists=1 --sync=m >> lab2b_list.csv; \
	#./lab2_list --threads=2   --iterations=1000 --lists=1 --sync=m >> lab2b_list.csv; \
	#./lab2_list --threads=4   --iterations=1000 --lists=1 --sync=m >> lab2b_list.csv; \
	#./lab2_list --threads=8   --iterations=1000 --lists=1 --sync=m >> lab2b_list.csv; \
	#./lab2_list --threads=12  --iterations=1000 --lists=1 --sync=m >> lab2b_list.csv; 
	./lab2_list --threads=1   --iterations=1000 --lists=4 --sync=m >> lab2b_list.csv; \
	./lab2_list --threads=2   --iterations=1000 --lists=4 --sync=m >> lab2b_list.csv; \
	./lab2_list --threads=4   --iterations=1000 --lists=4 --sync=m >> lab2b_list.csv; \
	./lab2_list --threads=8   --iterations=1000 --lists=4 --sync=m >> lab2b_list.csv; \
	./lab2_list --threads=12  --iterations=1000 --lists=4 --sync=m >> lab2b_list.csv; \
	./lab2_list --threads=1   --iterations=1000 --lists=8 --sync=m >> lab2b_list.csv; \
	./lab2_list --threads=2   --iterations=1000 --lists=8 --sync=m >> lab2b_list.csv; \
	./lab2_list --threads=4   --iterations=1000 --lists=8 --sync=m >> lab2b_list.csv; \
	./lab2_list --threads=8   --iterations=1000 --lists=8 --sync=m >> lab2b_list.csv; \
	./lab2_list --threads=12  --iterations=1000 --lists=8 --sync=m >> lab2b_list.csv; \
	./lab2_list --threads=1   --iterations=1000 --lists=16 --sync=m >> lab2b_list.csv; \
	./lab2_list --threads=2   --iterations=1000 --lists=16 --sync=m >> lab2b_list.csv; \
	./lab2_list --threads=4   --iterations=1000 --lists=16 --sync=m >> lab2b_list.csv; \
	./lab2_list --threads=8   --iterations=1000 --lists=16 --sync=m >> lab2b_list.csv; \
	./lab2_list --threads=12  --iterations=1000 --lists=16 --sync=m >> lab2b_list.csv; \
	#./lab2_list --threads=1   --iterations=1000 --lists=1 --sync=s >> lab2b_list.csv; \
	#./lab2_list --threads=2   --iterations=1000 --lists=1 --sync=s >> lab2b_list.csv; \
	#./lab2_list --threads=4   --iterations=1000 --lists=1 --sync=s >> lab2b_list.csv; \
	#./lab2_list --threads=8   --iterations=1000 --lists=1 --sync=s >> lab2b_list.csv; \
	#./lab2_list --threads=12  --iterations=1000 --lists=1 --sync=s >> lab2b_list.csv; 
	./lab2_list --threads=1   --iterations=1000 --lists=4 --sync=s >> lab2b_list.csv; \
	./lab2_list --threads=2   --iterations=1000 --lists=4 --sync=s >> lab2b_list.csv; \
	./lab2_list --threads=4   --iterations=1000 --lists=4 --sync=s >> lab2b_list.csv; \
	./lab2_list --threads=8   --iterations=1000 --lists=4 --sync=s >> lab2b_list.csv; \
	./lab2_list --threads=12  --iterations=1000 --lists=4 --sync=s >> lab2b_list.csv; \
	./lab2_list --threads=1   --iterations=1000 --lists=8 --sync=s >> lab2b_list.csv; \
	./lab2_list --threads=2   --iterations=1000 --lists=8 --sync=s >> lab2b_list.csv; \
	./lab2_list --threads=4   --iterations=1000 --lists=8 --sync=s >> lab2b_list.csv; \
	./lab2_list --threads=8   --iterations=1000 --lists=8 --sync=s >> lab2b_list.csv; \
	./lab2_list --threads=12  --iterations=1000 --lists=8 --sync=s >> lab2b_list.csv; \
	./lab2_list --threads=1   --iterations=1000 --lists=16 --sync=s >> lab2b_list.csv; \
	./lab2_list --threads=2   --iterations=1000 --lists=16 --sync=s >> lab2b_list.csv; \
	./lab2_list --threads=4   --iterations=1000 --lists=16 --sync=s >> lab2b_list.csv; \
	./lab2_list --threads=8   --iterations=1000 --lists=16 --sync=s >> lab2b_list.csv; \
	./lab2_list --threads=12  --iterations=1000 --lists=16 --sync=s >> lab2b_list.csv

profile: lab2_list
	gcc -lpthread -lprofiler -g -Wall -Wextra lab2_list.c SortedList.c -o lab2_list
	LD_PRELOAD=/usr/lib64/libprofiler.so CPUPROFILE=./raw.gperf ./lab2_list --threads=12 --iterations=1000 --sync=s
	pprof --text ./lab2_list ./raw.gperf > profile.out
	pprof --list=threadFunc ./lab2_list ./raw.gperf >> profile.out

graphs: tests
	export PATH=/usr/local/cs/bin:$PATH; \
	gnuplot lab2_list.gp
