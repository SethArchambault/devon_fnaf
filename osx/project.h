#include <stdio.h>
#include <string.h>





// if you put this above 75, the water stops showing
// Monster is 12 bytes
// 12 = 900 bytes
// ... maybe it's overflowing the stack?
#define Monster_Max  100000

// :monster type
typedef enum {
    BENNY, BUDDY, ALICE, ALEX, BOB, DENNIS, DONNY, MonsterTypeEnd
} MonsterType;

// :monster
typedef struct {
    int x;
    int y;
    int type; // 0-9
} Monster;

// :action
typedef enum {
    STAND, WALK, DEAD
} Action;

// :direction
typedef enum {
    UP, RIGHT, DOWN, LEFT
} Direction;

// :player
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
    int moving_x;
    int moving_y;
    Direction direction;
    Action action;
} Player;

// :trigger
typedef enum {
    Accident, EnterFire, LeaveFire, EnterCar, ViewsDarkness, SeeDamange, SeeGenerator, BadDoor, SeeFireplace, SeeSecret, DoorEntered, ReadsPaper, ReadsPaper2, TriggerEnd
} Trigger;

#define Dialogue_Max 20
#define Line_Max 200

// :state
typedef struct {
    int normal_control;
    int normal_display;
    int title_control;
    int title_display;
    int dialogue_control;
    int dialogue_display;
    int debug_control;
    int debug_display;
    int build_control;
    int intro_control;
    int monsters_control;
    int jumpscare_active;
    int clock_control;
    int clock_display;
    int failure_control;
    int failure_display;
    Monster monsters[Monster_Max];
    int monster_count;
    int monsters_loaded;
    Texture2D monsterTextures[MonsterTypeEnd];
    Player player;
    int triggers[20];
    int dialogue_index;
    char dialogue_queue[Dialogue_Max][Line_Max];
} State;

void print_state(State * state) {
    printf("failure_control %d\n", state->failure_control);
}

// :state type
typedef enum {
    NormalState, DialogueState
} StateType;

void AddDialogue(char (* dialogue_queue)[20][Line_Max], const char * text) {

    int i = 0;
    for (; (*dialogue_queue)[i][0] != '\0'; i++);
    strcpy((*dialogue_queue)[i], text);
    i++;
    (*dialogue_queue)[i][0] = '\0';
}
void changeState(State * state, StateType new_state) {
    if (new_state == NormalState) {
        state->dialogue_control     = 0;
        state->normal_control       = 1;
        state->normal_display       = 1;
        state->dialogue_display     = 0;
    }
    if (new_state == DialogueState) {
        state->intro_control        = 0;
        state->normal_control       = 0;
        state->dialogue_control     = 1;
        state->dialogue_display     = 1;
    }
}
