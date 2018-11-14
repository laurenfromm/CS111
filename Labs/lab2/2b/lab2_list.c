//NAME: Lauren Fromm
//EMAIL: laurenfromm@gmail.com
//ID: 404751250

#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <signal.h>
#include <stdlib.h>
#include "SortedList.h"
#include <pthread.h>
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
int listFlag = 0;
int listNum = 1;
int* listID;
int elementNum;
SortedList_t* list;
SortedListElement_t* elements;
struct timespec start, end;
struct timespec startLock, endLock;
int lockTime = 0;
int* spinLock = 0;
pthread_mutex_t* count_mutex;



void sig_handler()
{
  fprintf(stderr,"Segmentation fault caught");
  exit(2);
}

unsigned long hash(const char *key) {
  unsigned long val = 5381;
  int i = 0;
  for (; i < 3; i++)
    val = ((val << 5) + val) + key[i];
  return val;
}

void* threadFunc(void* arg)
{
  
  int i = *(int *) arg;
  if(debug)
    fprintf(stderr, "in threadfunc \n");
  for (; i < elementNum; i+=threadNum)
    {
      if(debug)
	fprintf(stderr, "in for\n");
      if(syncOp == 's')
	{
	  clock_gettime(CLOCK_MONOTONIC, &startLock);
	  while (__sync_lock_test_and_set(&spinLock[listID[i]], 1))
	    {	    
	      continue;
	
	    }
	  clock_gettime(CLOCK_MONOTONIC, &endLock);
	  lockTime += (BILLION * (endLock.tv_sec - startLock.tv_sec) + (endLock.tv_nsec - startLock.tv_nsec));
	}
      else if(syncOp == 'm'){ 
	if(debug)
	  fprintf(stderr, "in if\n");
	clock_gettime(CLOCK_MONOTONIC, &startLock);
	if(debug)
	  fprintf(stderr, "before\n");
	pthread_mutex_lock(&count_mutex[listID[i]]);
	if(debug)
	  fprintf(stderr, "after\n");
	clock_gettime(CLOCK_MONOTONIC, &endLock);
	lockTime += (BILLION * (endLock.tv_sec - startLock.tv_sec) + (endLock.tv_nsec - startLock.tv_nsec));
      }
      if(debug)
	fprintf(stderr, "after insert\n");
      SortedList_insert(&list[listID[i]], &elements[i]);
      if(debug)
	fprintf(stderr, "after insert\n");
      if(syncOp == 'm')
        pthread_mutex_unlock(&count_mutex[listID[i]]);
      if(syncOp == 's')
        __sync_lock_release(&spinLock[listID[i]]);

    }

  if(debug)
    fprintf(stderr, "length\n");

  int length;
  int it = 0;
  for(; it != listNum; it++)
    {
      if(syncOp == 's')
	{
	  clock_gettime(CLOCK_MONOTONIC, &startLock);
	  while (__sync_lock_test_and_set(&spinLock[it], 1))
	    {
	      continue;
	    }
	  clock_gettime(CLOCK_MONOTONIC, &endLock);
	  lockTime += (BILLION * (endLock.tv_sec - startLock.tv_sec) + (endLock.tv_nsec - startLock.tv_nsec));
	}
      else if(syncOp == 'm')
	{
	  clock_gettime(CLOCK_MONOTONIC, &startLock);
	  pthread_mutex_lock(&count_mutex[it]);
	  clock_gettime(CLOCK_MONOTONIC, &endLock);
	  lockTime += (BILLION * (endLock.tv_sec - startLock.tv_sec) + (endLock.tv_nsec - startLock.tv_nsec));
	}
      length += SortedList_length(&list[it]);

      if(syncOp == 'm')
	pthread_mutex_unlock(&count_mutex[it]);
      if(syncOp == 's')
	__sync_lock_release(&spinLock[it]);
    }


  int k = *(int *) arg;
  SortedListElement_t *del;
  if(debug)
    fprintf(stderr, "deleting elements\n");
  for(; k < elementNum; k +=threadNum)
    {
      if(syncOp == 's')
	{
	  clock_gettime(CLOCK_MONOTONIC, &startLock);
	  while (__sync_lock_test_and_set(&spinLock[listID[k]], 1))
	    {
	      continue;

	    }
	  clock_gettime(CLOCK_MONOTONIC, &endLock);
	  lockTime += (BILLION * (endLock.tv_sec - startLock.tv_sec) + (endLock.tv_nsec - startLock.tv_nsec));
	}
      else if(syncOp == 'm')
	{
	  clock_gettime(CLOCK_MONOTONIC, &startLock);
	  pthread_mutex_lock(&count_mutex[listID[k]]);
	  clock_gettime(CLOCK_MONOTONIC, &endLock);
	  lockTime += (BILLION * (endLock.tv_sec - startLock.tv_sec) + (endLock.tv_nsec - startLock.tv_nsec));
	}

      del = SortedList_lookup(&list[listID[k]], elements[k].key);
      SortedList_delete(del);

      if(syncOp == 'm')
	pthread_mutex_unlock(&count_mutex[listID[k]]);
      if(syncOp == 's')
	__sync_lock_release(&spinLock[listID[k]]);
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
    {"lists", required_argument, 0, 'l'},
    {0,0,0,0}
  };
  int c;
  while ((c = getopt_long(argc, argv, "", longopts, 0)) != -1)
    {
      size_t i = 0;
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
	  size_t len = strlen(optarg);
	  for (i = 0; i < len; i++)
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
	case 'l':
	  listFlag = 1;
	  listNum = atoi(optarg);
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

  if(syncOp == 'm')
    {
      count_mutex = malloc(listNum * sizeof(pthread_mutex_t));
    }
  else if (syncOp == 's')
    {
      spinLock = malloc(listNum * sizeof(int));
      
    }



  list = malloc(sizeof(SortedList_t));
  elementNum = threadNum * itNum;
  elements = malloc(elementNum * sizeof(SortedListElement_t));
  int k = 0;
  for(; k!= elementNum; k++)
    {
      int randNum = rand() % 26;
      char* randLet = malloc(2 * sizeof(char));
      randLet[0] = randNum +'A';
      randLet[1] = '\0';
      elements[k].key = randLet;
    }
  
  listID = malloc(elementNum * sizeof(int));
  int l = 0;
  for(; l != listNum; l++){
    list[l].key= NULL;
    list[l].next = &list[l];
    list[l].prev = &list[l];
    listID[l] = hash(elements[l].key) % listNum;
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
	  //free(threads);
	  //free(ids);
	  //free(list);
	  //free(elements);
	  //free(listID);
	  //if(syncOp == 's')
	    // free(spinLock);
	  //if(syncOp == 'm')
	    //free(count_mutex);
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
	  //free(threads);
	  //free(ids);
	  //free(elements);
	  //free(list);
	  //free(listID);
	  //if(syncOp == 's')
	    // free(spinLock);
	  //if(syncOp == 'm')
	    //    free(count_mutex);
          fprintf(stderr, "Error joining threads");
          exit(1);
        }
    }
  if(debug)
    fprintf(stderr, "after join\n");
  clock_gettime(CLOCK_MONOTONIC, &end);
  int length = SortedList_length(list);
  if(length != 0)
    {
      //      free(threads);
      // free(ids);
      //free(elements);
      //free(list);
      //free(listID);
      //      if(syncOp == 's')
      //	free(spinLock);
      //if(syncOp == 'm')
      //	free(count_mutex);
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

  int totalOps = threadNum * itNum * 3;
  long totalTime = BILLION * (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec);
  long avgTime = totalTime / totalOps;
  int waitPerLock = lockTime/ totalOps;
  printf("%s,%d,%d,%d,%d,%zu,%zu,%d\n", testName, threadNum, itNum, listNum,totalOps, totalTime, avgTime, waitPerLock);
  if(debug)
    fprintf(stderr,"after printf statement\n");
  // if(syncOp == 's')
    // free(spinLock);
    //if(syncOp == 'm')
    // free(count_mutex);
  //  free(threads);
  //free(ids);
  //free(list);
  //free(elements);
  //free(listID);
  exit(0);

}
