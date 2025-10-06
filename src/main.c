#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_BUFFER 512

void handleArgs(int argc, char *argv[], char **pattern, char **filename) {
    if (argc < 3) {
        printf("Usage: grepr <pattern> <filename>");
        exit(1);
    }

    *pattern = argv[1];
    *filename = argv[2];
}

int main(int argc, char *argv[]) {
    char *pattern;
    char *filename;
    handleArgs(argc, argv, &pattern, &filename);

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("grepr: Error opening file");
        return 1;
    }

    char lineBuffer[LINE_BUFFER];
    while (fgets(lineBuffer, LINE_BUFFER, file)) {
        if (strstr(lineBuffer, pattern))
            printf("%s", lineBuffer);
    }

    fclose(file);

    return 0;
}
