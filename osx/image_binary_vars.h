extern const unsigned char Water_memory;
extern const int Water_size;

extern const unsigned char Grass_memory;
extern const int Grass_size;

extern const unsigned char Tile_memory;
extern const int Tile_size;

extern const unsigned char Wood_memory;
extern const int Wood_size;

extern const unsigned char Concrete_memory;
extern const int Concrete_size;

extern const unsigned char BrokenTile_memory;
extern const int BrokenTile_size;

extern const unsigned char Asphalt_memory;
extern const int Asphalt_size;

extern const unsigned char AsphaltLines_memory;
extern const int AsphaltLines_size;

extern const unsigned char Sidewalk_memory;
extern const int Sidewalk_size;

extern const unsigned char Dirt_memory;
extern const int Dirt_size;

extern const unsigned char Walk_memory;
extern const int Walk_size;

extern const unsigned char Stand_memory;
extern const int Stand_size;

extern const unsigned char Newspaper_memory;
extern const int Newspaper_size;

extern const unsigned char Table_memory;
extern const int Table_size;

extern const unsigned char Chair_memory;
extern const int Chair_size;

extern const unsigned char Door_memory;
extern const int Door_size;

extern const unsigned char Sink_memory;
extern const int Sink_size;

extern const unsigned char Stove_memory;
extern const int Stove_size;

extern const unsigned char Counter_memory;
extern const int Counter_size;

extern const unsigned char Generator_memory;
extern const int Generator_size;

extern const unsigned char Switch_memory;
extern const int Switch_size;

extern const unsigned char Fire_memory;
extern const int Fire_size;

extern const unsigned char MiniVan_memory;
extern const int MiniVan_size;

extern const unsigned char Important_memory;
extern const int Important_size;

extern const unsigned char Wires_memory;
extern const int Wires_size;

extern const unsigned char Pliers_memory;
extern const int Pliers_size;

extern const unsigned char MusicBox_memory;
extern const int MusicBox_size;

extern const unsigned char Hammer_memory;
extern const int Hammer_size;

extern const unsigned char Crowbar_memory;
extern const int Crowbar_size;

extern const unsigned char Rope_memory;
extern const int Rope_size;

extern const unsigned char Pickaxe_memory;
extern const int Pickaxe_size;

extern const unsigned char Fuse_memory;
extern const int Fuse_size;

extern const unsigned char Shovel_memory;
extern const int Shovel_size;

extern const unsigned char Fireaxe_memory;
extern const int Fireaxe_size;

extern const unsigned char BENNY_memory;
extern const int BENNY_size;

extern const unsigned char BENNY_ANGRY_memory;
extern const int BENNY_ANGRY_size;

extern const unsigned char BUDDY_memory;
extern const int BUDDY_size;

extern const unsigned char ALICE_memory;
extern const int ALICE_size;

extern const unsigned char ALEX_memory;
extern const int ALEX_size;

extern const unsigned char BOB_memory;
extern const int BOB_size;

extern const unsigned char DENNIS_memory;
extern const int DENNIS_size;

extern const unsigned char DONNY_memory;
extern const int DONNY_size;

#define image_binary_vars_loadAllTextures() {\
floorTextures[Water] = loadTextureFromMemory(&Water_memory, Water_size);\
floorTextures[Grass] = loadTextureFromMemory(&Grass_memory, Grass_size);\
floorTextures[Tile] = loadTextureFromMemory(&Tile_memory, Tile_size);\
floorTextures[Wood] = loadTextureFromMemory(&Wood_memory, Wood_size);\
floorTextures[Concrete] = loadTextureFromMemory(&Concrete_memory, Concrete_size);\
floorTextures[BrokenTile] = loadTextureFromMemory(&BrokenTile_memory, BrokenTile_size);\
floorTextures[Asphalt] = loadTextureFromMemory(&Asphalt_memory, Asphalt_size);\
floorTextures[AsphaltLines] = loadTextureFromMemory(&AsphaltLines_memory, AsphaltLines_size);\
floorTextures[Sidewalk] = loadTextureFromMemory(&Sidewalk_memory, Sidewalk_size);\
floorTextures[Dirt] = loadTextureFromMemory(&Dirt_memory, Dirt_size);\
personTextures[Walk] = loadTextureFromMemory(&Walk_memory, Walk_size);\
personTextures[Stand] = loadTextureFromMemory(&Stand_memory, Stand_size);\
objectTextures[Newspaper] = loadTextureFromMemory(&Newspaper_memory, Newspaper_size);\
objectTextures[Table] = loadTextureFromMemory(&Table_memory, Table_size);\
objectTextures[Chair] = loadTextureFromMemory(&Chair_memory, Chair_size);\
objectTextures[Door] = loadTextureFromMemory(&Door_memory, Door_size);\
objectTextures[Sink] = loadTextureFromMemory(&Sink_memory, Sink_size);\
objectTextures[Stove] = loadTextureFromMemory(&Stove_memory, Stove_size);\
objectTextures[Counter] = loadTextureFromMemory(&Counter_memory, Counter_size);\
objectTextures[Generator] = loadTextureFromMemory(&Generator_memory, Generator_size);\
objectTextures[Switch] = loadTextureFromMemory(&Switch_memory, Switch_size);\
objectTextures[Fire] = loadTextureFromMemory(&Fire_memory, Fire_size);\
objectTextures[MiniVan] = loadTextureFromMemory(&MiniVan_memory, MiniVan_size);\
objectTextures[Important] = loadTextureFromMemory(&Important_memory, Important_size);\
objectTextures[Wires] = loadTextureFromMemory(&Wires_memory, Wires_size);\
objectTextures[Pliers] = loadTextureFromMemory(&Pliers_memory, Pliers_size);\
objectTextures[MusicBox] = loadTextureFromMemory(&MusicBox_memory, MusicBox_size);\
objectTextures[Hammer] = loadTextureFromMemory(&Hammer_memory, Hammer_size);\
objectTextures[Crowbar] = loadTextureFromMemory(&Crowbar_memory, Crowbar_size);\
objectTextures[Rope] = loadTextureFromMemory(&Rope_memory, Rope_size);\
objectTextures[Pickaxe] = loadTextureFromMemory(&Pickaxe_memory, Pickaxe_size);\
objectTextures[Fuse] = loadTextureFromMemory(&Fuse_memory, Fuse_size);\
objectTextures[Shovel] = loadTextureFromMemory(&Shovel_memory, Shovel_size);\
objectTextures[Fireaxe] = loadTextureFromMemory(&Fireaxe_memory, Fireaxe_size);\
monsterTextures[BENNY] = loadTextureFromMemory(&BENNY_memory, BENNY_size);\
monsterTextures[BENNY_ANGRY] = loadTextureFromMemory(&BENNY_ANGRY_memory, BENNY_ANGRY_size);\
monsterTextures[BUDDY] = loadTextureFromMemory(&BUDDY_memory, BUDDY_size);\
monsterTextures[ALICE] = loadTextureFromMemory(&ALICE_memory, ALICE_size);\
monsterTextures[ALEX] = loadTextureFromMemory(&ALEX_memory, ALEX_size);\
monsterTextures[BOB] = loadTextureFromMemory(&BOB_memory, BOB_size);\
monsterTextures[DENNIS] = loadTextureFromMemory(&DENNIS_memory, DENNIS_size);\
monsterTextures[DONNY] = loadTextureFromMemory(&DONNY_memory, DONNY_size);\
}
