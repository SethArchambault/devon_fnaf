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
    char * buffer = malloc(1000);
    const char *sound_names[_sound_max] = {
        "DoorClose", "DoorOpen", "MonsterScream"
    };

    for (int image_i = 0;image_i < _sound_max; ++image_i) {
        sprintf(findZero(buffer),
            "extern const unsigned char %s_memory;\n"
            "extern const int %s_size;\n\n",
            sound_names[image_i],
            sound_names[image_i]
        );
    }
    printf("%s", buffer);
}
