
void MonsterLoad(Monster *monsters, int *monster_count) {

    Monster _monsters[] = {
        {-9, -18, BENNY, UP, 0}, 
        {-2, -14, BUDDY, RIGHT, 0}, 
        {-7, -18, ALICE, LEFT, 0}, 
        {-11, -18, ALEX, DOWN, 0}, 
        {-27, -13, BOB, RIGHT, 0}, 
        {-41, -4, DENNIS, UP, 0}, 
        {-6, -15, DONNY, DOWN, 0}, 
    };
    *monster_count = 7;
    memcpy(monsters, &_monsters, sizeof(Monster) * *monster_count);
}
