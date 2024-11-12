#include "info.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

void travel(int i) {
  printf("Ship: Starting travel %d ...\n", i);
  sleep(1);
  printf("Ship: Travel %d ended.\n", i);
}

int main() {

  int msqid; // IPC descriptor for the message queue
  char pathname[] =
      "ship.c"; // The file name used to generate the key.
                // A file with this name must exist in the current directory.
  key_t key;    // IPC key
  int len;      // Cycle counter and the length of the informative part of the
                // message

  if ((key = ftok(pathname, 0)) < 0) {
    perror("Server:Can\'t generate key\n");
    exit(-1);
  }

  if ((msqid = msgget(key, 0666 | IPC_CREAT | IPC_EXCL)) < 0) {
    perror("Server: Can\'t get msqid\n");
    exit(-1);
  }

  if (errno == EEXIST) {
    printf("Server exists\n");
    exit(-1);
  }

  message mybuf;

  // INPUT
  int N, K;
  printf("Input N:\n");
  scanf("%d", &N);
  printf("Input K:\n");
  scanf("%d", &K);

  pid_t* pids_buf = malloc(K * sizeof(pid_t)); 

  for (int i = 0; i < N; ++i) {
    printf("Ship: waiting for passangers\n");
    for (int j = 0; j < K; ++j) {

      if ((len = msgrcv(msqid, (struct msgbuf *)&mybuf, MAXLEN, 1, 0)) < 0) {
        printf("Server: Can\'t receive message from queue\n");
        exit(-1);
      }

      pids_buf[j] = mybuf.info.pid;
    }

    travel(i + 1);

    for (int j = 0; j < K; ++j) {
      mybuf.mtype = pids_buf[j];
      mybuf.info.type = GO_OUT_MESSAGE;
      printf("Ship: go out!\n");
      if (msgsnd(msqid, (struct msgbuf *)&mybuf, MAXLEN, pids_buf[j]) < 0) {
        printf("Server: Can\'t send message to queue\n");
        exit(-1);
      }
    }

  }

  return 0;
}
