#include <re.h>

#define MATCH_BUFFER 10

typedef struct {
    int matchIndex;
    int matchLength;
} Match;

int findMatches(char *lineBuffer, re_t compiledPattern, Match **matches);
