#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[]){

    if (argc <= 1){
        perror("Not enough args!\n");
    }

    size_t size;
    int fd = atoi(argv[1]);

    const char *hwString = "Hello, World!";
    size = write(fd, hwString, 14);

    if (size != 14) {
      perror("Can\'t write all string to pipe\n");
      exit(-1);
    }

    if (close(fd) < 0) {
      perror("child: Can\'t close writing side of pipe\n"); exit(-1);
    }

    printf("Child writer.c: %s\n", hwString);

    return 0;
}
