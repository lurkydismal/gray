#include "stdafx.h"
#include "../xrRender/ResourceManager.h"
#include "blender_light_occq.h"
#include "blender_light_mask.h"
#include "blender_light_direct_cascade.h"
#include "blender_light_point.h"
#include "blender_light_spot.h"
#include "blender_light_reflected.h"
#include "blender_combine.h"
#include "../xrRender/blender_screen_postprocess.h"
#include "blender_bloom_build.h"
#include "blender_luminance.h"
#include "blender_ssao.h"
#include "../xrRender/blender_fxaa.h"
#include "../xrRender/blender_smaa.h"
#include "../../xrEngine/ICore_GPU.h"

#include "../xrRender/dxRenderDeviceRender.h"

void    CRenderTarget::u_setrt            (const ref_rt& _1, const ref_rt& _2, const ref_rt& _3, IDirect3DSurface9* zb)
{
    VERIFY                                    (_1);
    dwWidth                                    = _1->dwWidth;
    dwHeight                                = _1->dwHeight;
    if (_1) RCache.set_RT(_1->pRT,    0); else RCache.set_RT(nullptr,0);
    if (_2) RCache.set_RT(_2->pRT,    1); else RCache.set_RT(nullptr,1);
    if (_3) RCache.set_RT(_3->pRT,    2); else RCache.set_RT(nullptr,2);
    RCache.set_ZB                            (zb);
//    RImplementation.rmNormal                ();
}

void    CRenderTarget::u_setrt            (u32 W, u32 H, IDirect3DSurface9* _1, IDirect3DSurface9* _2, IDirect3DSurface9* _3, IDirect3DSurface9* zb)
{
    VERIFY                                    (_1);
    dwWidth                                    = W;
    dwHeight                                = H;
    VERIFY                                    (_1);
    RCache.set_RT                            (_1,    0);
    RCache.set_RT                            (_2,    1);
    RCache.set_RT                            (_3,    2);
    RCache.set_ZB                            (zb);
//    RImplementation.rmNormal                ();
}

void    CRenderTarget::u_stencil_optimize    (BOOL        common_stencil)
{
    VERIFY    (RImplementation.o.nvstencil);
    RCache.set_ColorWriteEnable    (FALSE);
    u32        Offset;
    float    _w                    = float(RCache.get_width());
    float    _h                    = float(RCache.get_height());
    u32        C                    = color_rgba    (255,255,255,255);
    float    eps                    = EPS_S;
    FVF::TL* pv                    = (FVF::TL*) RCache.Vertex.Lock    (4,g_combine->vb_stride,Offset);
    pv->set                        (eps,            float(_h+eps),    eps,    1.f, C, 0, 0);    pv++;
    pv->set                        (eps,            eps,            eps,    1.f, C, 0, 0);    pv++;
    pv->set                        (float(_w+eps),    float(_h+eps),    eps,    1.f, C, 0, 0);    pv++;
    pv->set                        (float(_w+eps),    eps,            eps,    1.f, C, 0, 0);    pv++;
    RCache.Vertex.Unlock        (4,g_combine->vb_stride);
    RCache.set_CullMode            (CULL_NONE    );
    if (common_stencil)            RCache.set_Stencil    (TRUE,D3DCMP_LESSEQUAL,dwLightMarkerID,0xff,0x00);    // keep/keep/keep
    RCache.set_Element            (s_occq->E[1]    );
    RCache.set_Geometry            (g_combine        );
    RCache.Render                (D3DPT_TRIANGLELIST,Offset,0,4,0,2);
}

// 2D texgen (texture adjustment matrix)
void    CRenderTarget::u_compute_texgen_screen    (Fmatrix& m_Texgen)
{
    float    _w                        = float(RCache.get_width());
    float    _h                        = float(RCache.get_height());
    float    o_w                        = (.5f / _w);
    float    o_h                        = (.5f / _h);
    Fmatrix            m_TexelAdjust        = 
    {
        0.5f,                0.0f,                0.0f,            0.0f,
        0.0f,                -0.5f,                0.0f,            0.0f,
        0.0f,                0.0f,                1.0f,            0.0f,
        0.5f + o_w,            0.5f + o_h,            0.0f,            1.0f
    };
    m_Texgen.mul    (m_TexelAdjust,RCache.xforms.m_wvp);
}

// 2D texgen for jitter (texture adjustment matrix)
void    CRenderTarget::u_compute_texgen_jitter    (Fmatrix&        m_Texgen_J)
{
    // place into    0..1 space
    Fmatrix            m_TexelAdjust        = 
    {
        0.5f,                0.0f,                0.0f,            0.0f,
        0.0f,                -0.5f,                0.0f,            0.0f,
        0.0f,                0.0f,                1.0f,            0.0f,
        0.5f,                0.5f,                0.0f,            1.0f
    };
    m_Texgen_J.mul    (m_TexelAdjust,RCache.xforms.m_wvp);

    // rescale - tile it
    float    scale_X            = float(RCache.get_width())    / float(TEX_jitter);
    float    scale_Y            = float(RCache.get_height())/ float(TEX_jitter);
    float    offset            = (.5f / float(TEX_jitter));
    m_TexelAdjust.scale            (scale_X,    scale_Y,1.f    );
    m_TexelAdjust.translate_over(offset,    offset,    0    );
    m_Texgen_J.mulA_44            (m_TexelAdjust);
}

u8        fpack            (float v)                {
    s32    _v    = iFloor    (((v+1)*.5f)*255.f + .5f);
    clamp    (_v,0,255);
    return    u8(_v);
}
u8        fpackZ            (float v)                {
    s32    _v    = iFloor    (_abs(v)*255.f + .5f);
    clamp    (_v,0,255);
    return    u8(_v);
}
Fvector    vunpack            (s32 x, s32 y, s32 z)    {
    Fvector    pck;
    pck.x    = (float(x)/255.f - .5f)*2.f;
    pck.y    = (float(y)/255.f - .5f)*2.f;
    pck.z    = -float(z)/255.f;
    return    pck;
}
Fvector    vunpack            (Ivector src)            {
    return    vunpack    (src.x,src.y,src.z);
}
Ivector    vpack            (Fvector src)
{
    Fvector            _v;
    int    bx            = fpack    (src.x);
    int by            = fpack    (src.y);
    int bz            = fpackZ(src.z);
    // dumb test
    float    e_best    = flt_max;
    int        r=bx,g=by,b=bz;
#ifdef DEBUG
    int        d=0;
#else
    int        d=3;
#endif
    for (int x=_max(bx-d,0); x<=_min(bx+d,255); x++)
    for (int y=_max(by-d,0); y<=_min(by+d,255); y++)
    for (int z=_max(bz-d,0); z<=_min(bz+d,255); z++)
    {
        _v                = vunpack(x,y,z);
        float    m        = _v.magnitude();
        float    me        = _abs(m-1.f);
        if    (me>0.03f)    continue;
        _v.div    (m);
        float    e        = _abs(src.dotproduct(_v)-1.f);
        if (e<e_best)    {
            e_best        = e;
            r=x,g=y,b=z;
        }
    }
    Ivector        ipck;
    ipck.set    (r,g,b);
    return        ipck;
}

void    generate_jitter    (DWORD*    dest, u32 elem_count)
{
    const    int        cmax        = 8;
    svector<Ivector2,cmax>        samples;
    while (samples.size()<elem_count*2)
    {
        Ivector2    test;
        test.set    (::Random.randI(0,256),::Random.randI(0,256));
        BOOL        valid = TRUE;
        for (u32 t=0; t<samples.size(); t++)
        {
            int        dist    = _abs(test.x-samples[t].x)+_abs(test.y-samples[t].y);
            if (dist<32)    {
                valid        = FALSE;
                break;
            }
        }
        if (valid)    samples.push_back    (test);
    }
    for    (u32 it=0; it<elem_count; it++, dest++)
        *dest    = color_rgba(samples[2*it].x,samples[2*it].y,samples[2*it+1].y,samples[2*it+1].x);
}

CRenderTarget::CRenderTarget        ()
{
    param_blur            = 0.f;
    param_gray            = 0.f;
    param_noise            = 0.f;
    param_duality_h        = 0.f;
    param_duality_v        = 0.f;
    param_noise_fps        = 25.f;
    param_noise_scale    = 1.f;

    im_noise_time = 1.0f / 100.0f;
    im_noise_shift_w    = 0;
    im_noise_shift_h    = 0;

    param_color_base    = color_rgba(127,127,127,    0);
    param_color_gray    = color_rgba(85,85,85,        0);
    param_color_add.set( 0.0f, 0.0f, 0.0f );

    dwAccumulatorClearMark            = 0;
    dxRenderDeviceRender::Instance().Resources->Evict            ();

    // Blenders
    b_occq                            = new CBlender_light_occq            ();
    b_accum_mask                    = new CBlender_accum_direct_mask    ();
    b_accum_direct_cascade            = new CBlender_accum_direct_cascade    ();
    b_accum_point                    = new CBlender_accum_point            ();
    b_accum_spot                    = new CBlender_accum_spot            ();
    b_accum_reflected                = new CBlender_accum_reflected        ();
    b_bloom                            = new CBlender_bloom_build            ();
    b_ssao                            = new CBlender_SSAO                    ();
    b_luminance                        = new CBlender_luminance            ();
    b_combine                        = new CBlender_combine                ();
    b_fxaa = new CBlender_FXAA();

    //    NORMAL
    {
        u32        w=RCache.get_width(), h=RCache.get_height();
        rt_Position.create            (r2_RT_P,        w,h,D3DFMT_A16B16G16R16F);
        rt_Normal.create            (r2_RT_N,        w,h,D3DFMT_A16B16G16R16F);

        // select albedo & accum
        if (RImplementation.o.mrtmixdepth)    
        {
            // NV50
            rt_Color.create            (r2_RT_albedo,    w,h,D3DFMT_A8R8G8B8        );
            rt_Accumulator.create    (r2_RT_accum,    w,h,D3DFMT_A16B16G16R16F);
        }
        else        
        {
            // can't - mix-depth
            if (RImplementation.o.fp16_blend) {
                // NV40
                rt_Color.create                (r2_RT_albedo,        w,h,D3DFMT_A16B16G16R16F);    // expand to full
                rt_Accumulator.create        (r2_RT_accum,        w,h,D3DFMT_A16B16G16R16F);
            } else {
                // R4xx, no-fp-blend,-> albedo_wo
                rt_Color.create                (r2_RT_albedo,        w,h,D3DFMT_A8R8G8B8        );    // normal
                rt_Accumulator.create        (r2_RT_accum,        w,h,D3DFMT_A16B16G16R16F);
                rt_Accumulator_temp.create    (r2_RT_accum_temp,    w,h,D3DFMT_A16B16G16R16F);
            }
        }

        // generic(LDR) RTs
        rt_Generic_0.create(r2_RT_generic0, w, h, D3DFMT_A16B16G16R16F);
        rt_Generic_1.create(r2_RT_generic1, w, h, D3DFMT_A8R8G8B8);

        //    Igor: for volumetric lights
        //    temp: for higher quality blends
        rt_Generic_2.create(r2_RT_generic2, w, h, D3DFMT_A16B16G16R16F);
    }

    // OCCLUSION
    s_occq.create                    (b_occq,        "r2\\occq");

    // DIRECT (spot)
    D3DFORMAT                        depth_format    = (D3DFORMAT)RImplementation.o.HW_smap_FORMAT;

    if (RImplementation.o.HW_smap)
    {
        D3DFORMAT    nullrt                = D3DFMT_R5G6B5;
        if (RImplementation.o.nullrt)    nullrt    = (D3DFORMAT)MAKEFOURCC('N','U','L','L');

        u32    size                    =RImplementation.o.smapsize    ;
        rt_smap_depth.create        (r2_RT_smap_depth,            size,size,depth_format    );
        rt_smap_surf.create            (r2_RT_smap_surf,            size,size,nullrt        );
        rt_smap_ZB                    = nullptr;
        s_accum_mask.create                (b_accum_mask,                "r2\\accum_mask");
        s_accum_direct_cascade.create    (b_accum_direct_cascade,    "r2\\accum_direct_cascade");
        s_accum_direct_volumetric_cascade.create("accum_volumetric_sun_cascade");
    }
    else
    {
        u32    size                    =RImplementation.o.smapsize    ;
        rt_smap_surf.create            (r2_RT_smap_surf,            size,size,D3DFMT_R32F);
        rt_smap_depth                = nullptr;
        R_CHK                        (RDevice->CreateDepthStencilSurface    (size,size,D3DFMT_D24X8,D3DMULTISAMPLE_NONE,0,TRUE,&rt_smap_ZB,nullptr));
        s_accum_mask.create                (b_accum_mask,                "r2\\accum_mask");
        s_accum_direct_cascade.create    (b_accum_direct_cascade,    "r2\\accum_direct_cascade");
        s_accum_direct_volumetric_cascade.create("accum_volumetric_sun_cascade");
    }

    // POINT
    {
        s_accum_point.create        (b_accum_point,                "r2\\accum_point_s");
        accum_point_geom_create        ();
        g_accum_point.create        (D3DFVF_XYZ,                g_accum_point_vb, g_accum_point_ib);
        accum_omnip_geom_create        ();
        g_accum_omnipart.create        (D3DFVF_XYZ,                g_accum_omnip_vb, g_accum_omnip_ib);
    }

    // SPOT
    {
        s_accum_spot.create            (b_accum_spot,                "r2\\accum_spot_s",    "lights\\lights_spot01");
        accum_spot_geom_create        ();
        g_accum_spot.create            (D3DFVF_XYZ,                g_accum_spot_vb, g_accum_spot_ib);
    }

    {
        s_accum_volume.create("accum_volumetric", "lights\\lights_spot01");
        accum_volumetric_geom_create();
        g_accum_volumetric.create( D3DFVF_XYZ, g_accum_volumetric_vb, g_accum_volumetric_ib);
    }
            

    // REFLECTED
    {
        s_accum_reflected.create    (b_accum_reflected,            "r2\\accum_refl");
    }

    // BLOOM
    {
        D3DFORMAT    fmt                = D3DFMT_A8R8G8B8;            //;        // D3DFMT_X8R8G8B8
        u32    w=BLOOM_size_X, h=BLOOM_size_Y;
        u32 fvf_build                = D3DFVF_XYZRHW|D3DFVF_TEX4|D3DFVF_TEXCOORDSIZE2(0)|D3DFVF_TEXCOORDSIZE2(1)|D3DFVF_TEXCOORDSIZE2(2)|D3DFVF_TEXCOORDSIZE2(3);
        u32 fvf_filter                = (u32)D3DFVF_XYZRHW|D3DFVF_TEX8|D3DFVF_TEXCOORDSIZE4(0)|D3DFVF_TEXCOORDSIZE4(1)|D3DFVF_TEXCOORDSIZE4(2)|D3DFVF_TEXCOORDSIZE4(3)|D3DFVF_TEXCOORDSIZE4(4)|D3DFVF_TEXCOORDSIZE4(5)|D3DFVF_TEXCOORDSIZE4(6)|D3DFVF_TEXCOORDSIZE4(7);
        rt_Bloom_1.create            (r2_RT_bloom1,    w,h,        fmt);
        rt_Bloom_2.create            (r2_RT_bloom2,    w,h,        fmt);
        g_bloom_build.create        (fvf_build,        RCache.Vertex.Buffer(), RCache.QuadIB);
        g_bloom_filter.create        (fvf_filter,    RCache.Vertex.Buffer(), RCache.QuadIB);
        s_bloom_dbg_1.create        ("effects\\screen_set",        r2_RT_bloom1);
        s_bloom_dbg_2.create        ("effects\\screen_set",        r2_RT_bloom2);
        s_bloom.create                (b_bloom,                    "r2\\bloom");
        f_bloom_factor                = 0.5f;
    }

    u32 w = RCache.get_width(), h = RCache.get_height();
    rt_ssao_temp.create(r2_RT_ssao_temp, w, h, D3DFMT_G16R16F);
    s_ssao.create(b_ssao, "r2\\ssao");

    // Screen Post Process
    {
        b_spp = new CBlender_SPP();
        s_spp.create(b_spp);
    }
    
    //FXAA
    s_fxaa.create(b_fxaa, "r2\\fxaa");
    g_fxaa.create(FVF::F_V, RCache.Vertex.Buffer(), RCache.QuadIB);

    // SMAA
    {
        u32 w = Device.TargetWidth, h = Device.TargetHeight;

        b_smaa = new CBlender_SMAA();
        s_smaa.create(b_smaa);

        rt_smaa_edgetex.create(r2_RT_smaa_edgetex, w, h, D3DFMT_A8R8G8B8);
        rt_smaa_blendtex.create(r2_RT_smaa_blendtex, w, h, D3DFMT_A8R8G8B8);
    }
    

    // TONEMAP
    {
        rt_LUM_64.create            (r2_RT_luminance_t64,    64, 64,    D3DFMT_A16B16G16R16F    );
        rt_LUM_8.create                (r2_RT_luminance_t8,    8,    8,    D3DFMT_A16B16G16R16F    );
        s_luminance.create            (b_luminance,                "r2\\luminance");
        f_luminance_adapt            = 0.5f;

        t_LUM_src.create            (r2_RT_luminance_src);
        t_LUM_dest.create            (r2_RT_luminance_cur);

        // create pool
        for (u32 it=0; it<Caps.iGPUNum*2; it++)    {
            string256                    name;
            xr_sprintf                        (name,"%s_%d",    r2_RT_luminance_pool,it    );
            rt_LUM_pool[it].create        (name,    1,    1,    D3DFMT_R32F                );
            u_setrt                        (rt_LUM_pool[it],    0,    0,    0            );
            CHK_DX                        (RDevice->Clear( 0L, nullptr, D3DCLEAR_TARGET,    0x7f7f7f7f,    1.0f, 0L));
        }
        u_setrt                        ( RCache.get_width(),RCache.get_height(),RTarget,nullptr,nullptr,RDepth);
    }

    // COMBINE
    {
        static D3DVERTEXELEMENT9 dwDecl[] =
        {
            { 0, 0,  D3DDECLTYPE_FLOAT4,    D3DDECLMETHOD_DEFAULT,     D3DDECLUSAGE_POSITION,    0 },    // pos+uv
            { 0, 16, D3DDECLTYPE_D3DCOLOR,    D3DDECLMETHOD_DEFAULT,     D3DDECLUSAGE_COLOR,        0 },
            { 0, 20, D3DDECLTYPE_FLOAT2,    D3DDECLMETHOD_DEFAULT,     D3DDECLUSAGE_TEXCOORD,    0 },
            D3DDECL_END()
        };
        s_combine.create                    (b_combine,                    "r2\\combine");
        s_combine_volumetric.create            ("combine_volumetric");
        s_combine_dbg_0.create                ("effects\\screen_set",        r2_RT_smap_surf        );    
        s_combine_dbg_1.create                ("effects\\screen_set",        r2_RT_luminance_t8    );
        s_combine_dbg_Accumulator.create    ("effects\\screen_set",        r2_RT_accum            );
        g_combine_VP.create                    (dwDecl,        RCache.Vertex.Buffer(), RCache.QuadIB);
        g_combine.create                    (FVF::F_TL,        RCache.Vertex.Buffer(), RCache.QuadIB);
        g_combine_2UV.create                (FVF::F_TL2uv,    RCache.Vertex.Buffer(), RCache.QuadIB);
        g_combine_cuboid.create                (FVF::F_L,    RCache.Vertex.Buffer(), RCache.Index.Buffer());

        u32 fvf_aa_blur                = D3DFVF_XYZRHW|D3DFVF_TEX4|D3DFVF_TEXCOORDSIZE2(0)|D3DFVF_TEXCOORDSIZE2(1)|D3DFVF_TEXCOORDSIZE2(2)|D3DFVF_TEXCOORDSIZE2(3);
        g_aa_blur.create            (fvf_aa_blur,    RCache.Vertex.Buffer(), RCache.QuadIB);

        u32 fvf_aa_AA                = D3DFVF_XYZRHW|D3DFVF_TEX7|D3DFVF_TEXCOORDSIZE2(0)|D3DFVF_TEXCOORDSIZE2(1)|D3DFVF_TEXCOORDSIZE2(2)|D3DFVF_TEXCOORDSIZE2(3)|D3DFVF_TEXCOORDSIZE2(4)|D3DFVF_TEXCOORDSIZE4(5)|D3DFVF_TEXCOORDSIZE4(6);
        g_aa_AA.create                (fvf_aa_AA,        RCache.Vertex.Buffer(), RCache.QuadIB);

        t_envmap_0.create            (r2_T_envs0);
        t_envmap_1.create            (r2_T_envs1);
    }

    // Build textures
    {
        // Build material(s)
        {
            // Surface
            R_CHK(RDevice->CreateVolumeTexture(TEX_material_LdotN, TEX_material_LdotH, 4, 1, 0, D3DFMT_A8L8, D3DPOOL_MANAGED, &t_material_surf, nullptr));
            t_material = dxRenderDeviceRender::Instance().Resources->_CreateTexture(r2_material);
            t_material->surface_set(t_material_surf);

            // Fill it (addr: x=dot(L,N),y=dot(L,H))
            D3DLOCKED_BOX R{};
            R_CHK(t_material_surf->LockBox(0, &R, 0, 0));
            for(u32 slice = 0; slice < 4; slice++) {
                for(u32 y = 0; y < TEX_material_LdotH; y++) {
                    for(u32 x = 0; x < TEX_material_LdotN; x++) {
                        u16* p = (u16*)(LPBYTE(R.pBits) + slice * R.SlicePitch + y * R.RowPitch + x * 2);
                        float    ld = float(x) / float(TEX_material_LdotN - 1);
                        float    ls = float(y) / float(TEX_material_LdotH - 1) + EPS_S;
                        ls *= powf(ld, 1 / 32.f);
                        float    fd, fs;

                        switch(slice) {
                            case 0:
                            { // looks like OrenNayar
                                fd = powf(ld, 0.75f);        // 0.75
                                fs = powf(ls, 16.f) * .5f;
                            }    break;
                            case 1:
                            {// looks like Blinn
                                fd = powf(ld, 0.90f);        // 0.90
                                fs = powf(ls, 24.f);
                            }    break;
                            case 2:
                            { // looks like Phong
                                fd = ld;                    // 1.0
                                fs = powf(ls * 1.01f, 128.f);
                            }    break;
                            case 3:
                            { // looks like Metal
                                float    s0 = _abs(1 - _abs(0.05f * _sin(33.f * ld) + ld - ls));
                                float    s1 = _abs(1 - _abs(0.05f * _cos(33.f * ld * ls) + ld - ls));
                                float    s2 = _abs(1 - _abs(ld - ls));
                                fd = ld;                // 1.0
                                fs = powf(_max(_max(s0, s1), s2), 24.f);
                                fs *= powf(ld, 1 / 7.f);
                            }    break;
                            default:
                            fd = fs = 0;
                        }
                        s32        _d = clampr(iFloor(fd * 255.5f), 0, 255);
                        s32        _s = clampr(iFloor(fs * 255.5f), 0, 255);
                        if((y == (TEX_material_LdotH - 1)) && (x == (TEX_material_LdotN - 1))) {
                            _d = 255; _s = 255;
                        }
                        *p = u16(_s * 256 + _d);
                    }
                }
            }
            R_CHK(t_material_surf->UnlockBox(0));
            // #ifdef DEBUG
            // R_CHK    (D3DXSaveTextureToFile    ("x:\\r2_material.dds",D3DXIFF_DDS,t_material_surf,0));
            // #endif
        }

        // Build noise table
        // Surfaces
        D3DLOCKED_RECT R[TEX_jitter_count];
        for(int it1 = 0; it1 < TEX_jitter_count; it1++) {
            string_path                    name;
            xr_sprintf(name, "%s%d", r2_jitter, it1);
            R_CHK(RDevice->CreateTexture(TEX_jitter, TEX_jitter, 1, 0, D3DFMT_Q8W8V8U8, D3DPOOL_MANAGED, &t_noise_surf[it1], nullptr));
            t_noise[it1] = dxRenderDeviceRender::Instance().Resources->_CreateTexture(name);
            t_noise[it1]->surface_set(t_noise_surf[it1]);
            R_CHK(t_noise_surf[it1]->LockRect(0, &R[it1], 0, 0));
        }

        // Fill it,
        for(u32 y = 0; y < TEX_jitter; y++) {
            for(u32 x = 0; x < TEX_jitter; x++) {
                DWORD data[TEX_jitter_count];
                generate_jitter(data, TEX_jitter_count);
                for(u32 it2 = 0; it2 < TEX_jitter_count; it2++) {
                    u32* p = (u32*)(LPBYTE(R[it2].pBits) + y * R[it2].Pitch + x * 4);
                    *p = data[it2];
                }
            }
        }

        for(int it3 = 0; it3 < TEX_jitter_count; it3++) {
            R_CHK(t_noise_surf[it3]->UnlockRect(0));
        }
    }

    // PP
    s_postprocess.create                ("postprocess");
    g_postprocess.create                (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_SPECULAR|D3DFVF_TEX3,RCache.Vertex.Buffer(),RCache.QuadIB);

    // Menu
    s_menu.create                        ("distort");
    g_menu.create                        (FVF::F_TL,RCache.Vertex.Buffer(),RCache.QuadIB);

    //    Igor: TMP
    //    Create an RT for online screenshot makining
    //u32        w = Device.TargetWidth, h = Device.TargetHeight;
    //RDevice->CreateOffscreenPlainSurface(Device.TargetWidth,Device.TargetHeight,D3DFMT_A8R8G8B8,D3DPOOL_SYSTEMMEM,&pFB,nullptr);
    //RDevice->CreateOffscreenPlainSurface(Device.TargetWidth,Device.TargetHeight,rt_Color->fmt,D3DPOOL_SYSTEMMEM,&pFB,nullptr);
    D3DSURFACE_DESC    desc;
    RTarget->GetDesc(&desc);
    RDevice->CreateOffscreenPlainSurface(RCache.get_width(),RCache.get_height(),desc.Format,D3DPOOL_SYSTEMMEM,&pFB,nullptr);

    // 
    dwWidth        = RCache.get_width();
    dwHeight    = RCache.get_height();
}

CRenderTarget::~CRenderTarget    ()
{
    _RELEASE(pFB);

    // Textures
    t_material->surface_set        (nullptr);

#ifdef DEBUG
    _SHOW_REF                    ("t_material_surf",t_material_surf);
#endif // DEBUG
    _RELEASE                    (t_material_surf);

    t_LUM_src->surface_set        (nullptr);
    t_LUM_dest->surface_set        (nullptr);

#ifdef DEBUG
    ID3DBaseTexture*    pSurf = 0;

    pSurf = t_envmap_0->surface_get();
    if (pSurf) pSurf->Release();
    _SHOW_REF("t_envmap_0 - #small",pSurf);

    pSurf = t_envmap_1->surface_get();
    if (pSurf) pSurf->Release();
    _SHOW_REF("t_envmap_1 - #small",pSurf);
    //_SHOW_REF("t_envmap_0 - #small",t_envmap_0->pSurface);
    //_SHOW_REF("t_envmap_1 - #small",t_envmap_1->pSurface);
#endif // DEBUG
    t_envmap_0->surface_set        (nullptr);
    t_envmap_1->surface_set        (nullptr);
    t_envmap_0.destroy            ();
    t_envmap_1.destroy            ();

    _RELEASE                    (rt_smap_ZB);

    // Jitter
    for (int it=0; it<TEX_jitter_count; it++)    {
        t_noise    [it]->surface_set    (nullptr);
#ifdef DEBUG
        _SHOW_REF("t_noise_surf[it]",t_noise_surf[it]);
#endif // DEBUG
        _RELEASE                    (t_noise_surf[it]);
    }

    // 
    accum_spot_geom_destroy        ();
    accum_omnip_geom_destroy    ();
    accum_point_geom_destroy    ();
    accum_volumetric_geom_destroy();

    // Blenders
    xr_delete                    (b_combine                );
    xr_delete                    (b_luminance            );
    xr_delete                    (b_bloom                );
    xr_delete                    (b_ssao                    );
    xr_delete(b_fxaa);
    xr_delete(b_smaa);
    xr_delete(b_spp);
    xr_delete                    (b_accum_reflected        );
    xr_delete                    (b_accum_spot            );
    xr_delete                    (b_accum_point            );
    xr_delete                    (b_accum_direct_cascade    );
    xr_delete                    (b_accum_mask            );
    xr_delete                    (b_occq                    );
}

void CRenderTarget::reset_light_marker( bool bResetStencil)
{
    dwLightMarkerID = 5;
    if (bResetStencil)
    {
        RCache.set_ColorWriteEnable    (FALSE);
        u32        Offset;
        float    _w                    = float(RCache.get_width());
        float    _h                    = float(RCache.get_height());
        u32        C                    = color_rgba    (255,255,255,255);
        float    eps                    = EPS_S;
        FVF::TL* pv                    = (FVF::TL*) RCache.Vertex.Lock    (4,g_combine->vb_stride,Offset);
        pv->set                        (eps,            float(_h+eps),    eps,    1.f, C, 0, 0);    pv++;
        pv->set                        (eps,            eps,            eps,    1.f, C, 0, 0);    pv++;
        pv->set                        (float(_w+eps),    float(_h+eps),    eps,    1.f, C, 0, 0);    pv++;
        pv->set                        (float(_w+eps),    eps,            eps,    1.f, C, 0, 0);    pv++;
        RCache.Vertex.Unlock        (4,g_combine->vb_stride);
        RCache.set_CullMode            (CULL_NONE    );
        //    Clear everything except last bit
        RCache.set_Stencil    (TRUE,D3DCMP_ALWAYS,dwLightMarkerID,0x00,0xFE, D3DSTENCILOP_ZERO, D3DSTENCILOP_ZERO, D3DSTENCILOP_ZERO);
        //RCache.set_Stencil    (TRUE,D3DCMP_ALWAYS,dwLightMarkerID,0x00,0xFF, D3DSTENCILOP_ZERO, D3DSTENCILOP_ZERO, D3DSTENCILOP_ZERO);
        RCache.set_Element            (s_occq->E[1]    );
        RCache.set_Geometry            (g_combine        );
        RCache.Render                (D3DPT_TRIANGLELIST,Offset,0,4,0,2);

/*
        u32        Offset;
        float    _w                    = float(Device.TargetWidth);
        float    _h                    = float(Device.TargetHeight);
        u32        C                    = color_rgba    (255,255,255,255);
        float    eps                    = 0;
        float    _dw                    = 0.5f;
        float    _dh                    = 0.5f;
        FVF::TL* pv                    = (FVF::TL*) RCache.Vertex.Lock    (4,g_combine->vb_stride,Offset);
        pv->set                        (-_dw,        _h-_dh,        eps,    1.f, C, 0, 0);    pv++;
        pv->set                        (-_dw,        -_dh,        eps,    1.f, C, 0, 0);    pv++;
        pv->set                        (_w-_dw,    _h-_dh,        eps,    1.f, C, 0, 0);    pv++;
        pv->set                        (_w-_dw,    -_dh,        eps,    1.f, C, 0, 0);    pv++;
        RCache.Vertex.Unlock        (4,g_combine->vb_stride);
        RCache.set_Element            (s_occq->E[2]    );
        RCache.set_Geometry            (g_combine        );
        RCache.Render                (D3DPT_TRIANGLELIST,Offset,0,4,0,2);
*/
    }
}

void CRenderTarget::increment_light_marker()
{
    dwLightMarkerID += 2;

    //if (dwLightMarkerID>10)
    if (dwLightMarkerID>255)
        reset_light_marker(true);
}

bool CRenderTarget::need_to_render_sunshafts()
{
    if (!ps_r_sun_shafts)
        return false;

    {
        CEnvDescriptor&    E = *g_pGamePersistent->Environment().CurrentEnv;
        Fcolor sun_color = ((light*)RImplementation.Lights.sun_adapted._get())->color;
        float fValue = E.m_fSunShaftsIntensity * u_diffuse2s(sun_color.r, sun_color.g, sun_color.b);
        if (fValue < EPS)
            return false;
    }

    return true;
}
