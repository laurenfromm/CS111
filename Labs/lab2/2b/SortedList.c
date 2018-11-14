//NAME:Lauren Fromm
//EMAIL: laurenfromm@gmail.com
//ID: 404751250


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "SortedList.h"


void SortedList_insert(SortedList_t *list, SortedListElement_t *element)
{
  
  if(list == NULL || element == NULL)
    return;
  SortedListElement_t* next = list->next;
  while(next != list)
    {
      if (strcmp(element->key, next->key) <= 0)
	{
	  if(opt_yield & INSERT_YIELD)
	    sched_yield();
	  next->prev->next = element;
	  element->prev = next->prev;
	  next->prev = element;
	  element->next = next;
	  break;
	}
      next = next->next;
    }

}


int SortedList_delete( SortedListElement_t *element)
{
  if(element == NULL)
    return 1;
  
  if(element->prev->next == element && element->next->prev == element)
    {
      if(opt_yield & DELETE_YIELD)
	sched_yield();
      element->prev->next = element->next;
      element->next->prev = element->prev;
      return 0;
    }
 
  return 1;
}

SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key)
{
  if(list == NULL || list->key == NULL)
    return NULL;
  
  SortedListElement_t* next = list->next;
  while(next != list)
  {
    if(strcmp(next->key,key))
	{
	  if(opt_yield & LOOKUP_YIELD)
	    sched_yield();
	  return(next);
	}
      next = next->next;
    }
  return NULL;
}


int SortedList_length(SortedList_t *list)
{
  if(list == NULL)
    return -1;
  int length = 0; 
  SortedListElement_t* next = list->next;
  while(next != list)
    {
      length++;
      if(opt_yield & LOOKUP_YIELD)
	sched_yield();
      next = next->next;
    }
  return length;
}
