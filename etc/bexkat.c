/* x86s running Linux */ 
#include <string.h>

static char rcsid[] = "$Id$";

#ifndef LCCDIR
#define LCCDIR "/home/mstock/lcc/"
#endif

char *suffixes[] = { ".c", ".i", ".s", ".o", ".out", 0 };
char inputs[256] = "";
char *cpp[] = { "/usr/bin/cpp", "-D__STRICT_ANSI__", "-Dunix",
	"-D__unix__", "-D__linux__", "-D__signed__=signed",
	"$1", "$2", "$3", 0 };
char *include[] = {"-I" LCCDIR "include", 0 };
char *com[] = {LCCDIR "rcc", "-target=bexkat1/bexkat1000", "$1", "$2", "$3", 0 };
char *as[] = { "/usr/local/bin/bexkat1-elf-as",  "-o", "$3", "$1", "$2", 0 };
char *ld[] = {
	/*  0 */ "/usr/local/bin/bexkat1-elf-ld", 
                 "$1", "$2",
	/* 12 */ "-L" LCCDIR,
	0 };

extern char *concat(char *, char *);

int option(char *arg) {
  return 1;
}
