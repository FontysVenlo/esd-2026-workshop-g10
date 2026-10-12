
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int input = getchar();

    if (input == 'X') {
        abort();  // Intentional crash for the demonstration
    }

    return 0;
}
