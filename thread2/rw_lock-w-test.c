#include "rw_lock.h"
//쓰기쓰레드가 lock획득을 위해 대기중일 때는 읽기쓰레드는 lock을 획득 할 수 없음
void init_rwlock(struct rw_lock * rw)
{
  //	Write the code for initializing your read-write lock.
  //    Write the code for initializing your read-write lock.
    pthread_mutex_init(&rw->mutex,NULL);
    pthread_mutex_init(&rw->lock,NULL);
    pthread_cond_init(&rw->read,NULL);
    pthread_cond_init(&rw->write,NULL);
    rw->Rcount =0;
    rw->Wcount =0;

}

void r_lock(struct rw_lock * rw)
{
  //	Write the code for aquiring read-write lock by the reader.
    if(rw->Wcount >0)
    {
      //write lock 있으면 끝날때까지 기다림
      pthread_cond_wait(&rw->write,&rw->lock);
    }
    pthread_mutex_lock(&rw->mutex);
    rw->Rcount++;
    pthread_mutex_unlock(&rw->mutex);

    pthread_mutex_lock(&rw->lock);
}

void r_unlock(struct rw_lock * rw)
{
  //	Write the code for releasing read-write lock by the reader.
    pthread_mutex_lock(&rw->mutex);
    rw->Rcount--;
    pthread_mutex_unlock(&rw->mutex);

	if(rw->Wcount >0)
    //write가 기다리고 있으면 시그널 보내줘서 ㄹ드 끝남을 알려줌
    pthread_cond_signal(&rw->read);
    pthread_mutex_unlock(&rw->lock);

}

void w_lock(struct rw_lock * rw)
{
  //	Write the code for aquiring read-write lock by the writer.
  //read lock들이 모두 끝날 떄 까지 기다림
    while(rw->Rcount >0)
    {
      pthread_cond_wait(&rw->read,&rw->lock);
    }
    //다른 쓰기 쓰레드도 끝나기를 기다려야 한다.
    while(rw->Wcount >0)
    {
      pthread_cond_wait(&rw->write,&rw->lock);
    }

    //Wcount ++
    pthread_mutex_lock(&rw->mutex);
    rw->Wcount++;
    pthread_mutex_unlock(&rw->mutex);

    //lock
    pthread_mutex_lock(&rw->lock);

}

void w_unlock(struct rw_lock * rw)
{
  //	Write the code for releasing read-write lock by the writer.
  //Wcount--
    pthread_mutex_lock(&rw->mutex);
    rw->Wcount--;
    pthread_mutex_unlock(&rw->mutex);

    if(rw->Rcount >0)
	  //read에게 시그널 보내주고 unlock
    pthread_cond_broadcast(&rw->read);
    //write에게도 끝났다는 시그널 보내준다.
	if(rw->Wcount >0)
    pthread_cond_signal(&rw->write);
    pthread_mutex_unlock(&rw->lock);

}
