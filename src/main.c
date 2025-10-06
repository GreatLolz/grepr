#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdbool.h>

#define LINE_BUFFER 512

char *pattern;
char *filename;
bool showLineIndex = false;


void handleArgs(int argc, char *argv[]) {
    int opt;

    while ((opt = getopt(argc, argv, "n")) != -1) {
        switch (opt) {
            case 'n':
                showLineIndex = true;
                break;
            default:
                fprintf(stderr, "Usage: grepr [OPTIONS] <pattern> <filename>\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (argc - optind < 2) {
        fprintf(stderr, "Usage: grepr [OPTIONS] <pattern> <filename>\n");
        exit(EXIT_FAILURE);
    }

    pattern = argv[optind];
    filename = argv[optind + 1];
}

int main(int argc, char *argv[]) {
    handleArgs(argc, argv);

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("grepr: Error opening file");
        return 1;
    }

    char lineBuffer[LINE_BUFFER];
    int lineIndex = 1;
    while (fgets(lineBuffer, LINE_BUFFER, file)) {
        if (strstr(lineBuffer, pattern)) {
            if (showLineIndex) 
                printf("%d: ", lineIndex);
            printf("%s", lineBuffer);
        }
        lineIndex++;
    }

    fclose(file);

    return 0;
}
