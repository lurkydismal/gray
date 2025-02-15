// Stats.h: interface for the CStats class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
#include "../../../xrEngine/Stats.h"
// refs
class CGameFont;

class ECORE_API CEStats:
    public CStats
{
public:
    float        fFPS,fRFPS,fTPS;    // FPS, RenderFPS, TPS
    u32        dwShader_Codes;        // Number of times the shader-code changes
    u32        dwShader_Textures;    // Number of times the shader-tex changes
    u32        dwShader_Matrices;    // Number of times the shader-xform changes
    u32        dwShader_Constants;    // Number of times the shader-consts changes
    u32        dwSND_Played,dwSND_Allocated;    // Play/Alloc
    u32        dwLevelSelFaceCount;    // Number of faces in scene
    u32        dwLevelSelVertexCount;    // Number of vertices in scene

    u32        lastDPS_verts;
    u32        lastDPS_polys;
    u32        lastDPS_calls;

    CStatTimer    RenderDUMP_RT;
    CStatTimer    RenderTOTAL;        //
    CStatTimer    RenderTOTAL_Real;
    CStatTimer    RenderCALC;            // portal traversal, frustum culling, entities "OnVisible"
    CStatTimer    RenderDUMP_SKIN;
    CStatTimer    Animation;            // skeleton calculation
    CStatTimer    RenderDUMP_DT_VIS;    // ...details visibility detection
    CStatTimer    RenderDUMP_DT_Render;// ...details rendering
    CStatTimer    RenderDUMP_DT_Cache;// ...details slot cache access

    u32        RenderDUMP_DT_Count;

    CStatTimer    Input;                // total time taken by input subsystem (accurate only in single-threaded mode)
    CStatTimer    clRAY;                // total: ray-testing
    CStatTimer    clBOX;                // total: box query
    CStatTimer    clFRUSTUM;            // total: frustum query

    CStatTimer    TEST0;                // debug counter
    CStatTimer    TEST1;                // debug counter
    CStatTimer    TEST2;                // debug counter
    CStatTimer    TEST3;                // debug counter

    virtual void    Show        () override;

    CEStats    ();
    virtual    ~CEStats    ();
};

#define UPDATEC(vert,poly,pass)        { EDevice->Statistic->dwVert+=(vert)*(pass);EDevice->Statistic->dwPoly+=(poly)*pass; EDevice->Statistic->dwCalls+=pass; }