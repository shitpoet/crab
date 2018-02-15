#define FLUSH() \
  {while (copy <= prev_line_end) { \
    *dest++ = *copy++;            \
  }}

// insert char
// * modifies `src` ptr
// should be faster than PUT
#define PUT_CHAR(skip, ch)      \
  {const int add = 1;              \
  src += skip;                    \
  char* p = copy;                 \
  while (p < src + add) {         \
    *(p - add) = *p; p++;         \
  }                               \
  copy -= add;                    \
  src -= add;                     \
  *src = ch;                      \
  prev_line_end -= add;}

// insert some small string
// * modifies `src` ptr
#define PUT(skip, add, str)     \
  {src += skip;                    \
  char* p = copy;                 \
  while (p < src + add) {         \
    *(p - add) = *p; p++;         \
  }                               \
  copy -= add;                    \
  src -= add;                     \
  char* q = str;                  \
  for (int i = 0; i < add; i++)   \
    *src++ = *q++;                \
  prev_line_end -= add;}

/* rewrite

    for each line:
      -- analyzing the line --
      caculate indent size
      skip strings
      skip comments
      find start of line
      do some ad hoc patching for `if`,`elif`,`fun` etc
      find end of line
      -- flush --
      copy analyzed fragment of source to destination up to
      the end of previous line (so we still have place to add
      opening brace)
      -- adding braces --
      given: starts and ends of current and previous lines
             indents of current and previous lines
      if indent changed add opening brace on increase of indent
      or add closing brace on decrease of indent

      -- php mode --
      replace id with $id except for `fun id` and `id(...)`

*/
void FUNCNAME(char* dest, char* src, size_t n) {
  char* start = src;
  char* end = src + n;
  char* copy = src;
  char* prev_line_start = src; // after comments
  char* prev_line_end = src; // before comments
  /*char* prev_line_end = src+1; // before comments*/
  stack_t stack_base[64];
  stack_t* stack = stack_base;
  int tab = calc_base_tab(src, n);
  int prev_tab = tab;
  stack[0] = tab;
  //char* prev_line_cmnt_start = src; ///todo: for braces after `*` only, consier to delete this pointer

  /*dump_stack(stack_base, stack);*/

  bool cobra = false; // it is needed to add `}`
  bool prev_cobra = false; // for prev line

  char pc = '\0'; // prev char for cobra and shortcut expands
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

    //char* line_cmnt_start = nil;

    // scan the line:
    // - skip comments
    // - skip strings
    // - detect keywords `function`, `if`, `for` etc
    /*int sol = true;*/
    while (src < end) {
      c = *src;
      char c2 = *(src+1);
      if (c==' ') {
        src++;
      } else if (c=='\n') {
        break;
      } else if (c=='/' && c2=='/') { // lcmnt
        /*if (*(src+2)=='i' && *(src+3)=='f') {
          printf("IF CMNT\n");
          skip_indent = true;
        }*/
        //line_cmnt_start = src;
        while (src<end && c!='\n') {
          c = *++src;
        }
        break;
      } else if (c=='/' && c2=='*') { // bcmnt
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
      } else { // not ws and not cmnt

        if (c=='\'') { // single quote
          src++;
          while (src<end && *src!='\'') {
            src++;
          }
          //src++;
          //if (src<end) c = *src;
        } else if (c=='"') { // double quote
          src++;
          while (src<end && *src!='"') {
            src++;
          }
          //src++;
          //if (src<end) c = *src;
        } else if (c=='`') { // back trick
          src++;
          while (src<end && *src!='`') {
            src++;
          }
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

              if (four == *((uint32_t*)"let ")) {
                // insert `=` after id if there's value
                char* s = src+4;
                while (s<end && *s==' ') s++;
                while (s<end && (
                  *s>='a'&&*s<='z' || *s>='A'&&*s<='Z' ||
                  *s>='0'&&*s<='9' || *s=='_'||*s=='$'
                )) s++;
                if (s < end && *s==' ') {
                  while (s < end && *s==' ') s++;
                  if (s < end && *s!='=' && *s!=',') {
                    *(s-1)='=';
                  }
                }
              } else if (c=='e' && c2=='l' && c3=='i' && c4=='f') {

                if (c5==' ' && c6=='(') { // with `(`
                } else if (c5==' ') { // no `(`
  #ifdef EXPAND
                  PUT_CHAR(5,'(');
                  src-=5;
  #else
                  *(src+4)='(';
  #endif
                  cobra = true;
                }//*/

                PUT(2,3,"se ");
              /*} else if ((four & 0xffffff) == *((uint32_t*)"for\0")) {*/
              } else if (
                (four == *((uint32_t*)"for "))
                /*(four == *((uint32_t*)"for("))  */
              ) {
                if (c5!='(') {
  #ifdef EXPAND
                  PUT_CHAR(4, '('); src-=3;
  #else
                  *(src+3)='(';
  #endif
                  if (!(
                    (c5=='v' && c6=='a' && c7=='r' && c8<=' ') ||
                    (c5=='l' && c6=='e' && c7=='t' && c8<=' ')
                  )) {
                    PUT(4, 4, "let ");
                  }
                  cobra = true;
                }
              } else if (four == *((uint32_t*)"whil") && c5 == 'e') {
              //} else if (c=='w' && c2=='h' && c3=='i' && c4=='l' && c5=='e') {
                if (c6==' ' && c7=='(') { // with `(`
                } else if (c6==' ') { // no `(`
  #ifdef EXPAND
                  PUT_CHAR(6,'(');
  #else
                  *(src+5)='(';
  #endif
                  cobra = true;
                }
              } else if (c=='f'&&c2=='o'&&c3=='r'&&c4=='e'&&c5=='a'&&c6=='c'&&c7=='h' && !is_dash_id_subch(c8, *(src+8))) {
                if (c8==' ' && src+9 < end && *(src+8)!='(') {
                  *(src+7)='(';
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

            // expand some keyword shorthands even inside of the line!
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
                if (c3==' ' && c4=='(') { // with `(`
                } else if (c3==' ') { // no `(`
  #ifdef EXPAND
                  PUT_CHAR(3, '(');
  #else
                  *(src+2)='(';
  #endif
                  cobra = true;
                }
              } else if (c=='f' && c2=='u' && c3=='n' && (c4<=' ' || c4=='(')) {
                // `fun` -> `function`
                PUT(3, 5, "ction");
              } else if (c=='r' && c2=='e' && c3=='t' && c4==' ') {
                PUT(3, 3, "urn");
              }
            }
          }

#ifdef PHP_MODE
          // replace id-id with id_id to write ids in scheme style %)
          // replace .id with ['id'] to access arrays through dot operator
          // replace `id:` with `'id' =>` (for arrays)
          // add sigil ($) to ids, excluding `fun id` and `id(...` cases
          if (*src>='a' && *src<='z' || *src>='A'&&*src<='Z' || *src=='_') {
            char* p = src;
            if (
              *p>='a' && *p<='z' ||
              *p>='A' && *p<='Z' ||
              *p=='_'
            ) { // first sym is id-ish

              int id_len = 0;
              bool is_const = true; // skip CONSTANTS
              while (p < end && (
                *p>='a'&&*p<='z' ||
                *p>='A' && *p<='Z' ||
                *p>='0'&&*p<='9' ||
                *p=='_' || *p=='-' && *(p+1)!='>'
              )) {
                if (*p>='a' && *p<='z') is_const = false;
                //printf("%c \n", *p);
                if (*p=='-') *p='_';
                p++;
                id_len++;
              }

              // make GLOBALS, _SERVER etc as exceptions of constants
              if (is_const) {
                if (id_len >= 4) {
                  int n = sizeof(php_vars) / sizeof(php_vars[0]);
                  for (int i = 0; i < n; i++) {
                    char* var = php_vars[i];
                    int m = strlen(var);
                    if (id_len==m && memcmp(src, var, m)==0) {
                      is_const = false;
                      break;
                    }
                  }
                } else if (id_len <= 2) {
                  is_const = false;
                }
              }

              //if (p < end && *p != '(') {
              if (!is_const && p < end && *p != '(') {
                char idn = *p;
                if (idn==':') {
                  // replace `id:` with `'id' =>` (for arrays)
                  PUT_CHAR(0, '\'');
                  PUT_CHAR(id_len + 1, '\'');
                  PUT_CHAR(1, ' ');
                  src++;
                  *src = '=';
                  PUT_CHAR(1, '>');
                } else {
                  // add '$' before var names
                  int m = end - src;
                  bool is_obj_field = false;
                  // do not add '$' before field names in `obj->field`
                  if (src-2 > start && *(src-2)=='-' && *(src-1)=='>')
                    is_obj_field = true;
                  if (!is_obj_field) {
                    bool is_kw = false;
                    for (int i = 0; i < sizeof(php_kws)/sizeof(php_kws[0]); i++) {
                      char* kw = php_kws[i];
                      int kw_len = strlen(kw);
                      //printf("%s %d \n", kw, MIN(m, strlen(kw)));
                      if (m > kw_len) {
                        char cm = *(src + kw_len);
                        char cm2 = *(src + kw_len + 1);
                        //printf("%c \n", cm);
                        bool cm_is_id = (
                          cm>='a'&&cm<='z' ||
                          cm>='A'&&cm<='Z' ||
                          cm>='0'&&cm<='9' ||
                          cm=='-'&&cm2!='>' || cm=='_'
                        );
                        if ((cm<=' '||!cm_is_id) && strncmp(src, kw, kw_len)==0) {
                          is_kw = true;
                          break;
                        }
                      }
                    }
                    if (!is_kw) {
                      PUT_CHAR(0, '$');
                    }
                  }
                }
              }
            }
            src = p - 1; // skip the id
          } else if (*src == '.') {
            char c2 = *(src+1);
            if (c2>='a' && c2<='z' || c2=='_') {
              *src = '[';
              PUT_CHAR(1, '\'');
              src++;
              while (src < end && (*src>='a' && *src<='z' || *src>='0' && *src<='9' || *src=='_' || *src=='-' && *(src+1)!='>')) {
                if (*src=='-') *src='_';
                src++;
              }
              PUT_CHAR(0, '\'');
              PUT_CHAR(1, ']');
            }
          } else if (*src == '{') {
            // replace `{}` with `array()`
            *src='(';
            PUT(0, strlen("array"), "array");
          } else if (*src == '}') {
            *src = ')';
          } else if (*src=='<' && *(src+1)=='?') {
            if (*(src+2)!='p' && *(src+2)!='=') {
              // do not replace `<?php` and `<?=`
              // but otherwise add `php`
              PUT(2,3,"php");
            } else {
              src+=4; // skip `php` (so it does not became `$php`
            }
          }
#endif
        }

        if (c=='{' && cobra) { // add closing bracket
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
        if ( // when to add braces
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
          (prev_line_end_ch != '*' || (prev_line_start==prev_line_end) ) &&
          prev_line_end_ch != '/' &&
          prev_line_end_ch != '<' &&
          prev_line_end_ch != '>' &&
          prev_line_end_ch != '`' && // js template strings
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
          ) /*&& (
            !prev_line_cmnt_start ||
            memcmp(prev_line_cmnt_start, "//if", 4) != 0
          )*/
        ) {

          /*printf("prev line cmnt start %s\n", prev_line_cmnt_start);*/

          stack++;
          *stack = prev_tab;

          FLUSH();

          if (prev_cobra) {
            *dest++ = ')';
            prev_cobra = false;
          }

          *dest++ = ' ';
          *dest++ = '{';

          /*dump_stack(stack_base, stack);*/

        } else {
          /*printf("skip adding {\n");*/
          FLUSH();
        }

      } else if (tab < prev_tab) {

        prev_tab = tab;

        /*printf("dedent:\n");*/
        /*while (tab < *stack) { // && stack > stack_base ) {*/
        while (tab < *stack && stack > stack_base ) {
          --stack;
          FLUSH();

#ifdef EXPAND
          *dest++ = '\n';
          for (int t = 0; t < *(stack+1)-2; t++)
            *dest++ = ' ';
          *dest++ = '}';

          // fix braceless `else` being at new line
          char* f = copy;
          while (f < end && *f <= ' ') f++; // skip ws
          if (
            ( // "else"
              f[0]=='e' && f[1]=='l' && f[2]=='s' && f[3]=='e'
            ) && !( // exclude "elseFunc", "else123", "else_id" etc
              f[4]>='a'&&f[4]<='z' || f[4]>='A'&&f[4]<='Z' ||
              f[4]>='0'&&f[4]<='9' || f[4]=='_' || f[4]=='$'
            )
          ) {
            *(f-1) = ' ';
            copy = f-1;
          }
#else
          *dest++ = ' ';
          *dest++ = '}';
#endif
        }

      } else {
        FLUSH();
      }

      prev_line_start = line_start; ///todo: for braces after `*`, consider deleting this pointer
      prev_line_end = line_end;
    }
    /*prev_line_cmnt_start = line_cmnt_start;*/

    if (src >= end) break;

    if (cobra) {
      prev_cobra = true;
      cobra = false;
    }

    //printf("tab %d length %d \n",tab,len);
    if (c=='\n') src++;
    //pc = c;
  }

  /* finalize  */
  while (stack > stack_base) {
    --stack;
    FLUSH();
#ifdef EXPAND
    *dest++ = '\n';
    for (int t = 0; t < *(stack+1)-2; t++)
      *dest++ = ' ';
    *dest++ = '}';
#else
    *dest++ = ' ';
    *dest++ = '}';
#endif
  }
  while (copy < end) {
    *dest++ = *copy++;
  }
  *dest = 0;
}
