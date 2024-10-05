#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
  int fd[2];

  size_t size;

  if (pipe(fd) < 0) {
    printf("Can\'t open pipe\n");
    exit(-1);
  }

  if (fcntl(fd[1], F_SETFL, O_NONBLOCK) == -1) {
    perror("fcntl");
    close(fd[1]);
    return 1;
  }

  if (fcntl(fd[1], F_SETFL, O_NONBLOCK) == -1) {
    perror("fcntl");
    close(fd[1]);
    return 1;
  }

  int sizeInBytes = 1;

  for (;; ++sizeInBytes) {
    // Write

    size = write(fd[1], " ", 1);

    if (size != 1) {
      break;
    }
  }

  if (close(fd[1]) < 0) {
    printf("Can\'t close writing side of pipe\n");
    exit(-1);
  }

  printf("Max size: %d\n", sizeInBytes - 1);

  return 0;
}
