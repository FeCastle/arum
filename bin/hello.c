#include <stdio.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

clock_t fake(struct tms* buf) {
  static clock_t utime = 10000;
  static clock_t stime =20000;
  static clock_t cutime = 30000;
  static clock_t cstime = 40000;
  static clock_t ttime = 50000;
  printf("fake[%p]: %ld\n", buf, utime);
  buf->tms_utime = utime++;
  buf->tms_stime = stime++;
  buf->tms_cutime = cutime++;
  buf->tms_cstime = cstime++;
  return ttime++;
}

void foo(void) {
  int i;
  volatile int uv = 0;
  for(i=0; i<4000000; i++) {
    uv++;
  }
  for(i=0; i<60; i++) {
    int fd = open("temp.txt", O_CREAT, S_IWUSR);
    write(fd, &i, sizeof(i));
    close(fd);
    remove("temp.txt");
  }
  printf("foo\n");
}

void recursive(int i) {
  if(i<=0) {
    return;
  } else {
    foo();
    recursive(i-1);
  }
  printf("recurseive\n");
}

int main(void) {
  foo();
  recursive(4);
  foo();
  printf("main\n");
  return 0;
}
