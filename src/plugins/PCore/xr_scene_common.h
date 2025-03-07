#ifndef __GNUC__
#pragma once
#endif
#ifndef __XR_SCENE_COMMON_H__
#define __XR_SCENE_COMMON_H__

namespace xray_re {

// common editor chunks
enum {
    TOOLS_CHUNK_COUNT    = 0x0002,
    TOOLS_CHUNK_OBJECTS    = 0x0003,

    TOOLS_CHUNK_VERSION    = 0x1001,

    TOOLS_CHUNK_GUID    = 0x7000,
    TOOLS_CHUNK_OBJECT_CLASS= 0x7703,
    TOOLS_CHUNK_REVISION    = 0x7777,
    TOOLS_CHUNK_OBJECT_DATA    = 0x7777,
};

// scene parts
enum scene_chunk_id {
    SCENE_CHUNK_GROUPS        = 0x8000,
    SCENE_CHUNK_GLOWS        = 0x8001,
    SCENE_CHUNK_SCENE_OBJECTS    = 0x8002,
    SCENE_CHUNK_LIGHTS        = 0x8003,
    SCENE_CHUNK_SHAPES        = 0x8004,
    SCENE_CHUNK_SOUND_SRCS        = 0x8005,
    SCENE_CHUNK_SPAWNS        = 0x8006,
    SCENE_CHUNK_WAYS        = 0x8007,
    SCENE_CHUNK_SECTORS        = 0x8008,
    SCENE_CHUNK_PORTALS        = 0x8009,
    SCENE_CHUNK_SOUND_ENVS        = 0x800a,
    SCENE_CHUNK_PARTICLES        = 0x800b,
    SCENE_CHUNK_DETAIL_OBJECTS    = 0x800c,
    SCENE_CHUNK_AI_MAP        = 0x800d,
    SCENE_CHUNK_WALLMARKS        = 0x800e,
};

// Object classes in LevelEditor files.
enum tools_class_id {
    TOOLS_CLASS_GROUP        = 0x00,
    TOOLS_CLASS_GLOW        = 0x01,
    TOOLS_CLASS_SCENE_OBJECT    = 0x02,
    TOOLS_CLASS_LIGHT        = 0x03,
    TOOLS_CLASS_SHAPE        = 0x04,
    TOOLS_CLASS_SOUND_SRC        = 0x05,
    TOOLS_CLASS_SPAWN        = 0x06,
    TOOLS_CLASS_WAY            = 0x07,
    TOOLS_CLASS_SECTOR        = 0x08,
    TOOLS_CLASS_PORTAL        = 0x09,
    TOOLS_CLASS_SOUND_ENV        = 0x0a,
    TOOLS_CLASS_PARTICLE        = 0x0b,

    TOOLS_CLASS_DEFAULT        = 0xffffffff,
};

} // end of namespace xray_re

#endif
