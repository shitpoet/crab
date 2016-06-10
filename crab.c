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

#define ID 1
#define SYM 2
#define STR1 3
#define STR2 4
#define NUM 5
#define RE 6
#define SOL 64

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

//char* KT[7] = {"","ID","SYM","STR1","STR2","NUM","RE"}; // tok names

typedef struct _tok {
  char* s;
  int t;
  //int pri; // operand priority
  int pos;
  int len;
  //int ln;
  union {
    int n;  // string length, for symbols
    int op; // op code
  };
} tok_t;

//char tbuf[BUF_SIZE]; // buf for token strings
//char sbuf[BUF_SIZE]; // buf for symbol strings

#ifdef STATS
int start_stats[256];
//int end_stats[256];
#endif


tok_t* tok;

/*#define DONT_CURLY 4

typedef struct stack_t_ {
  int tab;
  int flags;
} stack_t;*/

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

void rewrite(char* dest, char* src, size_t n) {
  char* end = src + n;

  stack_t stack_base[64];
  stack_t* stack = stack_base;

  char* copy = src;
  //char* line_end = src;
  char* prev_line_end = src; // before comments

  int tab = calc_base_tab(src, n);
  stack[0] = tab;
  int prev_tab = tab;

  /*printf("base tab %d \n", tab);*/

  /*dump_stack(stack_base, stack);*/

  char cobra = false;
  char prev_cobra = false;

  char pc = '\0';
  while (src < end) {
    char c = *src;
    //char c2 = *(src+1);

    // calc indent
    tab = 0;
    while (src<end && c==' ') {
      tab++;
      c = *++src;
    }

    char* line_start = nil;
    char* line_end = nil;

    // scan the line:
    // - detect `function`, `if`, `for` etc
    // - skip comments
    /*int sol = true;*/
    while (src < end) {
      c = *src;
      char c2 = *(src+1);
      if (c=='/' && c2=='/') {
        while (src<end && c!='\n') {
          c = *++src;
        }
        break;
      } else if (c=='/' && c2=='*') {
        /*printf("multiline cmnt\n");*/
        src+=2;
        c = *src;
        c2 = *(src+1);
        while (src<end && (c!='*' || c2!='/')) {
          src++;
          c = *src;
          c2 = *(src+1);
        }
        src+=2;
      } else if (c=='\n') {
        break;
      } else if (c==' ') {
        src++;
      } else {
        if (src+7 < end) {
          if (line_end == nil) { // start of line
            uint32_t four = *(uint32_t*)src;
            char c3 = *(src+2);
            char c4 = *(src+3);
            char c5 = *(src+4);
            char c6 = *(src+5);
            char c7 = *(src+6);
            char c8 = *(src+7);


            if (c=='e' && c2=='l' && c3=='i' && c4=='f') {

              if (c5==' ' && c6=='(') { // with `(`
                /*printf("IF with (\n");*/
              } else if (c5==' ') { // no `(`
                /*printf("IF w/o (\n");*/
              //if (c3!='(' || c3<=' '&&c4!='(') { // no (
                *(src+4)='(';
                cobra = true;
              }//*/

            /*} else if ((four & 0xffffff) == *((uint32_t*)"for\0")) {*/
            } else if (
              (four == *((uint32_t*)"for "))
              /*(four == *((uint32_t*)"for("))  */
            ) {
              if (c5!='(') {

            /*} else if (c=='f' && c2=='o' && c3=='r') {*/
              //if (c4==' ' && c5=='(') { // with `(`
                /*printf("IF with (\n");*/
              //} else if (c4==' ') { // no `(`
                /*printf("IF w/o (\n");*/
              //if (c3!='(' || c3<=' '&&c4!='(') { // no (
                *(src+3)='(';
                if (!(
                  (c5=='v' && c6=='a' && c7=='r' && c8<=' ') ||
                  (c5=='l' && c6=='e' && c7=='t' && c8<=' ')
                )) {
                  char* p = copy;
                  while (p < src + 4 + 4) {
                    *(p - 4) = *p; p++;
                  }
                  copy -= 4;
                  *(src) = 'l';
                  *(src+1) = 'e';
                  *(src+2) = 't';
                  *(src+3) = ' ';
                  //*if (line_start != nil) line_start -= 4;
                  prev_line_end -= 4;
                  /*src -= 4;*/
                  //printf("%d\n", (int)(src-copy));
                }
                cobra = true;

              }

            } else if (four == *((uint32_t*)"whil") && c5 == 'e') {
            //} else if (c=='w' && c2=='h' && c3=='i' && c4=='l' && c5=='e') {
              if (c6==' ' && c7=='(') { // with `(`

              } else if (c6==' ') { // no `(`
                *(src+5)='(';
                cobra = true;
              }
            }
          } else {
            pc = *(src-1);
          }

          /*char c3 = *(src+2);
          char c4 = *(src+3);
          if (c=='f' && c2=='u' && c3=='n' && c4<=' ') {
            printf("%3d %c \n", pc, pc);
          }*/

          if (
            line_end == nil ||
            pc<=' ' || pc=='(' || pc=='!' || pc==':' || pc==','
          ) {

            char c3 = *(src+2);
            char c4 = *(src+3);
            char c5 = *(src+4);
            char c6 = *(src+5);
            char c7 = *(src+6);
            char c8 = *(src+7);

            if (c=='i' && c2=='f') {
              /*printf("fourcc  %d \n", four & 0xffff);*/
              /*printf("fourcc* %d \n", (*(uint32_t*)"if  ") & 0xffff);*/
              /*printf("IF\n");*/
              if (c3==' ' && c4=='(') { // with `(`
                /*printf("IF with (\n");*/
              } else if (c3==' ') { // no `(`
                /*printf("IF w/o (\n");*/
              //if (c3!='(' || c3<=' '&&c4!='(') { // no (
                *(src+2)='(';
                cobra = true;
              }//*/
            } else if (c=='f' && c2=='u' && c3=='n' && (c4<=' ' || c4=='(')) {
              // `fun` -> `function`

              char* p = copy;
              while (p < src + 3 + 5) {
                *(p - 5) = *p; p++;
              }
              copy -= 5;
              src -= 2;
              *(src) = 'c';
              *(src+1) = 't';
              *(src+2) = 'i';
              *(src+3) = 'o';
              *(src+4) = 'n';
              *(src+5) = c4; // sp or `(`
              //*if (line_start != nil) line_start -= 4;
              prev_line_end -= 5;

            }

          }

        }

        if (c=='{' && cobra) {
          *(src-1)=')';
          cobra = false;
        }
        if (line_start == nil) line_start = src;
        line_end = src;
        src++;
      }
    }

    if (line_end) { // != curr_line_end) {

      //*line_end!='('

      /*printf("curr indent %d -- ", tab);*/
      /*printf("prev line end: %c -- ", *prev_line_end);*/
      /*printf("new line end: %c \n", *line_end);*/

      if (
        (tab > prev_tab)
      ) {

        prev_tab = tab;
        /*printf("indent to %d\n", tab);*/


        char line_start_ch = *line_start;
        char prev_line_end_ch0 = *(prev_line_end-1);
        char prev_line_end_ch = *prev_line_end;

        //if (1) {
        if (
          prev_line_end_ch != '(' &&
          prev_line_end_ch != '{' &&
          prev_line_end_ch != '[' &&
          prev_line_end_ch != ',' &&
          prev_line_end_ch != '=' &&
          prev_line_end_ch != '?' &&
          prev_line_end_ch != ':' &&
          prev_line_end_ch != '&' &&
          prev_line_end_ch != '|' &&
          (prev_line_end_ch != '+' || prev_line_end_ch0=='+') &&
          (prev_line_end_ch != '-' || prev_line_end_ch0=='-') &&
          prev_line_end_ch != '*' &&
          prev_line_end_ch != '/' &&
          prev_line_end_ch != '<' &&
          prev_line_end_ch != '>' &&
          line_start_ch != '.' &&
          !(
            *(prev_line_end-0) == 'r' &&
            *(prev_line_end-1) == 'a' &&
            *(prev_line_end-2) == 'v' &&
            *(prev_line_end-3) <= ' '
          ) &&
          !(
            *(prev_line_end-0) == 't' &&
            *(prev_line_end-1) == 'e' &&
            *(prev_line_end-2) == 'l' &&
            *(prev_line_end-3) <= ' '
          )
        ) {

          stack++;
          *stack = prev_tab;

          //printf("copy: ");
          while (copy <= prev_line_end) {
            //printf("%c",*copy);
            *dest++ = *copy++;
          }
          //printf("\n");

          if (prev_cobra) {
            *dest++ = ')';
            prev_cobra = false;
          }

          *dest++ = ' ';
          *dest++ = '{';

          /*dump_stack(stack_base, stack);*/

        } else {
          /*printf("skip adding {\n");*/

          /*while (copy <= prev_line_end) {
            //printf("%c",*copy);
            *dest++ = *copy++;
          }*/

        }

      } else if (tab < prev_tab) {

        prev_tab = tab;

        /*printf("dedent:\n");*/
        while (tab < *stack) {
          --stack;
          //if (*stack
          /*prev_tab = *stack;*/
          /*printf("dedent to %d\n",prev_tab);*/

          //printf("copy: ");
          while (copy <= prev_line_end) {
            //printf("%c",*copy);
            *dest++ = *copy++;
          }
          //printf("\n");

          *dest++ = ' ';
          *dest++ = '}';

          /*dump_stack(stack_base, stack);*/
        }

      } else {
        //printf("copy: ");
        while (copy <= prev_line_end) {
          //printf("%c",*copy);
          *dest++ = *copy++;
        }
        //printf("\n");

      }

      prev_line_end = line_end;
    }

    if (src >= end) break;

    if (cobra) {
      prev_cobra = true;
      cobra = false;
    }

    //printf("tab %d length %d \n",tab,len);
    if (c=='\n') src++; else {
      /*printf("nl expected\n");*/
    }

    //pc = c;
  }

  while (stack > stack_base) {
    //printf("copy: ");
    while (copy <= prev_line_end) {
      //printf("%c",*copy);
      *dest++ = *copy++;
    }
    //printf("\n");
    *dest++ = ' ';
    *dest++ = '}';
    stack--;
  }

  //printf("last copy: ");
  while (copy < end) {
    //printf("%c",*copy);
    *dest++ = *copy++;
  }
  //printf("\n");
  *dest = 0;
}

/*#define pt_printf(...) \
  if(DBG_PT) printf(__VA_ARGS__)*/

/*#define pe_dbg(...) \
  if(DBG_PE) { \
    for (int dd = 0; dd < d; dd++) printf(" "); \
    printf(__VA_ARGS__); \
  }*/

// ansi colors: margenta 35
#define GRAY   "\x1b[0;37m"
#define WHITE  "\x1b[1;37m"
#define GREEN  "\x1b[1;32m"
#define CYAN   "\x1b[1;36m"
#define BLUE   "\x1b[1;34m"
#define YELLOW "\x1b[1;33m"
#define RED    "\x1b[1;31m"
#define PURPLE "\x1b[1;35m"

char obuf[BUF_SIZE];
char sbuf[BUF_SIZE];
char dbuf[BUF_SIZE];
//char mbuf[BUF_SIZE];

char* read_and_rewrite(char* fn) {
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
  rewrite(dbuf, sbuf, n);
  /*mint = mcstime() - mint;*/
  /*sprintf(sbuf, "\n\n read_and_rewrite: %d mcs \n", mint);*/
  return dbuf;
}

// t - debug tokens
// p - debug parser
// d - dump ast tree
// f - fail on errors
// no args - just print code

void main(int argc, char** argv) {
  /*printf("%d args\n", argc);*/
  char* opts = nil;
  bool bench = false;
  char* ifn = nil;
  char* ofn = nil;
  for (int i = 1; i < argc; i++) {
    char* arg = argv[i];
    if (arg[0]=='-') {
      opts = arg;
      bench = strchr(opts,'b')>0;
      /*printf("opts %s\n", arg);*/
    } else if (ifn == nil) {
      ifn = arg;
      /*printf("input file %s\n", arg);*/
    } else {
      ofn = arg;
      /*printf("output file %s\n", arg);*/
    }
    /*printf("(%d) %s\n", i, argv[i]);*/
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
      rewrite(dbuf, obuf, n);
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
    rewrite(dbuf, obuf, n);
    /*printf(":::::::::::::::::::::::::::::::::::::::::\n");*/
    /*printf(dbuf);*/

    if (argc > 3) {
      FILE* of = fopen(ofn, "w");
      fwrite(dbuf, strlen(dbuf), 1, of);
      fclose(of);
    }
  }

}

int test(int arg) {
  return 123;
}
