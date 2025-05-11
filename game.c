#include <ncurses.h> //Ncurses se hum terminal screen ko control kar sakte hain
#include <stdlib.h>
#include <unistd.h> // for usleep

// Program ko kuch microseconds ke liye delay ya pause karna.
// Game loop ke andar, agar tum delay nahi doge toh:
// Snake bahut fast chalega
// CPU overload ho sakta hai
// Player ko khelna mushkil ho jayega

// Game settings
int width = 20;
int height = 20;
int headX, headY, fruitX, fruitY, score;
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
    headX = width / 2;
    headY = height / 2;
    fruitX = rand() % width;
    fruitY = rand() % height;
    score = 0;
    nTail = 0;
}

void Draw() {
    clear();

    // Draw top border
    for (int i = 0; i < width + 2; i++) printw("#");
    printw("\n");

    // Draw the game area
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (j == 0) printw("#"); // Left border

            // Draw the head
            if (i == headY && j == headX)
                printw("O");
            // Draw the fruit
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

            // Draw right border
            if (j == width - 1) printw("#");
        }
        printw("\n");
    }

    // Draw bottom border
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
    int previousX = tailX[0], previousY = tailY[0];  // Save the old position of the first part of the tail
    int secondPreviousX, secondPreviousY;            // Variables to remember the previous positions of the next parts of the tail
    tailX[0] = headX;                                // Move the first part of the tail to where the head was
    tailY[0] = headY;                                // Move the first part of the tail to where the head was
    
    for (int i = 1; i < nTail; i++) {                // For all the other parts of the tail (starting from the second part)
        secondPreviousX = tailX[i];                   // Remember the current position of this tail part
        secondPreviousY = tailY[i];                   // Remember the current position of this tail part

        tailX[i] = previousX;                         // Move this tail part to the previous position of the previous tail part
        tailY[i] = previousY;                         // Move this tail part to the previous position of the previous tail part

        previousX = secondPreviousX;                  // Now, set the 'previous' position to where this tail part was
        previousY = secondPreviousY;                  // Now, set the 'previous' position to where this tail part was
    }

    // Move the snake's head
    switch (dir) {
        case LEFT: headX--; break;
        case RIGHT: headX++; break;
        case UP: headY--; break;
        case DOWN: headY++; break;
        default: break;
    }

    // Boundary collision
    if (headX >= width || headX < 0 || headY >= height || headY < 0)
        gameOver = 1;

    // Self collision
    for (int i = 0; i < nTail; i++) {
        if (tailX[i] == headX && tailY[i] == headY)
            gameOver = 1;
    }

    // Fruit eaten
    if (headX == fruitX && headY == fruitY) {
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
