#ifndef _BINPRO_H_
#define _BINPRO_H_

struct InvokeInfo {
  char funcname[128];
  double ustart;
  double uend;
  double sstart;
  double send;
  double start;
  double end;
};
struct ProfInfo {
  int invokenum;
  struct InvokeInfo* invoke;
};

extern void instrumentProg(int argc, char* argv[], char* envp[]);
extern void getProfInfo(struct ProfInfo* profInfo);
#endif
