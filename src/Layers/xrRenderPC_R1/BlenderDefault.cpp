// BlenderDefault.cpp: implementation of the CBlender_default class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "BlenderDefault.h"
#include "../xrRender/uber_deffer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_default::CBlender_default    ()
{
    description.CLS                = B_DEFAULT;
    description.version         = 1;
    oTessellation.Count         = 4;
    oTessellation.IDselected    = 0;
}

CBlender_default::~CBlender_default    ()
{

}
void    CBlender_default::Save(    IWriter& fs )
{
    IBlender::Save    (fs);
    xrP_TOKEN::Item    I;
    xrPWRITE_PROP    (fs,"Tessellation",    xrPID_TOKEN, oTessellation);
    I.ID = 0; xr_strcpy(I.str,"NO_TESS");    fs.w        (&I,sizeof(I));
    I.ID = 1; xr_strcpy(I.str,"TESS_PN");    fs.w        (&I,sizeof(I));
    I.ID = 2; xr_strcpy(I.str,"TESS_HM");    fs.w        (&I,sizeof(I));
    I.ID = 3; xr_strcpy(I.str,"TESS_PN+HM");    fs.w        (&I,sizeof(I));
}
void    CBlender_default::Load(    IReader& fs, u16 version )
{
    IBlender::Load    (fs,version);
    if (version>0)
    {
        xrPREAD_PROP(fs,xrPID_TOKEN,oTessellation);
    }
}
void    CBlender_default::Compile(CBlender_Compile& C)
{
    IBlender::Compile(C);
#ifdef _EDITOR
    if (C.bEditor)    {
        uber_deffer(C, true, "deffer_base", "deffer_base", false, 0, true);
        C.r_End();
        //C.PassBegin        ();
        //{
        //    C.PassSET_ZB            (TRUE,TRUE);
        //    C.PassSET_Blend            (FALSE,D3DBLEND_ONE,D3DBLEND_ZERO,FALSE,0);
        //    C.PassSET_LightFog        (TRUE,TRUE);
        //    
        //    // Stage1 - Base texture
        //    C.StageBegin            ();
        //    C.StageSET_Color        (D3DTA_TEXTURE,      D3DTOP_MODULATE,        D3DTA_DIFFUSE);
        //    C.StageSET_Alpha        (D3DTA_TEXTURE,      D3DTOP_MODULATE,        D3DTA_DIFFUSE);
        //    C.StageSET_TMC            (oT_Name,oT_xform,"$null",0);
        //    C.StageEnd                ();
        //}
        //C.PassEnd            ();
    }
    else 
#endif
    {
        if (C.L_textures.size()<3)    Debug.fatal    (DEBUG_INFO,"Not enought textures for shader, base tex: %s",*C.L_textures[0]);
        switch (C.iElement)
        {
        case SE_R1_NORMAL_HQ:
            // Level view
            if (C.bDetail_Diffuse)
            {
                C.r_Pass    ("lmap_dt","lmap_dt",TRUE);
                C.r_Sampler    ("s_base",    C.L_textures[0]);
                C.r_Sampler    ("s_lmap",    C.L_textures[1]);
                C.r_Sampler    ("s_detail",C.detail_texture);
                C.r_Sampler_clf    ("s_hemi",*C.L_textures[2]);
                C.r_End        ();
            } else
            {
                C.r_Pass    ("lmap","lmap",TRUE);
                C.r_Sampler    ("s_base",C.L_textures[0]);
                C.r_Sampler    ("s_lmap",C.L_textures[1]);
                C.r_Sampler_clf    ("s_hemi",*C.L_textures[2]);
                C.r_End        ();
            }
            break;
        case SE_R1_NORMAL_LQ:
            C.r_Pass        ("lmap","lmap",TRUE);
            C.r_Sampler        ("s_base",C.L_textures[0]);
            C.r_Sampler        ("s_lmap",C.L_textures[1]);
            C.r_Sampler_clf    ("s_hemi",*C.L_textures[2]);
            C.r_End            ();
            break;
        case SE_R1_LPOINT:
            C.r_Pass        ("lmap_point","add_point",FALSE,TRUE,FALSE,TRUE,D3DBLEND_ONE,D3DBLEND_ONE,TRUE);
            C.r_Sampler        ("s_base",    C.L_textures[0]        );
            C.r_Sampler_clf    ("s_lmap",    TEX_POINT_ATT        );
            C.r_Sampler_clf    ("s_att",    TEX_POINT_ATT        );
            C.r_End            ();
            break;
        case SE_R1_LSPOT:
            C.r_Pass        ("lmap_spot","add_spot",FALSE,TRUE,FALSE,TRUE,D3DBLEND_ONE,D3DBLEND_ONE,TRUE);
            C.r_Sampler        ("s_base",    C.L_textures[0]);
            C.r_Sampler_clf    ("s_lmap",    "internal\\internal_light_att",        true);
            C.r_Sampler_clf    ("s_att",    TEX_SPOT_ATT        );
            C.r_End            ();
            break;
        case SE_R1_LMODELS:
            // Lighting only, not use alpha-channel
            C.r_Pass        ("lmap_l","lmap_l",FALSE);
            C.r_Sampler        ("s_base",C.L_textures[0]);
            C.r_Sampler        ("s_lmap",C.L_textures[1]);
            C.r_Sampler_clf    ("s_hemi",*C.L_textures[2]);
            C.r_End            ();
            break;
        }
    }
}
