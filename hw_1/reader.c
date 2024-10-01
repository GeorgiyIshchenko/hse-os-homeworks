#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char* argv[]){
    if (argc <= 1){
        perror("reader.c: Not enough args\n");
    }

    printf("Reader enter\n");

    int fd = strtol(argv[1], NULL, 10);

    size_t  size;

    char resstring[14];

    size = read(fd, resstring, 14);

    if (size < 0) {
      perror("Can\'t read string from pipe\n");
      exit(-1);
    }

    if (close(fd) < 0) {
      perror("reader: Can\'t close reading side of pipe\n"); exit(-1);
    }

    printf("Reader has read, string: %s\n", resstring);

    return 0;
}
