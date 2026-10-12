
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    char input[32];

    size_t length = fread(input, 1, sizeof(input), stdin);

    if (length >= 8) {
        abort();  // Intentional crash for the exercise
    }

    return 0;
}
