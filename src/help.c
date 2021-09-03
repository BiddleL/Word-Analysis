#include <stdio.h>
#include <string.h>

#include "stemmer.h"

int main(int argc, char *argv[]) {
    char *w = strdup(argv[1]);
    stem(w, 0, strlen(w) - 1);
    printf("%s\n", w);
    return 0;
}