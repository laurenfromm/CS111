//NAME: Lauren Fromm
//EMAIL: laurenfromm@gmail.com
//ID: 404751250

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h> 
#include <time.h>
#include <pthread.h>

#define BILLION  1000000000L

int debug = 0;
int threadFlag = 0;
int itFlag = 0;
int threadNum;
int itNum;
long long counter = 0;
char syncFlag;
struct timespec start, end;
pthread_mutex_t count_mutex;
int opt_yield;
volatile int lock = 0;
volatile int lock1 = 0;
void add(long long *pointer, long long value) {
  long long sum = *pointer + value;
  if (opt_yield)
    sched_yield();
  *pointer = sum;
}


void* threadFunc(void* pointer)
{
  pointer = (long long*) pointer;
  int i = 0;
  int j = 0;
  for(i; i != itNum; i++)
    {
      if(syncFlag == 'm')
	pthread_mutex_lock(&count_mutex);
      if(syncFlag == 's')
        while (__sync_lock_test_and_set(&lock, 1));

      if(syncFlag == 'c')
	{
          long long old;
          long long new;
          do{
            old= counter;
            new = old + 1;
          }while(__sync_val_compare_and_swap(&counter, old, new) != old);
        }
      else
	add(pointer,1);


      if(syncFlag == 's')
	__sync_lock_release(&lock);
      if(syncFlag == 'm')
	pthread_mutex_unlock(&count_mutex);
}

  for(j; j != itNum; j++)
    {
      if(syncFlag == 'm')
        pthread_mutex_lock(&count_mutex);
      if(syncFlag == 's')
	while (__sync_lock_test_and_set(&lock1, 1));

      if(syncFlag == 'c')
	{
	  long long old;
	  long long new;
	  do{
	    old = counter;
	    new = old - 1;
	  }while(__sync_val_compare_and_swap(&counter, old, new ) != old);
	}
      else
	add(pointer, -1);

      if(syncFlag == 's')
	__sync_lock_release(&lock1);
      if(syncFlag == 'm')
	pthread_mutex_unlock(&count_mutex);



    }





}
int main(int argc, char* argv[])
{
  struct option longopts[] = {
    {"threads", required_argument, 0, 't'},
    {"iterations", required_argument, 0, 'i'},
    {"yield", optional_argument, 0, 'y'},
    {"sync", required_argument, 0, 's'},
    {0, 0, 0, 0}
  };
  int c;
  while ((c=getopt_long(argc, argv, "", longopts, 0)) != -1)
    {
      switch(c)
	{
	case 'i': 
	  itFlag = 1;
	  itNum = atoi(optarg);
	  break;
	case 't':
	  threadFlag = 1;
	  threadNum = atoi(optarg);
	  break;
	case 'y':
	  opt_yield = 1;
	  break;
	case 's':
	  if(optarg[0] == 'c' || optarg[0] == 's' || optarg[0] == 'm')
	    syncFlag = optarg[0];
	  break;
	case 'd':
	  debug = 1;
	  break;
	default:
	  fprintf(stderr, "Argument not recognized: %d", c);
	  exit(1);
	  
	}

    }

  if(!itFlag)
    itNum = 1;
  if(!threadFlag)
    threadNum = 1;
  if(debug)
    fprintf(stderr, "Debug flag set");
  
  clock_gettime(CLOCK_MONOTONIC, &start);
  pthread_t threads[threadNum];
  int i;
  for(i=0; i != threadNum; i++)
    {
      if( pthread_create(&threads[i], NULL, threadFunc, &counter) == -1)
	{
	  fprintf(stderr, "Error creating threads");
	  exit(1);
	}

    }
  int j;
  for(j=0; j!= threadNum; j++)
    { 
      if(pthread_join(threads[j], NULL) == -1)
	{
	  fprintf(stderr, "Error joining threads");
	  exit(1);
	}
    }
  clock_gettime(CLOCK_MONOTONIC, &end);
  char* testName;
  if (opt_yield)
    {
      if(syncFlag == 'm')
	testName = "add-yield-m";
      else if(syncFlag == 's')
	testName = "add-yield-s";
      else if(syncFlag == 'c')
	testName = "add-yield-c";
      else
	testName = "add-yield-none";
    }
  else
    {
      if(syncFlag == 'm')
	testName = "add-m";
      else if(syncFlag == 's')
	testName = "add-s";
      else if(syncFlag == 'c')
	testName = "add-c";
      else
	testName ="add-none";
    }
  int totalOps = 2 * itNum * threadNum;
  long totalTime = BILLION * (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec);
  long avgTime = totalTime / totalOps;
  printf("%s,%d,%d,%d,%d,%d,%d\n", testName, threadNum, itNum, totalOps, totalTime, avgTime, counter);
  exit(0);

}
