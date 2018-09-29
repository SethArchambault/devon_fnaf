#define Role_Max 6
#define Tile_Size 64
#define Plant_Max 10000
#define Ground_Max 10000
#define Water_Max 10000
#include <math.h>


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
} Plant;

typedef struct {
    Ground items[Ground_Max];
    int count;
} Ground_a;

typedef struct {
    Water items[Water_Max];
    int count;
} Water_a;

typedef struct {
    Plant items[Plant_Max];
    int count;
} Plant_a;

void print_plant_a(char msg[30],Plant_a *plant_a) {
    printf("\n%s plant_a.count %d \n", msg, plant_a->count);
    for(int i = 0; i < plant_a->count; ++i) {
        printf("    %d x:%2d y:%2d\n", i, plant_a->items[i].x, plant_a->items[i].y);
    }
}

typedef enum {
    TITLE, CHARACTER, PLAY
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

    InitWindow(screen.x, screen.y, "Vibrant");
    SetTargetFPS(59);

    Texture2D ground_tex  = LoadTexture("assets/tile_floor.png");
    Texture2D plant_tex   = LoadTexture("assets/plant.png");
    Texture2D person_stand_tex  = LoadTexture("assets/devon_standing.png");
    Texture2D person_walk_tex  = LoadTexture("assets/devon_walking.png");
    Vector2 cursor; 

    Mode mode = TITLE; 

    int frame_long = 0;
    int frame_passed = 0;
    int frame = 0;
    while (!WindowShouldClose()) {
        ++frame_passed;
        ++frame_long;
        // :splash_screen
        if (TITLE == mode) {
            if (actionPressed()) {
                mode = PLAY;
            }
            BeginDrawing();
                ClearBackground(BLACK);
                cursor.x =  screen.x/2 - 450;
                cursor.y = 100;
                DrawText("Five Nights at Corey's",
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
                        Fade(WHITE,  1.4f + cos(frame_passed* 0.8f)  ) //textColor
                        );
            EndDrawing();
        }
        // :character selection
        // :play
        if (PLAY == mode) {
            static int init = 0;
            static Player player;
            static Plant_a *plant_a;
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
                plant_a = malloc(sizeof(Plant_a));
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
                        plant_a->count      = 0;
                    }
                }
                // :read plants.data
                {
                    FILE *f = fopen("assets/plants.data", "r");
                    if (f != NULL) {
                        long int f_len;
                        fseek(f, 0, SEEK_END);
                        f_len = ftell(f);
                        assert(f_len <= sizeof (Plant_a));
                        fseek(f, 0, SEEK_SET);
                        fread((void *)plant_a, f_len, 1, f);
                        //print_plant_a("After Read", plant_a);
                        fclose(f);
                    }
                    else {
                        plant_a->count      = 0;
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

            if(IsKeyPressed(KEY_Q)) {
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
            if(IsKeyPressed(KEY_W)) {
                for (int i = 0; i < ground_a->count; ++i) {
                    if(ground_a->items[i].x == player.x &&
                    ground_a->items[i].y == player.y) {
                        ground_a->items[i] = ground_a->items[ground_a->count - 1];
                        --ground_a->count;
                    }
                }
                FILE * f = fopen("assets/ground.data", "w");
                if (f != NULL) {
                    fwrite((void *)ground_a, sizeof (Ground_a), 1, f);
                    fclose(f);
                }
            }

            // @Hack: this should be later
            BeginDrawing();
                ClearBackground(BLACK);
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
                    for (int i = 0; i <  plant_a->count; ++i) {
                        Plant temp = plant_a->items[i];
                        if (plant_a->items[i].x < player.x) temp.x++;
                        if (plant_a->items[i].y < player.y) temp.y++;
                        if (plant_a->items[i].x > player.x) temp.x--;
                        if (plant_a->items[i].y > player.y) temp.y--;
                        int collide_b = 0;
                        for (int i2 = 0; i2 <  plant_a->count; ++i2) {
                            if (i2 == i) continue;
                            if (    player.x == temp.x && 
                                    player.y == temp.y ) {
                                collide_b = 1;
                                break;
                            }
                            if (    plant_a->items[i2].x == temp.x && 
                                    plant_a->items[i2].y == temp.y ) {
                                collide_b = 1;
                                break;
                            }
                        }
                        if (collide_b == 0) { 
                            plant_a->items[i] = temp;
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
                    for (int i = 0; i <  plant_a->count; ++i) {
                        if ( plant_a->items[i].x >= player.x_dest-1 && 
                             plant_a->items[i].x <= player.x_dest+1 &&
                             plant_a->items[i].y >= player.y_dest-1 && 
                             plant_a->items[i].y <= player.y_dest+1 
                           ) {
                            --plant_a->count;
                            plant_a->items[i] = plant_a->items[plant_a->count];

                        }
                    }
                }
            }

            /*
            if (!player.moving_y && !player.moving_x && actionPressed()) {
                // @Todo: 
                plant_a->items[plant_a->count].x = player.x;
                plant_a->items[plant_a->count].y = player.y;
                plant_a->items[plant_a->count].color = GREEN;
                ++plant_a->count;
                assert(plant_a->count < Plant_Max);
                // :write plants.data
                    FILE *f = fopen("assets/plants.data", "w");
                    assert(f != NULL);
                    //print_plant_a("Before Write", plant_a);
                    fwrite(plant_a, sizeof (Plant_a), 1, f);
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
                //print_plant_a("Before Write", plant_a);
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
                    //   :draw water
                    for (int i = 0; i < water_a->count; ++i) {
                        DrawRectangle(water_a->items[i].x * 64, water_a->items[i].y * 64, 64, 64, Fade(BLUE, 0.75f));
                    }
                    //   :draw plant 
                    for (int i = 0; i < plant_a->count; ++i) {
                        DrawTextureEx(plant_tex, (Vector2) { plant_a->items[i].x * 64, plant_a->items[i].y * Tile_Size - Tile_Size / 4  }, 0, 4, BLACK);
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
            {
                for (int i = 0; i <  plant_a->count; ++i) {
                    if (plant_a->items[i].x == player.x_dest && 
                        plant_a->items[i].y == player.y_dest ){
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
            EndDrawing();
        }
    }//while
}// main
