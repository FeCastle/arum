/*
 * dinner_party version 1.00.0
 * Copyright 2012 - 2012 Dejun Qian - electronseu@gmail.com 
 *                                  - http://web.cecs.pdx.edu/~dejun
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct state {
  int total;
  int assigned;
  int rotate; // rotate left
  int score;
  int assignment[0]; 
};

// global variables
int people_num;
int* preference = NULL;


/*
 * return the gender of people p
 *     0 - female
 *     1 - male
 */
int g(int p) {
  return (p < people_num/2) ? 0 : 1;
}

/*
 * return the preference which indicates how p1 likes p2.
 */
int h(int p1, int p2) {
  return preference[p1*people_num+p2];
}

int score(struct state* ps) {
  int i;
  int s = 0;

  // opposite
  for(i=0; i<(ps->assigned/2); i++) {
    s += h(ps->assignment[i*2], ps->assignment[i*2+1]) + h(ps->assignment[i*2+1], ps->assignment[i*2]);
    s += (g(ps->assignment[i*2]) == g(ps->assignment[i*2+1])) ? 0 : 2;
  }

  // next to each other
  for(i=0; i<(ps->assigned/2-1); i++) {
    int p0 = i*2;
    int p1 = i*2+1;
    int p2 = i*2+2;
    int p3 = i*2+3;
    s += h(ps->assignment[p0], ps->assignment[p2]) + h(ps->assignment[p2], ps->assignment[p0]);
    s += (g(ps->assignment[p0]) == g(ps->assignment[p2])) ? 0 : 1;
    s += h(ps->assignment[p1], ps->assignment[p3]) + h(ps->assignment[p3], ps->assignment[p1]);
    s += (g(ps->assignment[p1]) == g(ps->assignment[p3])) ? 0 : 1;
  }

  return s;
}

void dump_state(struct state* ps) {
  int i;

  printf("dump_state:\n");
  printf("\ttotal = %d\n", ps->total);
  printf("\tassigned = %d\n", ps->assigned);
  printf("\trotate = %d\n", ps->rotate);
  printf("\tscore = %d\n", ps->score);
  printf("\t");
  for(i=0; i<ps->total; i++) {
    printf("%02d ", ps->assignment[i]);
  }
  printf("\n");
}

int main(int argc, char** argv) {
  int i;
  int score1;
  int temp;
  FILE* infile = NULL;
  FILE* outfile = NULL;

  if(argc == 2) {
    if(0!=strcmp(argv[1], "hw1-inst1.txt") && 0!=strcmp(argv[1], "hw1-inst2.txt") && 0!=strcmp(argv[1], "hw1-inst3.txt")) {
      printf("main: input file name not supported\n");
      goto quit_point;
    }
    infile = fopen(argv[1], "r+");
    if(infile == NULL) {
      printf("main: open input file (%s) failed\n", argv[1]);
      goto quit_point;
    }
    char outfilename[16];
    strcpy(outfilename, argv[1]);
    outfilename[4] = 's';
    outfilename[5] = 'o';
    outfilename[6] = 'l';
    outfilename[7] = 'n';
    outfile = fopen(outfilename, "r+");
    if(outfile == NULL) {
      printf("main: open output file (%s) failed\n", outfilename);
      goto quit_point;
    }
  } else {
    goto quit_point;
  }

  if(1 != fscanf(infile, "%d", &people_num)) {
    printf("main: read the number of people failed\n");
    goto quit_point;
  }
  preference = malloc(people_num*people_num*sizeof(int));
  if(preference == NULL) {
    printf("main: allocate memory failed\n");
    goto quit_point;
  }
  for(i=0; i<people_num*people_num; i++) {
    if(1 != fscanf(infile, "%d", preference+i)) {
      printf("main: read preference data failed\n");
      goto quit_point;
    }
  }

  fscanf(outfile, "%d", &score1);
  printf("score1 = %d\n", score1);

  struct state* pstate = malloc(sizeof(struct state) + people_num*sizeof(int));
  if(pstate == NULL) {
    printf("main: malloc for pstate failed\n");
    goto quit_point;
  }
  pstate->total = people_num;
  pstate->assigned = people_num;
  pstate->rotate = 0;
  for(i=0; i<pstate->total/2; i++) {
    fscanf(outfile, "%d", pstate->assignment+2*i);
    fscanf(outfile, "%d", &temp);
  }
  for(i=0; i<pstate->total/2; i++) {
    fscanf(outfile, "%d", pstate->assignment+2*i+1);
    fscanf(outfile, "%d", &temp);
  }
  pstate->score = score(pstate);
  dump_state(pstate);

  if(pstate->score == score1) {
    printf("verify successfully!\n");
  }

quit_point:
  if(pstate != NULL) {
    free(pstate);
  }
  if(preference != NULL) {
    free(preference);
  }
  if(outfile != NULL) {
    fclose(outfile);
  }
  if(infile != NULL) {
    fclose(infile);
  }

  return 0;
}
