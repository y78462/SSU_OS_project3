#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>
#include "SSU_Sem.h"

#define NUM_THREADS 3
#define NUM_ITER 10

SSU_Sem s[3];

void *justprint(void *data)
{
  int thread_id = *((int *)data);

  SSU_Sem_down(&s[thread_id]);
  for(int i=0; i < NUM_ITER; i++)
    {
      printf("This is thread %d\n", thread_id);
	  switch (thread_id)
	  {
		case 0 :
		  SSU_Sem_up(&s[1]);
		  SSU_Sem_down(&s[0]);
		  break;
		case 1:
		  SSU_Sem_up(&s[2]);
		  SSU_Sem_down(&s[1]);
		  break;
		case 2:
		  SSU_Sem_up(&s[0]);
		  SSU_Sem_down(&s[2]);
		  break;
	  }
	  if(i == NUM_ITER-1)
	  {
		exit(1);
	  }
    }
  return 0;
}

int main(int argc, char *argv[])
{

  pthread_t mythreads[NUM_THREADS];
  int mythread_id[NUM_THREADS];

  for(int i =0; i < NUM_THREADS; i++)
    {
	  SSU_Sem_init(&s[i],0);
	}
  
  for(int i =0; i < NUM_THREADS; i++)
    {
      mythread_id[i] = i;
      pthread_create(&mythreads[i], NULL, justprint, (void *)&mythread_id[i]);
	 // SSU_Sem_up(&s[i]);
    }

  SSU_Sem_up(&s[0]);
  for(int i =0; i < NUM_THREADS; i++)
    {
      pthread_join(mythreads[i], NULL);
    }
  
  return 0;
}
