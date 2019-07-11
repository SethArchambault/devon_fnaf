bits 64
section .rodata

global _Water_memory
global _Water_size

global _Grass_memory
global _Grass_size

global _Tile_memory
global _Tile_size

global _Wood_memory
global _Wood_size

global _Concrete_memory
global _Concrete_size

global _BrokenTile_memory
global _BrokenTile_size

global _Asphalt_memory
global _Asphalt_size

global _AsphaltLines_memory
global _AsphaltLines_size

global _Sidewalk_memory
global _Sidewalk_size

global _Dirt_memory
global _Dirt_size

global _Walk_memory
global _Walk_size

global _Stand_memory
global _Stand_size

global _Newspaper_memory
global _Newspaper_size

global _Table_memory
global _Table_size

global _Chair_memory
global _Chair_size

global _Door_memory
global _Door_size

global _Sink_memory
global _Sink_size

global _Stove_memory
global _Stove_size

global _Counter_memory
global _Counter_size

global _Generator_memory
global _Generator_size

global _Switch_memory
global _Switch_size

global _Fire_memory
global _Fire_size

global _MiniVan_memory
global _MiniVan_size

global _Important_memory
global _Important_size

global _Wires_memory
global _Wires_size

global _Pliers_memory
global _Pliers_size

global _MusicBox_memory
global _MusicBox_size

global _Hammer_memory
global _Hammer_size

global _Crowbar_memory
global _Crowbar_size

global _Rope_memory
global _Rope_size

global _Pickaxe_memory
global _Pickaxe_size

global _Fuse_memory
global _Fuse_size

global _Shovel_memory
global _Shovel_size

global _Fireaxe_memory
global _Fireaxe_size

global _BENNY_memory
global _BENNY_size

global _BENNY_ANGRY_memory
global _BENNY_ANGRY_size

global _BUDDY_memory
global _BUDDY_size

global _ALICE_memory
global _ALICE_size

global _ALEX_memory
global _ALEX_size

global _BOB_memory
global _BOB_size

global _DENNIS_memory
global _DENNIS_size

global _DONNY_memory
global _DONNY_size

_Water_memory:   incbin "../assets/floor_Water.png"
_Water_size:     dd $-_Water_memory

_Grass_memory:   incbin "../assets/floor_Grass.png"
_Grass_size:     dd $-_Grass_memory

_Tile_memory:   incbin "../assets/floor_Tile.png"
_Tile_size:     dd $-_Tile_memory

_Wood_memory:   incbin "../assets/floor_Wood.png"
_Wood_size:     dd $-_Wood_memory

_Concrete_memory:   incbin "../assets/floor_Concrete.png"
_Concrete_size:     dd $-_Concrete_memory

_BrokenTile_memory:   incbin "../assets/floor_BrokenTile.png"
_BrokenTile_size:     dd $-_BrokenTile_memory

_Asphalt_memory:   incbin "../assets/floor_Asphalt.png"
_Asphalt_size:     dd $-_Asphalt_memory

_AsphaltLines_memory:   incbin "../assets/floor_AsphaltLines.png"
_AsphaltLines_size:     dd $-_AsphaltLines_memory

_Sidewalk_memory:   incbin "../assets/floor_Sidewalk.png"
_Sidewalk_size:     dd $-_Sidewalk_memory

_Dirt_memory:   incbin "../assets/floor_Dirt.png"
_Dirt_size:     dd $-_Dirt_memory

_Walk_memory:   incbin "../assets/person_Walk.png"
_Walk_size:     dd $-_Walk_memory

_Stand_memory:   incbin "../assets/person_Stand.png"
_Stand_size:     dd $-_Stand_memory

_Newspaper_memory:   incbin "../assets/object_Newspaper.png"
_Newspaper_size:     dd $-_Newspaper_memory

_Table_memory:   incbin "../assets/object_Table.png"
_Table_size:     dd $-_Table_memory

_Chair_memory:   incbin "../assets/object_Chair.png"
_Chair_size:     dd $-_Chair_memory

_Door_memory:   incbin "../assets/object_Door.png"
_Door_size:     dd $-_Door_memory

_Sink_memory:   incbin "../assets/object_Sink.png"
_Sink_size:     dd $-_Sink_memory

_Stove_memory:   incbin "../assets/object_Stove.png"
_Stove_size:     dd $-_Stove_memory

_Counter_memory:   incbin "../assets/object_Counter.png"
_Counter_size:     dd $-_Counter_memory

_Generator_memory:   incbin "../assets/object_Generator.png"
_Generator_size:     dd $-_Generator_memory

_Switch_memory:   incbin "../assets/object_Switch.png"
_Switch_size:     dd $-_Switch_memory

_Fire_memory:   incbin "../assets/object_Fire.png"
_Fire_size:     dd $-_Fire_memory

_MiniVan_memory:   incbin "../assets/object_MiniVan.png"
_MiniVan_size:     dd $-_MiniVan_memory

_Important_memory:   incbin "../assets/object_Important.png"
_Important_size:     dd $-_Important_memory

_Wires_memory:   incbin "../assets/object_Wires.png"
_Wires_size:     dd $-_Wires_memory

_Pliers_memory:   incbin "../assets/object_Pliers.png"
_Pliers_size:     dd $-_Pliers_memory

_MusicBox_memory:   incbin "../assets/object_MusicBox.png"
_MusicBox_size:     dd $-_MusicBox_memory

_Hammer_memory:   incbin "../assets/object_Hammer.png"
_Hammer_size:     dd $-_Hammer_memory

_Crowbar_memory:   incbin "../assets/object_Crowbar.png"
_Crowbar_size:     dd $-_Crowbar_memory

_Rope_memory:   incbin "../assets/object_Rope.png"
_Rope_size:     dd $-_Rope_memory

_Pickaxe_memory:   incbin "../assets/object_Pickaxe.png"
_Pickaxe_size:     dd $-_Pickaxe_memory

_Fuse_memory:   incbin "../assets/object_Fuse.png"
_Fuse_size:     dd $-_Fuse_memory

_Shovel_memory:   incbin "../assets/object_Shovel.png"
_Shovel_size:     dd $-_Shovel_memory

_Fireaxe_memory:   incbin "../assets/object_Fireaxe.png"
_Fireaxe_size:     dd $-_Fireaxe_memory

_BENNY_memory:   incbin "../assets/monster_BENNY.png"
_BENNY_size:     dd $-_BENNY_memory

_BENNY_ANGRY_memory:   incbin "../assets/monster_BENNY_ANGRY.png"
_BENNY_ANGRY_size:     dd $-_BENNY_ANGRY_memory

_BUDDY_memory:   incbin "../assets/monster_BUDDY.png"
_BUDDY_size:     dd $-_BUDDY_memory

_ALICE_memory:   incbin "../assets/monster_ALICE.png"
_ALICE_size:     dd $-_ALICE_memory

_ALEX_memory:   incbin "../assets/monster_ALEX.png"
_ALEX_size:     dd $-_ALEX_memory

_BOB_memory:   incbin "../assets/monster_BOB.png"
_BOB_size:     dd $-_BOB_memory

_DENNIS_memory:   incbin "../assets/monster_DENNIS.png"
_DENNIS_size:     dd $-_DENNIS_memory

_DONNY_memory:   incbin "../assets/monster_DONNY.png"
_DONNY_size:     dd $-_DONNY_memory

