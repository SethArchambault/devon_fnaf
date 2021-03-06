#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>  // rand

#include "stb_image.h"
#include "stb_vorbis.h"
#include "image_binary_vars.h"
#include "sound_binary_vars.h"

#define Role_Max 6
#define Image_Size 16 
#define Tile_Size 64
#define Floor_Max 10000
#define Object_Max 10000
#define Water_Max 10000
// if you put this above 75, the water stops showing
// Monster is 12 bytes
// 12 = 900 bytes
// ... maybe it's overflowing the stack?
#define Monster_Max  100000

#define CREATE_ENUM(name) name,
#define CREATE_STRINGS(name) #name,

// :direction
#define DIRECTION(func) \
	func(UP)        \
	func(DOWN)       \
	func(RIGHT)       \
	func(LEFT)       

typedef enum {
	DIRECTION(CREATE_ENUM)
} Direction;

char DirectionString[6][10] = {
	DIRECTION(CREATE_STRINGS)
};

// :shaders


typedef enum {
    FX_GRAYSCALE = 0,
    FX_POSTERIZATION,
    FX_DREAM_VISION,
    FX_PIXELIZER,
    FX_CROSS_HATCHING,
    FX_CROSS_STITCHING,
    FX_PREDATOR_VIEW,
    FX_SCANLINES,
    FX_FISHEYE,
    FX_SOBEL,
    FX_BLOOM,
    FX_BLUR,
    FX_DISTORTION
    //FX_FXAA
} PostproShader;

// :monster
typedef struct {
	int x;
	int y;
	int type; // 0-9
	Direction direction;
	int mode; // 0-3
} Monster;
// :monster type

#define MONSTER(f) \
	f(BENNY) f(BENNY_ANGRY) f(BUDDY) f(ALICE) f(ALEX) f(BOB) f(DENNIS) f(DONNY) f(MonsterTypeEnd)

typedef enum {
	MONSTER(CREATE_ENUM)
} MonsterType;

char MonsterTypeString[9][20] = {
	MONSTER(CREATE_STRINGS)
};

// :object
#define OBJECT(f) \
	f(Table) f(Door) f(Chair) f(Sink) f(Stove) f(Counter) f(Generator) f(Switch) f(Fire) f(MiniVan) f(Important) f(Newspaper) f(Wires) f(Pliers) f(MusicBox) f(Hammer) f(Crowbar) f(Rope) f(Pickaxe) f(Fuse) f(Shovel) f(Fireaxe) f(ObjectTypeEnd)

typedef enum {
	OBJECT(CREATE_ENUM)
} ObjectType;

char ObjectTypeString[23][20] = {
	OBJECT(CREATE_STRINGS)
};


typedef enum {
	Walk, Stand, PersonTypeEnd
} PersonType;

typedef struct {
	int x;
	int y;
	ObjectType type;
	int rotation;
	int actionFrame;
	int collectable;
} Object;

#include "objects.h"

typedef struct {
	Object items[Object_Max];
	int count;
} Objects;

#include "monsters.h"

typedef struct {
	int x;
	int y;
	int type;
} Floor;

typedef struct {
	Floor items[Floor_Max];
	int count;
} Floors;

// :floor type
#define FLOOR(f) \
	f(Wood) f(Tile) f(Concrete) f(BrokenTile) f(Blank) f(Grass) f(Asphalt) f(AsphaltLines) f(Sidewalk) f(Dirt) f(Water) f(FloorTypeEnd)

typedef enum {
	FLOOR(CREATE_ENUM)
} FloorType;

char FloorTypeString[13][20] =  {
	FLOOR(CREATE_STRINGS)
};

#include "floors.h"

int len(char * str){
	int i = 0;
	for (;str[i] != '\0';++i);
	return i;
}


void MonsterSave(Monster *monsters, int *monster_count) {
	void * buffer;
	char str[1000] = {0};

	strcat(str, 
			"void MonsterLoad(Monster *monsters, int *monster_count) {\n"
			"    Monster _monsters[] = {\n");
	for (int i = 0; i < *monster_count; ++i) {
		Monster * m = &monsters[i];
		sprintf(str + len(str), 
				"        {%d, %d, %s, %s, %d},\n",
				m->x, m->y, MonsterTypeString[m->type], DirectionString[m->direction], m->mode
			   );
	}
	sprintf(str + len(str), "    };\n"
			"    *monster_count = sizeof(_monsters) / sizeof(Monster);\n"
			"    memcpy(monsters, &_monsters, sizeof(Monster) * *monster_count);\n"
			"}\n");

	FILE *f = fopen("monsters.h", "w");
	fwrite(str, len(str), 1, f);
	fclose(f);
}

void ObjectSave(Object *objects, int *object_count) {
	void * buffer;
	char str[3000] = {0};

	strcat(str, 
			"void ObjectLoad(Object *objects, int *object_count) {\n"
			"    Object _objects[] = {\n");
	for (int i = 0; i < *object_count; ++i) {
		Object * s = &objects[i];
		sprintf(str + len(str), 
				"        {%d, %d, %s, %d, %d, %d},\n",
				s->x, s->y, ObjectTypeString[s->type], s->rotation, s->actionFrame, s->collectable
			   );
	}
	sprintf(str + len(str), "    };\n"
			"    *object_count = sizeof(_objects) / sizeof(Object);\n"
			"    memcpy(objects, &_objects, sizeof(Object) * *object_count);\n"
			"}\n");

	FILE *f = fopen("objects.h", "w");
	fwrite(str, len(str), 1, f);
	fclose(f);
}

void FloorSave(Floor *floors, int *floor_count) {
	void * buffer;
	// This could be a major problem later on.. we should use temporary memory
	// :unsafe
	char str[100000];
	str[0] = '\0';

	strcat(str, 
			"void FloorLoad(Floor *floors, int *floor_count) {\n"
			"    Floor _floors[] = {\n");
	for (int i = 0; i < *floor_count; ++i) {
		Floor * f = &floors[i];
		sprintf(str + len(str), 
				"        {%d, %d, %s},\n",
				f->x, f->y, FloorTypeString[f->type]
			   );
	}
	sprintf(str + len(str), "    };\n"
			"    *floor_count = sizeof(_floors) / sizeof(Floor);\n"
			"    memcpy(floors, &_floors, sizeof(Floor) * *floor_count);\n"
			"}\n");

	FILE *f = fopen("floors.h", "w");
	fwrite(str, len(str), 1, f);
	fclose(f);
}

// :action
typedef enum {
	STAND, WALK, DEAD
} Action;

// :monster direction
typedef struct{
	int x;
	int y;
	Direction direction;
} MonsterDirection;

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
	int todo_display;
	int build_control;
	int intro_control;
	int monsters_control;
	int jumpscare_active;
	int clock_control;
	int clock_display;
	int failure_control;
	int failure_display;
	int success_control;
	int success_display;
	int flashlight_display;
	int select_floor_display;
	int select_floor_control;
	int select_floor_ndx;
	int select_object_display;
	int select_object_control;
	int select_object_ndx;
	Monster monsters[Monster_Max];
	MonsterDirection monster_directions[Monster_Max];
	int monster_direction_count;
	int monster_count;
	int monsters_loaded;
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

void transport_player(int x, int y, Player *player) {
	player->x = x;
	player->x_dest       = player->x;
	player->x_pixel = player->x * Tile_Size;
	player->x_pixel_dest = player->x_pixel;
	player->y = y;
	player->y_dest       = player->y;
	player->y_pixel = player->y * Tile_Size;
	player->y_pixel_dest = player->y_pixel;
}





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

Texture2D loadTextureFromMemory(const unsigned char * buffer, int len){
	Image image = { 0 };

	int bpp = 0;
	image.data = stbi_load_from_memory(buffer, len, &image.width, &image.height, &bpp, 0);
	assert(bpp == 4);
	image.mipmaps = 1;
	image.format = UNCOMPRESSED_R8G8B8A8;

	return  LoadTextureFromImage(image);
}
    Sound loadSoundFromMemory(const unsigned char * sound_memory, int sound_size) {
		int error;
		stb_vorbis *oggFile =  stb_vorbis_open_memory(sound_memory, sound_size, &error, NULL);

		stb_vorbis_info info = stb_vorbis_get_info(oggFile);
        Wave wave;
		wave.sampleRate = info.sample_rate;
		wave.sampleSize = 16;    
		wave.channels = info.channels;
		wave.sampleCount = (unsigned int) stb_vorbis_stream_length_in_samples(oggFile)*info.channels; 

		float totalSeconds = stb_vorbis_stream_length_in_seconds(oggFile);

		wave.data = (short *)malloc(wave.sampleCount*wave.channels*sizeof(short));
		int numSamplesOgg = stb_vorbis_get_samples_short_interleaved(oggFile, info.channels, (short *)wave.data, wave.sampleCount*wave.channels);
		stb_vorbis_close(oggFile);
        Sound sound = LoadSoundFromWave(wave);
        UnloadWave(wave);
        return sound;
	}

void game() {
	_SIZE(Texture);

	srand(time(NULL));
	Vector2 screen = {1280, 760};
	InitWindow(screen.x, screen.y, "Vibrant");
	SetTargetFPS(59);
#define monster_max 10

	InitAudioDevice(); 


    // :load sound
    Sound doorOpenSound = loadSoundFromMemory(&DoorOpen_memory, DoorOpen_size);
    Sound monsterScreamSound = loadSoundFromMemory(&MonsterScream_memory, MonsterScream_size);
    Sound doorCloseSound = loadSoundFromMemory(&DoorClose_memory, DoorClose_size);



	//
	// :init 
	//
	
//:darkness
				Image flashlight_down_masked =   GenImageColor(screen.x,screen.y, BLACK);
				Image flashlight_left_masked =   GenImageColor(screen.x,screen.y, BLACK);
				Image flashlight_down_image = LoadImage("assets/flashlight_down_cone.png");
				Image flashlight_left_image = LoadImage("assets/flashlight_left_cone.png");
				float darkness_level = 0.9;
				ImageAlphaClear(&flashlight_down_image, BLACK, darkness_level);
				ImageAlphaClear(&flashlight_left_image, BLACK, darkness_level);
				ImageAlphaMask(&flashlight_down_masked, flashlight_down_image);
				ImageAlphaMask(&flashlight_left_masked, flashlight_left_image);
				ImageColorTint(&flashlight_down_masked, Fade(BLACK, darkness_level));
				ImageColorTint(&flashlight_left_masked, Fade(BLACK, darkness_level));
				Texture2D flashlight_down= LoadTextureFromImage(flashlight_down_masked);
				ImageRotateCW(&flashlight_down_masked);
				ImageRotateCW(&flashlight_down_masked);
				Texture2D flashlight_up = LoadTextureFromImage(flashlight_down_masked);
				Texture2D flashlight_left= LoadTextureFromImage(flashlight_left_masked);
				ImageRotateCW(&flashlight_left_masked);
				ImageRotateCW(&flashlight_left_masked);
				Texture2D flashlight_right = LoadTextureFromImage(flashlight_left_masked);

// :shaders
#define MAX_POSTPRO_SHADERS         13
 Shader shaders[MAX_POSTPRO_SHADERS] = { 0 };

    // NOTE: Defining 0 (NULL) for vertex shader forces usage of internal default vertex shader
    shaders[FX_GRAYSCALE] = LoadShader(0, "assets/shaders/grayscale.fs");
    shaders[FX_POSTERIZATION] = LoadShader(0, "assets/shaders/posterization.fs");
    shaders[FX_DREAM_VISION] = LoadShader(0, "assets/shaders/dream_vision.fs");
    shaders[FX_PIXELIZER] = LoadShader(0, "assets/shaders/pixelizer.fs");
    shaders[FX_CROSS_HATCHING] = LoadShader(0, "assets/shaders/cross_hatching.fs");
    shaders[FX_CROSS_STITCHING] = LoadShader(0, "assets/shaders/cross_stitching.fs");
    shaders[FX_PREDATOR_VIEW] = LoadShader(0, "assets/shaders/predator.fs");
    shaders[FX_SCANLINES] = LoadShader(0, "assets/shaders/scanlines.fs");
    shaders[FX_FISHEYE] = LoadShader(0, "assets/shaders/fisheye.fs");
    shaders[FX_SOBEL] = LoadShader(0, "assets/shaders/sobel.fs");
    shaders[FX_BLOOM] = LoadShader(0, "assets/shaders/bloom.fs");
    shaders[FX_BLUR] = LoadShader(0, "assets/shaders/blur.fs");
    shaders[FX_DISTORTION] = LoadShader(0, "assets/shaders/distortion.fs");
    int currentShader = FX_BLOOM;

    


    // Create a RenderTexture2D to be used for render to texture
    RenderTexture2D target = LoadRenderTexture(screen.x, screen.y);
    
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
	state.success_control    = 0;
	state.select_object_control = 0;
	state.normal_display     = 1;
	state.title_display      = 1;
	state.flashlight_display      = 0;
	state.dialogue_display   = 0;
	state.failure_display    = 0;
	state.success_display    = 0;
	state.jumpscare_active   = 0;
	state.debug_display      = 0;
	state.todo_display      = 0;
	state.clock_display      = 0;
	state.select_floor_control = 0;
	state.select_floor_display      = 0;
	state.select_floor_ndx = 0;
	state.select_object_control = 0;
	state.select_object_display      = 0;
	state.select_object_ndx = 0;

	state.monster_count      = 0;
	state.monsters_loaded    = 0;
	state.dialogue_index     = 0;

	// :floor
	// :texture  - loading
	// :load textures

	Texture2D floorTextures[FloorTypeEnd];
	Texture2D personTextures[PersonTypeEnd];
	Texture2D objectTextures[ObjectTypeEnd];
	Texture2D monsterTextures[MonsterTypeEnd];

	//"floorTextures[%s] = loadTextureFromMemory(&%s_memory, %s_size);\\\n",
	// This is pretty complex 
	// We are using meta programming to turn the images into objects
	// @Concern: This is very inflexible and possibly over optimized.
	image_binary_vars_loadAllTextures();

	int *dialogue_index = &state.dialogue_index;
	// @Todo: create a function which adds to the dialogue queue, and check character limit
	char (*dialogue_queue)[20][Line_Max] = &state.dialogue_queue;
	for (int i = 0; i < Dialogue_Max; ++i) {
		(*dialogue_queue)[i][0] = '\0';
	}



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

	//if(!LoadFromFilename((void *)floors, sizeof (Floors), "floor.data")) {
	//   floors->count      = 0;
	//}
	FloorLoad(floors->items, &floors->count);
	//FloorSave(floors->items, &floors->count);

	// :load objects

	ObjectLoad(objects->items, &objects->count);
	//ObjectSave(objects->items, &objects->count);

	// :objects collected
	ObjectType todos[] = {
		Wires, Pliers, Hammer, Crowbar, Rope, Pickaxe, Fuse, Shovel, Fireaxe, ObjectTypeEnd
	};

	int objects_collected[100]; 
	int objects_collected_count = 0;

	int * triggers = &state.triggers[0]; 
	for (int i = 0; i < TriggerEnd; ++i) {
		triggers[i] = 0;
	}
	while (!WindowShouldClose()) {

		// :monsters reload
		// :reload
		// once per 2 seconds, reload


		if (!state.monsters_loaded) {
			state.monsters_loaded = 1;

			Monster * monsters = &state.monsters[0];

			int * monster_count = &state.monster_count;
			MonsterLoad(monsters, monster_count);
			//MonsterSave(monsters, monster_count);


			int * monster_direction_count = &state.monster_direction_count;
			*monster_direction_count = 0;

			MonsterDirection * monster_directions = &state.monster_directions[0];

			char direction_string[1000];// = "-9 -21 L\n-15 -21 U\n-15 -22 R\n-6 -22 D\n";
		} 



		++timer;
		if (timer > 1000) timer = 0;
		++frame_passed;
		++frame_long;

		// :control
		// :debug control
		if (IsKeyPressed(KEY_F1)) {
			state.debug_display = !state.debug_display;
		}
		if (IsKeyPressed(KEY_F2)) {
			currentShader++;
			if (currentShader >= MAX_POSTPRO_SHADERS) currentShader = 0;
	}



		// :intro control
		if (state.intro_control) {
			camera_offset_y -= 3;
            //printf("camera_offset_y %d\n", camera_offset_y);
			if (camera_offset_y < 0) {
				camera_offset_y = 0;
				AddDialogue(dialogue_queue, "I wish I could do some fishing...");
				AddDialogue(dialogue_queue, "Too bad I have to get to my new job...");
				changeState(&state, DialogueState);
			}
		}

		// :build control
		if (state.build_control) {
			if(shift() && IsKeyPressed(KEY_F)) {
				state.normal_control = 0;
				state.select_floor_control = 1;
				state.select_floor_display = 1;
			}
			// :floor
			if(!shift() && IsKeyPressed(KEY_F)) {
				// Check if object exists at these coordinates
				int floorSameType = 0;
				for (int i = 0; i < floors->count;++i) {
					Floor *floor = &floors->items[i];
					if (floor->x == player->x && floor->y==player->y) {
						if (floor->type == state.select_floor_ndx) {
							floorSameType = 1;
						}
						*floor = floors->items[floors->count-1];
						--floors->count;
					}
				}
				if (!floorSameType) {
					Floor *floor = &floors->items[floors->count];
					floor->x = player->x;
					floor->y = player->y;
					floor->type = state.select_floor_ndx;
					++floors->count;
				}
				// :save floors
				//SaveToFilename((void *)floors, sizeof(Floors),"floor.data");
				FloorSave(floors->items, &floors->count);
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
				//SaveToFilename((void *)objects, sizeof(Objects),"objects.data");
				ObjectSave(objects->items, &objects->count);
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
						if (object->rotation >= 4 || object->rotation < 0){
							object->rotation = 0;
						} 
						break;
					}
				}
				// :save objects
				ObjectSave(objects->items, &objects->count);
			}

			// :object create
			if(shift() && IsKeyPressed(KEY_O)) {
				state.select_object_display = 1;
				state.select_object_control = 1;
				state.normal_control = 0;
			}
			if (!shift() && IsKeyPressed(KEY_O)) {
				// Check if object exists at these coordinates
				int objectExists = 0;
				for (int i = 0; i < objects->count;++i) {
					Object *object = &objects->items[i];
					if (object->x == player->x && object->y==player->y) {
						objectExists = 1;
						*object = objects->items[objects->count-1];
						--objects->count;
					}
				}
				if (!objectExists) {
					Object *object = &objects->items[objects->count];
					object->x = player->x;
					object->y = player->y;
					object->type = state.select_object_ndx;
					object->rotation = 0;
					object->actionFrame = 0;
					++objects->count;
				}
				// :save objects
				ObjectSave(objects->items, &objects->count);
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
				ObjectSave(objects->items, &objects->count);
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
				AddDialogue(dialogue_queue, "\"Benny's Plaza reopens, after the tragedy \nlast month, which left 1 dead...");
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
					// :monster create
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
                    state.todo_display = 1;
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
				state.flashlight_display = 1;

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
				// :objects collected collectable
				// :collect object

				if (triggers[ReadsPaper2] && object->collectable && object->x == player->x && object->y == player->y) {
					int object_already_collected = 0;
					for (int obj_index = 0; obj_index < objects_collected_count; ++obj_index) {
						if(i == objects_collected[obj_index]){
							object_already_collected = 1;
						}
					}
					if (!object_already_collected) {
						objects_collected[objects_collected_count] = i;
						++objects_collected_count;
					}
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
		// :success
		if (state.success_control) {
			if (IsKeyPressed(KEY_ENTER) || 
					IsMouseButtonPressed(MOUSE_LEFT_BUTTON)
			   ) {
				printf("open browser to horrorpixel.us\n");
				system("open http://horrorpixel.us");
			}
		}
		// :title control
		if (state.title_control) {
			// :build selection
			if (IsKeyPressed(KEY_B)) {
				(*dialogue_queue)[0][0] = '\0';
				camera_offset_y = 0;
				state.build_control      = 1;
				state.title_control      = 0;
				state.normal_control     = 1;
				state.todo_display = 1;

				state.title_display      = 0;
				state.jumpscare_active   = 0;
				state.normal_display     = 1;
				state.monsters_control    = 1;
				// :debug - transport player into cafe
				transport_player(-9,-22, player);

			}
			if (IsKeyPressed(KEY_ENTER)) {
				state.title_control      = 0;
				state.intro_control      = 1;


				/* instant play mode */
				state.jumpscare_active   = 0;
				state.title_display      = 0;
				state.normal_display     = 1;
				state.monsters_control    = 0;
				// :debug - transport player into cafe
				//transport_player(-9,-22, player);
			}
		}

		// :select floor control
		if (state.select_floor_control) {
			int i = state.select_floor_ndx;
			if (leftPressed()){
				i--;
			}
			if (downPressed()){
				i+= 5;
			}
			if (upPressed()) {
				i-= 5;
			}
			if (rightPressed()){
				i++;
			}
			if (i >= ObjectTypeEnd) {
				i = 0;
			} 
			else if (i < 0) {
				i = ObjectTypeEnd - 1;
			}
			if (IsKeyPressed(KEY_ENTER)) {
				state.select_floor_control = 0;
				state.select_floor_display = 0;
				state.normal_control = 1;
			}
			state.select_floor_ndx = i;
		}

		// :select object control
		if (state.select_object_control) {
			int i = state.select_object_ndx;
			if (leftPressed()) {
				i--;
			}
			if (downPressed()) {
				i+= 5;
			}
			if (upPressed()) {
				i-= 5;
			}
			if (rightPressed()) {
				i++;
			}
			if (i >= ObjectTypeEnd) {
				i = 0;
			} 
			else if (i < 0) {
				i = ObjectTypeEnd - 1;
			}
			if (IsKeyPressed(KEY_ENTER)) {
				state.select_object_control = 0;
				state.select_object_display = 0;
				state.normal_control = 1;
			}
			state.select_object_ndx = i;
		}

		// :monsters control
		// :monster control
		if (state.monsters_control) {
			// :monster movement
			++enemy_timer;
			int enemy_speed = 50;
			if (enemy_timer > enemy_speed && !noclip) {
				for (int i = 0; i < *monster_count; ++i) {
					Monster * monster = &monsters[i];
					memcpy(&temp_monster,monster, sizeof(Monster));
					enemy_timer = 0;

					int monster_follow_player = 0;

					if (monster->type == BENNY) {
						monster->mode = 0;
						if (monster->y == player->y) {
							int random_number = rand() %5;
							if (random_number != 0) {
								monster_follow_player = 1;
								monster->mode = 1;
							}
						} 
						else {
							int random_number = rand() %3;
							if (random_number == 0) {
								monster_follow_player = 1;
								monster->mode = 1;
							}
						}
					}
					else {
						if (monster->y == player->y) {
							int random_number = rand() %3;
							if (random_number != 0) {
								monster_follow_player = 1;
							}
						} 
						else {
							int random_number = rand() %5;
							if (random_number == 0) {
								monster_follow_player = 1;
							}
						}
					}
					// :monster follow player
					if (monster_follow_player) {
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
					} else { // :monster random movement
						int random_number = rand() %4;
						if (random_number == 0) {
							monster->x += 1;
						}
						if (random_number == 1) {
							monster->x -= 1;
						}
						if (random_number == 2) {
							monster->y += 1;
						}
						if (random_number == 3) {
							monster->y -= 1;
						}
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
        // :shader
            BeginTextureMode(target);       // Enable drawing to texture
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
				int object_collected = 0;
				for (int obj_ndx = 0;obj_ndx < objects_collected_count; ++obj_ndx){
					if (objects_collected[obj_ndx] == i) {
						object_collected = 1;
					}
				}
				if (object->y < -24 && hide_outdoors) {
					// skip drwaing outdoors
				}
				else if (!object_collected){
					DrawTexturePro(objectTextures[object->type],  (Rectangle){ object->actionFrame * 16, object->rotation * 16,16,16}, RectanglePixelsFromXYCoords(object->x, object->y, Tile_Size), (Vector2){0,0}, 0, WHITE);
				}
			}

			// :monster draw
			for (int i = 0; i < *monster_count; ++i) {
				Monster * monster = &monsters[i];
				if (monster->type == BENNY && monster->mode == 1) {
					DrawTextureEx(monsterTextures[BENNY_ANGRY], (Vector2) { monster->x * 64, monster->y * Tile_Size - Tile_Size / 4  }, 0, 4, WHITE);
				}
				else {
					DrawTextureEx(monsterTextures[monster->type], (Vector2) { monster->x * 64, monster->y * Tile_Size - Tile_Size / 4  }, 0, 4, WHITE);
				}
			} 

			// :draw player
			Rectangle player_rect; 
			player_rect.height = Tile_Size;
			player_rect.width = Tile_Size;
			player_rect.x = player->x_pixel;
			player_rect.y = player->y_pixel - Tile_Size / 4;
			if (player->action == STAND) {
				DrawTexturePro(personTextures[Stand], (Rectangle) { 32 * frame, 0, 32, 32 }, player_rect, (Vector2){ 0, 0 }, 0, WHITE);
			} 
			else if (player->action == DEAD) {
				DrawTexturePro(personTextures[Walk], (Rectangle) { 32 * frame, 0, 32, 32 }, player_rect, (Vector2){ -12, 64 }, 90, WHITE);
			} else {
				DrawTexturePro(personTextures[Walk], (Rectangle) { 32 * frame, 0, 32, 32 }, player_rect, (Vector2){ 0, 0 }, 0, WHITE);
			}

			EndMode2D();
            // :flashlight display
			if (state.flashlight_display) {

// create image

                Vector2 Person = {screen.x/2, screen.y/2};
                int width = 100;
                int length = 300;
                Color Blue = Fade(BLUE, 0.5);
                if (player->direction == RIGHT) {
                    //Vector2 Flashlight1 = {Person.x + length, Person.y+width,};
                    //Vector2 Flashlight2 = {Person.x + length, Person.y-width,};
                    //DrawTriangle(Person, Flashlight1, Flashlight2, Blue);
					DrawTexture(flashlight_right, 0, 0,WHITE);
                }
                if (player->direction == LEFT) {
                    //Vector2 Flashlight1 = {Person.x - length, Person.y+width,};
                    //Vector2 Flashlight2 = {Person.x - length, Person.y-width,};
                    //DrawTriangle(Flashlight1, Person, Flashlight2, Blue);
					DrawTexture(flashlight_left, 0, 0,WHITE);
                }
                if (player->direction == UP ) {
                    Vector2 Flashlight1 = {Person.x - width, Person.y-length,};
                    Vector2 Flashlight2 = {Person.x + width, Person.y-length,};
                    //DrawTriangle(Flashlight1, Person, Flashlight2, Blue);
					DrawTexture(flashlight_up, 0, 0,WHITE);
                }
                if (player->direction == DOWN ) {
                    //Vector2 Flashlight1 = {Person.x - width, Person.y+length,};
                    //Vector2 Flashlight2 = {Person.x + width, Person.y+length,};
                //    DrawTriangle(Person, Flashlight1, Flashlight2, Blue);
					DrawTexture(flashlight_down, 0, 0,WHITE);
                }
				//Image flashlight_image = GetTextureData(flashlight_texture);


            }

		} // state.normal_display
		if (state.title_display) {
			PrintData print_data;
			print_data.cursor.x =  screen.x/2 - 450;
			print_data.cursor.y = 100;
			print_data.fontsize = 60;
			print_data.height   = 70;
			print_data.color    = WHITE;
			print("Three Nights at Benny's Plaza", &print_data);
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

					if (monster->type == BENNY) {
						DrawTexturePro(monsterTextures[BENNY_ANGRY], (Rectangle) { 0, 0, 16,   16 }, (Rectangle) { 612,  512, 1024, 1024 }, (Vector2) { 512,  512}, -8 +(16),  WHITE);
					}
					else {
						DrawTexturePro(monsterTextures[monster->type], (Rectangle) { 0, 0, 16,   16 }, (Rectangle) { 612,  512, 1024, 1024 }, (Vector2) { 512,  512}, -8 +(16),  WHITE);
					}
					// :jumpscare
				} 
				else {
					state.failure_display = 1;
					state.normal_display = 0;
					StopSound(monsterScreamSound);
				}
			}
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



		// :todo display
		if (state.todo_display) {
			PrintData print_data;
			print_data.cursor.x =  20;
			print_data.cursor.y = 20;
			print_data.fontsize = 18;
			print_data.height   = 25;
			print_data.color    = WHITE;
			DrawRectangle(0, 0, 240,280, Fade(BLACK, 0.5));
			char text[60];
			int game_complete = 1;
			for (int todo_i = 0; todos[todo_i] != ObjectTypeEnd; ++todo_i) {

				char item_found = '_';
				for (int collected_i = 0; collected_i <objects_collected_count; ++collected_i) {
					if (objects->items[objects_collected[collected_i]].type == todos[todo_i]) {
						item_found = 'X';
						break;
					}
				} 
				if (item_found == '_') {
					game_complete = 0;
				}

				sprintf(text, "[%c] Collect %s", item_found, ObjectTypeString[todos[todo_i]]);
				print(text, &print_data);
			}
			if (game_complete) {
				state.success_display = 1;
				state.success_control = 1;
				state.normal_display = 0;
			}
		} // display todo

		// :select object display
		if (state.select_object_display) {
			Rectangle src_rect = (Rectangle){ 0, 0,16,16};
			int size = 80;
			int cols = 5;
			int y = 100;
			int x = 100;
			int padding = 10;
			for(int i = 0; i != ObjectTypeEnd; ++i) {
				Color color = Fade(BLACK, 0.9);
				if (i == state.select_object_ndx) {
					color = Fade(WHITE, 0.9);
				}
				if (i % cols == 0) {
					y += size + padding*2;
					x = 100;
				}
				DrawRectangle(
						x, 
						y, 
						(size + padding *2), 
						(size + padding * 2), 
						color
						);
				Rectangle dest_rect = (Rectangle){ x+padding, y+padding,size,size};
				DrawTexturePro(
						objectTextures[i], src_rect, 
						dest_rect, (Vector2){0,0}, 0, WHITE);
				x += size+ padding*2;
			}
		}

		// :select floor display
		if (state.select_floor_display) {
			Rectangle src_rect = (Rectangle){ 0, 0,16,16};
			int size = 80;
			int cols = 5;
			int y = 100;
			int x = 100;
			int padding = 10;
			for(int i = 0; i != FloorTypeEnd; ++i) {
				Color color = Fade(BLACK, 0.9);
				if (i == state.select_floor_ndx) {
					color = Fade(WHITE, 0.9);
				}
				if (i % cols == 0) {
					y += size + padding*2;
					x = 100;
				}
				DrawRectangle(
						x, 
						y, 
						(size + padding *2), 
						(size + padding * 2), 
						color
						);
				Rectangle dest_rect = (Rectangle){ x+padding, y+padding,size,size};
				DrawTexturePro(
						floorTextures[i], src_rect, 
						dest_rect, (Vector2){0,0}, 0, WHITE);
				x += size+ padding*2;
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
			DrawRectangle(0, 0, screen.x,screen.y, BLACK);
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

		// :success display
		if (state.success_display) {
			clock[FailureClock]++;
			PrintData print_data;
			print_data.cursor.x =  screen.x/2 - 450;
			print_data.cursor.y = 100;
			print_data.fontsize = 60;
			print_data.height   = 70;
			print_data.color    = WHITE;
			DrawRectangle(0, 0, screen.x,screen.y, BLACK);
			print("This was a demo.", &print_data);
			print("You Won!", &print_data);
			print_data.fontsize = 26;
			print("You got all the things, and the monsters realized they were no match for you", &print_data);
			print("and opened the door so you could leave.", &print_data);
			print("You spent the rest of the morning fishing. You caught 2 fish!", &print_data);
			print("Download the full game at:", &print_data);
			print("www.horrorpixel.us", &print_data);
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
			char text[60];
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
					sprintf(text, "object: %s %d", ObjectTypeString[object->type], object->collectable);
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

        EndTextureMode();
		BeginShaderMode(shaders[currentShader]);
            DrawTextureRec(target.texture, (Rectangle){ 0, 0, target.texture.width, -target.texture.height }, (Vector2){ 0, 0 }, WHITE);
        EndShaderMode();

		EndDrawing();
	}//while
}// main

