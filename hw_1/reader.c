#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[]){

    if (argc <= 1){
        perror("Not enough args\n");
    }

    size_t size;
    int fd = atoi(argv[1]);
    char resstring[14];

    size = read(fd, resstring, 14);

    if (size < 0) {
      perror("Can\'t read string from pipe\n");
      exit(-1);
    }

    if (close(fd) < 0) {
      perror("parent: Can\'t close reading side of pipe\n"); exit(-1);
    }

    printf("Parent read, string: %s\n", resstring);

    return 0;
}