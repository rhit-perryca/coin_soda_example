#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
const int soda_count = 8;
const int soda_cost = 3;
int coin_insert = 0;
int do_coin_release=0;
int soda_release = 0;
int sodas_due = 0;
int current_paid = 0;
pthread_mutex_t lock;
pthread_cond_t release_coins;
pthread_cond_t transaction_over;
pthread_cond_t release_can;
pthread_cond_t insert_coin;
void *coin(void *arg) {
  printf("Coin initialized\n");

  pthread_mutex_lock(&lock);
  while (coin_insert == 0) {
    pthread_cond_wait(&insert_coin, &lock);
  }
  coin_insert = 0;
  printf("Coin inserted\n");
  pthread_mutex_unlock(&lock);

  pthread_mutex_lock(&lock);
  if (current_paid < soda_cost) {
    while (do_coin_release==0)
      pthread_cond_wait(&release_coins, &lock);
  } else {
    do_coin_release=1;
    pthread_cond_broadcast(&release_coins);
  }
  printf("Coin released into change bin\n");
  current_paid--;
  if (current_paid == 0){
    soda_release = 1;
    do_coin_release=0;
    pthread_cond_signal(&release_can);
  }
  pthread_mutex_unlock(&lock);
  return NULL;
}
void *soda(void *arg) {
  printf("Soda initialized\n");

  pthread_mutex_lock(&lock);
  while (soda_release == 0){
    pthread_cond_wait(&release_can, &lock);
  }
  soda_release = 0;
  sodas_due--;
  printf("Soda dispensed\n");
  pthread_mutex_unlock(&lock);

  pthread_mutex_lock(&lock);
  if (sodas_due == 0) {
    printf("Transaction over(soda)\n\n");
    pthread_cond_signal(&transaction_over);
  } else {
    soda_release = 1;
    pthread_cond_signal(&release_can);
  }
  pthread_mutex_unlock(&lock);
  return NULL;
}
int main(int argc, char **argv) {
  pthread_cond_init(&release_coins, NULL);
  pthread_cond_init(&release_can, NULL);
  pthread_cond_init(&insert_coin, NULL);
  pthread_cond_init(&transaction_over, NULL);
  pthread_mutex_init(&lock, NULL);
  int coin_count = (soda_count/2) * soda_cost;
  pthread_t threads[soda_count + coin_count];
  int i = 0;
  for (; i < soda_count; i++) {
    pthread_create(&threads[i], NULL, soda, NULL);
  }
  for (; i < soda_count + coin_count; i++) {
    pthread_create(&threads[i], NULL, coin, NULL);
  }
  for (int i = 0; i < coin_count; i++) {
    usleep(500);
    pthread_mutex_lock(&lock);
    coin_insert = 1;
    current_paid++;
    if (soda_cost == current_paid) {
      sodas_due = 2;
    }
    pthread_cond_signal(&insert_coin);
    pthread_mutex_unlock(&lock);

    pthread_mutex_lock(&lock);
    if (sodas_due>0) {
      while (current_paid > 0 || sodas_due > 0) {
        pthread_cond_wait(&transaction_over, &lock);
      }
    }
    pthread_mutex_unlock(&lock);
  }
  for (int i = 0; i < coin_count + soda_count; i++) {
    pthread_join(threads[i], NULL);
  }
}
