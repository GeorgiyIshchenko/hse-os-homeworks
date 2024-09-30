#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
  int     fd[2], result;

  if (pipe(fd) < 0) {
    perror("Can\'t open pipe\n");
    exit(-1);
  }

  result = fork();

  if (result < 0) {
    perror("Can\'t fork child\n");
    exit(-1);
  } else if (result > 0) {

   /* Parent process */

    if (close(fd[1]) < 0) {
      perror("child: Can\'t close writing side of pipe\n"); exit(-1);
    }

    int exitCode = execl("reader.out", "reader.out", fd[0], NULL);

    if (exitCode) {
      perror("Parent exec reader.out with error!\n");
    }

  } else {

    /* Child process */

    if (close(fd[0]) < 0) {
      perror("parent: Can\'t close reading side of pipe\n"); exit(-1);
    }

    int exitCode = execl("writer.out", "writer.out", fd[1], NULL);

    if (exitCode) {
      perror("Child exec writer.out with error!\n");
    }

  }

  return 0;
}
