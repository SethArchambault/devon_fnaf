/*
- game_stuff image make them individual 
 * notes: 
 * - png, y - different direction x - animation / different states
 * door sound
 * https://www.youtube.com/watch?v=k3v37Ac_CvI
 * footstep sound
 * https://www.youtube.com/watch?v=btiw_49DeUU
 * ideas:
 *  - eneimies can make thinks mobile
 *  - build your own pizza ria simulation
 *  - make the draw mode load pngs?
 */




#include <math.h>
#include <string.h>
#include "/opt/raylib/src/external/stb_image_write.h"
#include <time.h>  // rand
#define Role_Max 6
#define Image_Size 16 
#define Tile_Size 64
#define Monster_Max 100000
#define Floor_Max 10000
#define Object_Max 10000
#define Water_Max 10000

// :floor type
typedef enum {
    Wood, Tile, Concrete, BrokenTile, Blank, Grass, Asphalt, AsphaltLines, Sidewalk, Dirt, Water, FloorTypeEnd
} FloorType;


// :object
typedef enum {
    Table, Door, Chair, Sink, Stove, Counter, Generator, Switch, Fire, MiniVan, ObjectTypeEnd
} ObjectType;




typedef struct {
    int x;
    int y;
    ObjectType type;
    int rotation;
    int actionFrame;
} Object;

typedef struct {
    Object items[Object_Max];
    int count;
} Objects;

typedef struct {
    int x;
    int y;
    Color color;
    int type; // 0-9
} Monster;


typedef struct {
    int x;
    int y;
    int type;
} Floor;

typedef struct {
    Floor items[Floor_Max];
    int count;
} Floors;


typedef struct {
    Monster items[Monster_Max];
    int count;
} MonsterArray;
void print_monsterArray(char msg[30],MonsterArray *monsterArray) {
    printf("\n%s monsterArray.count %d \n", msg, monsterArray->count);
    for(int i = 0; i < monsterArray->count; ++i) {
        printf("    %d x:%2d y:%2d\n", i, monsterArray->items[i].x, monsterArray->items[i].y);
    }
}

// :modes
// :control mode
typedef struct {
    bool normal;
    bool title;
    bool dialogue;
    bool debug;
    bool build;
} ControlMode;

// :display mode
typedef struct {
    bool normal;
    bool title;
    bool dialogue;
    bool debug;
} DisplayMode;

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
    Action action;
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

Vector2 Vector2PixelsFromXYCoords(int x, int y) {
    return (Vector2){ x * Tile_Size, y * Tile_Size };
}
Rectangle RectanglePixelsFromXYCoords(int x, int y, int size) {
    return (Rectangle){ x * Tile_Size, y * Tile_Size, size, size};
}

int SaveToFilename(void * data, int size, char * filename) {
    char path[100] = "assets/";
    strcat(path, filename);
    FILE * f = fopen(path, "w");
    if (f != NULL) {
        fwrite(data, size, 1, f);
        fclose(f);
        return 1;
    }
    return 0;
}

int SaveToFormattedFilename(void * data, int size, char * string, int value) {
    char filename[100];
    sprintf(filename, string, value);
    return SaveToFilename(data, size, filename);
}

int LoadFromFilename(void * data, int size, char * filename) {
    char path[100] = "assets/";
    strcat(path, filename);
    FILE *f = fopen(path, "r");
    if (f != NULL) {
        long int f_len;
        fseek(f, 0, SEEK_END);
        f_len = ftell(f);
        printf("%s %ld %d\n", filename, f_len, size);
        assert(f_len <= size);
        fseek(f, 0, SEEK_SET);
        fread(data, f_len, 1, f);
        fclose(f);
        return 1;
    }
    return 0;
}
int LoadFromFormattedFilename(void * data, int size, char * string, int value) { 
    char filename[100];
    sprintf(filename, string, value);
    return LoadFromFilename(data, size, filename);
}


// :print text
void printCustom(char * text, Vector2 *cursor, int fontsize, Color color) {
    DrawText(text, cursor->x, cursor->y, fontsize, color);
}
void printWithHeight(char * text, Vector2 *cursor, int fontsize, int height, Color color) {
    printCustom(text, cursor, fontsize, color);
    cursor->y += height;
}
typedef struct {
    Vector2 cursor;
    int fontsize;
    int height;
    Color color; 
} PrintData;
void print(char * text, PrintData * print_data) {
    printWithHeight(text, &print_data->cursor, print_data->fontsize, print_data->height, print_data->color);
}


void game() {
   
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

    Texture2D person_stand_tex      = LoadTexture("assets/devon_standing.png");
    Texture2D person_walk_tex       = LoadTexture("assets/devon_walking.png"); 
    
    //:objectTexture
    Texture2D objectTextureArray[ObjectTypeEnd];
    objectTextureArray[Table]       = LoadTexture("assets/objects/table.png");
    objectTextureArray[Chair]       = LoadTexture("assets/objects/chair.png");
    objectTextureArray[Door]        = LoadTexture("assets/objects/door.png");
    objectTextureArray[Sink]        = LoadTexture("assets/objects/sink.png");
    objectTextureArray[Stove]       = LoadTexture("assets/objects/stove.png");
    objectTextureArray[Counter]     = LoadTexture("assets/objects/counter.png");
    objectTextureArray[Generator]   = LoadTexture("assets/objects/generator.png");
    objectTextureArray[Switch]      = LoadTexture("assets/objects/switch.png");
    objectTextureArray[Fire]        = LoadTexture("assets/objects/fire.png");
    objectTextureArray[MiniVan]     = LoadTexture("assets/objects/mini_van.png");
    ObjectType objectTypeLastUsed = Table;

    // :floor
    Texture2D floorTextureArray[FloorTypeEnd];
    floorTextureArray[Tile]         = LoadTexture("assets/floor/tile_floor.png");
    floorTextureArray[Wood]         = LoadTexture("assets/floor/wood_floor.png");
    floorTextureArray[Concrete]     = LoadTexture("assets/floor/concrete_floor.png");
    floorTextureArray[BrokenTile]   = LoadTexture("assets/floor/broken_tile_floor.png");
    floorTextureArray[Grass]   = LoadTexture("assets/floor/grass_ground.png");
    floorTextureArray[Asphalt]   = LoadTexture("assets/floor/asphalt_floor.png");
    floorTextureArray[AsphaltLines]   = LoadTexture("assets/floor/asphalt_w_line.png");
    floorTextureArray[Sidewalk]   = LoadTexture("assets/floor/sidewalk.png");
    floorTextureArray[Dirt]   = LoadTexture("assets/floor/dirt.png");
    floorTextureArray[Water]   = LoadTexture("assets/floor/water.png");
    FloorType floorTypeLastUsed = Tile;


    // :load sound
    InitAudioDevice(); 
    Sound monsterScreamSound = LoadSound("assets/sound/monster_scream.ogg");  

    Sound doorOpenSound = LoadSound("assets/sound/door_open.ogg");
    Sound doorCloseSound = LoadSound("assets/sound/door_close.ogg");


    // :init
    ControlMode control_mode;
        control_mode.normal     = false;
        control_mode.title      = true;
        control_mode.dialogue   = false;
        control_mode.debug      = false;
        control_mode.build      = false;
    DisplayMode display_mode;
        display_mode.normal     = true;
        display_mode.title      = true;
        display_mode.dialogue   = false;
        display_mode.debug      = false;
    int timer = 0;
    int frame_long = 0;
    int text_tick = 0;
    int frame_passed = 0;
    int frame = 0;
    int jumpscare = 0;

    static int noclip = 0;
    static Player player; 
        player.x            = -11;
        player.y            = -48;
        player.x_dest       = player.x;
        player.y_dest       = player.y;
        player.y_pixel      = player.y * Tile_Size;
        player.x_pixel      = player.x * Tile_Size;
        player.distance     = 0;
        player.moving_x     = false;
        player.moving_y     = false;
        player.y_pixel_dest = player.y_pixel;
        player.x_pixel_dest = player.x_pixel;
        player.direction    = DOWN;
        player.action = STAND;
    static Objects *objects;
        objects         = malloc(sizeof(Objects));
        objects->count  = 0;
    static MonsterArray *monsterArray;
        monsterArray        = malloc(sizeof(MonsterArray));
    static Camera2D camera;
        camera.rotation     = 0.0f;
        camera.zoom         = 1.5f;
    static Floors *floors;
        floors = malloc(sizeof(Floors));
    static int enemy_timer = 0;
    static Monster temp_monster;
    // :read ground.data
    
    if(!LoadFromFilename((void *)floors, sizeof (Floors), "floor.data")) {
        floors->count      = 0;
    }
    // :read monster.data

    if(!LoadFromFilename((void *)monsterArray, sizeof (MonsterArray), "monster.data")) {
        monsterArray->count      = 0;
    }

    // :load objects
    if(LoadFromFilename((void *) objects, sizeof(Objects), "objects.data")) {
        printf("loaded %d objects\n", objects->count);
    }

    
    int dialogue_index = 0;

    // @Todo: create a function which adds to the dialogue queue, and check character limit
    char dialogue_queue[20][100]; 

    typedef enum {
        Accident, EnterFire, LeaveFire, EnterCar
    } Trigger;

    bool triggers[10]; 
    while (!WindowShouldClose()) {
        ++timer;
        if (timer > 1000) timer = 0;
        ++frame_passed;
        ++frame_long;

        // :control
        // :debug control
        if (IsKeyPressed(KEY_F1)) {
            display_mode.debug = !display_mode.debug;
        }


        // :build control
        if (control_mode.build) {
            if(IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_F)) {
                printf("key_left_shift\n");
                // Check if object exists at these coordinates
                int floorExists = 0;
                for (int floorIndex = 0; floorIndex < floors->count;++floorIndex) {
                    Floor *floor = &floors->items[floorIndex];
                    if (floor->x == player.x && floor->y==player.y) {
                        floorExists = 1;
                        ++floor->type;
                        floorTypeLastUsed = floor->type;
                        if (floor->type >= FloorTypeEnd){
                            *floor = floors->items[floors->count-1];
                            --floors->count;
                           
                        } 
                        break;
                    }
                }
                if (!floorExists) {
                    Floor *floor = &floors->items[floors->count];
                    floor->x = player.x;
                    floor->y = player.y;
                    floor->type = 0;
                    floorTypeLastUsed = floor->type;
                    ++floors->count;
                }
                // :save floors
                SaveToFilename((void *)floors, sizeof(Floors),"floor.data");
            }
            if(!IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_F)) {
                // Check if object exists at these coordinates
                int floorExists = 0;
                for (int floorIndex = 0; floorIndex < floors->count;++floorIndex) {
                    Floor *floor = &floors->items[floorIndex];
                    if (floor->x == player.x && floor->y==player.y) {
                        floorExists = 1;
                        if (floorTypeLastUsed) {
                            floor->type = floorTypeLastUsed;
                        }
                        else {
                            *floor = floors->items[floors->count-1];
                            --floors->count;
                        } 
                        break;
                    }
                }
                if (!floorExists && floorTypeLastUsed) {
                    Floor *floor = &floors->items[floors->count];
                    floor->x = player.x;
                    floor->y = player.y;
                    floor->type = floorTypeLastUsed;
                    ++floors->count;
                }
                // :save floors
                SaveToFilename((void *)floors, sizeof(Floors),"floor.data");
            }
            // :change state of object
            if (IsKeyPressed(KEY_S)) {
                // Check if object exists at these coordinates
                for (int objectIndex = 0; objectIndex < objects->count;++objectIndex) {
                    Object *object = &objects->items[objectIndex];
                    if (object->x == player.x && object->y==player.y) {
                        ++object->actionFrame;
                        if (object->actionFrame >= 4){
                            object->actionFrame = 0;
                        } 
                        break;
                    }
                }
                // :save objects
                SaveToFilename((void *)objects, sizeof(Objects),"objects.data");
            }
            // :noclip
            if (IsKeyPressed(KEY_N)) {
                noclip = !noclip;
            }
            // :change rotation of object
            if (IsKeyPressed(KEY_R)) {
                // Check if object exists at these coordinates
                for (int objectIndex = 0; objectIndex < objects->count;++objectIndex) {
                    Object *object = &objects->items[objectIndex];
                    if (object->x == player.x && object->y==player.y) {
                        ++object->rotation;
                        printf("rotated %d\n", object->rotation);
                        if (object->rotation >= 4){
                            object->rotation = 0;
                        } 
                        break;
                    }
                }
                // :save objects
                SaveToFilename((void *)objects, sizeof(Objects),"objects.data");
            }

            // :key_o
            // :create object
            if (IsKeyPressed(KEY_O)) {
                // Check if object exists at these coordinates
                int objectExists = 0;
                for (int objectIndex = 0; objectIndex < objects->count;++objectIndex) {
                    Object *object = &objects->items[objectIndex];
                    if (object->x == player.x && object->y==player.y) {
                        objectExists = 1;
                        ++object->type;
                        if (object->type >= ObjectTypeEnd){
                            printf("end %d\n", ObjectTypeEnd);
                            *object = objects->items[objects->count-1];
                            --objects->count;
                        } 
                        break;
                    }
                }
                if (!objectExists) {
                    Object *object = &objects->items[objects->count];
                    object->x = player.x;
                    object->y = player.y;
                    object->type = 0;
                    ++objects->count;
                }
                // :save objects
                SaveToFilename((void *)objects, sizeof(Objects),"objects.data");
            }
            if (IsKeyPressed(KEY_EQUAL)) {
                camera.zoom += 1;
            }
            if (IsKeyPressed(KEY_MINUS)) {
                camera.zoom -= 1;
            }
            // :place monster
            int key_pressed = GetKeyPressed();
            if (key_pressed >= 48 && key_pressed <= 57) {
                if (monsterArray->count < Monster_Max) {
                    monsterArray->items[monsterArray->count].x = player.x;
                    monsterArray->items[monsterArray->count].y = player.y;
                    monsterArray->items[monsterArray->count].type = key_pressed-48; // 0-9
                    ++monsterArray->count;
                    SaveToFilename((void *) monsterArray,sizeof(MonsterArray), "monster.data");
                }
            }
            // :clear
            if(IsKeyDown(KEY_C)) {
                for (int i = 0; i < monsterArray->count; ++i) {
                    if(monsterArray->items[i].x == player.x &&
                    monsterArray->items[i].y == player.y) {
                        monsterArray->items[i] = monsterArray->items[monsterArray->count - 1];
                        --monsterArray->count;
                    }
                }
                for (int i = 0; i < objects->count; ++i) {
                    if(objects->items[i].x == player.x &&
                    objects->items[i].y == player.y) {
                        objects->items[i] = objects->items[objects->count - 1];
                        --objects->count;
                    }
                }
                SaveToFilename((void *)monsterArray, sizeof(MonsterArray), "monster.data");
                SaveToFilename((void *)objects, sizeof(Objects),"objects.data");

            }
        }

        // :normal control
        if (control_mode.normal) {
            // controls
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
            static float speed = 4.0f; 

            // :collision
            int collision = 0;
            if (!noclip) {
                collision = 1;
                for(int i = 0; i < floors->count; ++i) {
                    Floor *floor = &floors->items[i];
                    if (floor->x == player.x_dest && floor->y == player.y_dest) {
                        if (floor->type != Water) {
                            collision = 0;
                        }

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

            // :move player
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

            // :triggers
            if (!triggers[Accident] && player.y == -32) {
                triggers[Accident] = true;
                strcpy(dialogue_queue[0], "Oh no! What a horrible accident!");
                strcpy(dialogue_queue[1], "I hope no ones hurt.");
                strcpy(dialogue_queue[2], "Though there doesn't seem to be much chance of that...");
                strcpy(dialogue_queue[3], "\0");
                // @Todo: This should be moved somewhere.. state control?
                control_mode.dialogue = true;
                control_mode.normal = false;
                display_mode.dialogue = true;
            }

            bool player_stands_on_fire = false;
            for (int i = 0; i < objects->count; ++i) {
                Object * object = &objects->items[i];
                if (object->type == Fire && object->x == player.x && object->y == player.y) {
                    player_stands_on_fire = true;
                }
            }

            if (!triggers[EnterFire] && player_stands_on_fire) {
                triggers[EnterFire] = true;
                strcpy(dialogue_queue[0], "Ow it burns!");
                strcpy(dialogue_queue[1], "My whole body is on fire!!");
                strcpy(dialogue_queue[2], "...I hope this won't make me late for work.");
                strcpy(dialogue_queue[3], "\0");
                // @Todo: This should be moved somewhere.. state control?
                control_mode.dialogue = true;
                control_mode.normal = false;
                display_mode.dialogue = true;
            }
            if (triggers[EnterFire]  && !triggers[LeaveFire] && !player_stands_on_fire) {
                triggers[LeaveFire] = true;
                strcpy(dialogue_queue[0], "Phew!");
                strcpy(dialogue_queue[1], "Good thing I wore my fireproof clothes!");
                strcpy(dialogue_queue[2], "...and fireproof facecream.");
                strcpy(dialogue_queue[3], "\0");
                // @Todo: This should be moved somewhere.. state control?
                control_mode.dialogue = true;
                control_mode.normal = false;
                display_mode.dialogue = true;
            }

            bool player_stands_car = false;
            for (int i = 0; i < objects->count; ++i) {
                Object * object = &objects->items[i];
                if (object->type == MiniVan && object->x == player.x && object->y == player.y) {
                    player_stands_car = true;
                }
            }

            if (!triggers[EnterCar] && player_stands_car) {
                triggers[EnterCar] = true;
                strcpy(dialogue_queue[0], "The passengers are so tiny.");
                strcpy(dialogue_queue[1], "I'm sure they'll be fine though.");
                strcpy(dialogue_queue[2], "...once a tiny ambulance arrives.");
                strcpy(dialogue_queue[3], "\0");
                // @Todo: This should be moved somewhere.. state control?
                control_mode.dialogue = true;
                control_mode.normal = false;
                display_mode.dialogue = true;
            }

           // :spacebar
            // :action
            // :shoot
#define MonsterDead 9
            if (actionPressed()) {
                if (player.direction == RIGHT) {
                    // @Todo: Add back in shooting rectangle
                    for (int i = 0; i < 10; ++i) {
                        for (int mi = 0; mi < monsterArray->count; ++mi) {
                            Monster * monster = &monsterArray->items[mi];
                            if (monster->x == player.x + i && monster->y == player.y) {
                                monster->type = MonsterDead;
                            goto ShootingDone;
                            }
                        }
                    }
                }
                if (player.direction == LEFT) {
                    // @Todo: Add back in shooting rectangle
                    for (int i = 0; i < 10; ++i) {
                        for (int mi = 0; mi < monsterArray->count; ++mi) {
                            Monster * monster = &monsterArray->items[mi];
                            if (monster->x == player.x - i && monster->y == player.y) {
                                monster->type = MonsterDead;
                                goto ShootingDone;
                            }
                        }

                    }
                }
                if (player.direction == UP) {
                    // @Todo: Add back in shooting rectangle
                    for (int i = 0; i < 10; ++i) {
                        for (int mi = 0; mi < monsterArray->count; ++mi) {
                            Monster * monster = &monsterArray->items[mi];
                            if (monster->y == player.y - i && monster->x == player.x) {
                                monster->type = MonsterDead;
                                goto ShootingDone;
                            }
                        }

                    }
                }
                if (player.direction == DOWN) {
                    // @Todo: Add back in shooting rectangle
                    for (int i = 0; i < 10; ++i) {
                        for (int mi = 0; mi < monsterArray->count; ++mi) {
                            Monster * monster = &monsterArray->items[mi];
                            if (monster->y == player.y + i && monster->x == player.x) {
                                monster->type = MonsterDead;
                                goto ShootingDone;
                            }
                        }

                    }
                }

            }
            ShootingDone:;
                         
            // :move monsters
                    ++enemy_timer;
                    int enemy_speed = 100;
#define SethIndex 2
#define DoorIndex 8
                    if (enemy_timer > enemy_speed && !noclip) {
                        for (int i = 0; i < monsterArray->count; ++i) {
                            Monster * monster = &monsterArray->items[i];
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
                            for (int n = 0; n < monsterArray->count; ++n) {
                                Monster * monster2 = &monsterArray->items[n];
                                if (n == i) continue;
                                if (monster2->x == monster->x && monster->y == monster2->y) {
                                    memcpy(monster, &temp_monster, sizeof(Monster));
                                    break;
                                }

                            }
                            int collision = 1;
                            for(int w = 0; w < floors->count; ++w) {
                                if (floors->items[w].x == monster->x && floors->items[w].y == monster->y) {
                                    collision = 0;
                                    break;
                                }
                            }
                            if (collision) {
                                memcpy(monster, &temp_monster, sizeof(Monster));
                            }
                        }
                    }


        } // normal control

        // :dialogue control
        if (control_mode.dialogue) {
            if (IsKeyPressed(KEY_ENTER)) {
                dialogue_index++;
                text_tick = 0;
                if (dialogue_queue[dialogue_index][0] == '\0') {
                    strcpy(dialogue_queue[0], "\0");
                    dialogue_index = 0;
                    control_mode.dialogue   = false;
                    control_mode.normal     = true;
                    display_mode.normal     = true;
                    display_mode.dialogue   = false;
                } 
            }
        }
        // :title control
        if (control_mode.title) {
            if (IsKeyPressed(KEY_B)) {
                control_mode.title      = false;
                control_mode.normal     = true;
                control_mode.build      = true;

                display_mode.title      = false;
                display_mode.normal     = true;
            }
            if (IsKeyPressed(KEY_ENTER)) {
                control_mode.title      = false;
                control_mode.dialogue   = true;

                display_mode.title      = false;
                display_mode.normal     = true;
                display_mode.dialogue   = true;

                strcpy(dialogue_queue[0], "I wish I could do some fishing...");
                strcpy(dialogue_queue[1], "Too bad I have to get to my new job...");
                strcpy(dialogue_queue[2], "\0");
            }
        }

        // :display
        BeginDrawing();
        ClearBackground(BLACK);

        if (display_mode.normal) {
            // :camera calibration
            camera.offset.x     = screen.x/2 - 32 + -(player.x_pixel * camera.zoom);
            camera.offset.y     = screen.y/2 - 32 + -(player.y_pixel * camera.zoom);

            // :animate objects
            if (timer % 20 == 0) {
                for (int objectIndex = 0; 
                       objectIndex < objects->count; 
                       ++objectIndex) {
                    Object * object = &objects->items[objectIndex];
                    if (object->type == Generator) {
                        if (object->actionFrame <= 3) {
                            object->actionFrame++;
                            if (object->actionFrame > 3) {
                                object->actionFrame  = 0;
                            }
                        }// if
                    } //if
                    if (object->type == Door) {
                        if (object->actionFrame > 0 && object->actionFrame < 3) {
                            if (!IsSoundPlaying(doorOpenSound)) {
                                PlaySound(doorOpenSound);
                            }
                            object->actionFrame++;
                        }// if
                    } //if
                    if (timer % 140 == 0) {
                        if (object->type == Sink) {
                            if (object->actionFrame > 0 && object->actionFrame < 3) {
                                object->actionFrame++;
                            }// if
                        } //if
                    }//if
                }//for
            }

            // :animate player

            if (player.action != DEAD && frame_passed > 10) {
                if (player.moving_y || player.moving_x) { 
                    player.action = WALK;
                } else { 
                    player.action = STAND;
                }
                frame_passed = 0;
                ++frame;
                if (frame > 4) frame = 0;
            }

            BeginMode2D(camera);
                //   :draw floor 
                for (int floorIndex = 0; 
                       floorIndex < floors->count; 
                       ++floorIndex) {
                    Floor * floor = &floors->items[floorIndex];
                    if (floor->type != Blank) {
                        DrawTextureEx(floorTextureArray[floor->type], Vector2PixelsFromXYCoords(floor->x, floor->y), 0, 4, WHITE);
                    }
                }
                //   :draw objects
                for (int objectIndex = 0; 
                       objectIndex < objects->count; 
                       ++objectIndex) {
                    Object * object = &objects->items[objectIndex];
                    // horizontal - all object should have orientation.. North East South West
                    // vertical - different states,
                    // - door is closed
                    // - door is opening
                    // - door is open
                    // - DOOR IS CLOSING
                    // - switch is on
                    // - switch is turning off
                    // - switch is off
                    // - switch is turning on
                    // - oven is on
                    // void DrawTexturePro(Texture2D texture, Rectangle sourceRec, Vector2 position, Color tint);
                    DrawTexturePro(objectTextureArray[object->type],  (Rectangle){ object->actionFrame * 16, object->rotation * 16,16,16}, RectanglePixelsFromXYCoords(object->x, object->y, Tile_Size), (Vector2){0,0}, 0, WHITE);
                }
                // :draw monster
                for (int monsterIndex = 0; 
                        monsterIndex < monsterArray->count; 
                        ++monsterIndex) {
                    Monster * monster = &monsterArray->items[monsterIndex];
                    if(enemy_timer > 30 && monster->type == SethIndex) {
                        DrawRectangle(monster->x*64 - 64, monster->y * 64 -64, 260,32, (Color) {37,75,165,255});
                        DrawText("Would you like a glass of SHUT UP NOW?",
                            monster->x * 64 - 54, // xpos
                            monster->y * 64 - 52,
                            12, // fontsize
                            WHITE
                        );
                    }
                    // @Todo: convert this to a variable or something
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
                // :draw player
                static Rectangle player_rect; 
                player_rect.height = Tile_Size;
                player_rect.width = Tile_Size;
                player_rect.x = player.x_pixel;
                player_rect.y = player.y_pixel - Tile_Size / 4;
                if (player.action == STAND) {
                    DrawTexturePro(person_stand_tex, (Rectangle) { 32 * frame, 0, 32, 32 }, player_rect, (Vector2){ 0, 0 }, 0, WHITE);
                } 
                else if (player.action == DEAD) {
                    DrawTexturePro(person_walk_tex, (Rectangle) { 32 * frame, 0, 32, 32 }, player_rect, (Vector2){ -12, 64 }, 90, WHITE);
                } else {
                    DrawTexturePro(person_walk_tex, (Rectangle) { 32 * frame, 0, 32, 32 }, player_rect, (Vector2){ 0, 0 }, 0, WHITE);
                }
            EndMode2D();

            // :jumpscare
            // @todo: compress
            if (jumpscare && !noclip) {
                Monster * collidedMonster = NULL;
                for (int monsterIndex = 0; 
                    monsterIndex < monsterArray->count; 
                    ++monsterIndex) {
                    Monster * monster = &monsterArray->items[monsterIndex];
                    if (monster->x == player.x && monster->y == player.y) {
                        collidedMonster = monster;
                        break;
                    }
                }
                if (collidedMonster) {
                    if (!IsSoundPlaying(monsterScreamSound)) {
                        PlaySound(monsterScreamSound);
                    }
                    DrawRectangle(0,0,screen.x, screen.y, BLACK);
                    DrawRectangle(0,0,screen.x, screen.y, Fade(RED,(rand() %100)/100.0f));
                    DrawTexturePro(monster_tex_a[collidedMonster->type], 
                     /* src    */   (Rectangle) { 0,      0,   16,   16 }, 
                     /* dest   */   (Rectangle) { 612,  512, 1024, 1024 },
                     /* origin   */ (Vector2)   { 512,  512},             
                     /* rotation */ -8 +(rand() % 16),  /* tint */ WHITE);
                } else {
                    if (IsSoundPlaying(monsterScreamSound)) {
                        StopSound(monsterScreamSound);
                    }
                }
            }// noclip

        } // display_mode.normal


        if (display_mode.title) {
            PrintData print_data;
                print_data.cursor.x =  screen.x/2 - 450;
                print_data.cursor.y = 100;
                print_data.fontsize = 60;
                print_data.height   = 70;
                print_data.color    = WHITE;
            print("Five Nights at YouTube Factory", &print_data);
            print_data.fontsize = 46;
            print("A Horror Pixel Production", &print_data);
            print_data.fontsize = 23;
            print("Press Enter to Play", &print_data);
        }

        // :dialogue display
        if (display_mode.dialogue) {
            PrintData print_data;
                print_data.cursor.x = screen.x /2 + 100 ;
                print_data.cursor.y = 200;
                print_data.fontsize = 16;
                print_data.height   = 40;
                print_data.color    = WHITE;

            DrawRectangle(print_data.cursor.x-25, print_data.cursor.y -25, 500,300, Fade(BLACK, 0.5));

            char text[200];
            strcpy(text, dialogue_queue[dialogue_index]);
            if (text_tick < strlen(text)) {
                text[text_tick] = '\0';
                if (frame_long % 2 == 0) text_tick++; 
            } 
            // -33 car accident
            print(text, &print_data);
        }

        // :debug display
        if (display_mode.debug) {
            PrintData print_data;
                print_data.cursor.x =  20;
                print_data.cursor.y = 20;
                print_data.fontsize = 18;
                print_data.height   = 25;
                print_data.color    = WHITE;


            DrawRectangle(0, 0, 300,600, Fade(BLACK, 0.5));
            char text[40];
            sprintf(text, "X: %d Y: %d", player.x, player.y);
            print(text, &print_data);
            if (control_mode.title) print("control title", &print_data); 
            if (control_mode.normal) print("control normal", &print_data); 
            if (control_mode.dialogue) print("control dialogue", &print_data); 
            if (display_mode.title) print("display title", &print_data); 
            if (display_mode.normal) print("display normal", &print_data); 
            if (display_mode.dialogue) print("display dialogue", &print_data); 
            print("KEYS", &print_data); 
            print("c = clear", &print_data);
            print("f = draw floor", &print_data);
            print("shift f = change floor", &print_data);
            print("m = monster", &print_data);
            print("o = place object", &print_data);

        } // display debug
        EndDrawing();
    }//while
}// main

