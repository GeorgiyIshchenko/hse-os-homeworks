#include <sys/types.h>

#ifndef INFO_H
#define INFO_H

typedef struct {
  long mtype;
  struct {
    int type;
    pid_t pid;
  } info;
} message;

#define ENTER_MESSAGE 200
#define GO_OUT_MESSAGE 100
#define MAXLEN (sizeof(mybuf) - sizeof(long))

#endif
