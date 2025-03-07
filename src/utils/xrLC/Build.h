// Build.h: interface for the CBuild class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#include "../xrDXT/xrDXT.h"
#include "../../xrCore/FS.h"
#include "../Shader_xrLC.h"

#include "../xrLC_Light/b_build_texture.h"
#include "../xrLC_Light/xrFaceDefs.h"

class xrLC_GlobalData;
class xrMU_Model;
class xrMU_Reference;
extern "C" XRLC_LIGHT_API xrLC_GlobalData*    lc_global_data();
//////////////////////////////////////////////////////////////////////////
// tesselator callbacks



typedef    int        tesscb_estimator    ( const Face* F );    // -1 = none, 0,1,2 = edge-number
typedef void    tesscb_face            (Face*        F);    // new face
typedef void    tesscb_vertex        (Vertex*    V);    // new vertex

class  base_lighting;
 
//////////////////////////////////////////////////////////////////////////
class CBuild  
{
public:
    CMemoryWriter                    &err_invalid();
    CMemoryWriter                    &err_tjunction();
    CMemoryWriter                    &err_multiedge();
    void                            err_save    ();

    Fbox                            scene_bb;
    xr_vector<b_shader>                shader_render;
    xr_vector<b_shader>                shader_compile;
    xr_vector<b_light_dynamic>        L_dynamic;
    xr_vector<b_glow>                glows;
    xr_vector<b_portal>                portals;
    xr_vector<b_lod>                lods;
    string_path                        path;
    xr_vector<LPCSTR>                g_Shaders;


    xr_vector<b_material>            &materials()    ;
    xr_vector<b_BuildTexture>        &textures()    ;
    base_lighting                    &L_static();
    xr_vector<xrMU_Model*>            &mu_models();
    xr_vector<xrMU_Reference*>        &mu_refs();

    Shader_xrLC_LIB                    &shaders();


    void    mem_Compact                ();
    void    mem_CompactSubdivs        ();
public:
    void    Load                    (const b_params& P, const IReader&  fs);
    void    Run                        (LPCSTR path);
    void    StartMu                    ();
    void    RunAfterLight            ( IWriter* fs    );
    void    Tesselate                ();
    void    PreOptimize                ();
    void    CorrectTJunctions        ();

    void    xrPhase_AdaptiveHT        ();
    void    u_Tesselate                (tesscb_estimator* E, tesscb_face* F, tesscb_vertex* V);
    void    u_SmoothVertColors        (int count);

    void    CalcNormals                ();
    void    MU_ModelsCalculateNormals();
    void    xrPhase_TangentBasis    ();

    void    BuildCForm                ();
    void    BuildPortals            (IWriter &fs);
    void    BuildRapid                (BOOL bSave);
    void    xrPhase_Radiosity        ();
        
    void    IsolateVertices            (BOOL bProgress);
    void    xrPhase_ResolveMaterials();
    void    xrPhase_UVmap            ();
    void    xrPhase_Subdivide        ();
    void    ImplicitLighting        ();


    void    Light_prepare            ();
    void    Light                    ();
    void    LMapsLocal                ();
    void    LMaps                    ();
    //void    Light_R2                ();
    void    LightVertex                ();
    void    xrPhase_MergeLM            ();
    void    xrPhase_MergeGeometry    ();

    void    Flex2OGF                ();
    void    BuildSectors            ();

    void    SaveLights                (IWriter &fs);
    void    SaveTREE                (IWriter &fs);
    void    SaveSectors                (IWriter &fs);

    void    validate_splits            ();
    bool    IsOGFContainersEmpty    ();
    void    CheckBeforeSave            ( u32 stage );
    void    TempSave                ( u32 stage );
 

    CBuild    ();
    ~CBuild    ();
};

extern CBuild*        pBuild;            ;
extern vec2Face        g_XSplit        ;
