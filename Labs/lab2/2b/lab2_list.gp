#! /usr/bin/gnuplot
#
# purpose:
#  generate data reduction graphs for the multi-threaded list project
#
# input: lab2_list.csv
# 1. test name
# 2. # threads
# 3. # iterations per thread
# 4. # lists
# 5. # operations performed (threads x iterations x (ins + lookup + delete))
# 6. run time (ns)
# 7. run time per operation (ns)
# 8. Lock wait time
# output:
# lab2_list-1.png ... cost per operation vs threads and iterations
# lab2_list-2.png ... threads and iterations that run (un-protected) w/o failure
# lab2_list-3.png ... threads and iterations that run (protected) w/o failure
# lab2_list-4.png ... cost per operation vs number of threads
#
# Note:
# Managing data is simplified by keeping all of the results in a single
# file.  But this means that the individual graphing commands have to
# grep to select only the data they want.
#
#	Early in your implementation, you will not have data for all of the
#	tests, and the later sections may generate errors for missing data.
#

# general plot parameters
set terminal png
set datafile separator ","

#  total number of operations per second for each synchronization method
set title "List-1: Total number of operations per second for each synchronization method"
set xlabel "Threads"
set xrange [0:30]
set ylabel "Total number of Operations"
set logscale y 10
set output 'lab2b_1.png'

#grep out only non yielding -m and -s 
plot \
	"< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'Mutex synchronization method' with linespoints lc rgb 'red', \
	"< grep 'list-none-s,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'Spin lock synchronization method' with linespoints lc rgb 'green'

set title "List-2: Wait-for-lock time and the Average time per operation for Mutex"
set xlabel "Threads"
set xrange [0:30]
set ylabel "Time per Operations"
set logscale y 10
set output 'lab2b_2.png'

plot \
     "< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):($8) \
     title 'Wait for lock time' with linespoints lc rgb 'blue', \
     "< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):($7) \
     title 'Total time' with linespoints lc rgb 'green'

set title "List-3: Unprotected and Protected runs with lists"
set xlabel "Threads"
set xrange [0:4]
set ylabel "Iterations"
set logscale y 10
set output 'lab2b_3.png'

plot \
     "< grep 'list-id-none,[0-9]*,[0-9]*,4,' lab2b_list.csv" using (1):($3) \
     title 'Unprotected run' with points lc rgb 'blue', \
     "< grep 'list-id-s,[0-9]*,[0-9]*,4,' lab2b_list.csv" using (2):($3) \
     title 'Spin lock' with points lc rgb 'green', \
     "< grep 'list-id-m,[0-9]*,[0-9]*,4,' lab2b_list.csv" using (3):($3) \
     title 'Mutex lock' with points lc rgb 'red'


set title "List-4: Mutex throughput"
set xlabel "Threads"
set xrange [0:12]
set ylabel "Total operations"
set logscale y
set output 'lab2b_4.png'
set key left top
plot \
     "< grep -e 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
     title '1 List' with linespoints lc rgb 'blue', \
     "< grep 'list-none-m,[0-9]*,1000,4,' lab2b_list.csv" using ($2):(1000000000/($7)) \
     title '4 Lists' with linespoints lc rgb 'green', \
     "< grep 'list-none-m,[0-9]*,1000,8,' lab2b_list.csv" using ($2):(1000000000/($7)) \
     title '8 Lists' with linespoints lc rgb 'red', \
     "< grep 'list-none-m,[0-9]*,1000,16,' lab2b_list.csv" using ($2):(1000000000/($7)) \
     title '16 Lists' with linespoints lc rgb 'orange'

set title "List-5: Spin lock throughput"
set xlabel "Threads"
set xrange [0:12]
set ylabel "Total operations"
set logscale y
set output 'lab2b_5.png'
set key left top
plot \
     "< grep -e 'list-none-s,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
     title '1 List' with linespoints lc rgb 'blue', \
     "< grep -e 'list-none-s,[0-9]*,1000,4,' lab2b_list.csv" using ($2):(1000000000/($7)) \
     title '4 Lists' with linespoints lc rgb 'green', \
     "< grep -e 'list-none-s,[0-9]*,1000,8,' lab2b_list.csv" using ($2):(1000000000/($7)) \
     title '8 Lists' with linespoints lc rgb 'red', \
     "< grep -e 'list-none-s,[0-9]*,1000,16,' lab2b_list.csv" using ($2):(1000000000/($7)) \
     title '16 Lists' with linespoints lc rgb 'orange'
