#ifndef _attachdebugger
#define _attachdebugger

#include <stdio.h>


#define DEFAULT_DEBUG_PRINT_LEVEL 0 // overridable with MINET_DEBUGLEVEL env
#define DEFAULT_DEBUG_PRINT_FILE  stderr

void BreakHere();
void AttachDebuggerHere(char *execname=0);

void DEBUGSETLEVEL(int level);
void DEBUGSETFILE(FILE *file);
void DEBUGPRINTF(int level, char *fmt, ...);


#define STRINGIZE(T) #T
#endif
