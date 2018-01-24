#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>

#define ALLOC_SIZE 4096

int fdprintf(int fd, const char *format, ...) {

  static char *buffer = NULL;
  static int bufsiz = 0;
  char *newbuf;
  int len;
  va_list args;

  if (!bufsiz) {
    bufsiz = ALLOC_SIZE;
    if ((buffer = malloc(bufsiz)) == NULL) {
       return -1;
    }
  }
  va_start(args, format);
  len = vsnprintf(buffer, bufsiz, format, args);
  va_end(args);
  if ((len + 1) > bufsiz) {
    len++;
    bufsiz = (len & ~ALLOC_SIZE) + ((len & ALLOC_SIZE) ? ALLOC_SIZE : 0);
    if ((newbuf = realloc(buffer, bufsiz)) == NULL) {
       return -1;
    }
    buffer = newbuf;
    va_start(args, format);
    len = vsnprintf(buffer, bufsiz, format, args);
    va_end(args);
  }
  write(fd, buffer, len);
  return len;
}

int fdprintf_r(int fd, const char *format, ...) {

  char *buffer = NULL;
  int bufsiz = ALLOC_SIZE;
  char *newbuf;
  int len;
  va_list args;

  if ((buffer = malloc(bufsiz)) == NULL) {
    return -1;
  }
  va_start(args, format);
  len = vsnprintf(buffer, bufsiz, format, args);
  va_end(args);
  if ((len + 1) > bufsiz) {
    len++;
    if ((newbuf = realloc(buffer, len)) == NULL) {
      free(buffer);
      return -1;
    }
    buffer = newbuf;
    va_start(args, format);
    len = vsnprintf(buffer, bufsiz, format, args);
    va_end(args);
  }
  write(fd, buffer, len);
  free(buffer);
  return len;
}
