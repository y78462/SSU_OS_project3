#include <pthread.h>

typedef struct SSU_Sem {
 
  int value;
  pthread_mutex_t mutex1;
  pthread_mutex_t mutex2;
  pthread_cond_t cond;
  

} SSU_Sem;

void SSU_Sem_init(SSU_Sem *, int);
void SSU_Sem_up(SSU_Sem *);
void SSU_Sem_down(SSU_Sem *);
