void FUNCNAME(char* dest, char* src, size_t n) {
  char* end = src + n;
  char* copy = src;
  char* prev_line_end = src+1; // before comments
  stack_t stack_base[64];
  stack_t* stack = stack_base;
  int tab = calc_base_tab(src, n);
  int prev_tab = tab;
  stack[0] = tab;

  /*dump_stack(stack_base, stack);*/

  char cobra = false; // it is needed to add `}`
  char prev_cobra = false; // for prev line

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

    // scan the line:
    // - skip comments
    // - detect keywords `function`, `if`, `for` etc
    /*int sol = true;*/
    while (src < end) {
      c = *src;
      char c2 = *(src+1);
      if (c==' ') {
        src++;
      } else if (c=='\n') {
        break;
      } else if (c=='/' && c2=='/') {
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
                /*printf("IF with (\n");*/
              } else if (c5==' ') { // no `(`
                /*printf("IF w/o (\n");*/
              //if (c3!='(' || c3<=' '&&c4!='(') { // no (
                *(src+4)='(';
                cobra = true;
              }//*/

              int skip = 2;
              int add = 3;
              char* p = copy;
              while (p < src + skip + add) {
                *(p - add) = *p; p++;
              }
              copy -= add;
              src -= skip-1;
              *(src) = 's';
              *(src+1) = 'e';
              *(src+2) = ' ';
              /**(src+3) = 'i';*/
              /**(src+4) = 'f';*/
              /**(src+5) = '_';//c5; // sp or `(`*/
              //*if (line_start != nil) line_start -= 4;
              prev_line_end -= add;

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

            } else if (c=='r' && c2=='e' && c3=='t' && c4==' ') {

              int skip = 3; // 'ret'
              int add = 3;  // 'urn'
              char* p = copy;
              while (p < src + skip + add) {
                *(p - add) = *p; p++;
              }
              copy -= add;
              //src -= skip-1;
              *(src) = 'u';
              *(src+1) = 'r';
              *(src+2) = 'n';
              /**(src+3) = 'i';*/
              /**(src+4) = 'f';*/
              /**(src+5) = '_';//c5; // sp or `(`*/
              //*if (line_start != nil) line_start -= 4;
              prev_line_end -= add;

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
          ) /*&&
          prev_line_end > src+1*/
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
        /*while (tab < *stack) { // && stack > stack_base ) {*/
        while (tab < *stack && stack > stack_base ) {
          --stack;

          while (copy <= prev_line_end) {
            *dest++ = *copy++;
          }

#ifdef EXPAND_BRACES
          *dest++ = '\n';
          for (int t = 0; t < *(stack+1)-2; t++)
            *dest++ = ' ';
          *dest++ = '}';

          // fix braceless else being at new line
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
    while (copy <= prev_line_end) {
      *dest++ = *copy++;
    }
#ifdef EXPAND_BRACES
    *dest++ = '\n';
    for (int t = 0; t < *(stack+1)-2; t++)
      *dest++ = ' ';
    *dest++ = '}';
#else
    *dest++ = ' ';
    *dest++ = '}';
#endif
    stack--;
  }

  while (copy < end) {
    *dest++ = *copy++;
  }
  *dest = 0;
}
