#define Role_Max 6
#define Tile_Size 64
#define Monster_Max 10000
#define Ground_Max 10000
#define Water_Max 10000
#include <math.h>
#include <string.h>


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
    Vector2 screen = {1280, 760};
    //Vector2 screen = {720, 480};

    InitWindow(screen.x, screen.y, "Vibrant");
    SetTargetFPS(59);

    Texture2D ground_tex  = LoadTexture("assets/tile_floor.png");
    Texture2D monster_tex   = LoadTexture("assets/monster.png");
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
            EndDrawing();
        }
        if (DRAW == mode) {
            static Vector2 cursor;
#define DefaultColor BLANK 
#define ColorMax 9
            static Color colors[ColorMax] = { DefaultColor, BLUE,RED, GREEN, ORANGE, WHITE, GRAY, PURPLE, VIOLET};
            // :load colors
            static int colors_loaded = 0;
            if (!colors_loaded) {
                colors_loaded = 1;
                    FILE *f = fopen("assets/color_set.data", "r");
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
               float  fade_cos = fabs(cos(frame_long * 0.1f));
               static int cursor_color = 0;
                static char * canvas = {0};
#define CanvasW 16
#define CanvasSize CanvasW   * CanvasW

                if (!canvas) { 
                    canvas = calloc(CanvasSize+1,1);
                    // :load canvas
                    {
                        FILE *f = fopen("assets/canvas.data", "r");
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
                if (IsKeyPressed(KEY_S)) {
                    PixelsFromCanvas;
                    stbi_write_png("assets/monster.png", CanvasW, CanvasW, 4, pixels,CanvasW*4); 
                    // :save colors
                    {
                        FILE * f = fopen("assets/color_set.data", "w");
                        if (f != NULL) {
                            fwrite((void *)colors, ColorMax, 1, f);
                            fclose(f);
                        }
                    }
                    // :save canvas
                    {
                        FILE * f = fopen("assets/canvas.data", "w");
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
                            UpdateTexture(monster_tex, pixels);
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
                            UpdateTexture(monster_tex, pixels);
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
                    DrawRectangle(CanvasOffset + 32 + (i % CanvasW)*2,CanvasOffset + (i / CanvasW)*2, 3, 3, colors[canvas[i]]);
                }

                // :colorpallet
                for (int i = 0; i < ColorMax; ++i) {
                    if (cursor.x == 0 && i == cursor.y-1) continue;
                    DrawRectangle(PixelFromCursorX(0), PixelFromCursorX(i+1), CursorPixelW, CursorPixelW, colors[i]);
                }

                // :canvas
                DrawRectangleLines(PixelFromCursorX(1), PixelFromCursorX(1), CanvasW*CursorPixelW, CanvasW*CursorPixelW, Fade(WHITE,0.3));
                // :cursor
                if (cursor.y > 0) {
                    DrawRectangle(PixelFromCursorX(cursor.x), PixelFromCursorX(cursor.y), CursorPixelW, CursorPixelW, Fade(colors[cursor_color],fade_cos));
                }
                DrawRectangleLines(PixelFromCursorX(cursor.x), PixelFromCursorX(cursor.y), CursorPixelW, CursorPixelW, GRAY);
            EndDrawing();
        }
        // :character selection
        // :play
        if (PLAY == mode) {
            static int init = 0;
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
                camera.rotation     = 0.0f;
                camera.zoom         = 1.5f;
                monster_a = malloc(sizeof(Monster_a));
                water_a = malloc(sizeof(Water_a));
                ground_a = malloc(sizeof(Ground_a));
                // :read ground.data
                {
                    FILE *f = fopen("assets/ground.data", "r");
			assert(f);
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
                        //print_monster_a("After Read", monster_a);
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
            if(IsKeyPressed(KEY_M)) {
                monster_a->items[monster_a->count].x = player.x;
                monster_a->items[monster_a->count].y = player.y;
                ++monster_a->count;
                FILE * f = fopen("assets/monster.data", "w");
                if (f != NULL) {
                    fwrite((void *)monster_a, sizeof (Ground_a), 1, f);
                    fclose(f);
                }
		if (!found) {
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
                //ClearBackground(WHITE);
                // :fade in
                static float fade_cos;
                //fade_cos = (1.0f + cos(frame_long * 0.05f)) / 3;
                fade_cos = 1.0f;//(1.0f + cos(frame_long * 0.05f)) / 3;
                    static Action player_action = STAND;
                if (player_action == DEAD) {
                    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(RED, fade_cos/3));
                }
                static int move_times = 0;
                static int low_fire = 1;
                if (fade_cos <= 0.0001) {
                    low_fire = 1;
                }
                if (fade_cos >= 0.5 && low_fire == 1) {
                    ++move_times; 
                    low_fire = 0;
                }
                if (fade_cos <= 0.0001 && move_times >= 2) {
                    move_times = 0;
                    for (int i = 0; i <  monster_a->count; ++i) {
                        Monster temp = monster_a->items[i];
                        if (monster_a->items[i].x < player.x) temp.x++;
                        if (monster_a->items[i].y < player.y) temp.y++;
                        if (monster_a->items[i].x > player.x) temp.x--;
                        if (monster_a->items[i].y > player.y) temp.y--;
                        int collide_b = 0;
                        for (int i2 = 0; i2 <  monster_a->count; ++i2) {
                            if (i2 == i) continue;
                            if (    player.x == temp.x && 
                                    player.y == temp.y ) {
                                collide_b = 1;
                                break;
                            }
                            if (    monster_a->items[i2].x == temp.x && 
                                    monster_a->items[i2].y == temp.y ) {
                                collide_b = 1;
                                break;
                            }
                        }
                        if (collide_b == 0) { 
                            monster_a->items[i] = temp;
                        }
                    } // for
                } // if
            static Vector2 mouse;
            mouse = GetMousePosition();
            //DrawCircleGradient(GetScreenWidth()/2+ Tile_Size /3, GetScreenHeight()/2, Tile_Size*2.5f, RED, BLACK);
            if (player_action != DEAD && actionPressed()) {
                flashlight_on = !flashlight_on;
                //DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), RED);
                {
                     DrawCircleGradient(GetScreenWidth()/2+ Tile_Size /3, GetScreenHeight()/2, Tile_Size*.5f, RED, BLACK);
                     //DrawCircle(GetScreenWidth()/2+ Tile_Size /3, GetScreenHeight()/2, Tile_Size*.5f, RED, BLACK);
                    for (int i = 0; i <  monster_a->count; ++i) {
                        if ( monster_a->items[i].x >= player.x_dest-1 && 
                             monster_a->items[i].x <= player.x_dest+1 &&
                             monster_a->items[i].y >= player.y_dest-1 && 
                             monster_a->items[i].y <= player.y_dest+1 
                           ) {
                            --monster_a->count;
                            monster_a->items[i] = monster_a->items[monster_a->count];

                        }
                    }
                }
            }

            /*
            if (!player.moving_y && !player.moving_x && actionPressed()) {
                // @Todo: 
                monster_a->items[monster_a->count].x = player.x;
                monster_a->items[monster_a->count].y = player.y;
                monster_a->items[monster_a->count].color = GREEN;
                ++monster_a->count;
                assert(monster_a->count < Monster_Max);
                // :write plants.data
                    FILE *f = fopen("assets/plants.data", "w");
                    assert(f != NULL);
                    //print_monster_a("Before Write", monster_a);
                    fwrite(monster_a, sizeof (Monster_a), 1, f);
                    fclose(f);
            }
            if (!player.moving_y && !player.moving_x && waterPressed()) {
                water_a->items[water_a->count].x = player.x;
                water_a->items[water_a->count].y = player.y;
                ++water_a->count;
                assert(water_a->count < Water_Max);
                // :write plants.data
                FILE *f = fopen("assets/water.data", "w");
                assert(f != NULL);
                //print_monster_a("Before Write", monster_a);
                fwrite(water_a, sizeof (Water_a), 1, f);
                fclose(f);
            }
            */

            if (player_action != DEAD) {
                if (   up()   && !player.moving_y) {
                    player.y_pixel_dest -= Tile_Size;
                    player.y_dest   -= 1;
                    player.moving_y = 1;
                }
                if ( down()   && !player.moving_y) {
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
                    player.x_pixel_dest += Tile_Size;
                    player.x_dest   += 1;
                    player.moving_x = 1;
                }
                if ( left()   && !player.moving_x) {
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
            } // alive
            static float speed = 4.0f; 
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


            BeginDrawing();
                ClearBackground(BLACK);
            camera.offset.x     = screen.x/2 - 32 + -(player.x_pixel * camera.zoom);
            camera.offset.y     = screen.y/2 - 32 + -(player.y_pixel * camera.zoom);
            if (actionPressed()) {
                mode = 2;
            }
            // :draw
                BeginMode2D(camera); // All that happens in here will move with the camera
                    // :ground
                    for (int i = 0; i < ground_a->count; ++i) {
                        DrawTextureEx(ground_tex, (Vector2) {ground_a->items[i].x * Tile_Size, ground_a->items[i].y * Tile_Size}, 0, 4, WHITE);
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
                        DrawTexturePro(person_stand_tex, (Rectangle) { 32 * frame, 0, 32, 32 }, player_rect, (Vector2){ 0, 0 }, 0, Fade(WHITE, fade_cos+.1));
                    } 
                    else if (player_action == DEAD) {
                        DrawTexturePro(person_walk_tex, (Rectangle) { 32 * frame, 0, 32, 32 }, player_rect, (Vector2){ -12, 64 }, 90, Fade(WHITE, fade_cos+.1));
                    } else {
                        DrawTexturePro(person_walk_tex, (Rectangle) { 32 * frame, 0, 32, 32 }, player_rect, (Vector2){ 0, 0 }, 0, Fade(WHITE, fade_cos+.1));
                    }
                EndMode2D();
                    // :flashlight

/*
                    if (player_action != DEAD && flashlight_on){
                        float half = GetScreenWidth()/2;

                        Vector2 midpoint;
                        midpoint.x = GetScreenWidth()/2;
                        midpoint.y = GetScreenHeight()/2 + 20 ;

                        float adjacent = midpoint.x - mouse.x;
                        float opposite = midpoint.y - mouse.y; 
                        float alpha_radian = atan(opposite/ adjacent);  
                        //float beta_radian = alpha_radian - M_PI / 2;
                        //float beta_radian = alpha_radian - M_PI / 2;
                        int cursor_y = 0;
                        int line_height = 20;

                        //static char * s = NULL;
                        //if (!s) s = (char *)malloc(sizeof (char) * 32);
                        //
                        HideCursor();
                        float radius = 80;

                        Vector2 l_tan;
                        l_tan.y = mouse.y - sin(beta_radian) * radius; // opposite
                        l_tan.x = mouse.x - cos(beta_radian) * radius; // adjacent

                        Vector2 r_tan;
                        r_tan.y = mouse.y + sin(beta_radian) * radius; // opposite
                        r_tan.x = mouse.x + cos(beta_radian) * radius; // adjacent

                        Vector2 points[4];
                        points[0] = midpoint;
                        if (mouse.x > midpoint.x)  {
                            points[1] = l_tan;
                            points[2] = r_tan;
                        }
                        else {
                            points[2] = l_tan;
                            points[1] = r_tan;
                        }
                        //DrawPolyEx(points, 3, Fade(RED, fade_cos));
                        DrawPolyEx(points, 3, Fade(RED,0.3));
                        //DrawCircleGradient(mouse.x, mouse.y, radius, Fade(RED, fade_cos), Fade(BLACK, fade_cos));
                        //DrawCircle(mouse.x, mouse.y, radius, Fade(RED, fade_cos));

                    }
*/
                // :noise
#if false
            {
                static Image noise_img = {0};
                static int noise_img_b = 0;
                noise_img = GenImageWhiteNoise(GetScreenWidth(), GetScreenHeight(), 0.5);
                static Texture2D noise_tex;
                static int noise_tex_b = 0;
                if (!noise_tex_b) {
                    noise_tex_b = 1;
                    noise_tex = LoadTextureFromImage(noise_img);
                }
                DrawTexture(noise_tex, 0, 0, Fade( RED, 0.5f));
            }
#endif
#if 0
            {
                for (int i = 0; i <  monster_a->count; ++i) {
                    if (monster_a->items[i].x == player.x_dest && 
                        monster_a->items[i].y == player.y_dest ){
                        player_action = DEAD;
                            DrawText("Samari slice that like button!",
                                    250, // xpos
                                    80, // ypos
                                    30, // fontsize
                                    RED //textColor
                                    );

                    }
                }
            }
#endif
            static int time_hour = 1;
                // brothers that play game at, for people to play games at!
                // two brother that create games for other people 
                // i got 'a

                
                /*
                DrawText(sprintf(NULL, "%d AM", time_hour),
                        GetScreenWidth()/2-100, // xpos
                        100, // ypos
                        60, // fontsize
                        Fade(WHITE, 0.5-fade_cos) //textColor
                        );
                        */
DrawFPS(10,10);
            EndDrawing();
        }
    }//while
}// main
