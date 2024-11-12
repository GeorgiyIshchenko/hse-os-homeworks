#include "info.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

int msqid; // IPC descriptor for the message queue

void* passangerEnter(void *args) {

  int i = *((int *)args);

  message mybuf;
  mybuf.mtype = 1;
  mybuf.info.pid = getpid();
  mybuf.info.type = ENTER_MESSAGE;

  if (msgsnd(msqid, (struct msgbuf *)&mybuf, MAXLEN, 1) < 0) {
    perror("Passanger: Can\'t send message to queue\n");
    exit(-1);
  }

  printf("Passanger %d waiting for the ship.\n", i);

  if (msgrcv(msqid, (struct msgbuf *)&mybuf, MAXLEN, mybuf.info.pid, 0) < 0) {
    perror("Passanger: Can\'t receive message from queue\n");
    exit(-1);
  }

  if (mybuf.info.type == GO_OUT_MESSAGE) {
    printf("Passanger %d went out from the ship.\n", i);
  }

  free((int *) args);

  return NULL;
}

int main() {

  char pathname[] =
      "ship.c"; // The file name used to generate the key.
                 // A file with this name must exist in the current directory.
  key_t key;     // IPC key

  if ((key = ftok(pathname, 0)) < 0) {
    printf("Passanger: Can\'t generate key\n");
    exit(-1);
  }

  if ((msqid = msgget(key, 0666 )) < 0) {
    printf("Passanger: Can\'t get msqid\n");
    exit(-1);
  }

  // INPUT
  int N, K;
  printf("Input N: ");
  scanf("%d", &N);
  printf("Input K: ");
  scanf("%d", &K);

  pthread_t *th = (pthread_t *)malloc(sizeof(pthread_t) * (N * K));
  for (int i = 0; i < K * N; i++) {
    int *temp = (int *)malloc(sizeof(int));
    *temp = i + 1;
    int result = pthread_create(th + i, NULL, passangerEnter, temp);
    if (result != 0) {
      printf("Error on thread create, return value = %d\n", result);
      exit(-1);
    }
  }

  for (int i = 0; i < K * N; i++) {
    if (pthread_join(th[i], (void **)NULL) != 0) {
      printf("Error joining thread: %d\n", i + 1);
      return 1;
    }
  }

  return 0;
}