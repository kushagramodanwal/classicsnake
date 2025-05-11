#include <ncurses.h> //Ncurses se hum terminal screen ko control kar sakte hain
#include <stdlib.h>
#include <unistd.h> // for usleep

//Program ko kuch microseconds ke liye delay ya pause karna.

//Game loop ke andar, agar tum delay nahi doge toh:

// Snake bahut fast chalega

// CPU overload ho sakta hai

// Player ko khelna mushkil ho jayega

// Game settings
int width = 20;
int height = 20;
int x, y, fruitX, fruitY, score;
int tailX[100], tailY[100];
int nTail;
int gameOver;

enum direction { STOP = 0, LEFT, RIGHT, UP, DOWN };
enum direction dir;

void Setup() {
    initscr();              // Start ncurses mode
    clear();
    noecho();               // Don't echo typed characters
    cbreak();               // Disable line buffering
    curs_set(0);            // Hide the cursor
    keypad(stdscr, TRUE);   // Enable arrow keys
    nodelay(stdscr, TRUE);  // Non-blocking input

    gameOver = 0;
    dir = STOP;
    x = width / 2;
    y = height / 2;
    fruitX = rand() % width;
    fruitY = rand() % height;
    score = 0;
}

void Draw() {
    clear();

    for (int i = 0; i < width + 2; i++) printw("#");
    printw("\n");

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (j == 0) printw("#");

            if (i == y && j == x)
                printw("O");
            else if (i == fruitY && j == fruitX)
                printw("@");
            else {
                int isTail = 0;
                for (int k = 0; k < nTail; k++) {
                    if (tailX[k] == j && tailY[k] == i) {
                        printw("o");
                        isTail = 1;
                    }
                }
                if (!isTail) printw(" ");
            }

            if (j == width - 1) printw("#");
        }
        printw("\n");
    }

    for (int i = 0; i < width + 2; i++) printw("#");

    printw("\nScore: %d\n", score);
    refresh();
}

void Input() {
    int ch = getch();
    switch (ch) {
        case 'a': case KEY_LEFT: dir = LEFT; break;
        case 'd': case KEY_RIGHT: dir = RIGHT; break;
        case 'w': case KEY_UP: dir = UP; break;
        case 's': case KEY_DOWN: dir = DOWN; break;
        case 'x': gameOver = 1; break;
    }
}

void Logic() {
    int prevX = tailX[0], prevY = tailY[0];
    int prev2X, prev2Y;
    tailX[0] = x;
    tailY[0] = y;
    for (int i = 1; i < nTail; i++) {
        prev2X = tailX[i];
        prev2Y = tailY[i];
        tailX[i] = prevX;
        tailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }

    switch (dir) {
        case LEFT: x--; break;
        case RIGHT: x++; break;
        case UP: y--; break;
        case DOWN: y++; break;
        default: break;
    }

    // Boundary collision
    if (x >= width || x < 0 || y >= height || y < 0)
        gameOver = 1;

    // Self collision
    for (int i = 0; i < nTail; i++) {
        if (tailX[i] == x && tailY[i] == y)
            gameOver = 1;
    }

    // Fruit eaten
    if (x == fruitX && y == fruitY) {
        score += 10;
        fruitX = rand() % width;
        fruitY = rand() % height;
        nTail++;
    }
}

int main() {
    Setup();
    while (!gameOver) {
        Draw();
        Input();
        Logic();
        usleep(100000); // 0.1 second delay
    }

    clear();
    printw("Game Over! Final Score: %d\n", score);
    refresh();
    nodelay(stdscr, FALSE);
    getch(); // Wait for key before exiting
    endwin(); // End ncurses mode

    return 0;
}

