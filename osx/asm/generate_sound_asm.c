#include<stdio.h>
#include<stdlib.h>

int len(char * str) {
    int i = 0;
    for(;str[i] != '\0'; ++i);
    return i;
}
char * findZero(char * str) {
    return &str[len(str)];
}

#define _sound_max 3

int main() {
    char *buffer = malloc(1000);
    buffer[0] = '\0';
    sprintf(findZero(buffer),
        "bits 64\n"
        "section .rodata\n\n"
    );
    const char *sound_names[_sound_max] = {
        "DoorClose", "DoorOpen", "MonsterScream"
    };
    for (int sound_i = 0;sound_i < _sound_max; ++sound_i) {
        sprintf(findZero(buffer),
            "global _%s_memory\n"
            "global _%s_size\n\n",
            sound_names[sound_i],
            sound_names[sound_i]
        );
    } // sound
    for (int sound_i = 0;sound_i < _sound_max; ++sound_i) {
        sprintf(findZero(buffer),
            "_%s_memory:   incbin \"../assets/sound_%s.ogg\"\n"
            "_%s_size:     dd $-_%s_memory\n\n",
            sound_names[sound_i], sound_names[sound_i],
            sound_names[sound_i], sound_names[sound_i]
        );
    }
    printf("%s", buffer); 
}
