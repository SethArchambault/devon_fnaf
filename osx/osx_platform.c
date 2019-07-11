
#include <raylib.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>


bool left() {

    if (IsKeyDown(KEY_LEFT)|| IsKeyDown(KEY_A)) {
        return 1;
    } else {
        return 0;
    }
}
bool leftPressed() {
    if (IsKeyPressed(KEY_LEFT)|| IsKeyPressed(KEY_A)) {
        return 1;
    } else {
        return 0;
    }
}
bool right() {
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
        return 1;
    } else {
        return 0;
    }
}
bool rightPressed() {
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
        return 1;
    } else {
        return 0;
    }
}
bool shift() {
    if(IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) {
        return 1;
    } else {
        return 0;
    }
}

bool down() {
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) {
        return 1;
    } else {
        return 0;
    }
}

bool downPressed() {
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        return 1;
    } else {
        return 0;
    }
}
bool up() {
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) {
        return 1;
    } else {
        return 0;
    }
}
bool upPressed() {
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
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

