#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
  int     fd[2], result;
  size_t  size;

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

    char resstring[13];

    size = read(fd[0], resstring, 13);

    if (size < 0) {
      perror("Can\'t read string from pipe\n");
      exit(-1);
    }

    if (close(fd[0]) < 0) {
      perror("parent: Can\'t close reading side of pipe\n"); exit(-1);
    }

    printf("Parent read, string: %s\n", resstring);

    int exitCode = execl("/bin/echo", "echo", resstring, NULL);

    if (exitCode) {
      perror("Parent exec cat with error!\n");
    }

  } else {

    /* Child process */

    if (close(fd[0]) < 0) {
      perror("parent: Can\'t close reading side of pipe\n"); exit(-1);
    }

    if (dup2(fd[1], STDOUT_FILENO) == -1) {
      perror("Dup2 stdout error!\n");
      close(fd[1]);
      exit(-1);
    }

    int exitCode = execl("/bin/echo", "echo", "SOME_MESSAGE", NULL);

    if (exitCode) {
      perror("Child exec echo with error!\n");
    }

  }

  return 0;
}
