#include <stdio.h>
#include <stdlib.h> // For atoi()

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <number>\n", argv[0]);
        return 1; // Indicate an error
    }
    printf("argv[0] is: %s\n", argv[0]);
    printf("argv[1] is: %s\n", argv[1]);
    printf("argc is: %d\n", argc);
    int value = atoi(argv[1]); // Convert the first argument (a string) to an integer
    printf("The number passed as argument is: %d\n", value);
    return 0;
}