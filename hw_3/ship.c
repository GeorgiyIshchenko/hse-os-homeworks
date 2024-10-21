#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

void travel(int i) {
  printf("SHIP: Starting travel %d ...\n", i);
  sleep(1);
  printf("SHIP: Travel %d ended.\n", i);
}

int main(void) {
  // Getting ship, passangers, input semaphore
  int shipSemId;
  char shipPathname[] = "dummy.txt";
  key_t shipKey;
  // SHIP | PASSANGERS | INPUT
  struct sembuf shipBuf[3];
  if ((shipKey = ftok(shipPathname, 0)) < 0) {
    printf("Can\'t generate ship key\n");
    exit(-1);
  }

  if ((shipSemId = semget(shipKey, 3, 0666 | IPC_CREAT)) < 0) {
    printf("Can\'t create semaphore ship set\n");
    exit(-1);
  }
  struct sembuf *shipSem = shipBuf;
  struct sembuf *passangerSem = shipBuf + 1;
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
      perror("SHIP: Can\'t create shared memory\n");
      exit(-1);
    } else {
      if ((shmid = shmget(shipKey, 2 * sizeof(int), 0)) < 0) {
        perror("SHIP: Can\'t find shared memory\n");
        exit(-1);
      }
    }
  }

  inputSem->sem_op = 2;
  if (semop(shipSemId, inputSem, 1) < 0) {
    perror("PASSANGER: Can\'t init input sem!\n");
    exit(-1);
  }

  int K;
  scanf("%d", &K);

  if ((array = (int *)shmat(shmid, NULL, 0)) == (int *)(-1)) {
    perror("SHIP: Can't attach shared memory\n");
    exit(-1);
  }

  // K has 1 index
  array[1] = K;

  inputSem->sem_op = -1;
  if (semop(shipSemId, inputSem, 1) < 0) {
    perror("SHIP: Can\'t decrease input sem!\n");
    exit(-1);
  }

  inputSem->sem_op = 0;
  if (semop(shipSemId, inputSem, 1) < 0) {
    perror("SHIP: Can\'t wait input sem!\n");
    exit(-1);
  }

  int N = array[0];

  printf("SHIP: N=%d, K=%d\n", N, K);

  shipSem->sem_op = N;
  if (semop(shipSemId, shipSem, 1) < 0) {
    printf("SHIP: Can\'t init board!\n");
    exit(-1);
  }

  printf("SHIP: init board with %d places...\n", N);

  for (int i = 0; i < K; ++i) {
    // Ждем пока корабль заполнится
    shipSem->sem_op = 0;
    printf("%d TRAVEL) SHIP: waitinig for passengers!\n", i + 1);
    if (semop(shipSemId, shipSem, 1) < 0) {
      printf("%d TRAVEL) SHIP: Can\'t wait for condition\n", i + 1);
      exit(-1);
    }

    //  Разгрузили пристань
    passangerSem->sem_op = -N;
    if (semop(shipSemId, passangerSem, 1) < 0) {
      printf("%d TRAVEL) SHIP: Can\'t get them out!\n", i + 1);
      exit(-1);
    }

    travel(i + 1);

    // Теперь разгружаем наш корабль (если последняя итерация - то не делаем
    // этого)
    printf("%d TRAVEL) SHIP: THROW PASSANGERS OUT!\n", i + 1);
    if (i != K - 1) {
      shipSem->sem_op = N;
      if (semop(shipSemId, shipSem, 1) < 0) {
        printf("%d TRAVEL) SHIP: Can\'t get them out!\n", i + 1);
        exit(-1);
      }
    }
  }

  // Wait for ship to be empty
  shipSem->sem_op = 0;
  if (semop(shipSemId, shipSem, 1) < 0) {
    perror("SHIP: Can\'t wait ship sem!\n");
    exit(-1);
  }

  printf("SHIP: SHIP IS EMPTY!\n");

  return 0;
}
