//NAME: Lauren Fromm
//EMAIL: laurenfromm@gmail.com
//ID: 404751250

#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <signal.h>
#include <stdlib.h>
#include "SortedList.h"
#include <time.h>

#define BILLION  1000000000L

int threadNum = 0;
int threadFlag = 0;
int itFlag = 0;
int itNum = 0;
int yieldFlag = 0;
int yieldl=0;
int yieldd=0;
int yieldi=0;
char* yieldOp;
int syncFlag = 0;
char syncOp;
int debug =0;
int opt_yield = 0;

int elementNum;
SortedList_t* list;
SortedListElement_t* elements;
struct timespec start, end;


volatile int lock = 0;
volatile int lock1 = 0;
volatile int lock2 = 0;
pthread_mutex_t count_mutex;



void sig_handler()
{
  fprintf(stderr,"Segmentation fault caught");
  exit(2);
}

void* threadFunc(void* arg)
{
  
  int i = *(int *) arg;
  if(debug)
    fprintf(stderr, "in threadfunc \n");
  for (i; i < elementNum; i+=threadNum)
    {
      if(syncOp == 's')
	{
	  while (__sync_lock_test_and_set(&lock, 1))
	    {	    
	      continue;
	
	    }
	}
      if(syncOp == 'm')
        pthread_mutex_lock(&count_mutex);

      SortedList_insert(list, &elements[i]);
      
      if(syncOp == 'm')
        pthread_mutex_unlock(&count_mutex);
      if(syncOp == 's')
        __sync_lock_release(&lock);

    }

  if(debug)
    fprintf(stderr, "length\n");

  int length;
  if(syncOp == 's')
    {
      while (__sync_lock_test_and_set(&lock1, 1));

   }
  else if(syncOp == 'm')
    pthread_mutex_lock(&count_mutex);
  
  length = SortedList_length(list);

  if(syncOp == 'm')
    pthread_mutex_unlock(&count_mutex);
  if(syncOp == 's')
    __sync_lock_release(&lock1);

  int k = *(int *) arg;
  SortedListElement_t *del;
  if(debug)
    fprintf(stderr, "deleting elements\n");
  for(k; k < elementNum; k +=threadNum)
    {
      if(syncOp == 's')
	{
	  while (__sync_lock_test_and_set(&lock2, 1))
	    continue;
	}
      else if(syncOp == 'm')
	pthread_mutex_lock(&count_mutex);

      del = SortedList_lookup(list, elements[k].key);
      SortedList_delete(del);

      if(syncOp == 'm')
	pthread_mutex_unlock(&count_mutex);
      if(syncOp == 's')
	__sync_lock_release(&lock2);
    }


  if(debug)
    fprintf(stderr, "leaving threadfunc\n");
  return NULL;
}


int main(int argc, char* argv[])
{
  struct option longopts[] = {
    {"threads", required_argument, 0, 't'},
    {"iterations",required_argument, 0, 'i'},
    {"yield", required_argument, 0, 'y'},
    {"debug", optional_argument, 0, 'd'},
    {"sync", required_argument, 0, 's'},
    {0,0,0,0}
  };
  int c;
  while ((c = getopt_long(argc, argv, "", longopts, 0)) != -1)
    {
      int i = 0;
      switch(c)
	{
	case 't':
	  threadFlag = 1;
	  threadNum = atoi(optarg);
	  break;
	case 'i':
	  itFlag = 1;
	  itNum =atoi(optarg);
	  break;
	case 'y':
	  yieldFlag = 1;
	  for (i; i != strlen(optarg); i++)
	    {    
	      if(optarg[i] == 'i')
	      {
		  yieldi = 1;
		  opt_yield |= INSERT_YIELD;
		}
	      else if (optarg[i] == 'd')
		{
		  yieldd=1;
		  opt_yield |= DELETE_YIELD;
		}
	      else if (optarg[i] == 'l')
		{
		  yieldl = 1;
		  opt_yield |= LOOKUP_YIELD;
		}
	      else
		{
		  fprintf(stderr, "Unrecognizes yield argument");
		  exit(1);
		}
	    }
	  if(yieldi && yieldd && yieldl)
	    yieldOp = "idl";
	  else if (yieldi && yieldd)
	    yieldOp = "id";
	  else if(yieldi && yieldl)
	    yieldOp = "il";
	  else if(yieldd && yieldl)
	    yieldOp ="dl";
	  else if(yieldi)
	    yieldOp ="i";
	  else if(yieldd)
	    yieldOp = "d";
	  else if(yieldl)
	    yieldOp = "l";

	  break;
	case 's':
	  syncFlag = 1;
	  if(optarg[0] == 's' || optarg[0] == 'm')
	    syncOp = optarg[0];
	  else
	    {
	      fprintf(stderr, "Unrecognized sync option");
	      exit(1);
	    }
	  break;
	case 'd':
	  debug = 1;
	  break;
	default:
	  fprintf(stderr, "Error unrecognized argument: %d\n", c);
	  exit(1);
	}
    }
  if(!itFlag)
    itNum = 1;
  if(!threadFlag)
    threadNum = 1;

  signal(SIGSEGV, sig_handler);



  list = malloc(sizeof(SortedList_t));
  list->key= NULL;
  list->next = list;
  list->prev =list;

  elementNum = threadNum * itNum;
  elements = malloc(elementNum * sizeof(SortedListElement_t));
  int k = 0;
  for(k; k!= elementNum; k++)
    {
      int randNum = rand() % 26;
      char* randLet = malloc(2 * sizeof(char));
      randLet[0] = randNum +'A';
      randLet[1] = '\0';
      elements[k].key = randLet;
    }
  pthread_t *threads = malloc(sizeof(pthread_t) * threadNum);
  int* ids = malloc(sizeof(int) * threadNum);




  clock_gettime(CLOCK_MONOTONIC, &start);
 
  int i;
  for(i=0; i != threadNum; i++)
    {
      ids[i] = i;
      if( pthread_create(&threads[i], NULL, threadFunc, &ids[i]) != 0)
        {
	  free(threads);
	  free(ids);
	  free(list);
	  free(elements);
          fprintf(stderr, "Error creating threads");
          exit(1);
        }
    }
  int j;
  for(j=0; j!= threadNum; j++)
    {
      if(debug)
	fprintf(stderr, "joining\n");
      if(pthread_join(threads[j], NULL) != 0)
        {
	  free(threads);
	  free(ids);
	  free(elements);
	  free(list);
          fprintf(stderr, "Error joining threads");
          exit(1);
        }
    }
  clock_gettime(CLOCK_MONOTONIC, &end);
  int length = SortedList_length(list);
  if(length != 0)
    {
      free(threads);
      free(ids);
      free(elements);
      free(list);
      fprintf(stderr, "Error: final count is not zero\n");
      exit(2);
    }

  char *testName;
  if(!yieldFlag)
    {
      if(!syncFlag)
	testName = "list-none-none";
      else if (syncOp == 's')
	testName = "list-none-s";
      else if (syncOp == 'm')
	testName = "list-none-m";

    }
  else
    {
      if(!syncFlag)
	{
	  if(yieldi && yieldd && yieldl)
            testName = "list-idl-none";
          else if (yieldi && yieldd)
            testName = "list-id-none";
          else if(yieldi && yieldl)
            testName = "list-il-none";
          else if(yieldd && yieldl)
            testName="list-dl-none";
          else if(yieldi)
            testName ="list-i-none";
          else if(yieldd)
            testName = "list-d-none";
          else if(yieldl)
            testName = "list-l-none";
	}
      else if (syncOp == 's')
	{
	  if(yieldi && yieldd && yieldl)
            testName = "list-idl-s";
          else if (yieldi && yieldd)
            testName = "list-id-s";
          else if(yieldi && yieldl)
            testName = "list-il-s";
          else if(yieldd && yieldl)
            testName="list-dl-s";
          else if(yieldi)
            testName ="list-i-s";
          else if(yieldd)
            testName = "list-d-s";
          else if(yieldl)
            testName = "list-l-s";
	}
      else if (syncOp == 'm')
	{
	  if(yieldi && yieldd && yieldl)
            testName = "list-idl-m";
          else if (yieldi && yieldd)
            testName = "list-id-m";
          else if(yieldi && yieldl)
            testName = "list-il-m";
          else if(yieldd && yieldl)
            testName="list-dl-m";
          else if(yieldi)
            testName ="list-i-m";
          else if(yieldd)
            testName = "list-d-m";
          else if(yieldl)
            testName = "list-l-m";
	}
    }

  int numlist = 1;
  int totalOps = threadNum * itNum * 3;
  long totalTime = BILLION * (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec);
  long avgTime = totalTime / totalOps;
  printf("%s,%d,%d,%d,%d,%d,%d\n", testName, threadNum, itNum, numlist,totalOps, totalTime, avgTime);
  free(threads);
  free(ids);
  free(list);
  free(elements);
  exit(0);

}
