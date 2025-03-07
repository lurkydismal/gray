#ifndef __GNUC__
#pragma once
#endif
#ifndef __XR_OBJECT_FORMAT_H__
#define __XR_OBJECT_FORMAT_H__

#include "xr_types.h"

namespace xray_re {

const uint16_t EOBJ_VERSION = 0x10;

enum eobj_chunk_id {
    EOBJ_CHUNK_MAIN        = 0x7777,

    EOBJ_CHUNK_VERSION    = 0x0900,
    EOBJ_CHUNK_FLAGS    = 0x0903,
    EOBJ_CHUNK_SURFACES_0    = 0x0905,    // old format
    EOBJ_CHUNK_SURFACES_1    = 0x0906,    // old format
    EOBJ_CHUNK_SURFACES_2    = 0x0907,
    EOBJ_CHUNK_MESHES    = 0x0910,
    EOBJ_CHUNK_0911        = 0x0911,    // ignored by AE
    EOBJ_CHUNK_USERDATA    = 0x0912,
    EOBJ_CHUNK_BONES_0    = 0x0913,    // old format
    EOBJ_CHUNK_MOTIONS    = 0x0916,
    EOBJ_CHUNK_SHADERS_0    = 0x0918,    // old format
    EOBJ_CHUNK_PARTITIONS_0    = 0x0919,    // old format
    EOBJ_CHUNK_TRANSFORM    = 0x0920,
    EOBJ_CHUNK_BONES_1    = 0x0921,
    EOBJ_CHUNK_REVISION    = 0x0922,    // file revision
    EOBJ_CHUNK_PARTITIONS_1    = 0x0923,
    EOBJ_CHUNK_MOTION_REFS    = 0x0924,
    EOBJ_CHUNK_LOD_REF    = 0x0925,    // LOD\Reference
};

enum eobj_clip_chunk_id {
    EOBJ_CLIP_VERSION_CHUNK    = 0x9000,
    EOBJ_CLIP_DATA_CHUNK    = 0x9001,
};

const uint16_t EMESH_VERSION = 0x11;

enum emesh_chunk_id {
    EMESH_CHUNK_VERSION    = 0x1000,
    EMESH_CHUNK_MESHNAME    = 0x1001,
    EMESH_CHUNK_FLAGS    = 0x1002,
    EMESH_CHUNK_BBOX    = 0x1004,
    EMESH_CHUNK_VERTS    = 0x1005,
    EMESH_CHUNK_FACES    = 0x1006,
    EMESH_CHUNK_VMAPS_0    = 0x1007,
    EMESH_CHUNK_VMREFS    = 0x1008,
    EMESH_CHUNK_SFACE    = 0x1009,
    EMESH_CHUNK_OPTIONS    = 0x1010,
    EMESH_CHUNK_VMAPS_1    = 0x1011,
    EMESH_CHUNK_VMAPS_2    = 0x1012,
    EMESH_CHUNK_SG        = 0x1013,
    EMESH_CHUNK_VNORMALS= 0x1014
};

const uint16_t BONE_VERSION_1 = 0x1;
const uint16_t BONE_VERSION_2 = 0x2;
const uint16_t BONE_VERSION = BONE_VERSION_2;

enum ebone_chunk_id {
    BONE_CHUNK_VERSION    = 0x0001,
    BONE_CHUNK_DEF        = 0x0002,
    BONE_CHUNK_BIND_POSE    = 0x0003,
    BONE_CHUNK_MATERIAL    = 0x0004,
    BONE_CHUNK_SHAPE    = 0x0005,
    BONE_CHUNK_IK_JOINT    = 0x0006,
    BONE_CHUNK_MASS_PARAMS    = 0x0007,
    BONE_CHUNK_IK_FLAGS    = 0x0008,
    BONE_CHUNK_BREAK_PARAMS    = 0x0009,
    BONE_CHUNK_FRICTION    = 0x0010,
};

} // end of namespace xray_re

#endif
