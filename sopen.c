#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#define DEBUG

typedef struct fsocket {
  pid_t pid;
  FILE *in, *out;
  int fd;
} fsocket;

fsocket *sopen(const char *program) {

  int fds[2];
  fsocket *sock;

  if ((sock = malloc(sizeof(struct fsocket))) == NULL) {
    return NULL;
  }
  if (socketpair(AF_UNIX, SOCK_STREAM, 0, fds) < 0) {
    free(sock);
    return NULL;
  }
  switch (sock->pid = vfork()) {
    case -1:
      close(fds[0]);
      close(fds[1]);
      free(sock);
      return NULL;
    case 0:
      close(fds[0]);
      dup2(fds[1], 0);
      dup2(fds[1], 1);
      close(fds[1]);
      execl("/bin/sh", "sh", "-c", program, NULL);
  }
  close(fds[1]);
  sock->in = fdopen(fds[0], "r");
  fds[1] = dup(fds[0]);
  sock->fd = fds[1];
  sock->out = fdopen(fds[1], "w");
  setvbuf(sock->out, NULL, _IONBF, 0);
  return sock;
}

int sclose(fsocket *sock) {

  int status;
  pid_t pid;

  pid = sock->pid;
  fclose(sock->in);
  fclose(sock->out);
  free(sock);
  while(waitpid(pid, &status, 0) < 0) {
    if (errno != EINTR) {
      return 0;
    }
  }
  if (WIFEXITED(status)) {
    return WEXITSTATUS(status);
  }
  return 1;
}

#ifdef DEBUG

int main(int argc, char const **argv) {

  fsocket *sock;
  char buffer[1000];

  sock = sopen("tr a-z A-Z");
  fprintf(sock->out, "testing\ntesting again\n");
  shutdown(sock->fd, SHUT_WR);
  while(fgets(buffer, 1000, sock->in)) {
    printf("%s", buffer);
  }
  sclose(sock);
  return 0;
}

#endif

FILE *sopen_old(const char *program)
{
    int fds[2];
    pid_t pid;

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fds) < 0)
      return NULL;
    switch(pid=vfork()) {
    case -1:    /* Error */
        close(fds[0]);
        close(fds[1]);
        return NULL;
    case 0:     /* child */
        close(fds[0]);
        dup2(fds[1], 0);
        dup2(fds[1], 1);
        close(fds[1]);
        execl("/bin/sh", "sh", "-c", program, NULL);
        _exit(127);
    }
    /* parent */
    close(fds[1]);
    return fdopen(fds[0], "r+");
}
