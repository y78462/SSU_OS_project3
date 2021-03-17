#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include "SSU_Sem.h"

void SSU_Sem_init(SSU_Sem *s, int value) {
 
  s->value = value;
  pthread_mutex_init(&s->mutex1,NULL);
  pthread_mutex_init(&s->mutex2,NULL);
  pthread_cond_init(&s->cond,NULL);
}

void SSU_Sem_down(SSU_Sem *s) {

  pthread_mutex_lock(&s->mutex1);
  s->value--;
  pthread_mutex_unlock(&s->mutex1);
  if(s->value <0)
	//이 프로세스를 재움 큐에 추가(잠듬)
  {
	pthread_cond_wait(&s->cond,&s->mutex2);
  }
}

void SSU_Sem_up(SSU_Sem *s) {

  pthread_mutex_lock(&s->mutex1);
  s->value++;
  pthread_mutex_unlock(&s->mutex1);
  if(s->value<=0)
	//재움 큐로부터 프로세스를 제거(깨움)
  {
	pthread_cond_signal(&s->cond);
	pthread_mutex_unlock(&s->mutex2);
  }
}
