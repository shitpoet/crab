#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/time.h>
#include <stdint.h>

//#define STATS

char DBG_T = 0;
char DBG_PT = 0;
char DBG_PT_POOL = 0;
char DBG_PE = 1;
char FAIL = 0;

#define true 1
#define false 0

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

char tbuf[BUF_SIZE]; // buf for token strings
char sbuf[BUF_SIZE]; // buf for symbol strings

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

void rewrite(char* dest, char* src, size_t n) {
  char* end = src + n;

  stack_t stack_base[64];
  stack_t* stack = stack_base;

  char* copy = src;
  //char* line_end = src;
  char* prev_line_end = src; // before comments

  char tab = 0;

  char* pre = src;
  char pre_ch = *pre;
  while (pre<end && pre_ch==' ') {
    tab++;
    pre_ch = *++pre;
  }
  stack[0] = tab;
  int prev_tab = tab;

  /*printf("base tab %d \n", tab);*/

  /*dump_stack(stack_base, stack);*/

  char cobra = false;
  char prev_cobra = false;

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
            char c3 = *(src+2);
            char c4 = *(src+3);
            char c5 = *(src+4);
            char c6 = *(src+5);
            char c7 = *(src+6);
            char c8 = *(src+7);
            if (c=='i' && c2=='f') {
              /*printf("IF\n");*/
              if (c3==' ' && c4=='(') { // with `(`
                /*printf("IF with (\n");*/
              } else if (c3==' ') { // no `(`
                /*printf("IF w/o (\n");*/
              //if (c3!='(' || c3<=' '&&c4!='(') { // no (
                *(src+2)='(';
                cobra = true;
              }//*/
            } else if (c=='f' && c2=='o' && c3=='r') {
              if (c4==' ' && c5=='(') { // with `(`
                /*printf("IF with (\n");*/
              } else if (c4==' ') { // no `(`
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
            } else if (c=='w' && c2=='h' && c3=='i' && c4=='l' && c5=='e') {
              if (c6==' ' && c7=='(') { // with `(`

              } else if (c6==' ') { // no `(`
                *(src+5)='(';
                cobra = true;
              }
            }
          }

         char pc = *(src-1);
         if (pc<=' ') {

            char c3 = *(src+2);
            char c4 = *(src+3);
            char c5 = *(src+4);
            char c6 = *(src+5);
            char c7 = *(src+6);
            char c8 = *(src+7);

            if (c=='f' && c2=='u' && c3=='n' && c4<=' ') {
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
              *(src+5) = ' ';
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
          prev_line_end_ch != '+' &&
          prev_line_end_ch != '-' &&
          prev_line_end_ch != '*' &&
          prev_line_end_ch != '/' &&
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

void rewrite______(char* dbuf0, char* buf, size_t n) {
  //int ln = 1;
  char c0 = 0, c1, c, c2, c3, c4;
  int i0;
  int i = 0;
  char* id;
  memcpy(tbuf, buf, n);
  memcpy(sbuf, buf, n);

  //memset(tok, 0, sizeof(tok));

  memset(tok, 0, sizeof(tok_t)*n);
  //return 0;

  //char* dbuf0 = malloc(n*2);
  memset(dbuf0, 0, n*2);
  char* dbuf = dbuf0;

  char* src = buf;
  char* dest = dbuf;

  int indents[100];
  int* indent_stack = indents;
  int indent = 0;
  int last_indent = 0;
  int level = 0;

  int sol = true;
  uint32_t four;
  //tok_t* pretok = tok;
  tok_t* curtok = tok+1;
  //while (i < n) {
  while (src < buf+n) {
    char c = *src;

    *dest++ = c;
    src++;
  }

  while (0) {
    //i0 = i;
    //c0 = pretok->s ? pretok->s[0] : 0;

    /*c = buf[i];
    c2 = buf[i+1];
    c3 = buf[i+2];
    c4 = buf[i+3];*/

    /*four = *(uint32_t*)(&buf[i]);
    c = four & 0xff;
    c2 = (four >> 8) & 0xff;
    c3 = (four >> 16) & 0xff;
    c4 = (four >> 24) & 0xff;*/

    //curtok->ln = ln;
    //putchar(c);

#ifdef STATS
    start_stats[c]++;
#endif

    /*while (c==' ') {
      indent++;
      c = buf[++i];
    }*/
    // skip cmnts and ws

  }


  while (0) {
    if (c==' ') {
      if (sol) {
        indent = 0;
        while (c==' ') {
          indent++;
          c = buf[++i];
        }
        if (indent > last_indent) {
          last_indent = indent;
        }
      } else {
        *dbuf++ = c;
        i++;
        //if (sol) indent++;
      }
    } else if (c=='\n') {
      *dbuf++ = c;
      i++;
      if (!sol) {
        sol = 1;
        /*if (indent > last_indent) {
          last_indent = indent;
          *indent_stack++ = indent;
          level++;
          *dbuf++ = '{';
        }
        printf("%d \n", indent);*/
      }
      indent = 0;
    } else if (((c>='a')&&(c<='z'))||((c>='A')&&(c<='Z'))||(c=='_')||(c=='$')) {
      sol = 0;
      //state = ID;
      curtok->t = ID;
      curtok->s = &tbuf[i];
      int h = 0;
      while (i < n && (
        c>='a'&&c<='z' || c>='A'&&c<='Z' ||
        c>='0'&&c<='9' || c=='_' || c=='$'
      )) {
        h = h * 256 + h;
        *dbuf++ = c;
        c = buf[++i];
      }
      tbuf[i] = 0;
      //printf(" %s \n", curtok->s);

      if (h == ('f'*256+'u')*256+'n') {
        //patch(i0, i, "function", 8);
      }
      curtok++;
    } else if (c=='\'') {
      sol = 0;
      curtok->t = STR1;
      curtok->s = &tbuf[i+1];
      while ((i < n) && (c = buf[++i]) !='\'') {
      }
      tbuf[i++] = 0;
      strcat(dbuf,curtok->s);
      curtok++;
    } else if (c=='"') {
      sol = 0;
      curtok->t = STR2;
      curtok->s = &tbuf[i+1];
      do {
        c1 = c;
        c = buf[++i];
      } while (i < n && ((c1 == '\\') || (c != '"')));
      tbuf[i++] = 0;
      curtok++;
    } else if (
      c>='0' && c<='9' ||
      c=='0' && c2=='x' ||
      c=='.' && c2>='0' && c2<='9'
    ) {
      sol = 0;
      curtok->t = NUM;
      curtok->s = &tbuf[i];
      while ((i < n) && (
        ((c>='0')&&(c<='9')) || ((c>='a')&&(c<='f')) || ((c>='A')&&(c<='F')) || (c=='.') || (c=='x')
      )) {
        *dbuf++ = c;
        c = buf[++i];

      }
      tbuf[i] = 0;

      strcpy(dbuf,curtok->s);

      curtok++;
    } else if ((c == '/') && (c2 == '/')) { // skip // comment
      while (i<n && (c != 13) && (c != 10)) {
        tbuf[i] = 0;
        *dbuf++ = c;
        c = buf[++i];
      }
    } else if ((c=='/') && (c2=='*')) {
      do {
        i++;
        *dbuf++ = c;
        c = buf[i];
        c2 = buf[i+1];
        //if (c == 10) ln++;
      } while (i < n && ((c!='*') || (c2!='/')));
      *dbuf++ = c;
      *dbuf++ = c2;
      i+=2;
    } else if ((c == '/') && (
      c0=='=' || c0=='(' || c0=='[' ||
      c0==':' || c0=='&' || c0=='|'
    )) {
      sol = 0;
      curtok->t = RE;
      curtok->s = &tbuf[i];
      c2 = 0; c1 = 0;
      do {
        c2 = c1; c1 = c;
        c = buf[++i];
        if (c == '\\') i++;
      } while (i<n && c != '/');
      //} while (((c2!='\\')&&(c1 == '\\')) || (c != '/'));
      do {
        c = buf[++i];
      } while ((c=='g') || (c=='i') || (c=='m'));
      tbuf[i] = 0;
      curtok++;
    } else {
      sol = 0;
      //if (c=='*' && c2=='/') err("cmnt");
      curtok->t = SYM;
      curtok->s = &sbuf[i];
      if ((c4=='=')&&(c3=='>')&&(c2=='>')&&(c=='>')) {
        curtok->n = 4;
      } else if (
        (c3=='=')&&(
          (c2=='=')&&( (c=='=')||(c=='!') ) ||
          (c2=='>')&&(c=='>') || (c2=='<')&&(c=='<')
        ) || ( (c=='>')&&(c2=='>')&&(c3=='>') )
      ) {
        curtok->n = 3;
      } else if (
        ((c2=='=') && (
          (c=='=')||(c=='<')||(c=='>')||(c=='+')||(c=='-')||(c=='*')||(c=='/')||(c=='%')||(c=='&')||(c=='^')||(c=='|')||c=='!'
        )) ||
        ((c=='&')&&(c2=='&')) || ((c=='|')&&(c2=='|')) ||
        ((c=='+')&&(c2=='+')) || ((c=='-')&&(c2=='-')) ||
        ((c=='>')&&(c2=='>')) || ((c=='<')&&(c2=='<'))
        // dont merge '(' and ')' there
      ) {
        curtok->n = 2;
      } else {
        curtok->n = 1;
      }
      i += curtok->n;

      strncpy(dbuf, curtok->s, curtok->n);
      dbuf += curtok->n;

      curtok++;
    }

    tok_t* pretok = (curtok-1);
    //if (pretok == 0) err("pretok == NULL");
    //if (pretok->s == 0) err("pretok->s == NULL");
    //if (strlen(pretok->s) == 0) err("strlen(pretok->s) == NULL");
    if (pretok->s) c0 = (curtok-1)->s[0];
  }
  //return ((size_t)curtok - (size_t)tok)/sizeof(tok_t);
  //return ln;
  //return dbuf0;
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
//char mbuf[BUF_SIZE];

// t - debug tokens
// p - debug parser
// d - dump ast tree
// f - fail on errors
// no args - just print code

void main(int argc, char** argv) {
  char dump = 0;
  char dbg = argc > 2;
  if (argc > 2) {
    char* opts = argv[2];
    DBG_T = DBG_PT = strchr(opts,'t')>0;
    DBG_PE = strchr(opts, 'p')>0;
    dump = strchr(opts,'d')>0;
    FAIL = strchr(opts, 'f')>0;
  } else {
    DBG_T = DBG_PT = DBG_PT_POOL = DBG_PE = 0;
    dump = 0;
    FAIL = 1;
  }

  /*for (int i = 0; i < OPS; i++) {
    UNARY[i] = op_desc[2*i] & 1;
    STMNT[i] = op_desc[2*i] & 2;
    PRI[i] = op_desc[1+2*i];
  }*/

  FILE* f = fopen(argv[1], "r");
  if (!f) printf("io\n"), exit(1);
  fseek(f, 0, SEEK_END); // seek to end of file
  size_t n = ftell(f); // get current file pointer
  fseek(f, 0, SEEK_SET); // seek back to beginning of file
  fread(obuf, n, 1, f);
  fclose(f);

  /*puts(obuf);
  puts("\n=========================================\n");//*/

  char* dbuf = malloc(n*2);
  tok = malloc(sizeof(tok_t)*n);

  if (DBG_T) {

#ifdef STATS
    for (int j = 0; j < 256; j++) start_stats[j] = 0;
#endif

    uint64_t mint = mcstime();
    int runs = 100;
    for (int test = 1; test <= runs; test++) {
      uint64_t t0 = mcstime();
      rewrite(dbuf, obuf, n);
      uint64_t t = mcstime() - t0;
      if (t < mint) mint = t;
    }
    //printf("rewritten in %d mcs\n", t/runs);
    printf("\nrewritten in %d mcs (min)\n", mint);

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

    FILE* of = fopen("out.js", "w");
    fwrite(dbuf, strlen(dbuf), 1, of);
    fclose(of);
  }

}
