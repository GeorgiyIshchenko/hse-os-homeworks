#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char* argv[]){
    if (argc <= 1){
        perror("writer.c: Not enough args\n");
    }

    printf("Writer enter\n");

    int fd = strtoll(argv[1], NULL, 10);

    size_t size;

    size = write(fd, "Hello, world!", 14);

    if (size != 14) {
      perror("Can\'t write all string to pipe\n");
      exit(-1);
    }

    if (close(fd) < 0) {
      perror("writer: Can\'t close writing side of pipe\n"); exit(-1);
    }

    printf("Writer exit\n");

    return 0;
}
