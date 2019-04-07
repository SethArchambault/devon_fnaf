/*
 * Robustness
 *   [ ] reload triggers
 *
 */

#include <math.h>
#include <string.h>
#include "/opt/raylib/src/external/stb_image_write.h"
#include <time.h>  // rand
#define Role_Max 6
#define Image_Size 16 
#define Tile_Size 64
#define Floor_Max 10000
#define Object_Max 10000
#define Water_Max 10000
#include "project.h"
#include <dlfcn.h> // external 

// in external
typedef void LIVE(State *);



// :floor type
typedef enum {
    Wood, Tile, Concrete, BrokenTile, Blank, Grass, Asphalt, AsphaltLines, Sidewalk, Dirt, Water, FloorTypeEnd
} FloorType;

// :object
typedef enum {
    Table, Door, Chair, Sink, Stove, Counter, Generator, Switch, Fire, MiniVan, Important, Newspaper, ObjectTypeEnd
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
    int type;
} Floor;

typedef struct {
    Floor items[Floor_Max];
    int count;
} Floors;



// :timer
typedef enum{
    JumpscareClock, WaterClock, FailureClock, LoadClock
} Clock;


Vector2 v2_pixel_from_v2_coord(Vector2 coord) {
    coord.x *= 64;
    coord.y *= 64;
    return coord;
}


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

#define _P(value) { printf("%s %d\n", #value, value);}
#define _SIZE(value) { printf("%s %luB\n", #value, sizeof(value));}

void game() {
    _SIZE(Texture);

    srand(time(NULL));
    Vector2 screen = {1280, 760};
    InitWindow(screen.x, screen.y, "Vibrant");
    SetTargetFPS(59);
#define monster_max 10
    Texture2D person_stand_tex      = LoadTexture("assets/devon_standing.png");
    Texture2D person_walk_tex       = LoadTexture("assets/devon_walking.png"); 
    
    //:objectTexture
    Texture2D objectTextures[ObjectTypeEnd];
    objectTextures[Newspaper]   = LoadTexture("assets/objects/newspaper.png");
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
    // :state
    State state;
        state.normal_control     = 0;
        state.title_control      = 1;
        state.intro_control      = 0;
        state.dialogue_control   = 0;
        state.debug_control      = 0;
        state.build_control      = 0;
        state.monsters_control   = 0;
        state.failure_control    = 0;
        state.normal_display     = 1;
        state.title_display      = 1;
        state.dialogue_display   = 0;
        state.failure_display    = 0;
        state.jumpscare_active   = 0;
        state.debug_display      = 0;
        state.clock_display      = 0;

        state.monster_count      = 0;
        state.monsters_loaded    = 0;
        state.dialogue_index     = 0;

    int *dialogue_index = &state.dialogue_index;
    // @Todo: create a function which adds to the dialogue queue, and check character limit
    char (*dialogue_queue)[20][Line_Max] = &state.dialogue_queue;


    // :monster
    // @Performance: For some reason, loading the textures here, and then reloading later is more performant than just loading later
    Texture2D * monsterTextures = &state.monsterTextures[0];
        monsterTextures[BENNY]      = LoadTexture("assets/monsters/1benny_beaver_bear.png");
        monsterTextures[BUDDY]      = LoadTexture("assets/monsters/2buddy_the_wolf.png");
        monsterTextures[ALICE]      = LoadTexture("assets/monsters/3alice_the_duck.png");
        monsterTextures[ALEX]       = LoadTexture("assets/monsters/4alex_the_honeybadger.png");
        monsterTextures[BOB]        = LoadTexture("assets/monsters/5bob_the_giraffe.png");
        monsterTextures[DENNIS]     = LoadTexture("assets/monsters/6dennis_the_allegator.png");
        monsterTextures[DONNY]      = LoadTexture("assets/monsters/7donny_the_dog.png");


    Monster *monsters = &state.monsters[0];
    int * monster_count = &state.monster_count;
    int clock[10];
    clock[LoadClock]        = 0;
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
    Player * player = &state.player; 
        player->x            = -11;
        player->y            = -48;
        player->x_dest       = player->x;
        player->y_dest       = player->y;
        player->y_pixel      = player->y * Tile_Size;
        player->x_pixel      = player->x * Tile_Size;
        player->distance     = 0;
        player->moving_x     = 0;
        player->moving_y     = 0;
        player->y_pixel_dest = player->y_pixel;
        player->x_pixel_dest = player->x_pixel;
        player->direction    = DOWN;
        player->action = STAND;
    Objects *objects;
        objects         = malloc(sizeof(Objects));
        objects->count  = 0;
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

    


    int * triggers = &state.triggers[0]; 
    for (int i = 0; i < TriggerEnd; ++i) {
        triggers[i] = 0;
    }
    while (!WindowShouldClose()) {

        // :monsters reload
        // :reload
        // once per 2 seconds, reload


        // :live
        clock[LoadClock]++;
        //if (!state.monsters_loaded) {
            if (clock[LoadClock] > 13) {
                printf("loaded monsters\n");
                clock[LoadClock] = 0;
                for (int i = 0; i < MonsterTypeEnd; ++i) {
                    UnloadTexture(monsterTextures[i]);
                }
                monsterTextures[BENNY] = LoadTexture("assets/monsters/1benny_beaver_bear.png");
                monsterTextures[BUDDY] = LoadTexture("assets/monsters/2buddy_the_wolf.png");
                monsterTextures[ALICE] = LoadTexture("assets/monsters/3alice_the_duck.png");
                monsterTextures[ALEX]  = LoadTexture("assets/monsters/4alex_the_honeybadger.png");
                monsterTextures[BOB]   = LoadTexture("assets/monsters/5bob_the_giraffe.png");
                monsterTextures[DENNIS]= LoadTexture("assets/monsters/6dennis_the_allegator.png");

                monsterTextures[DONNY] = LoadTexture("assets/monsters/7donny_the_dog.png");
                printf("loading\n");
                void* PROJECT2 = dlopen("libproject2.dylib", RTLD_LAZY | RTLD_GLOBAL);
                assert(PROJECT2);
                //print_state(&state);
                LIVE * live = (LIVE *) dlsym(PROJECT2, "live");
                const char *dlsym_error = dlerror();
                live(&state);

                dlclose(PROJECT2);
                assert(!dlsym_error);
            }
       // }
        
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
                for (int i = 0; i < Dialogue_Max; ++i) {
                    (*dialogue_queue)[i][0] = '\0';
                }
                AddDialogue(dialogue_queue, "I wish I could do some fishing...");
                AddDialogue(dialogue_queue, "Too bad I have to get to my new job...");
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
                    if (floor->x == player->x && floor->y==player->y) {
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
                    floor->x = player->x;
                    floor->y = player->y;
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
                    if (floor->x == player->x && floor->y==player->y) {
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
                    floor->x = player->x;
                    floor->y = player->y;
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
                    if (object->x == player->x && object->y==player->y) {
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
                    if (object->x == player->x && object->y==player->y) {
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

            // :object create
            if (IsKeyPressed(KEY_O)) {
                // Check if object exists at these coordinates
                int objectExists = 0;
                for (int i = 0; i < objects->count;++i) {
                    Object *object = &objects->items[i];
                    if (object->x == player->x && object->y==player->y) {
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
                    object->x = player->x;
                    object->y = player->y;
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
            // :clear
            if(IsKeyDown(KEY_C)) {
                for (int i = 0; i < objects->count; ++i) {
                    if(objects->items[i].x == player->x &&
                    objects->items[i].y == player->y) {
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
            if (   up()   && !player->moving_y) {
                player->direction = UP;
                player->y_pixel_dest -= Tile_Size;
                player->y_dest   -= 1;
                player->moving_y = 1;
            }
            if ( down()   && !player->moving_y) {
                player->direction = DOWN;
                player->y_pixel_dest += Tile_Size;
                player->y_dest += 1;
                player->moving_y = 1;
            }
            if (right()   && !player->moving_x) {
                player->direction = RIGHT;
                player->x_pixel_dest += Tile_Size;
                player->x_dest   += 1;
                player->moving_x = 1;
            }
            if ( left()   && !player->moving_x) {
                player->direction = LEFT;
                player->x_pixel_dest -= Tile_Size;
                player->x_dest   -= 1;
                player->moving_x = 1;
            }
            // :monsters reload
            if (IsKeyPressed(KEY_R)) {
                state.monsters_loaded = 0;
            }

            // :collision
            int collision = 0;
            if (!noclip) {
                collision = 1;
                for(int i = 0; i < floors->count; ++i) {
                    Floor *floor = &floors->items[i];
                    if (floor->x == player->x_dest && floor->y == player->y_dest) {
                        if (floor->type != Water) {
                            collision = 0;
                        }
                        break;
                    }
                }
                if (player->y_dest < -24 && hide_outdoors) {
                    collision = 1;
                }
                if (collision) {
                    player->x_dest = player->x;
                    player->y_dest = player->y;
                    player->x_pixel_dest = player->x_pixel;
                    player->y_pixel_dest = player->y_pixel;
                }
            }

            // :move player
            if (!collision) {
                if (player->x_pixel < player->x_pixel_dest)       player->x_pixel += speed;
                else if (player->x_pixel > player->x_pixel_dest)  player->x_pixel -= speed;
                else {
                    player->moving_x = 0;
                    player->x = player->x_dest;
                    player->x_pixel = player->x * Tile_Size;
                    player->x_pixel_dest = player->x_pixel;
                }
                if (player->y_pixel < player->y_pixel_dest)       player->y_pixel += speed;
                else if (player->y_pixel > player->y_pixel_dest)  player->y_pixel -= speed;
                else {
                    player->moving_y = 0;
                    player->y = player->y_dest;
                    player->y_pixel = player->y * Tile_Size;
                    player->y_pixel_dest = player->y_pixel;
                }
            }

            // :triggers
#if 1
            
            if (!triggers[ReadsPaper] && player->x == -6 && player->y == -20) {
                triggers[ReadsPaper] = 1;
                AddDialogue(dialogue_queue, "\"Benny's Pizzeria reopens, after the tragedy \nlast month, which left 1 dead...");
                AddDialogue(dialogue_queue, "..and 2 critically decapitated..");
                AddDialogue(dialogue_queue, "In an official statement a Benny's representative said:\n\"It could of been much much worse!\"");
                AddDialogue(dialogue_queue, "Hmm...");
                AddDialogue(dialogue_queue, "I wonder if I should ask for a raise.");
                changeState(&state, DialogueState);
            }

            
            // :safe room
            {
                int x = -12;
                int y = 18;
                if (!triggers[ReadsPaper2] && player->x == x && player->y == y) {
                    triggers[ReadsPaper2] = 1;
                    AddDialogue(dialogue_queue, "\"In exclusive tell-all, former security guard for Benny's\ndescribes the job.");
                    AddDialogue(dialogue_queue, "You basically wait in a room all night, until they come\nfor you.\"");
                    AddDialogue(dialogue_queue, "When asked \"Who comes for you?\", the security guard\nlet out a groan, and died on the spot.");
                    changeState(&state, DialogueState);
                    // create monster.. x:-12 y:3
                    /* 
                    if (*monster_count < Monster_Max) {
                        monsters[*monster_count].x = player->x - 10;
                        monsters[*monster_count].y = player->y;
                        monsters[*monster_count].type = 0; // 0-9
                        ++*monster_count;
                    }
                    */
                    //state.monsters_control = 1;
                    state.clock_display      = 1;
                    // show clock
                }
            }

            if (!triggers[ViewsDarkness] && player->y == -41 && player->x == -5) {
                triggers[ViewsDarkness] = 1;
                AddDialogue(dialogue_queue, "Sometimes I wonder if there is a world beyond the darkness..");
                AddDialogue(dialogue_queue, "But try as I might, I can't bring myself to step foot into it.");
                AddDialogue(dialogue_queue, "As if there is an invisible force compelling me...");
                changeState(&state, DialogueState);
            }

            if (!triggers[Accident] && player->x == -9 && player->y == -32) {
                triggers[Accident] = 1;
                AddDialogue(dialogue_queue, "Oh no! What a horrible accident!");
                AddDialogue(dialogue_queue, "I hope no ones hurt.");
                AddDialogue(dialogue_queue, "Though there doesn't seem to be much chance of that...");
                
                changeState(&state, DialogueState);
            }

            if (!triggers[DoorEntered] && player->y == -23) {
                if (!IsSoundPlaying(doorCloseSound)) {
                    PlaySound(doorCloseSound);
                }
                triggers[DoorEntered] = 1;
                // close door
                objects->items[14].actionFrame = 0;
                hide_outdoors = 1;

                AddDialogue(dialogue_queue, "Dang! The wind slammed the door on me!");
                AddDialogue(dialogue_queue, "And it won't budge! Guess I'm here till the next shift comes for me..");
                
                changeState(&state, DialogueState);
            }

            int player_stands_on_fire = 0;
            for (int i = 0; i < objects->count; ++i) {
                Object * object = &objects->items[i];
                if (object->type == Fire && object->x == player->x && object->y == player->y) {
                    player_stands_on_fire = 1;
                }
            }

            if (!triggers[EnterFire] && player_stands_on_fire) {
                triggers[EnterFire] = 1;
                AddDialogue(dialogue_queue, "Ow it burns!");
                AddDialogue(dialogue_queue, "My whole body is on fire!!");
                AddDialogue(dialogue_queue, "...I hope this won't make me late for work.");
                
                changeState(&state, DialogueState);
            }
            if (triggers[EnterFire]  && !triggers[LeaveFire] && !player_stands_on_fire) {
                triggers[LeaveFire] = 1;
                AddDialogue(dialogue_queue, "Phew!");
                AddDialogue(dialogue_queue, "Good thing I wore my fireproof clothes!");
                AddDialogue(dialogue_queue, "...and fireproof facecream.");
                
                changeState(&state, DialogueState);
            }

            int player_stands_car = 0;
            for (int i = 0; i < objects->count; ++i) {
                Object * object = &objects->items[i];
                if (object->type == MiniVan && object->x == player->x && object->y == player->y) {
                    player_stands_car = 1;
                }
            }

            if (!triggers[EnterCar] && player_stands_car) {
                triggers[EnterCar] = 1;
                AddDialogue(dialogue_queue, "The passengers are so tiny.");
                AddDialogue(dialogue_queue, "I'm sure they'll be fine though.");
                AddDialogue(dialogue_queue, "...once a tiny ambulance arrives.");
                
                changeState(&state, DialogueState);
            }


            if (!triggers[SeeDamange] && player->x == -5  && player->y == -15) {
                triggers[SeeDamange] = 1;
                AddDialogue(dialogue_queue, "Man this place is a dump..");
                AddDialogue(dialogue_queue, "How could they let it get this way?");
                AddDialogue(dialogue_queue, "...Unless this just happened?");
                
                changeState(&state, DialogueState);
            }

            if (!triggers[SeeGenerator] && player->x == -2  && player->y == 3) {
                triggers[SeeGenerator] = 1;
                AddDialogue(dialogue_queue, "What an odd place for a generator..");
                AddDialogue(dialogue_queue, "Who designed this place?");
                
                changeState(&state, DialogueState);
            }
            
            if (!triggers[BadDoor] && player->x == 12 && player->y == -8) {
                triggers[BadDoor] = 1;
                AddDialogue(dialogue_queue, "I can pass right through doors!");
                AddDialogue(dialogue_queue, "No wonder this place is so drafty.");
                
                changeState(&state, DialogueState);
            }

            if (!triggers[SeeFireplace] && player->x == -28  && player->y == -7) {
                triggers[SeeFireplace] = 1;
                AddDialogue(dialogue_queue, "Someone left the fireplace going!");
                AddDialogue(dialogue_queue, "They must of left in a hurry..");
                
                changeState(&state, DialogueState);
            }

            if (!triggers[SeeSecret] && player->x == -2  && player->y == 20) {
                triggers[SeeSecret] = 1;
                AddDialogue(dialogue_queue, "There's a place down there to the right, but I can't get to it..");
                AddDialogue(dialogue_queue, "Wait a mi--");
                AddDialogue(dialogue_queue, "..I can see through walls!");
                
                changeState(&state, DialogueState);
            }
#endif

        } // normal control

        // :dialogue control
        if (state.dialogue_control) {
            if (IsKeyPressed(KEY_ENTER)) {
                (*dialogue_index)++;
                text_tick = 0;
                if ((*dialogue_queue)[*dialogue_index][0] == '\0') {
                    *dialogue_index = 0;
                    (*dialogue_queue)[0][0] = '\0';
                    changeState(&state, NormalState);
                } 
            }
        }
        // :failure
        if (state.failure_control) {
            if (IsKeyPressed(KEY_R)) {
                printf("game end\n");
                exit(1);
            }
        }
        // :title control
        if (state.title_control) {
            // :build selection
            if (IsKeyPressed(KEY_B)) {
                camera_offset_y = 0;
                state.build_control      = 1;
                state.title_control      = 0;
                state.normal_control     = 1;

                state.title_display      = 0;
                state.jumpscare_active   = 0;
                state.normal_display     = 1;
            }
            if (IsKeyPressed(KEY_ENTER)) {
                state.title_control      = 0;
                state.intro_control      = 1;

                state.title_display      = 0;
                state.normal_display     = 1;
            }
        }

        // :monsters control
        if (state.monsters_control) {
            // :move monsters
            ++enemy_timer;
            int enemy_speed = 40;
            if (enemy_timer > enemy_speed && !noclip) {
                for (int i = 0; i < *monster_count; ++i) {
                    Monster * monster = &monsters[i];
                    memcpy(&temp_monster,monster, sizeof(Monster));
                    enemy_timer = 0;
                    if (monster->x > player->x) {
                        monster->x--;
                    }
                    if (monster->x < player->x) {
                        monster->x++;
                    }
                    if (monster->y < player->y) {
                        monster->y++;
                    }
                    if (monster->y > player->y) {
                        monster->y--;
                    }

                    for (int n = 0; n < *monster_count; ++n) {
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
            camera.offset.x     = screen.x/2 - 32 + -(player->x_pixel * camera.zoom);
            camera.offset.y     = camera_offset_y + screen.y/2 - 32 + -(player->y_pixel * camera.zoom);

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

            if (player->action != DEAD && frame_passed > 10) {
                if (player->moving_y || player->moving_x) { 
                    player->action = WALK;
                } else { 
                    player->action = STAND;
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

                // :monster draw
                for (int i = 0; i < *monster_count; ++i) {
                    Monster * monster = &monsters[i];
                    DrawTextureEx(monsterTextures[monster->type], (Vector2) { monster->x * 64, monster->y * Tile_Size - Tile_Size / 4  }, 0, 4, WHITE);
                } 

                // :draw player
                Rectangle player_rect; 
                player_rect.height = Tile_Size;
                player_rect.width = Tile_Size;
                player_rect.x = player->x_pixel;
                player_rect.y = player->y_pixel - Tile_Size / 4;
                if (player->action == STAND) {
                    DrawTexturePro(person_stand_tex, (Rectangle) { 32 * frame, 0, 32, 32 }, player_rect, (Vector2){ 0, 0 }, 0, WHITE);
                } 
                else if (player->action == DEAD) {
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
            print("Three Nights at Benny's Pizzeria", &print_data);
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
            strcpy(text, (*dialogue_queue)[*dialogue_index]);
            if (text_tick < strlen(text)) {
                text[text_tick] = '\0';
                if (frame_long % 2 == 0) text_tick++; 
            } 
            // -33 car accident
            print(text, &print_data);
        }

        // :jumpscare display
        if (state.jumpscare_active) {
            int should_jump_scare = 0;
            Monster * monster = NULL;
            for (int i = 0; i < *monster_count; ++i) {
                monster = &monsters[i];
                if (monster->x == player->x && monster->y == player->y) {
                    should_jump_scare = 1;
                    break;
                }
            }
            if (should_jump_scare) {
                state.clock_display = 0;
                state.normal_control = 0;
                state.normal_display = 0;
                clock[JumpscareClock]++;
                if (clock[JumpscareClock] < 40) {
                    if (!IsSoundPlaying(monsterScreamSound)) {
                        PlaySound(monsterScreamSound);
                    }
                    DrawRectangle(0,0,screen.x, screen.y, BLACK);
                    //DrawRectangle(0,0,screen.x, screen.y, Fade(RED,(rand() %100)/100.0f));
                    DrawRectangle(0,0,screen.x, screen.y, RED);

                    DrawTexturePro(monsterTextures[monster->type], (Rectangle) { 0, 0, 16,   16 }, (Rectangle) { 612,  512, 1024, 1024 }, (Vector2) { 512,  512}, -8 +(16),  WHITE);
                    // DrawTexturePro(monsterTextures[monster->type], (Rectangle) { 0, 0, 16,   16 }, (Rectangle) { 612,  512, 1024, 1024 }, (Vector2) { 512,  512}, -8 +(rand() % 16),  WHITE);
                    // :jumpscare
                } 
                else {
                    state.failure_display = 1;
                    state.normal_display = 0;
                    StopSound(monsterScreamSound);
                }
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
            /*
            if (clock[FailureClock] < 80) {
                print("Type R to Restart", &print_data);
                state.failure_control = 1;
            }
            */
        }

        // :clock display
        if (state.clock_display) {
        
            int clock_width = 100;
            PrintData print_data;
                print_data.cursor.x =  screen.x/2 - clock_width;
                print_data.cursor.y = 20;
                print_data.fontsize = 90;
                print_data.height   = 25;
                print_data.color    = RED;
            char text[40];
            static int total_seconds =  0;
            int total_minutes = (int) total_seconds / 60;
            int total_hours = (int) total_minutes / 60;
            int military_hours = (total_hours % 24) ;
            int hours =  ((military_hours + 11) % 12) + 1;
            int minutes = total_minutes % 60;
            total_seconds += 7;

            char am_pm[4] = "AM";
            if (military_hours >= 12) {
                am_pm[0] = 'P';
            } 
            if (military_hours >= 3) {
                state.monsters_control = 1;
                state.jumpscare_active   = 1;
            }
            sprintf(text, "%2d:%02d", hours, minutes);
            print(text, &print_data);
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
            sprintf(text, "X: %d Y: %d", player->x, player->y);
            print(text, &print_data);
            if (state.title_control)        print("control title",      &print_data); 
            if (state.normal_control)       print("control normal",     &print_data); 
            if (state.dialogue_control)     print("control dialogue",   &print_data); 
            if (state.title_display)        print("display title",      &print_data); 
            if (state.normal_display)       print("display normal",     &print_data); 
            if (state.dialogue_display)     print("display dialogue",   &print_data); 
            for (int i = 0; i < objects->count; ++i) {
                Object * object = &objects->items[i];
                if (player->x == object->x && player->y == object->y) {
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

		// :darkness
            //DrawRectangle(0, 0, screen.x, screen.y, Fade(BLACK, 0.7));

        EndDrawing();
    }//while
}// main

