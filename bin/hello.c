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

void foo(int num) {
  int i;
  clock_t t1, t2;
  struct tms time1, time2;
  printf("foo\n");
  t1 = times(&time1);
  volatile int uv = 0;
  for(i=0; i<num*1000; i++) {
    uv++;
  }
  for(i=0; i<num; i++) {
    int fd = open("temp.txt", O_CREAT, S_IWUSR);
    write(fd, &i, sizeof(i));
    close(fd);
    remove("temp.txt");
  }
  t2 = times(&time2);
  printf("%jd %jd %jd %jd %jd %jd\n", time1.tms_utime, time2.tms_utime, time1.tms_stime, time2.tms_stime, t1, t2);
}

void recursive(int i) {
  if(i<=0) {
    return;
  } else {
    foo(20);
    recursive(i-1);
  }
}

int main(void) {
  int i;
  printf("hello, world!\n");
  volatile int uv = 0;
  foo(200);
  for(i=0; i<200000; i++) {
    uv++;
  }
  for(i=0; i<200; i++) {
    int fd = open("temp.txt", O_CREAT, S_IWUSR);
    write(fd, &i, sizeof(i));
    close(fd);
    remove("temp.txt");
  }
  recursive(10);
  foo(200);
  return 0;
}
