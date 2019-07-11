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

int main() {
    char *buffer = malloc(1000);
    buffer[0] = '\0';
    sprintf(findZero(buffer),
        "bits 64\n"
        "section .rodata\n\n"
    );
    // this is kind of a weird coupling.. probably would make more sense to do
    // "floor", "Water" well.. maybe not. it's kind of a SOA arrangement..
    const char *types[] = {"floor", "person","object","monster","\0"};
    const char *entity_names[] = { 
        // Floor
        "Water", "Grass", "Tile", "Wood", "Concrete", "BrokenTile", "Asphalt", "AsphaltLines", "Sidewalk", "Dirt", "\0",
        // Person
        "Walk", "Stand", "\0",
         // Object
        "Newspaper", "Table", "Chair", "Door", "Sink", "Stove", "Counter", "Generator", "Switch", "Fire", "MiniVan", "Important", "Wires", "Pliers", "MusicBox", "Hammer", "Crowbar", "Rope", "Pickaxe", "Fuse", "Shovel", "Fireaxe", "\0",
        // Monster
        "BENNY", "BENNY_ANGRY", "BUDDY", "ALICE", "ALEX", "BOB", "DENNIS", "DONNY", "\0",
    };
    {

        int image_i = 0;
        for (int type_i = 0;types[type_i][0] != '\0'; ++type_i) {
            for (;entity_names[image_i][0] != '\0'; ++image_i) {
                sprintf(findZero(buffer),
                    "global _%s_memory\n"
                    "global _%s_size\n\n",
                    entity_names[image_i],
                    entity_names[image_i]
                );
            } // image
            ++image_i;
        } // type
    }
    {
        int image_i = 0;
        for (int type_i = 0;types[type_i][0] != '\0'; ++type_i) {
            for (;entity_names[image_i][0] != '\0'; ++image_i) {
                sprintf(findZero(buffer),
                    "_%s_memory:   incbin \"../assets/%s_%s.png\"\n"
                    "_%s_size:     dd $-_%s_memory\n\n",
                    entity_names[image_i], types[type_i], entity_names[image_i],
                    entity_names[image_i], entity_names[image_i]
                );
            }
            ++image_i;
        }
    }

    FILE *f = fopen("image_binary_data.asm", "w");
    fwrite(buffer, len(buffer), 1,  f);
    fclose(f);

    buffer[0] = '\0';
    {
        int image_i = 0;
        for (int type_i = 0;types[type_i][0] != '\0'; ++type_i) {
            for (;entity_names[image_i][0] != '\0'; ++image_i) {
                sprintf(findZero(buffer),
                    "extern const unsigned char %s_memory;\n"
                    "extern const int %s_size;\n\n",
                    entity_names[image_i],
                    entity_names[image_i]
                );
            }
            ++image_i;
        }

    }


    // This generates texture loading code
    // I think this is somewhat in the wrong place..
    // or maybe not.. perhaps we just have a flag for file type "Image", "Sound"
    {
        sprintf(findZero(buffer),
                    "#define image_binary_vars_loadAllTextures() {\\\n");
        int image_i = 0;
        for (int type_i = 0;types[type_i][0] != '\0'; ++type_i) {
            for (;entity_names[image_i][0] != '\0'; ++image_i) {
                sprintf(findZero(buffer),
                    "%sTextures[%s] = loadTextureFromMemory(&%s_memory, %s_size);\\\n",
                    types[type_i], entity_names[image_i], 
                    entity_names[image_i], 
                    entity_names[image_i]
                );
            }
            ++image_i;
        }
        sprintf(findZero(buffer),
                    "}\n"
                    );


        FILE *f = fopen("../image_binary_vars.h", "w");
        fwrite(buffer, len(buffer), 1,  f);
        fclose(f);
    }
}
