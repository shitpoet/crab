.SUFFIXES: .c .o .h .a

ifdef GTK3
GTKVERSION=gtk+-3.0
else
GTKVERSION=gtk+-2.0
endif

INCLUDEDIRS=-Iscintilla/include
CXXFLAGS= -DGTK -DSCI_LEXER -W -Wall -Wno-unused-variable -Wno-unused-parameter -Wno-comment -Wno-unused-function
LEXEROBJS=$(wildcard scintilla/gtk/Lex*.o)

all: rite

.c.o:
	gcc -fPIC -std=c99 `pkg-config --cflags $(GTKVERSION)` $(INCLUDEDIRS) $(CXXFLAGS) -c $< -o $@
rite: rite.o $(LEXEROBJS) scintilla/bin/scintilla.a
	gcc -shared -DGTK $^ -o librite.so -lstdc++ `pkg-config --libs $(GTKVERSION) gthread-2.0` -lm -lgmodule-2.0
	# gcc -shared -DGTK $^ -o $@ -lstdc++ `pkg-config --libs $(GTKVERSION) gthread-2.0` -lm -lgmodule-2.0
clean:
	rm -rf librite.so *.o
