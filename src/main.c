#include <stdio.h>
#include <stdlib.h>

char *pattern;
char *filename;

void handleArgs(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: grepr <pattern> <filename>");
        exit(1);
    }

    pattern = argv[1];
    filename = argv[2];
}

int main(int argc, char *argv[]) {
    handleArgs(argc, argv);

    printf("pattern: %s\nfilename: %s", pattern, filename);

    return 0;
}
