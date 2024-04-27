#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
const int soda_count=5;
void * coin(void* arg){
  printf("Coin initialized\n");

  printf("Coin inserted\n");
  
  printf("Coin released into change bin\n");
  return NULL;
}
void *soda(void* arg){
  printf("Soda initialized\n");
  
  printf("Soda dispensed\n");
  return NULL; 
}
int main(int argc,char**argv){
  int coin_count=soda_count*2;
  pthread_t threads[soda_count+coin_count];
  int i =0;
  for(;i<soda_count;i++){
    pthread_create(&threads[i],NULL,soda,NULL); 
  }
  for(;i<soda_count+coin_count;i++){
    pthread_create(&threads[i],NULL,coin,NULL); 
  }
  for(int i =0;i<coin_count;i++){
    //TODO signal coin insertion here
  }
  for(int i = 0; i<coin_count+soda_count;i++){
    pthread_join(threads[i], NULL);
  }
}
