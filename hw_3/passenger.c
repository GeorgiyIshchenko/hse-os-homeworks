#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

int shipSemId;
struct sembuf *passangerSem;
struct sembuf *shipSem;

void *passangerEnter(void *args) {
  int i = *((int *)args);
  passangerSem->sem_op = 1;
  printf("PASSENGER %d is waiting!\n", i);
  if (semop(shipSemId, passangerSem, 1) < 0) {
    printf("PASSENGER: Can\'t go from pierce!\n");
    exit(-1);
  }
  shipSem->sem_op = -1;
  if (semop(shipSemId, shipSem, 1) < 0) {
    printf("PASSENGER: Can\'t get on board!\n");
    exit(-1);
  }
  printf("PASSENGER %d is going on board...\n", i);

  free((int *)args);

  return NULL;
}

int main(void) {
  // Getting ship, passangers, input semaphore
  char shipPathname[] = "dummy.txt";
  key_t shipKey;
  // SHIP | PASSANGERS | INPUT
  struct sembuf shipBuf[3];
  if ((shipKey = ftok(shipPathname, 0)) < 0) {
    perror("Can\'t generate ship key\n");
    exit(-1);
  }

  if ((shipSemId = semget(shipKey, 3, 0666 | IPC_CREAT)) < 0) {
    perror("Can\'t create semaphore ship set\n");
    exit(-1);
  }
  shipSem = shipBuf;
  passangerSem = shipBuf + 1;
  struct sembuf *inputSem = shipBuf + 2;
  shipSem->sem_num = 0;
  passangerSem->sem_num = 1;
  inputSem->sem_num = 2;
  shipSem->sem_flg = 0;
  passangerSem->sem_flg = 0;
  inputSem->sem_flg = 0;

  // Init shared memory
  int *array;
  int shmid;
  if ((shmid = shmget(shipKey, 2 * sizeof(int), 0666 | IPC_CREAT | IPC_EXCL)) <
      0) {
    if (errno != EEXIST) {
      perror("PASSANGER: Can\'t create shared memory\n");
      exit(-1);
    } else {
      if ((shmid = shmget(shipKey, 2 * sizeof(int), 0)) < 0) {
        perror("PASSANGER: Can\'t find shared memory\n");
        exit(-1);
      }
    }
  }

  int N;
  scanf("%d", &N);

  if ((array = (int *)shmat(shmid, NULL, 0)) == (int *)(-1)) {
    perror("PASSANGER: Can't attach shared memory\n");
    exit(-1);
  }

  // N has 0 index
  array[0] = N;

  inputSem->sem_op = -1;
  if (semop(shipSemId, inputSem, 1) < 0) {
    perror("PASSANGER: Can\'t decrease input sem!\n");
    exit(-1);
  }

  inputSem->sem_op = 0;
  if (semop(shipSemId, inputSem, 1) < 0) {
    perror("PASSANGER: Can\'t wait input sem!\n");
    exit(-1);
  }

  int K = array[1];

  printf("PASSANGERS: N=%d, K=%d\n", N, K);

  printf("PASSANGERS: init passengers with %d people...\n", 0);

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
    } else {
      // printf("Thread %d is joined.\n", i + 1);
    }
  }

  printf("PASSANGERS: ALL PROCESSES HAS JOINED!\n");

  free(th);

  // Wait for passengers to be empty
  passangerSem->sem_op = 0;
  if (semop(shipSemId, passangerSem, 1) < 0) {
    perror("PASSANGER: Can\'t wait passengers sem!\n");
    exit(-1);
  }

  printf("PASSANGERS: QUEUE IS EMPTY!\n");

  return 0;
}
