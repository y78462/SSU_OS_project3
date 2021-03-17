#include "rw_lock.h"
//read/write lock을 읽기쓰레드의 기본설정으로 구현 
// ---> 쓰기 쓰레드가 기다리는 중이라도, 추가적으로 읽기 쓰레드는 이전 읽기 쓰레드와 동시에 락 획득 가능
void init_rwlock(struct rw_lock * rw)
{
  //	Write the code for initializing your read-write lock.
   pthread_mutex_init(&rw->mutex,NULL);
   pthread_mutex_init(&rw->lock,NULL);
   pthread_cond_init(&rw->read,NULL);
   pthread_cond_init(&rw->write,NULL);
   rw->Rcount =0;
   rw->Wcount =0;
   printf("R%d W%d\n",rw->Rcount,rw->Wcount);
}

void r_lock(struct rw_lock * rw)
{
   pthread_mutex_lock(&rw->mutex);
   rw->Rcount++;
   pthread_mutex_unlock(&rw->mutex);
   printf("R%d W%d\n",rw->Rcount,rw->Wcount);

   pthread_mutex_lock(&rw->lock);
}

void r_unlock(struct rw_lock * rw)
{
  //	Write the code for releasing read-write lock by the reader.
   pthread_mutex_lock(&rw->mutex);
   rw->Rcount--;
   pthread_mutex_unlock(&rw->mutex);
   printf("R%d W%d\n",rw->Rcount,rw->Wcount);

   //write가 기다리고 있으면 시그널 보내줘서 ㄹ드 끝남을 알려줌
   pthread_cond_signal(&rw->write);
   pthread_mutex_unlock(&rw->lock);
}

void w_lock(struct rw_lock * rw)
{
   printf(" before : R%d W%d\n",rw->Rcount,rw->Wcount);
  //	Write the code for aquiring read-write lock by the writer.
  //read lock들이 모두 끝날 떄 까지 기다림
   while(rw->Rcount >0)
   {
     pthread_cond_wait(&rw->read,&rw->lock);
   }
   //다른 읽기 쓰레드도 끝나기를 기다려야 한다.
   while(rw->Wcount >0)
   {
	 pthread_cond_wait(&rw->write,&rw->lock);
   }

   //Wcount ++
   pthread_mutex_lock(&rw->mutex);
   rw->Wcount++;
   pthread_mutex_unlock(&rw->mutex);

   printf("after : R%d W%d\n",rw->Rcount,rw->Wcount);
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

   printf("R%d W%d\n",rw->Rcount,rw->Wcount);
   if(rw->Rcount >0)
   //read에게 시그널 보내주고 unlock
   pthread_cond_broadcast(&rw->read);
   if(rw->Wcount >0)
   //write에게도 끝났다는 시그널 보내준다.
   pthread_cond_signal(&rw->write);
   pthread_mutex_unlock(&rw->lock);
}
