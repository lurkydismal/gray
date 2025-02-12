// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently

#pragma once

#include <d3d9.h>

#pragma warning(disable:4995)
#include "../../xrEngine/stdafx.h"
#pragma warning(disable:4714)
#pragma warning( 4 : 4018 )
#pragma warning( 4 : 4244 )

#include "../xrRenderDX9/xrD3DDefs.h"

#include "../xrRender/HW.h"
#include "../xrRender/Shader.h"
#include "../xrRender/R_Backend.h"
#include "../xrRender/R_Backend_Runtime.h"

#include "../xrRender/Debug/dxPixEventWrapper.h"

#define        R_R1    1
#define        R_R2    2
#define        R_R4    4
#define        RENDER    R_R1

#include "../xrRender/ResourceManager.h"

#include "../../xrEngine/vis_common.h"
#include "../../xrEngine/Render.h"
#include "../../xrEngine/_d3d_extensions.h"

#ifndef _EDITOR
#include "../../xrEngine/IGame_Level.h"

#include "../../xrParticles/psystem.h"
#include "../xrRender/xrRender_console.h"
#include "FStaticRender.h"
#endif
#include "../xrRender/dxRenderDeviceRender.h"

#define        TEX_POINT_ATT    "internal\\internal_light_attpoint"
#define        TEX_SPOT_ATT    "internal\\internal_light_attclip"


#include "../xrRender/blenders/Blender.h"
#include "../xrRender/blenders/Blender_CLSID.h"
