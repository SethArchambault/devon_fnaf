
#include "raylib.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>


bool left() {
    if (IsKeyDown(KEY_LEFT)) {
        return 1;
    } else {
        return 0;
    }
}
bool leftPressed() {
    if (IsKeyPressed(KEY_LEFT)) {
        return 1;
    } else {
        return 0;
    }
}
bool right() {
    if (IsKeyDown(KEY_RIGHT)) {
        return 1;
    } else {
        return 0;
    }
}
bool rightPressed() {
    if (IsKeyPressed(KEY_RIGHT)) {
        return 1;
    } else {
        return 0;
    }
}

bool down() {
    if (IsKeyDown(KEY_DOWN)) {
        return 1;
    } else {
        return 0;
    }
}

bool downPressed() {
    if (IsKeyPressed(KEY_DOWN)) {
        return 1;
    } else {
        return 0;
    }
}
bool up() {
    if (IsKeyDown(KEY_UP)) {
        return 1;
    } else {
        return 0;
    }
}
bool upPressed() {
    if (IsKeyPressed(KEY_UP)) {
        return 1;
    } else {
        return 0;
    }
}
bool waterPressed() {
    if (IsKeyPressed(KEY_W))  {
        return 1;
    } else {
        return 0;
    }
}

bool actionPressed() {
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))  {
        return 1;
    } else {
        return 0;
    }
}
#include "project.c"

int main() {
        game();
}
