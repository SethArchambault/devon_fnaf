
typedef void* Texture2D;

#include "project.h" 
void MonsterAdd(Monster *monsters, int x, int y, int type, int * monster_count) {
        monsters[*monster_count].x = x;
        monsters[*monster_count].y = y;
        monsters[*monster_count].type = type; // 0-9
        printf("test monster x %d y %d\n", x, y);
        ++*monster_count;
}


void live(State * state){

    if (!state->monsters_loaded) {
        state->monsters_loaded = 1;

        Monster * monsters = &state->monsters[0];

        int * monster_count = &state->monster_count;
        *monster_count = 0;
        //BENNY, BUDDY, ALICE, ALEX, BOB, DENNIS, DONNY 
        //MonsterAdd(monsters, -16, -40, BENNY,  monster_count);
        //MonsterAdd(monsters, -10, -46, BENNY,  monster_count);
        MonsterAdd(monsters, -9, -18, BENNY,  monster_count);
        MonsterAdd(monsters, -2, -14, BUDDY,  monster_count);
        MonsterAdd(monsters, -7, -18, ALICE,  monster_count);
        MonsterAdd(monsters,-11, -18, ALEX,   monster_count);
        MonsterAdd(monsters,-27, -13, BOB,    monster_count);
        MonsterAdd(monsters,-41, -4, DENNIS, monster_count);
        MonsterAdd(monsters, -6, -15, DONNY,  monster_count);
    }


}

