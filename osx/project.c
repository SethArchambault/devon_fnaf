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
 *  x -10 y -60
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


// :state type
typedef enum {
    NormalState, DialogueState
} StateType;


// :floor type
typedef enum {
    Wood, Tile, Concrete, BrokenTile, Blank, Grass, Asphalt, AsphaltLines, Sidewalk, Dirt, Water, FloorTypeEnd
} FloorType;

// :object
typedef enum {
    Table, Door, Chair, Sink, Stove, Counter, Generator, Switch, Fire, MiniVan, Important, ObjectTypeEnd
} ObjectType;

// monster:
typedef enum {
    BENNY, BUDDY, ALICE, ALEX, BOB, DENNIS, DONNY, MonsterTypeEnd
} MonsterType;

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

// :modes
// :control mode
typedef struct {
    bool normal_control;
    bool normal_display;
    bool title_control;
    bool title_display;
    bool dialogue_control;
    bool dialogue_display;
    bool debug_control;
    bool debug_display;
    bool build_control;
    bool intro_control;
    bool monsters_control;
    bool jumpscare_display;
    bool failure_control;
    bool failure_display;
} State;

void changeState(State * state, StateType new_state) {
    if (new_state == NormalState) {
        state->dialogue_control     = false;
        state->normal_control       = true;
        state->normal_display       = true;
        state->dialogue_display     = false;
    }
    if (new_state == DialogueState) {
        state->intro_control        = false;
        state->normal_control       = false;
        state->dialogue_control     = true;
        state->dialogue_display     = true;
    }
}

// :timer
typedef enum{
    JumpscareClock, WaterClock, FailureClock
} Clock;

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
    Texture2D objectTextures[ObjectTypeEnd];
    objectTextures[Table]       = LoadTexture("assets/objects/table.png");
    objectTextures[Chair]       = LoadTexture("assets/objects/chair.png");
    objectTextures[Door]        = LoadTexture("assets/objects/door.png");
    objectTextures[Sink]        = LoadTexture("assets/objects/sink.png");
    objectTextures[Stove]       = LoadTexture("assets/objects/stove.png");
    objectTextures[Counter]     = LoadTexture("assets/objects/counter.png");
    objectTextures[Generator]   = LoadTexture("assets/objects/generator.png");
    objectTextures[Switch]      = LoadTexture("assets/objects/switch.png");
    objectTextures[Fire]        = LoadTexture("assets/objects/fire.png");
    objectTextures[MiniVan]     = LoadTexture("assets/objects/mini_van.png");
    objectTextures[Important]   = LoadTexture("assets/objects/important.png");

    // :floor
    Texture2D floorTextures[FloorTypeEnd];
    floorTextures[Tile]         = LoadTexture("assets/floor/tile_floor.png");
    floorTextures[Wood]         = LoadTexture("assets/floor/wood_floor.png");
    floorTextures[Concrete]     = LoadTexture("assets/floor/concrete_floor.png");
    floorTextures[BrokenTile]   = LoadTexture("assets/floor/broken_tile_floor.png");
    floorTextures[Grass]        = LoadTexture("assets/floor/grass_ground.png");
    floorTextures[Asphalt]      = LoadTexture("assets/floor/asphalt_floor.png");
    floorTextures[AsphaltLines] = LoadTexture("assets/floor/asphalt_w_line.png");
    floorTextures[Sidewalk]     = LoadTexture("assets/floor/sidewalk.png");
    floorTextures[Dirt]         = LoadTexture("assets/floor/dirt.png");
    floorTextures[Water]        = LoadTexture("assets/floor/water_moving.png");
    FloorType floorTypeLastUsed = Tile;

    // :monster
    Texture2D monsterTextures[MonsterTypeEnd];
    monsterTextures[BENNY]      = LoadTexture("assets/monsters/1benny_beaver_bear.png");
    monsterTextures[BUDDY]      = LoadTexture("assets/monsters/2buddy_the_wolf.png");
    monsterTextures[ALICE]      = LoadTexture("assets/monsters/3alice_the_duck.png");
    monsterTextures[ALEX]       = LoadTexture("assets/monsters/4alex_the_honeybadger.png");
    monsterTextures[BOB]        = LoadTexture("assets/monsters/5bob_the_giraffe.png");
    monsterTextures[DENNIS]     = LoadTexture("assets/monsters/6dennis_the_allegator.png");
    monsterTextures[DONNY]      = LoadTexture("assets/monsters/7donny_the_dog.png");

    // :load sound
    InitAudioDevice(); 
    Sound monsterScreamSound    = LoadSound("assets/sound/monster_scream.ogg");  
    Sound doorOpenSound         = LoadSound("assets/sound/door_open.ogg");
    Sound doorCloseSound        = LoadSound("assets/sound/door_close.ogg");


    //
    // :init 
    //
    int camera_offset_y = 500;
    int hide_outdoors = 0;
    State state;
        state.normal_control     = false;
        state.title_control      = true;
        state.intro_control      = false;
        state.dialogue_control   = false;
        state.debug_control      = false;
        state.build_control      = false;
        state.monsters_control   = true;
        state.normal_display     = true;
        state.title_display      = true;
        state.dialogue_display   = false;
        state.failure_display     = false;
        state.jumpscare_display  = true;
        state.debug_display      = false;
    int clock[10];
    clock[WaterClock]       = 0;
    clock[JumpscareClock]   = 0;
    clock[FailureClock]     = 0;
    int timer = 0;
    int frame_long = 0;
    int text_tick = 0;
    int frame_passed = 0;
    int frame = 0;
    int jumpscare = 0;
    Monster * collidedMonster = NULL;
    int noclip = 0;
    float speed = 4.0f; 
    Player player; 
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
    Objects *objects;
        objects         = malloc(sizeof(Objects));
        objects->count  = 0;
    Monster monsters[Monster_Max];
    int monster_count;
    Camera2D camera;
        camera.rotation     = 0.0f;
        camera.zoom         = 1.5f;
    Floors *floors;
        floors = malloc(sizeof(Floors));
    int enemy_timer = 0;
    Monster temp_monster;
    // :read ground.data
    
    if(!LoadFromFilename((void *)floors, sizeof (Floors), "floor.data")) {
        floors->count      = 0;
    }

    // :load objects
    if(LoadFromFilename((void *) objects, sizeof(Objects), "objects.data")) {
        printf("loaded %d objects\n", objects->count);
    }

    
    int dialogue_index = 0;

    // @Todo: create a function which adds to the dialogue queue, and check character limit
    char dialogue_queue[20][100]; 

    // :trigger
    typedef enum {
        Accident, EnterFire, LeaveFire, EnterCar, ViewsDarkness, SeeDamange, SeeGenerator, BadDoor, SeeFireplace, SeeSecret, DoorEntered
    } Trigger;

    bool triggers[20]; 
    while (!WindowShouldClose()) {
        ++timer;
        if (timer > 1000) timer = 0;
        ++frame_passed;
        ++frame_long;

        // :control
        // :debug control
        if (IsKeyPressed(KEY_F1)) {
            state.debug_display = !state.debug_display;
        }

        // :intro control
        if (state.intro_control) {
            camera_offset_y -= 3;
            if (camera_offset_y < 0) {
                camera_offset_y = 0;
                strcpy(dialogue_queue[0], "I wish I could do some fishing...");
                strcpy(dialogue_queue[1], "Too bad I have to get to my new job...");
                strcpy(dialogue_queue[2], "\0");
                changeState(&state, DialogueState);
            }
        }

        // :build control
        if (state.build_control) {
            if(IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_F)) {
                printf("key_left_shift\n");
                // Check if object exists at these coordinates
                int floorExists = 0;
                for (int i = 0; i < floors->count;++i) {
                    Floor *floor = &floors->items[i];
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
                for (int i = 0; i < floors->count;++i) {
                    Floor *floor = &floors->items[i];
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
                for (int i = 0; i < objects->count;++i) {
                    Object *object = &objects->items[i];
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
                for (int i = 0; i < objects->count;++i) {
                    Object *object = &objects->items[i];
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
                for (int i = 0; i < objects->count;++i) {
                    Object *object = &objects->items[i];
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
                if (monster_count < Monster_Max) {
                    monsters[monster_count].x = player.x;
                    monsters[monster_count].y = player.y;
                    monsters[monster_count].type = key_pressed-48; // 0-9
                    ++monster_count;
                }
            }
            // :clear
            if(IsKeyDown(KEY_C)) {
                for (int i = 0; i < monster_count; ++i) {
                    if(monsters[i].x == player.x &&
                    monsters[i].y == player.y) {
                        monsters[i] = monsters[monster_count - 1];
                        --monster_count;
                    }
                }
                for (int i = 0; i < objects->count; ++i) {
                    if(objects->items[i].x == player.x &&
                    objects->items[i].y == player.y) {
                        objects->items[i] = objects->items[objects->count - 1];
                        --objects->count;
                    }
                }
                SaveToFilename((void *)objects, sizeof(Objects),"objects.data");
            }
        }

        // :normal control
        if (state.normal_control) {
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
                if (player.y_dest < -24 && hide_outdoors) {
                    collision = 1;
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
            if (!triggers[ViewsDarkness] && player.y == -41 && player.x == -5) {
                triggers[ViewsDarkness] = true;
                strcpy(dialogue_queue[0], "Sometimes I wonder if there is a world beyond the darkness..");
                strcpy(dialogue_queue[1], "But try as I might, I can't bring myself to step foot into it.");
                strcpy(dialogue_queue[2], "As if there is an invisible force compelling me...");
                strcpy(dialogue_queue[3], "\0");
                changeState(&state, DialogueState);
            }

            if (!triggers[Accident] && player.y == -32) {
                triggers[Accident] = true;
                strcpy(dialogue_queue[0], "Oh no! What a horrible accident!");
                strcpy(dialogue_queue[1], "I hope no ones hurt.");
                strcpy(dialogue_queue[2], "Though there doesn't seem to be much chance of that...");
                strcpy(dialogue_queue[3], "\0");
                changeState(&state, DialogueState);
            }

            if (!triggers[DoorEntered] && player.y == -23) {
                if (!IsSoundPlaying(doorCloseSound)) {
                    PlaySound(doorCloseSound);
                }
                triggers[DoorEntered] = true;
                // close door
                objects->items[14].actionFrame = 0;
                hide_outdoors = 1;


                strcpy(dialogue_queue[0], "Dang! The wind slammed the door on me!");
                strcpy(dialogue_queue[1], "And it won't budge! Guess I'm here till the next shift comes for me..");
                strcpy(dialogue_queue[2], "\0");
                changeState(&state, DialogueState);
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
                changeState(&state, DialogueState);
            }
            if (triggers[EnterFire]  && !triggers[LeaveFire] && !player_stands_on_fire) {
                triggers[LeaveFire] = true;
                strcpy(dialogue_queue[0], "Phew!");
                strcpy(dialogue_queue[1], "Good thing I wore my fireproof clothes!");
                strcpy(dialogue_queue[2], "...and fireproof facecream.");
                strcpy(dialogue_queue[3], "\0");
                changeState(&state, DialogueState);
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
                changeState(&state, DialogueState);
            }


            if (!triggers[SeeDamange] && player.x == -5  && player.y == -15) {
                triggers[SeeDamange] = true;
                strcpy(dialogue_queue[0], "Man this place is a dump..");
                strcpy(dialogue_queue[1], "How could they let it get this way?");
                strcpy(dialogue_queue[2], "...Unless this just happened?");
                strcpy(dialogue_queue[3], "\0");
                changeState(&state, DialogueState);
            }

            if (!triggers[SeeGenerator] && player.x == -2  && player.y == 3) {
                triggers[SeeGenerator] = true;
                strcpy(dialogue_queue[0], "What an odd place for a generator..");
                strcpy(dialogue_queue[1], "Who designed this place?");
                strcpy(dialogue_queue[2], "\0");
                changeState(&state, DialogueState);
                // create monster.. x:-12 y:3
                if (monster_count < Monster_Max) {
                    monsters[monster_count].x = player.x - 10;
                    monsters[monster_count].y = player.y;
                    monsters[monster_count].type = 0; // 0-9
                    ++monster_count;
                }
            }
            
            if (!triggers[BadDoor] && player.x == 12 && player.y == -8) {
                triggers[BadDoor] = true;
                strcpy(dialogue_queue[0], "I can pass right through doors!");
                strcpy(dialogue_queue[1], "No wonder this place is so drafty.");
                strcpy(dialogue_queue[2], "\0");
                changeState(&state, DialogueState);
            }

            if (!triggers[SeeFireplace] && player.x == -28  && player.y == -7) {
                triggers[SeeFireplace] = true;
                strcpy(dialogue_queue[0], "Someone left the fireplace going!");
                strcpy(dialogue_queue[1], "They must of left in a hurry..");
                strcpy(dialogue_queue[2], "\0");
                changeState(&state, DialogueState);
            }

            if (!triggers[SeeSecret] && player.x == -2  && player.y == 20) {
                triggers[SeeSecret] = true;
                strcpy(dialogue_queue[0], "There's a place down there to the right, but I can't get to it..");
                strcpy(dialogue_queue[1], "Wait a mi--");
                strcpy(dialogue_queue[2], "..I can see through walls!");
                strcpy(dialogue_queue[3], "\0");
                changeState(&state, DialogueState);
            }

        } // normal control

        // :dialogue control
        if (state.dialogue_control) {
            if (IsKeyPressed(KEY_ENTER)) {
                dialogue_index++;
                text_tick = 0;
                if (dialogue_queue[dialogue_index][0] == '\0') {
                    strcpy(dialogue_queue[0], "\0");
                    dialogue_index = 0;
                    changeState(&state, NormalState);
                } 
            }
        }
        if (state.failure_control) {
            if (IsKeyPressed(KEY_R)) {
                exit(1);
            }
        }
        // :title control
        if (state.title_control) {
            // :build selection
            if (IsKeyPressed(KEY_B)) {
                camera_offset_y = 0;
                state.build_control      = true;
                state.title_control      = false;
                state.normal_control     = true;

                state.title_display      = false;
                state.jumpscare_display  = false;
                state.normal_display     = true;
            }
            if (IsKeyPressed(KEY_ENTER)) {
                state.title_control      = false;
                state.intro_control      = true;

                state.title_display      = false;
                state.normal_display     = true;
            }
        }

        // :monsters control
        if (state.monsters_control) {
            // :move monsters
            ++enemy_timer;
            int enemy_speed = 40;
            if (enemy_timer > enemy_speed && !noclip) {
                for (int i = 0; i < monster_count; ++i) {
                    Monster * monster = &monsters[i];
                    memcpy(&temp_monster,monster, sizeof(Monster));
                    enemy_timer = 0;
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

                    for (int n = 0; n < monster_count; ++n) {
                        Monster * monster2 = &monsters[n];
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
        }

        // :display
        BeginDrawing();
        ClearBackground(BLACK);

        if (state.normal_display) {
            // :timers
            if(frame_long % 30 == 0) {
                clock[WaterClock]++;
            }
            if(clock[WaterClock] > 1) clock[WaterClock] = 0;

            // :camera calibration
            camera.offset.x     = screen.x/2 - 32 + -(player.x_pixel * camera.zoom);
            camera.offset.y     = camera_offset_y + screen.y/2 - 32 + -(player.y_pixel * camera.zoom);

            // :animate objects
            if (timer % 20 == 0) {
                for (int i = 0; i < objects->count; ++i) {
                    Object * object = &objects->items[i];
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
                for (int i = 0; i < floors->count; ++i) {
                    Floor * floor = &floors->items[i];
                    if (floor->y < -24 && hide_outdoors) {
                        // skip drwaing outdoors
                    }
                    else if (floor->type == Water) {
                        DrawTexturePro(floorTextures[Water], (Rectangle) { 16 * clock[WaterClock], 0, 16, 16 }, RectanglePixelsFromXYCoords(floor->x, floor->y, Tile_Size), (Vector2){0,0}, 0, WHITE);
                    }
                    else if (floor->type != Blank) {
                        DrawTextureEx(floorTextures[floor->type], Vector2PixelsFromXYCoords(floor->x, floor->y), 0, 4, WHITE);
                    }
                }
                //   :draw objects
                for (int i = 0; i < objects->count; ++i) {
                    Object * object = &objects->items[i];
                    if (object->y < -24 && hide_outdoors) {
                        // skip drwaing outdoors
                    }
                    else {
                        DrawTexturePro(objectTextures[object->type],  (Rectangle){ object->actionFrame * 16, object->rotation * 16,16,16}, RectanglePixelsFromXYCoords(object->x, object->y, Tile_Size), (Vector2){0,0}, 0, WHITE);
                    }
                }

                // :draw monster
                for (int i = 0; i < monster_count; ++i) {
                    Monster * monster = &monsters[i];
                    DrawTextureEx(monster_tex_a[monster->type], (Vector2) { monster->x * 64, monster->y * Tile_Size - Tile_Size / 4  }, 0, 4, WHITE);
                } 

                // :draw player
                Rectangle player_rect; 
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

        } // state.normal_display
        if (state.title_display) {
            PrintData print_data;
                print_data.cursor.x =  screen.x/2 - 450;
                print_data.cursor.y = 100;
                print_data.fontsize = 60;
                print_data.height   = 70;
                print_data.color    = WHITE;
            print("Five Nights at Benny's Pizzeria", &print_data);
            print_data.fontsize = 30;
            print("A Horror Pixel Production", &print_data);
            print_data.fontsize = 23;
            print("Press Enter to Play", &print_data);
        }
// :dialogue display
        if (state.dialogue_display) {
            PrintData print_data;
                print_data.cursor.x = screen.x /2 + 100 ;
                print_data.cursor.y = 250;
                print_data.fontsize = 16;
                print_data.height   = 40;
                print_data.color    = WHITE;

            DrawRectangle(print_data.cursor.x-25, print_data.cursor.y -25, 500,300, Fade(BLACK, 0.8));
            DrawRectangleLines(print_data.cursor.x-25, print_data.cursor.y -25, 500,300, Fade(WHITE, 0.8));

            char text[200];
            strcpy(text, dialogue_queue[dialogue_index]);
            if (text_tick < strlen(text)) {
                text[text_tick] = '\0';
                if (frame_long % 2 == 0) text_tick++; 
            } 
            // -33 car accident
            print(text, &print_data);
        }

        // :jumpscare display
        // @todo finish this it's currently broken
        if (state.jumpscare_display) {
            clock[JumpscareClock]++;
            if (clock[JumpscareClock] < 40) {
                if (!IsSoundPlaying(monsterScreamSound)) {
                    PlaySound(monsterScreamSound);
                }
                DrawRectangle(0,0,screen.x, screen.y, BLACK);
                DrawRectangle(0,0,screen.x, screen.y, Fade(RED,(rand() %100)/100.0f));
                // :jumpscare
                for (int i = 0; i < monster_count; ++i) {
                    Monster * monster = &monsters[i];
                    if (monster->x == player.x && monster->y == player.y) {
                        state.normal_control = false;
                        state.normal_display = false;
                        state.jumpscare_display = true;
                        DrawTexturePro(monster_tex_a[monster->type], 
                         /* src    */   (Rectangle) { 0,      0,   16,   16 }, 
                         /* dest   */   (Rectangle) { 612,  512, 1024, 1024 },
                         /* origin   */ (Vector2)   { 512,  512},             
                         /* rotation */ -8 +(rand() % 16),  /* tint */ WHITE);
                        break;
                    }
                }
            } 
            else {
                state.failure_display = true;
                state.jumpscare_display = false;
                state.normal_display = false;
                StopSound(monsterScreamSound);
            }
        }

        // :failure display
        if (state.failure_display) {
            clock[FailureClock]++;
            PrintData print_data;
                print_data.cursor.x =  screen.x/2 - 450;
                print_data.cursor.y = 100;
                print_data.fontsize = 60;
                print_data.height   = 70;
                print_data.color    = WHITE;
            print("You were Eaten", &print_data);
            print_data.fontsize = 46;
            print("Now you'll never solve the mystery.", &print_data);
            if (clock[FailureClock] < 80) {
                print("Type R to Restart", &print_data);
                state.failure_control = true;
            }
        }

        // :debug display
        if (state.debug_display) {
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
            if (state.title_control) print("control title", &print_data); 
            if (state.normal_control) print("control normal", &print_data); 
            if (state.dialogue_control) print("control dialogue", &print_data); 
            if (state.title_display) print("display title", &print_data); 
            if (state.normal_display) print("display normal", &print_data); 
            if (state.dialogue_display) print("display dialogue", &print_data); 
            for (int i = 0; i < objects->count; ++i) {
                Object * object = &objects->items[i];
                if (player.x == object->x && player.y == object->y) {
                    sprintf(text, "i: %d", i);
                    print(text, &print_data);
                }

            }
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

