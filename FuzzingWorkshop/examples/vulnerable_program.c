
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    char input[100];
    size_t length = fread(input, 1, sizeof(input), stdin);

    if (length >= 5 && memcmp(input, "CRASH", 5) == 0) {
        abort();
    }

    return 0;
}