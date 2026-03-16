#include "BearLibTerminal.h"
#include <stdio.h>

int main() {
    if (!terminal_open()) {
        printf("Failed to open terminal\n");
        return 1;
    }

    // Printing text
    terminal_print(1, 1, "Hello, world!");
    terminal_refresh();

    // Wait until user close the window
    while (terminal_read() != TK_CLOSE);

    terminal_close();
    return 0;
}