#define Role_Max 6
#define Tile_Size 64
#define Monster_Max 100
#define Ground_Max 10000
#define Water_Max 10000
#include <math.h>
#include <string.h>
#include "/opt/raylib/src/external/stb_image_write.h"
#include <time.h>  // rand


typedef struct {
    int x;
    int y;
    int type;
} Ground;


typedef struct {
    int x;
    int y;
} Water;

typedef struct {
    int x;
    int y;
    Color color;
    int type; // 0-9
} Monster;

typedef struct {
    Ground items[Ground_Max];
    int count;
} Ground_a;

typedef struct {
    Water items[Water_Max];
    int count;
} Water_a;

typedef struct {
    Monster items[Monster_Max];
    int count;
} Monster_a;

void print_monster_a(char msg[30],Monster_a *monster_a) {
    printf("\n%s monster_a.count %d \n", msg, monster_a->count);
    for(int i = 0; i < monster_a->count; ++i) {
        printf("    %d x:%2d y:%2d\n", i, monster_a->items[i].x, monster_a->items[i].y);
    }
}

typedef enum {
    TITLE, DRAW, PLAY
} Mode;
typedef enum {
    STAND, WALK, DEAD
} Action;

Vector2 v2_pixel_from_v2_coord(Vector2 coord) {
    coord.x *= 64;
    coord.y *= 64;
    return coord;
}

typedef enum {
    UP, RIGHT, DOWN, LEFT
} Direction;

typedef struct {
    int x;
    int y;
    int x_dest;
    int y_dest;
    float x_pixel;
    float y_pixel;
    float x_pixel_dest;
    float y_pixel_dest;
    float distance;
    bool moving_x;
    bool moving_y;
    Direction direction;
} Player;


int int_from_float(float f) {
    int i;
    int precision = 100;
    if (f > 0) {
        i = (int)(f * precision + .5);
    } else {
        i = (int)(f * precision - .5);
    }
    return i;
}

void game() {
    //Vector2 screen = {1280, 446};
    // :init
   
    srand(time(NULL));
   
    Vector2 screen = {1280, 760};

    InitWindow(screen.x, screen.y, "Vibrant");
    SetTargetFPS(59);

    Texture2D ground_tex  = LoadTexture("assets/tile_floor.png");
#define monster_max 10
    Texture2D monster_tex_a[monster_max];
    for(int i = 0; i < monster_max;++i) {
        char filename[100];
        sprintf(filename, "assets/monster%d.png", i);

        monster_tex_a[i] = LoadTexture(filename);
    }

    Texture2D person_stand_tex  = LoadTexture("assets/devon_standing.png");
    Texture2D person_walk_tex  = LoadTexture("assets/devon_walking.png");



    Mode mode = TITLE; 

    int frame_long = 0;
    int frame_passed = 0;
    int frame = 0;
    while (!WindowShouldClose()) {
        ++frame_passed;
        ++frame_long;
        // :splash_screen
        if (TITLE == mode) {
            Vector2 cursor;
            if (IsKeyPressed(KEY_P)) {
                mode = PLAY;
            }
            if (IsKeyPressed(KEY_D)) {
                mode = DRAW;
            }
            BeginDrawing();
                ClearBackground(BLACK);
                cursor.x =  screen.x/2 - 450;
                cursor.y = 100;
                DrawText("Five Nights at YouTube Factory",
                        cursor.x, // xpos
                        cursor.y, // ypos
                        60, // fontsize
                        WHITE //textColor
                        );
                cursor.y += 70;
                DrawText("A Horror Pixel Production",
                        cursor.x, // xpos
                        cursor.y, // ypos
                        46, // fontsize
                        WHITE
                        );
                cursor.y += 70;
                DrawText("Press p to Play",
                        cursor.x, // xpos
                        cursor.y, // ypos
                        23, // fontsize
                        WHITE
                        );
                cursor.y += 40;
                DrawText("Press d to Draw",
                        cursor.x, // xpos
                        cursor.y, // ypos
                        23, // fontsize
                        WHITE
                        );
                cursor.y += 70;
                DrawText("KEYS",
                        cursor.x, // xpos
                        cursor.y, // ypos
                        23, // fontsize
                        WHITE
                        );
                cursor.y += 40;
                DrawText("c = clear",
                        cursor.x, // xpos
                        cursor.y, // ypos
                        23, // fontsize
                        WHITE
                        );
                cursor.y += 40;
                DrawText("f = floor",
                        cursor.x, // xpos
                        cursor.y, // ypos
                        23, // fontsize
                        WHITE
                        );
                cursor.y += 40;
                DrawText("m = monster",
                        cursor.x, // xpos
                        cursor.y, // ypos
                        23, // fontsize
                        WHITE
                        );
                cursor.y += 80;
                DrawText("easter egg to be found press n key and go into the darkness to find it...",
                        cursor.x, // xpos
                        cursor.y, // ypos
                        23, // fontsize
                        WHITE
                        );
            EndDrawing();
        }
        if (DRAW == mode) {
            static Vector2 cursor;
#define DefaultColor BLANK 
#define ColorMax 9
            // :colors
            static Color * colors; 
            if (!colors) {
                colors = malloc(ColorMax * sizeof(Color));
                Color colors_t[ColorMax] = { DefaultColor, BLUE,RED, GREEN, ORANGE, WHITE, GRAY, PURPLE, VIOLET};
                memcpy(colors, colors_t, sizeof(Color) *  ColorMax);
            }
            static int colors_loaded = 0;
            static int current_monster = 0;
            if (!colors_loaded) {
                colors_loaded = 1;
            // :load colors
                    char filename[100];
                    sprintf(filename, "assets/color_set%d.data", current_monster);
                    FILE *f = fopen(filename, "r");
                    if (f != NULL) {
                        printf("loaded colors\n");
                        long int f_len;
                        fseek(f, 0, SEEK_END);
                        f_len = ftell(f);
                        assert(f_len <= ColorMax * sizeof (Color));
                        fseek(f, 0, SEEK_SET);
                        fread((void *)colors, f_len, 1, f);
                        fclose(f);
                    }
            }
               static int cursor_color = 0;
                static char * canvas = {0};
#define CanvasW 16
#define CanvasSize CanvasW   * CanvasW

                if (!canvas) { 
                    canvas = calloc(CanvasSize+1,1);
                    // :load canvas
                    {
                        char filename[100];
                        sprintf(filename,"assets/monster%d.data", current_monster); 
                        FILE *f = fopen(filename, "r");
                        if (f != NULL) {
                            long int f_len;
                            fseek(f, 0, SEEK_END);
                            f_len = ftell(f);
                            assert(f_len <= CanvasSize);
                            fseek(f, 0, SEEK_SET);
                            fread((void *)canvas, f_len, 1, f);
                            fclose(f);
                        }
                    }
                }
#define PixelsFromCanvas for(int y = 0; y < CanvasW; ++y) {\
                            for(int x = 0; x < CanvasW; ++x) {\
                                pixels[y * CanvasW+ x] = colors[canvas[y* CanvasW+x]];\
                            }\
                        }
                static Color *pixels; 
                if (!pixels) pixels= (Color *)malloc(CanvasW*CanvasW*sizeof(Color));
                int key_pressed = GetKeyPressed();
                // :press 1-10
                if (key_pressed >= 48 && key_pressed <= 57) {
                    // :save monster
                    {
                        char filename[100];
                        sprintf(filename,"assets/monster%d.data", current_monster); 
                        FILE * f = fopen(filename, "w");
                        if (f != NULL) {
                            fwrite((void *)canvas, CanvasSize, 1, f);
                            fclose(f);
                        }
                    }
                    // :save colors
                    {
                        char filename[100];
                        sprintf(filename, "assets/color_set%d.data", current_monster);
                        FILE * f = fopen(filename, "w");
                        if (f != NULL) {
                            fwrite((void *)colors, ColorMax, 1, f);
                            fclose(f);
                        }
                    }
                    current_monster = key_pressed-48;
                    // :load colors
                    char filename[100];
                    sprintf(filename, "assets/color_set%d.data", current_monster);
                    FILE *f = fopen(filename, "r");
                    if (f != NULL) {
                        printf("loaded colors\n");
                        long int f_len;
                        fseek(f, 0, SEEK_END);
                        f_len = ftell(f);
                        assert(f_len <= ColorMax * sizeof (Color));
                        fseek(f, 0, SEEK_SET);
                        fread((void *)colors, f_len, 1, f);
                        fclose(f);
                    }
                    printf("current_monster %d\n", current_monster);
                    // @Todo: compress this
                    // :load canvas
                    {
                        char filename[100];
                        sprintf(filename,"assets/monster%d.data", current_monster); 
                        FILE *f = fopen(filename, "r");
                        if (f != NULL) {
                            long int f_len;
                            fseek(f, 0, SEEK_END);
                            f_len = ftell(f);
                            assert(f_len <= CanvasSize);
                            fseek(f, 0, SEEK_SET);
                            fread((void *)canvas, f_len, 1, f);
                            fclose(f);
                        }
                    }
                }
                
                if (IsKeyPressed(KEY_S)) {
                    PixelsFromCanvas;
                    char filename[100];
                    sprintf(filename, "assets/monster%d.png", current_monster);

                    stbi_write_png(filename, CanvasW, CanvasW, 4, pixels,CanvasW*4); 
                    // :save colors
                    {
                        char filename[100];
                        sprintf(filename, "assets/color_set%d.data", current_monster);
                        FILE * f = fopen(filename, "w");
                        if (f != NULL) {
                            fwrite((void *)colors, ColorMax, 1, f);
                            fclose(f);
                        }
                    }
                    // :save canvas
                    {
                        char filename[100];
                        sprintf(filename,"assets/monster%d.data", current_monster); 
                        FILE * f = fopen(filename, "w");
                        if (f != NULL) {
                            fwrite((void *)canvas, CanvasSize, 1, f);
                            fclose(f);
                        }
                    }
                        // update texture
                        {
                            int width = 16;
                            for(int y = 0; y < width; ++y) {
                                for(int x = 0; x < width; ++x) {
                                    pixels[y * width + x] = colors[canvas[y* width+x]];
                                }
                            }
                            UpdateTexture(monster_tex_a[current_monster], pixels);
                        }
                }
               if (cursor.x >= 0) {
                    if (IsKeyPressed(KEY_P)) {
                        mode = PLAY;
                    }
                    if (IsKeyPressed(KEY_DOWN)) {
                        cursor.y += 1;
                    }
                    if (IsKeyPressed(KEY_UP)) {
                        cursor.y -= 1;
                    }
                    if (IsKeyPressed(KEY_RIGHT)) {
                        cursor.x += 1;
                    }
                    if (IsKeyPressed(KEY_LEFT)) {
                        cursor.x -= 1;
                    }
                    if (IsKeyDown(KEY_SPACE) && cursor.x > 0 && cursor.y > 0) {
                        int position = ((int)cursor.y-1) * CanvasW + ((int)cursor.x-1);
                        canvas[position] = cursor_color;


                        // update texture
                        {
                            int width = 16;
                            for(int y = 0; y < width; ++y) {
                                for(int x = 0; x < width; ++x) {
                                    pixels[y * width + x] = colors[canvas[y* width+x]];
                                }
                            }
                            UpdateTexture(monster_tex_a[0], pixels);
                        }
                    }
               }
               else {
                    if (IsKeyDown(KEY_DOWN)) {
                        if((int)cursor.x == -3) {
                            colors[(int)cursor.y-1].r -= 1;
                        }
                        if((int)cursor.x == -2) {
                            colors[(int)cursor.y-1].g -= 1;
                        }
                        if((int)cursor.x == -1) {
                            colors[(int)cursor.y-1].b -= 1;
                        }
                    }
                    if (IsKeyDown(KEY_UP)) {
                        if((int)cursor.x == -3) {
                            colors[(int)cursor.y-1].r += 1;
                        }
                        if((int)cursor.x == -2) {
                            colors[(int)cursor.y-1].g += 1;
                        }
                        if((int)cursor.x == -1) {
                            colors[(int)cursor.y-1].b += 1;
                        }
                    }
                    if (IsKeyPressed(KEY_RIGHT)) {
                        cursor.x += 1;
                    }
                    if (IsKeyPressed(KEY_LEFT)) {
                        cursor.x -= 1;
                    }
               }
                //:colorpallet interaction
                if (cursor.x == 0) {
                    if (cursor.y > 0 && cursor.y-1 < ColorMax) {
                        cursor_color = (int)cursor.y-1;
                    }
                }
            BeginDrawing();
                ClearBackground(BLACK);

#define CanvasOffset 90
#define CursorPixelW 32


#define PixelFromCursorX(x) (x) * CursorPixelW + CanvasOffset

                // :canvas
                for (int i = 0; i < CanvasSize;++i) {
                    //printf("canvas %d '%c'\n",i, canvas[i]);
                    assert((int)canvas[i] < ColorMax);
                    DrawRectangle(PixelFromCursorX((int)((i) % CanvasW)+1), PixelFromCursorX(((int)(i)/CanvasW)+1), CursorPixelW, CursorPixelW, colors[canvas[i]]);
                }
                // :minimap
                for (int i = 0; i < CanvasSize;++i) {
                    DrawRectangle(current_monster * 32+ CanvasOffset + 32 + (i % CanvasW)*2,CanvasOffset + (i / CanvasW)*2, 3, 3, colors[canvas[i]]);
                }

                // :colorpallet
                for (int i = 0; i < ColorMax; ++i) {
                    if (cursor.x == 0 && i == cursor.y-1) continue;
                    DrawRectangle(PixelFromCursorX(0), PixelFromCursorX(i+1), CursorPixelW, CursorPixelW, colors[i]);
                }

                DrawRectangleLines(PixelFromCursorX(1), PixelFromCursorX(1), CanvasW*CursorPixelW, CanvasW*CursorPixelW, Fade(WHITE,0.3));
                // :cursor
                if (cursor.y > 0) {
                    DrawRectangle(PixelFromCursorX(cursor.x), PixelFromCursorX(cursor.y), CursorPixelW, CursorPixelW, Fade(colors[cursor_color],1));
                }
                DrawRectangleLines(PixelFromCursorX(cursor.x), PixelFromCursorX(cursor.y), CursorPixelW, CursorPixelW, GRAY);
            EndDrawing();
        }
        // :character selection
        // :play mode
        if (PLAY == mode) {
            static int init = 0;
            static int noclip = 1;
            static Player player;
            static Monster_a *monster_a;
            static Water_a *water_a;
            static Camera2D camera;
            static Ground_a *ground_a;
            if (init == 0) {
                init                = 1;
                player.x              = 1;
                player.y              = 0;
                player.x_dest              = player.x;
                player.y_dest              = player.y;
                player.y_pixel = player.y * Tile_Size;
                player.x_pixel = player.x * Tile_Size;
                player.distance     = 0;
                player.moving_x     = false;
                player.moving_y     = false;
                player.y_pixel_dest = player.y_pixel;
                player.x_pixel_dest = player.x_pixel;
                player.direction    = DOWN;
                camera.rotation     = 0.0f;
                camera.zoom         = 1.5f;
                monster_a = malloc(sizeof(Monster_a));
                water_a = malloc(sizeof(Water_a));
                ground_a = malloc(sizeof(Ground_a));
                // :read ground.data
                {
                    FILE *f = fopen("assets/ground.data", "r");
                    if (f != NULL) {
                        long int f_len;
                        fseek(f, 0, SEEK_END);
                        f_len = ftell(f);
                        assert(f_len <= sizeof (Ground_a));
                        fseek(f, 0, SEEK_SET);
                        fread((void *)ground_a, f_len, 1, f);
                        fclose(f);
                    }
                    else {
                        monster_a->count      = 0;
                    }
                }
                // :read monster.data
                {
                    FILE *f = fopen("assets/monster.data", "r");
                    if (f != NULL) {
                        long int f_len;
                        fseek(f, 0, SEEK_END);
                        f_len = ftell(f);
                        assert(f_len <= sizeof (Monster_a));
                        fseek(f, 0, SEEK_SET);
                        fread((void *)monster_a, f_len, 1, f);
                        print_monster_a("After Read", monster_a);
                        fclose(f);
                    }
                    else {
                        monster_a->count      = 0;
                    }
                }
                FILE *f = fopen("assets/water.data", "r");
                if (f != NULL) {
                    long int f_len;
                    fseek(f, 0, SEEK_END);
                    f_len = ftell(f);
                    assert(f_len <= sizeof (Water_a));
                    fseek(f, 0, SEEK_SET);
                    fread((void *)water_a, f_len, 1, f);
                    fclose(f);
                }
                else {
                    water_a->count      = 0;
                }
            }
                    static int flashlight_on = 0;

            if (IsKeyPressed(KEY_D)) {
                mode = DRAW;
            }
            if (IsKeyPressed(KEY_N)) {
                noclip = !noclip;
            }
            if (IsKeyPressed(KEY_EQUAL)) {
                camera.zoom += 1;
            }
            if (IsKeyPressed(KEY_MINUS)) {
                camera.zoom -= 1;
            }

            if(IsKeyDown(KEY_F)) {
                int floor_detected = 0;
                for (int i = 0; i < ground_a->count; ++i) {
                    if(ground_a->items[i].x == player.x &&
                    ground_a->items[i].y == player.y) {
                        floor_detected = 1;
                    }
                }
                if (!floor_detected) {
                    ground_a->items[ground_a->count].x = player.x;
                    ground_a->items[ground_a->count].y = player.y;
                    ground_a->items[ground_a->count].type  =1; // tile
                    ++ground_a->count;
                    FILE * f = fopen("assets/ground.data", "w");
                    if (f != NULL) {
                        fwrite((void *)ground_a, sizeof (Ground_a), 1, f);
                        fclose(f);
                    }
                }
            }

            // :place monster
            int key_pressed = GetKeyPressed();
            if (key_pressed >= 48 && key_pressed <= 57) {
                if (monster_a->count < Monster_Max) {
                    monster_a->items[monster_a->count].x = player.x;
                    monster_a->items[monster_a->count].y = player.y;
                    monster_a->items[monster_a->count].type = key_pressed-48; // 0-9
                    ++monster_a->count;
                    FILE * f = fopen("assets/monster.data", "w");
                    if (f != NULL) {
                        fwrite((void *)monster_a, sizeof (Monster_a), 1, f);
                        fclose(f);
                    }
                }
            }
            if(IsKeyDown(KEY_C)) {
                for (int i = 0; i < monster_a->count; ++i) {
                    if(monster_a->items[i].x == player.x &&
                    monster_a->items[i].y == player.y) {
                        monster_a->items[i] = monster_a->items[monster_a->count - 1];
                        --monster_a->count;
                    }
                }
                static FILE * f;
               f = fopen("assets/monster.data", "w");
                if (f != NULL) {
                    fwrite((void *)monster_a, sizeof (Monster_a), 1, f);
                    fclose(f);
                }

                // delete floor
                for (int i = 0; i < ground_a->count; ++i) {
                    if(ground_a->items[i].x == player.x &&
                    ground_a->items[i].y == player.y) {
                        ground_a->items[i] = ground_a->items[ground_a->count - 1];
                        --ground_a->count;
                    }
                }
                {
                    f = fopen("assets/ground.data", "w");
                    if (f != NULL) {
                        fwrite((void *)ground_a, sizeof (Ground_a), 1, f);
                        fclose(f);
                    }
                }
            }

            // @Hack: this should be later
            BeginDrawing();
            ClearBackground(BLACK);
            // :fade in
            static Action player_action = STAND;
            if (   up()   && !player.moving_y) {
                player.direction = UP;
                player.y_pixel_dest -= Tile_Size;
                player.y_dest   -= 1;
                player.moving_y = 1;
            }
            if ( down()   && !player.moving_y) {
                player.direction = DOWN;
                player.y_pixel_dest += Tile_Size;
                player.y_dest += 1;
                player.moving_y = 1;
            }
            {
                int water_count = 0;
                for (int i = 0; i <  water_a->count; ++i) {
                    if (water_a->items[i].x == player.x_dest && 
                        water_a->items[i].y == player.y_dest ){
                        ++water_count;
                    }
                }
                if (water_count > 1) {
                    player.y_dest = player.y;
                    player.y_pixel_dest = player.y_pixel;
                }
            }
            if (right()   && !player.moving_x) {
                player.direction = RIGHT;
                player.x_pixel_dest += Tile_Size;
                player.x_dest   += 1;
                player.moving_x = 1;
            }
            if ( left()   && !player.moving_x) {
                player.direction = LEFT;
                player.x_pixel_dest -= Tile_Size;
                player.x_dest   -= 1;
                player.moving_x = 1;
            }
            {
            int water_count = 0;
                for (int i = 0; i <  water_a->count; ++i) {
                    if (water_a->items[i].x == player.x_dest && 
                        water_a->items[i].y == player.y_dest ){
                        water_count++;
                    }
                }
                if (water_count > 1) {
                    player.x_dest = player.x;
                    player.x_pixel_dest = player.x_pixel;
                }
            }
            static float speed = 4.0f; 
            // :collision
            int collision = 0;
            if (!noclip) {
                collision = 1;
                for(int i = 0; i < ground_a->count; ++i) {
                    if (ground_a->items[i].x == player.x_dest && ground_a->items[i].y == player.y_dest) {
                        collision = 0;
                        break;
                    }
                }
                if (collision) {
                        player.x_dest = player.x;
                        player.y_dest = player.y;
                        player.x_pixel_dest = player.x_pixel;
                        player.y_pixel_dest = player.y_pixel;
                }
            }
            if (!collision) {
                if (player.x_pixel < player.x_pixel_dest)       player.x_pixel += speed;
                else if (player.x_pixel > player.x_pixel_dest)  player.x_pixel -= speed;
                else {
                    player.moving_x = 0;
                    player.x = player.x_dest;
                    player.x_pixel = player.x * Tile_Size;
                    player.x_pixel_dest = player.x_pixel;
                }
                if (player.y_pixel < player.y_pixel_dest)       player.y_pixel += speed;
                else if (player.y_pixel > player.y_pixel_dest)  player.y_pixel -= speed;
                else {
                    player.moving_y = 0;
                    player.y = player.y_dest;
                    player.y_pixel = player.y * Tile_Size;
                    player.y_pixel_dest = player.y_pixel;
                }
            }


            camera.offset.x     = screen.x/2 - 32 + -(player.x_pixel * camera.zoom);
            camera.offset.y     = screen.y/2 - 32 + -(player.y_pixel * camera.zoom);
            // :draw
                BeginMode2D(camera); // All that happens in here will move with the camera
                    // :ground
                    for (int i = 0; i < ground_a->count; ++i) {
                        DrawTextureEx(ground_tex, (Vector2) {ground_a->items[i].x * Tile_Size, ground_a->items[i].y * Tile_Size}, 0, 4, WHITE);
                    }
                    //   :draw water
                    for (int i = 0; i < water_a->count; ++i) {
                        DrawRectangle(water_a->items[i].x * 64, water_a->items[i].y * 64, 64, 64, Fade(BLUE, 0.75f));
                    }
            // :spacebar
            // :action
            // :shoot
#define MonsterDead 9
            if (actionPressed()) {
                if (player.direction == RIGHT) {
                    DrawRectangle(player.x*64, player.y * 64, 1000,64, (Color) {248,57,53,255});
                    for (int i = 0; i < 10; ++i) {
                        for (int mi = 0; mi < monster_a->count; ++mi) {
                            Monster * monster = &monster_a->items[mi];
                            if (monster->x == player.x + i && monster->y == player.y) {
                                monster->type = MonsterDead;
                                /*
                                monster_a->items[mi] = monster_a->items[monster_a->count - 1];
                                --monster_a->count;
                                */
                            goto ShootingDone;
                            }
                        }
                    }
                }
                if (player.direction == LEFT) {
                    DrawRectangle(player.x*64 - 1000, player.y * 64, 1000,64, (Color) {248,57,53,255});
                    for (int i = 0; i < 10; ++i) {
                        for (int mi = 0; mi < monster_a->count; ++mi) {
                            Monster * monster = &monster_a->items[mi];
                            if (monster->x == player.x - i && monster->y == player.y) {
                                monster->type = MonsterDead;
                                /*
                                monster_a->items[mi] = monster_a->items[monster_a->count - 1];
                                --monster_a->count;
                                */
                                goto ShootingDone;
                            }
                        }

                    }
                }
                if (player.direction == UP) {
                    DrawRectangle(player.x*64, player.y * 64 - 1000, 64, 1000, (Color) {248,57,53,255});
                    for (int i = 0; i < 10; ++i) {
                        for (int mi = 0; mi < monster_a->count; ++mi) {
                            Monster * monster = &monster_a->items[mi];
                            if (monster->y == player.y - i && monster->x == player.x) {
                                monster->type = MonsterDead;
                                /*
                                monster_a->items[mi] = monster_a->items[monster_a->count - 1];
                                --monster_a->count;
                                */
                                goto ShootingDone;
                            }
                        }

                    }
                }
                if (player.direction == DOWN) {
                    DrawRectangle(player.x*64, player.y * 64, 64, 1000, (Color) {248,57,53,255});
                    for (int i = 0; i < 10; ++i) {
                        for (int mi = 0; mi < monster_a->count; ++mi) {
                            Monster * monster = &monster_a->items[mi];
                            if (monster->y == player.y + i && monster->x == player.x) {
                                monster->type = MonsterDead;
                                /*
                                monster_a->items[mi] = monster_a->items[monster_a->count - 1];
                                --monster_a->count;
                                */
                                goto ShootingDone;
                            }
                        }

                    }
                }

            }
            ShootingDone:;
                        // :move monsters
            static int enemy_timer = 0;
                    static Monster temp_monster;
                    ++enemy_timer;
                    int enemy_speed = 100;
#define SethIndex 2
#define DoorIndex 8
                    if (enemy_timer > enemy_speed && !noclip) {
                        for (int i = 0; i < monster_a->count; ++i) {
                            Monster * monster = &monster_a->items[i];
                            memcpy(&temp_monster,monster, sizeof(Monster));
                            enemy_timer = 0;
                            if (monster->type == MonsterDead) { 
                            }else if (monster->type == SethIndex) { 
                                monster->x = (int)player.x  - 5 + (rand() % 10);
                                monster->y = (int)player.y  - 3 + (rand() % 6);
                            } else {
                                if (monster->x > player.x) {
                                    monster->x--;
                                }
                                if (monster->x < player.x) {
                                    monster->x++;
                                }
                                if (monster->y < player.y) {
                                    monster->y++;
                                }
                                if (monster->y > player.y) {
                                    monster->y--;
                                }
                            }
                            for (int n = 0; n < monster_a->count; ++n) {
                                Monster * monster2 = &monster_a->items[n];
                                if (n == i) continue;
                                if (monster2->x == monster->x && monster->y == monster2->y) {
                                    memcpy(monster, &temp_monster, sizeof(Monster));
                                    break;
                                }

                            }
                            int collision = 1;
                            for(int w = 0; w < ground_a->count; ++w) {
                                if (ground_a->items[w].x == monster->x && ground_a->items[w].y == monster->y) {
                                    collision = 0;
                                    break;
                                }
                            }
                            if (collision) {
                                memcpy(monster, &temp_monster, sizeof(Monster));
                            }
                        }
                    }
                    // :draw monster
                    for (int i = 0; i < monster_a->count; ++i) {
                        Monster * monster = &monster_a->items[i];
                        if(enemy_timer > 30 && monster->type == SethIndex) {
                            DrawRectangle(monster->x*64 - 64, monster->y * 64 -64, 260,32, (Color) {37,75,165,255});
                            DrawText("Would you like a glass of SHUT UP NOW?",
                                monster->x * 64 - 54, // xpos
                                monster->y * 64 - 52,
                                12, // fontsize
                                WHITE
                            );
                        }
#define BarneyIndex 1
                        if(monster->type == BarneyIndex) {
                            DrawRectangle(monster->x*64 - 64, monster->y * 64 -64, 240,32, (Color) {165,37,165,255});
                            DrawText("I love you. You love me..",
                                monster->x * 64 - 54, // xpos
                                monster->y * 64 - 52,
                                12, // fontsize
                                WHITE
                            );
                        }
                        DrawTextureEx(monster_tex_a[monster->type], (Vector2) { monster->x * 64, monster->y * Tile_Size - Tile_Size / 4  }, 0, 4, WHITE);
                    }

                    if (player_action != DEAD && frame_passed > 10) {
                        if (player.moving_y || player.moving_x) { 
                            player_action = WALK;
                        } else { 
                            player_action = STAND;
                        }
                        frame_passed = 0;
                        ++frame;
                        if (frame > 4) frame = 0;
                    }
                    // :draw player
                    static Rectangle player_rect; 
                    player_rect.height = Tile_Size;
                    player_rect.width = Tile_Size;
                    player_rect.x = player.x_pixel;
                    player_rect.y = player.y_pixel - Tile_Size / 4;
                    if (player_action == STAND) {
                        DrawTexturePro(person_stand_tex, (Rectangle) { 32 * frame, 0, 32, 32 }, player_rect, (Vector2){ 0, 0 }, 0, WHITE);
                    } 
                    else if (player_action == DEAD) {
                        DrawTexturePro(person_walk_tex, (Rectangle) { 32 * frame, 0, 32, 32 }, player_rect, (Vector2){ -12, 64 }, 90, WHITE);
                    } else {
                        DrawTexturePro(person_walk_tex, (Rectangle) { 32 * frame, 0, 32, 32 }, player_rect, (Vector2){ 0, 0 }, 0, WHITE);
                    }
                EndMode2D();
                    // :flashlight
            EndDrawing();
        }
    }//while
}// main
