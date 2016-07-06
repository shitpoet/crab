#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/time.h>
#include <stdint.h>

//#define STATS

#define true 1
#define false 0
#define bool char
#define nil NULL

#define BUF_SIZE (512*1024)

uint64_t mstime() {
  struct timeval  tv;
  gettimeofday(&tv, NULL);
  return (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;
}

uint64_t mcstime() {
  struct timeval  tv;
  gettimeofday(&tv, NULL);
  return (tv.tv_sec) * 1000000 + (tv.tv_usec);
}

void err(char* s){ puts(s); exit(1); }

#ifdef STATS
int start_stats[256];
//int end_stats[256];
#endif

typedef int stack_t;

void dump_stack(int* base, int* ptr) {
  printf("stack: ");
  while (ptr >= base) {
    printf("%d ", *ptr);
    ptr--;
  }
  printf("\n");
}

// base tab is whole file indent
// find first line with non-ws characters
// and return indent of it
int calc_base_tab(char* src, size_t n) {
  char* end = src + n;
  int tab;
  char ch = *src;
  do {
    tab = 0;
    while (src<end && ch==' ') {
      tab++;
      ch = *++src;
    }
    if (ch != '\n') break;
    while (src<end && ch=='\n') {
      ch = *++src;
    }
  } while (src<end);
  /*printf("base tab %d \n", tab);*/
  return tab;
}

// produce several versions of rewrite function

#define FUNCNAME rewrite_compact
#include "rewrite.c"

#undef FUNCNAME
#define FUNCNAME rewrite_expanded
#define EXPAND
#include "rewrite.c"

#undef FUNCNAME
#undef EXPAND

void rewrite(char* dest, char* src, size_t n, bool expanded) {
  if (expanded)
    rewrite_expanded(dest, src, n);
  else
    rewrite_compact(dest, src, n);
}

/*// ansi colors: margenta 35
#define GRAY   "\x1b[0;37m"
#define WHITE  "\x1b[1;37m"
#define GREEN  "\x1b[1;32m"
#define CYAN   "\x1b[1;36m"
#define BLUE   "\x1b[1;34m"
#define YELLOW "\x1b[1;33m"
#define RED    "\x1b[1;31m"
#define PURPLE "\x1b[1;35m"*/

char obuf[BUF_SIZE];
char sbuf[BUF_SIZE];
char dbuf[BUF_SIZE];
//char mbuf[BUF_SIZE];

char* read_and_rewrite(char* fn, bool expanded) {
  /*uint64_t mint = mcstime();*/
  FILE* f = fopen(fn, "r");
  if (!f) printf("io\n"), exit(1);
  fseek(f, 0, SEEK_END); // seek to end of file
  size_t n = ftell(f); // get current file pointer
  /*char* sbuf = malloc(n*4);*/
  fseek(f, 0, SEEK_SET); // seek back to beginning of file
  fread(sbuf, n, 1, f);
  fclose(f);
  sbuf[n] = 0;
  /*char* dbuf = sbuf + n + 1;*/
  rewrite(dbuf, sbuf, n, expanded);
  /*mint = mcstime() - mint;*/
  /*sprintf(sbuf, "\n\n read_and_rewrite: %d mcs \n", mint);*/
  return dbuf;
}

int main(int argc, char** argv) {
  printf("%d args\n", argc);
  char* opts = nil;
  bool bench = false;
  bool expanded = false;
  char* ifn = nil;
  char* ofn = nil;
  for (int i = 1; i < argc; i++) {
    char* arg = argv[i];
    if (arg[0]=='-') {
      opts = arg;
      bench = strchr(opts,'b')>0;
      expanded = strchr(opts,'e')>0;
      printf("opts %s\n", arg);
    } else if (ifn == nil) {
      ifn = arg;
      printf("input file %s\n", arg);
    } else {
      ofn = arg;
      printf("output file %s\n", arg);
    }
    printf("(%d) %s\n", i, argv[i]);
  }
  if (ifn==nil) {
    printf("usage: crab [-b] infile [outfile] \n");
    printf("  -b - bench \n");
    printf("  -e - expanded \n");
    return 1;
  }

  FILE* f = fopen(ifn, "r");
  if (!f) printf("io\n"), exit(1);
  fseek(f, 0, SEEK_END); // seek to end of file
  size_t n = ftell(f); // get current file pointer
  fseek(f, 0, SEEK_SET); // seek back to beginning of file
  fread(obuf, n, 1, f);
  fclose(f);

  /*puts(obuf);
  puts("\n=========================================\n");//*/

  char* dbuf = malloc(n*2);

  if (bench) {

#ifdef STATS
    for (int j = 0; j < 256; j++) start_stats[j] = 0;
#endif

    uint64_t mint = mcstime();
    int runs = 50;
    /*int runs = 100;*/
    /*int runs = 1000;*/
    for (int test = 1; test <= runs; test++) {
      uint64_t t0 = mcstime();
      if (expanded)
        rewrite_expanded(dbuf, obuf, n);
      else
        rewrite_compact(dbuf, obuf, n);
      uint64_t t = mcstime() - t0;
      if (t < mint) mint = t;
    }
    //printf("rewritten in %d mcs\n", t/runs);
    printf("\nrewritten in %.3f ms (min)\n", mint * 1.0 / 1000);

#ifdef STATS
    printf("start stats\n");
    int total = 0;
    for (int j = 0; j < 256; j++) {
      total += start_stats[j];
    }
    for (int j = 0; j < 256; j++) {
      printf("%d ('%c') %d%% \n", j, j, start_stats[j] * 100 / total );
    }
#endif
  } else {
    rewrite(dbuf, obuf, n, expanded);
    if (ofn) {
      FILE* of = fopen(ofn, "w");
      fwrite(dbuf, strlen(dbuf), 1, of);
      fclose(of);
    }
  }

}


