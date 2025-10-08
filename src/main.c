#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdbool.h>
#include <re.h>
#include <errno.h>

#define LINE_BUFFER 512

#define RED "\033[31m"
#define WHITE "\033[0m"
#define BLUE "\033[34m"

typedef struct {
    char *pattern;
    re_t compiledPattern;
    int fileCount;
    char **filenames;
    bool showLineNumbers;
    bool hideFileHeaders;
    bool inverted;
} GreprConfig;

void handleArgs(int argc, char *argv[], GreprConfig *config);
void printHelp(void);
void processFile(char *filename, GreprConfig *config);
void processLine(char *filename, char *lineBuffer, int lineIndex, GreprConfig *config);
void printMatch(char *lineBuffer, re_t compiledPattern);

int main(int argc, char *argv[]) {
    GreprConfig config = {
        .pattern = NULL,
        .compiledPattern = NULL,
        .fileCount = 0,
        .filenames = NULL,
        .showLineNumbers = false,
        .hideFileHeaders = false,
        .inverted = false
    };

    handleArgs(argc, argv, &config);

    for (int i = 0; i < config.fileCount; i++) {
        processFile(config.filenames[i], &config);
    }
    
    if (config.filenames != NULL) {
        free(config.filenames);
    }
    return 0;
}

void handleArgs(int argc, char *argv[], GreprConfig *config) {
    int opt;

    static struct option longOptions[] = {
        {"help", no_argument, 0, 0},
        {0, 0, 0, 0}
    };

    int longIndex = 0;
    while ((opt = getopt_long(argc, argv, "nhv", longOptions, &longIndex)) != -1) {
        switch (opt) {
            case 0:
                if (longIndex == 0) {
                    printHelp();
                }
            case 'n':
                config->showLineNumbers = true;
                break;
            case 'h':
                config->hideFileHeaders = true;
                break;
            case 'v':
                config->inverted = true;
                break;
            default:
                fprintf(stderr, "Usage: grepr [OPTIONS] <pattern> <filename>...\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (argc - optind < 2) {
        fprintf(stderr, "Usage: grepr [OPTIONS] <pattern> <filename>...\n");
        exit(EXIT_FAILURE);
    }

    // first non-option arg
    config->pattern = argv[optind];
    config->compiledPattern = re_compile(config->pattern);

    config->fileCount = argc - optind - 1;
    config->filenames = malloc(config->fileCount * sizeof(char *));
    if (config->filenames == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    // remaining non-option args
    for (int i = 0; i < config->fileCount; i++) {
        config->filenames[i] = argv[optind + i + 1];
    }
}

void printHelp() {
    printf(
        "Usage: grepr [OPTIONS] <pattern> <filename>...\n"
        "OPTIONS:\n"
        "   -n          Show line numbers\n"
        "   -h          Hide file headers when matching multiple files\n"
        "   -v          Inverted mode (prints lines without matches)\n"
        "   --help      Prints this help message\n"
    );
    exit(EXIT_SUCCESS);
}

void processFile(char *filename, GreprConfig *config) {
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


        processLine(filename, lineBuffer, lineIndex, config);
        lineIndex++;
    }

    free(lineBuffer);
    fclose(file);
}

void processLine(char *filename, char *lineBuffer, int lineIndex, GreprConfig *config) {
    int matchLength;
    bool hasMatch = re_matchp(config->compiledPattern, lineBuffer, &matchLength) != -1;

    if ((hasMatch && !config->inverted) || (!hasMatch && config->inverted)) {
        // print file headers
        if (config->fileCount > 1 && !config->hideFileHeaders) 
            printf("%s%s%s:", BLUE, filename, WHITE);
        // show line numbers
        if (config->showLineNumbers) 
            printf("%d:", lineIndex);

        if (hasMatch && !config->inverted) {
            printMatch(lineBuffer, config->compiledPattern);
        } else {
            printf("%s", lineBuffer);
        } 

        // add newline if missing
        if (lineBuffer[strlen(lineBuffer) - 1] != '\n')
            printf("\n");
    }
}

void printMatch(char *lineBuffer, re_t compiledPattern) {
    int offset = 0;
    int matchLength;
    int matchIndex;

    while ((matchIndex = re_matchp(compiledPattern, lineBuffer + offset, &matchLength)) >= 0) {
        fwrite(lineBuffer + offset, 1, matchIndex, stdout);

        printf("%s", RED);
        fwrite(lineBuffer + offset + matchIndex, 1, matchLength, stdout);
        printf("%s", WHITE);

        offset += matchIndex + matchLength;
    }

    printf("%s", lineBuffer + offset);
}