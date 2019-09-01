bits 64
section .rodata

global _DoorClose_memory
global _DoorClose_size

global _DoorOpen_memory
global _DoorOpen_size

global _MonsterScream_memory
global _MonsterScream_size

_DoorClose_memory:   incbin "../assets/sound_DoorClose.ogg"
_DoorClose_size:     dd $-_DoorClose_memory

_DoorOpen_memory:   incbin "../assets/sound_DoorOpen.ogg"
_DoorOpen_size:     dd $-_DoorOpen_memory

_MonsterScream_memory:   incbin "../assets/sound_MonsterScream.ogg"
_MonsterScream_size:     dd $-_MonsterScream_memory

