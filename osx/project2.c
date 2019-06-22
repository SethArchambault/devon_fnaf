
typedef void* Texture2D;
#include <assert.h>
#include <stdlib.h>
#include "project.h" 
void MonsterAdd(Monster *monsters, int x, int y, int type, Direction direction, int * monster_count) {
        monsters[*monster_count].x = x;
        monsters[*monster_count].y = y;
        monsters[*monster_count].type = type; // 0-9
        monsters[*monster_count].direction = direction; // 0-3
        ++*monster_count;
}

void DirectionAdd(MonsterDirection *monster_directions, int x, int y, Direction direction, int * monster_direction_count) {
        monster_directions[*monster_direction_count].x = x;
        monster_directions[*monster_direction_count].y = y;
        monster_directions[*monster_direction_count].direction = direction; // 0-3
        ++*monster_direction_count;
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
        MonsterAdd(monsters, -9, -18, BENNY, UP,  monster_count);
        MonsterAdd(monsters, -2, -14, BUDDY,  RIGHT, monster_count);
        MonsterAdd(monsters, -7, -18, ALICE,  LEFT, monster_count);
        MonsterAdd(monsters,-11, -18, ALEX,   DOWN, monster_count);
        MonsterAdd(monsters,-27, -13, BOB,    RIGHT, monster_count);
        MonsterAdd(monsters,-41, -4, DENNIS, UP, monster_count);
        MonsterAdd(monsters, -6, -15, DONNY,  DOWN, monster_count);


            
    }
    {
        int * monster_direction_count = &state->monster_direction_count;
        *monster_direction_count = 0;

        MonsterDirection * monster_directions = &state->monster_directions[0];

        char direction_string[1000];// = "-9 -21 L\n-15 -21 U\n-15 -22 R\n-6 -22 D\n";

        char * direction_pointer = &direction_string[0];

        {
            FILE * f = fopen("assets/monster_directions.txt", "r");
            assert(f);
            fseek(f, 0, SEEK_END);
            long int length = ftell(f);
            assert(length < 1000);
            fseek(f, 0, SEEK_SET);
            fread(direction_pointer, length, 1, f);
            fclose(f);
        }
        printf("direction_pointer %s", direction_pointer);



        int total_newlines = 0;
        for (int i = 0; i < strlen(direction_string); ++i) {
            if (direction_pointer[i] == '\n') {
                total_newlines++;
            }
        }
        printf("total_newlines %d\n", total_newlines);

        for (int i = 0; i < total_newlines; ++i) {
            int x = atoi(direction_pointer);
            char * y_pointer = strchr(direction_pointer, ' ');
            y_pointer++; // to skip this space
            int y = atoi(y_pointer);
            char * dir_pointer = strchr(y_pointer, ' ');
            dir_pointer++; // skip space
            char dir = dir_pointer[0];
            Direction direction;
            if (dir == 'R')
                direction = RIGHT;
            if (dir == 'L')
                direction = LEFT;
            if (dir == 'U')
                direction = UP;
            if (dir == 'D')
                direction = DOWN;
            DirectionAdd(monster_directions, x, y, direction, monster_direction_count);
            direction_pointer = strchr(dir_pointer, '\n');
            direction_pointer++;
        }
    }


}

