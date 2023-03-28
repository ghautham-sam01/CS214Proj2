#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "words.h"
int main()
{
    char *word;
    int fd = open("test.txt", O_RDONLY);
    words_init(fd);
    while ((word = words_next())) {
        puts(word);
        free(word);
    }
    return EXIT_SUCCESS;
}