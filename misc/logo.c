// Used for generating the logo

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define ROWS 4
#define COLS 4
#define SLEEP_DURATION 40000 // Microseconds

void initializeArray(char array[ROWS][COLS]) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            array[i][j] = ' ';
        }
    }
}

void printArray(char array[ROWS][COLS], int pos) {
    int index = 0;
    printf("   ecs.h\n"); // Centered header
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (index == pos) {
                printf("[_]");
            } else {
                printf("[%c]", array[i][j]);
            }
            index++;
        }
        printf("\n");
    }
}

char getRandomChar() {
    char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*";
    int randomIndex = rand() % (sizeof(charset) - 1);
    return charset[randomIndex];
}

int main() {
    char array[ROWS][COLS];
    int pos = 0;

    srand(time(NULL));
    initializeArray(array);

    while (1) {
        array[pos / COLS][pos % COLS] = getRandomChar();
        printArray(array, pos);
        usleep(SLEEP_DURATION);
        pos = (pos + 1) % (ROWS * COLS);
        printf("\033[H\033[J"); // Clear screen and move cursor to top-left
    }

    return 0;
}

