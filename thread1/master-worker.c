#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>
#include <unistd.h>

int item_to_produce, curr_buf_size;
int total_items, max_buf_size, num_workers, num_masters;
int consumed_item,out,in;
int count;

int *buffer; //SHARED VALUE

void print_produced(int num, int master) {
  printf("Produced %d by master %d\n", num, master);
}
void print_consumed(int num, int worker) {
  printf("Consumed %d by worker %d\n", num, worker);
}
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t buffer_has_space = PTHREAD_COND_INITIALIZER;
pthread_cond_t buffer_has_data = PTHREAD_COND_INITIALIZER;

//produce items and place in buffer
//modify code below to synchronize correctly
void *generate_requests_loop(void *data)
{
  int thread_id = *((int *)data);
  while(1)
    {
	  //lock
      pthread_mutex_lock(&mutex);
	  if(item_to_produce >= total_items)
	  {
	    pthread_cond_signal(&buffer_has_data);
		pthread_mutex_unlock(&mutex);
		break;
	  }
	  //버퍼가 꽉 차있다면 worker가 버퍼 비워주길 기다림 
	  if(count == max_buf_size && item_to_produce < total_items)
	  {
        pthread_cond_wait(&buffer_has_space, &mutex);
	  }
	  if(item_to_produce >= total_items)
	  {
	    pthread_cond_broadcast(&buffer_has_data);
		pthread_mutex_unlock(&mutex);
		break;
	  }
	  //set buffer full
	  for(int j=0;j<max_buf_size;j++){
	  in++; in %= max_buf_size;
      buffer[in] = item_to_produce;
      print_produced(item_to_produce, thread_id);
      item_to_produce++;
	  count++;
	  }//for
	  //printf("count:%d,produce_item:%d,id:%d\n",count,item_to_produce,thread_id);
	  //signal for consumer
	  pthread_cond_signal(&buffer_has_data);
      pthread_mutex_unlock(&mutex);
	  //unlock
    }
		//pthread_mutex_unlock(&mutex);
  //printf("worker%d out!\n",thread_id);
  return 0;
}
//워커 쓰레드가 실행 할 함수 작성--> 공유버퍼에서 항목을 제거하고 표준출력(소비)
void *do_worker_job(void *data)
{
  int thread_id = *((int *)data);
  int number;
  while(1)
  {

	usleep(1000);
	pthread_mutex_lock(&mutex);

	if(consumed_item >= total_items) 
	{
	  pthread_cond_signal(&buffer_has_space);
	  pthread_mutex_unlock(&mutex);
	  break;
	}

	//버퍼가 비어있으면 마스터가 채워줄때까지 기다림
	if((count == 0) &&(consumed_item < total_items))
	{
	  pthread_cond_wait(&buffer_has_data,&mutex);
	}

	//consume finished
	if(consumed_item >= total_items)
	{
	  pthread_cond_signal(&buffer_has_space);
	  pthread_mutex_unlock(&mutex);
	  break;
	}
	//set buffer empty
	for(int j=0; j<max_buf_size; j++)
	{
	out++; out %= max_buf_size;
	number = buffer[out];
	print_consumed(number,thread_id);
	count--;
	consumed_item++;
	//pthread_cond_signal(&buffer_has_space);
	}//for
	//printf("count :%d, consumed : %d, worker:%d\n",count,consumed_item,thread_id);
	pthread_cond_signal(&buffer_has_space);
	pthread_mutex_unlock(&mutex);
  }
 return 0;
}

//write function to be run by worker threads
//ensure that the workers call the function print_consumed when they consume an item

int main(int argc, char *argv[])
{
  int *master_thread_id;
  pthread_t *master_thread;
  int *worker_thread_id;
  pthread_t *worker_thread; //워커쓰레드번호 넣을 변수
  item_to_produce = 0;
  curr_buf_size = 0;
  consumed_item = 0;
  count = 0;
  out= -1;
  in = -1;
  
  int i;
  
   if (argc < 5) {
    printf("./master-worker #total_items #max_buf_size #num_workers #masters e.g. ./exe 10000 1000 4 3\n");
    exit(1);
  }
  else {
    num_masters = atoi(argv[4]);
    num_workers = atoi(argv[3]);
    total_items = atoi(argv[1]);
    max_buf_size = atoi(argv[2]);
  }
    

   buffer = (int *)malloc (sizeof(int) * max_buf_size);

   //create master producer threads
   master_thread_id = (int *)malloc(sizeof(int) * num_masters);
   master_thread = (pthread_t *)malloc(sizeof(pthread_t) * num_masters);
  for (i = 0; i < num_masters; i++)
    master_thread_id[i] = i;

   //create worker producer threads
   worker_thread_id = (int *)malloc(sizeof(int) * num_workers);
   worker_thread = (pthread_t *)malloc(sizeof(pthread_t) * num_workers);
  for (i = 0; i < num_workers; i++)
    worker_thread_id[i] = i;
  //마스터쓰레드 개수만큼 생성 
  for (i = 0; i < num_masters; i++)
    pthread_create(&master_thread[i], NULL, generate_requests_loop, (void *)&master_thread_id[i]);
  //create worker consumer threads
  //worker쓰레드 개수만큼 생성 
  for (i = 0; i < num_workers; i++)
    pthread_create(&worker_thread[i], NULL, do_worker_job, (void *)&worker_thread_id[i]);
  //wait for all threads to complete
  //wait for all threads to complete
  for (i = 0; i < num_workers; i++)
    {
      pthread_join(worker_thread[i], NULL);
      printf("worker %d joined\n", i);
    }
  for (i = 0; i < num_masters; i++)
    {
      pthread_join(master_thread[i], NULL);
      printf("master %d joined\n", i);
    }
  /*----Deallocating Buffers---------------------*/
  
  free(buffer);
  free(master_thread);
  free(worker_thread_id);
  free(worker_thread);
  
  return 0;
}
