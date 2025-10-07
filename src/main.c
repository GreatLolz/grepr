#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdbool.h>

#define LINE_BUFFER 512

#define RED "\033[31m"
#define WHITE "\033[0m"

char *pattern;
int fileCount = 0;
char **filenames = NULL;

// flags
bool showLineIndex = false;
bool hideFileHeaders = false;
bool inverted = false;

void handleArgs(int argc, char *argv[]);
void printMatch(char *line);
void processLine(char *filename, char *line, int lineIndex);
void processFile(char *filename);

int main(int argc, char *argv[]) {
    handleArgs(argc, argv);

    for (int i = 0; i < fileCount; i++) {
        processFile(filenames[i]);
    }
    
    if (filenames != NULL) {
        free(filenames);
    }
    return 0;
}

void handleArgs(int argc, char *argv[]) {
    int opt;

    while ((opt = getopt(argc, argv, "nhv")) != -1) {
        switch (opt) {
            case 'n':
                showLineIndex = true;
                break;
            case 'h':
                hideFileHeaders = true;
                break;
            case 'v':
                inverted = true;
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

    // first non-option arg
    pattern = argv[optind];

    fileCount = argc - optind - 1;
    filenames = malloc(fileCount * sizeof(char *));
    if (filenames == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    // remaining non-option args
    for (int i = 0; i < fileCount; i++) {
        filenames[i] = argv[optind + i + 1];
    }
}

void processFile(char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "grepr: Error opening file %s: %s\n", filename, strerror(errno));
        exit(EXIT_FAILURE);
    }

    size_t lineBufferSize = LINE_BUFFER;

    char *lineBuffer = malloc(lineBufferSize);
    if (lineBuffer == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    
    int lineIndex = 1;
    while (fgets(lineBuffer, LINE_BUFFER, file)) {
        // check if buffer is full
        size_t length = strlen(lineBuffer);
        while (length == LINE_BUFFER - 1) {
            lineBufferSize *= 2;
            char *tmp = realloc(lineBuffer, lineBufferSize);
            if (tmp == NULL) {
                perror("malloc");
                exit(EXIT_FAILURE);
            }
            lineBuffer = tmp;
            fgets(lineBuffer + length, lineBufferSize - length, file);
            length = strlen(lineBuffer);
        }

        processLine(filename, lineBuffer, lineIndex);
        lineIndex++;
    }

    free(lineBuffer);
    fclose(file);
}

void processLine(char *filename, char *lineBuffer, int lineIndex) {
    bool hasMatch = strstr(lineBuffer, pattern) != NULL;

    if (hasMatch && !inverted || !hasMatch && inverted) {
        // print file headers
        if (fileCount > 1 && !hideFileHeaders) 
            printf("%s:", filename);
        // show line numbers
        if (showLineIndex) 
            printf("%d:", lineIndex);

        if (hasMatch && !inverted) {
            printMatch(lineBuffer);
        } else {
            printf("%s", lineBuffer);
        } 

        // add newline if missing
        if (lineBuffer[strlen(lineBuffer) - 1] != '\n')
            printf("\n");
    }
}

void printMatch(char *lineBuffer) {
    char *start = lineBuffer;
    char *match;
    // iterate through all matches in line
    while (match = strstr(start, pattern)) {
        fwrite(start, 1, match - start, stdout);
        printf("%s%s%s", RED, pattern, WHITE);
        start = match + strlen(pattern);
    }
    fputs(start, stdout);
}