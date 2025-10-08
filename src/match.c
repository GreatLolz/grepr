#include <stdio.h>
#include <stdlib.h>
#include <re.h>
#include <errno.h>

#define MATCH_BUFFER 10

typedef struct {
    int matchIndex;
    int matchLength;
} Match;

int findMatches(char *lineBuffer, re_t compiledPattern, Match **matches);

int findMatches(char *lineBuffer, re_t compiledPattern, Match **matches) {
    int count = 0;
    int capacity = MATCH_BUFFER;

    *matches = malloc(capacity * sizeof(Match));
    if (*matches == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    int offset = 0;
    int matchLength;
    int matchIndex;

    while ((matchIndex = re_matchp(compiledPattern, lineBuffer + offset, &matchLength)) != -1) {
        if (count >= capacity) {
            capacity *= 2;
            Match *tmp = realloc(*matches, capacity * sizeof(Match));
            if (tmp == NULL) {
                perror("realloc");
                exit(EXIT_FAILURE);
            }
            *matches = tmp;
        }

        (*matches)[count].matchIndex = offset + matchIndex;
        (*matches)[count].matchLength = matchLength;
        count++;
        offset += matchIndex + matchLength;
    }

    return count;
}