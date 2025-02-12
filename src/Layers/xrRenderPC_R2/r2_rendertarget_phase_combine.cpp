#include "stdafx.h"
#include "../../xrEngine/IGame_Persistent.h"
#include "../../xrEngine/Environment.h"

#include "../xrRender/dxEnvironmentRender.h"

#define STENCIL_CULL 0

void CRenderTarget::DoAsyncScreenshot()
{
    if (RImplementation.m_bMakeAsyncSS)
    {
        HRESULT hr;

        IDirect3DSurface9*    pFBSrc = RTarget;

        //    Don't addref, no need to release.
    //    ID3DTexture2D *pTex = rt_Color->pSurface;

    //    hr = pTex->GetSurfaceLevel(0, &pFBSrc);

        //    SHould be async function
        hr = RDevice->GetRenderTargetData( pFBSrc, pFB );

    //    pFBSrc->Release();

        RImplementation.m_bMakeAsyncSS = false;
    }
}


float    hclip(float v, float dim)        { return 2.f*v/dim - 1.f; }
void    CRenderTarget::phase_combine    ()
{
    bool    _menu_pp    = g_pGamePersistent?g_pGamePersistent->OnRenderPPUI_query():false;

    u32            Offset                    = 0;
    Fvector2    p0,p1;

    //*** exposure-pipeline
    u32            gpu_id    = Device.dwFrame%Caps.iGPUNum;
    {
        t_LUM_src->surface_set        (rt_LUM_pool[gpu_id*2+0]->pSurface);
        t_LUM_dest->surface_set        (rt_LUM_pool[gpu_id*2+1]->pSurface);
    }

    RCache.set_CullMode    ( CULL_NONE );

    //if (RImplementation.SSAO.test(ESSAO_DATA::SSAO_OPT_DATA))
    //{
    //    phase_downsamp();
    //}
    //else if (RImplementation.SSAO.test(ESSAO_DATA::SSAO_BLUR))
    //{
    //    phase_ssao();
    //}

    // low/hi RTs
    u_setrt(rt_Generic_0, 0, 0, RDepth);
    RCache.set_Stencil(FALSE);

    BOOL split_the_scene_to_minimize_wait = FALSE;
    if(ps_r2_ls_flags.test(R2FLAG_EXP_SPLIT_SCENE))    split_the_scene_to_minimize_wait = TRUE;

    // draw skybox
    if (1)
    {
        RCache.set_ColorWriteEnable                    ();
        CHK_DX(RDevice->SetRenderState            ( D3DRS_ZENABLE,    FALSE                ));
        g_pGamePersistent->Environment().RenderSky    ();
        //    Igor: Render clouds before compine without Z-test
        //    to avoid siluets. HOwever, it's a bit slower process.
        g_pGamePersistent->Environment().RenderClouds    ();
        CHK_DX(RDevice->SetRenderState            ( D3DRS_ZENABLE,    TRUE                ));
    }

    RCache.set_Stencil(TRUE, D3DCMP_LESSEQUAL, 0x01, 0xff, 0x00);    // stencil should be >= 1
    if (RImplementation.o.nvstencil) {
        u_stencil_optimize(FALSE);
        RCache.set_ColorWriteEnable();
    }

    // Draw full-screen quad textured with our scene image
    if (!_menu_pp)
    {
        // Compute params
        Fmatrix        m_v2w;            m_v2w.invert                (Device.mView        );
        CEnvDescriptorMixer& envdesc= *g_pGamePersistent->Environment().CurrentEnv        ;
        const float minamb            = 0.001f;
        Fvector4    ambclr            = { _max(envdesc.ambient.x*2,minamb),    _max(envdesc.ambient.y*2,minamb),            _max(envdesc.ambient.z*2,minamb),    0    };
                    ambclr.mul        (ps_r2_sun_lumscale_amb);

//.        Fvector4    envclr            = { envdesc.sky_color.x*2+EPS,    envdesc.sky_color.y*2+EPS,    envdesc.sky_color.z*2+EPS,    envdesc.weight                    };
        Fvector4    envclr            = { envdesc.hemi_color.x*2+EPS,    envdesc.hemi_color.y*2+EPS,    envdesc.hemi_color.z*2+EPS,    envdesc.weight                    };

        Fvector4    fogclr            = { envdesc.fog_color.x,    envdesc.fog_color.y,    envdesc.fog_color.z,        0    };
                    envclr.x        *= 2*ps_r2_sun_lumscale_hemi; 
                    envclr.y        *= 2*ps_r2_sun_lumscale_hemi; 
                    envclr.z        *= 2*ps_r2_sun_lumscale_hemi;
        Fvector4    sunclr,sundir;

        float        fSSAONoise = 2.0f;
        fSSAONoise *= tan(deg2rad(67.5f/2.0f));
        fSSAONoise /= tan(deg2rad(Device.fFOV/2.0f));

        float        fSSAOKernelSize = 150.0f;
        fSSAOKernelSize *= tan(deg2rad(67.5f/2.0f));
        fSSAOKernelSize /= tan(deg2rad(Device.fFOV/2.0f));

        // sun-params
        {
            light*        fuckingsun        = (light*)RImplementation.Lights.sun_adapted._get()    ;
            Fvector        L_dir,L_clr;    float L_spec;
            L_clr.set                    (fuckingsun->color.r,fuckingsun->color.g,fuckingsun->color.b);
            L_spec                        = u_diffuse2s    (L_clr);
            Device.mView.transform_dir    (L_dir,fuckingsun->direction);
            L_dir.normalize                ();

            sunclr.set                (L_clr.x,L_clr.y,L_clr.z,L_spec);
            sundir.set                (L_dir.x,L_dir.y,L_dir.z,0);
        }

        // Fill VB
        float    _w                    = float(RCache.get_width());
        float    _h                    = float(RCache.get_height());
        p0.set                        (.5f/_w, .5f/_h);
        p1.set                        ((_w+.5f)/_w, (_h+.5f)/_h );

        // Fill vertex buffer
        //Fvector4* pv                = (Fvector4*)    RCache.Vertex.Lock    (4,g_combine_VP->vb_stride,Offset);
        //pv->set                        (hclip(EPS,        _w),    hclip(_h+EPS,    _h),    p0.x, p1.y);    pv++;
        //pv->set                        (hclip(EPS,        _w),    hclip(EPS,        _h),    p0.x, p0.y);    pv++;
        //pv->set                        (hclip(_w+EPS,    _w),    hclip(_h+EPS,    _h),    p1.x, p1.y);    pv++;
        //pv->set                        (hclip(_w+EPS,    _w),    hclip(EPS,        _h),    p1.x, p0.y);    pv++;
        //RCache.Vertex.Unlock        (4,g_combine_VP->vb_stride);

        // Fill VB
        float    scale_X                = float(RCache.get_width())    / float(TEX_jitter);
        float    scale_Y                = float(RCache.get_height())/ float(TEX_jitter);

        FVF::TL* pv                    = (FVF::TL*)    RCache.Vertex.Lock    (4,g_combine_VP->vb_stride,Offset);
        pv->set                        (hclip(EPS,        _w),    hclip(_h+EPS,    _h),    p0.x, p1.y, 0, 0,            scale_Y    );    pv++;
        pv->set                        (hclip(EPS,        _w),    hclip(EPS,        _h),    p0.x, p0.y, 0, 0,            0        );    pv++;
        pv->set                        (hclip(_w+EPS,    _w),    hclip(_h+EPS,    _h),    p1.x, p1.y, 0, scale_X,    scale_Y    );    pv++;
        pv->set                        (hclip(_w+EPS,    _w),    hclip(EPS,        _h),    p1.x, p0.y, 0, scale_X,    0        );    pv++;
        RCache.Vertex.Unlock        (4,g_combine_VP->vb_stride);

        dxEnvDescriptorMixerRender &envdescren = *(dxEnvDescriptorMixerRender*)(&*envdesc.m_pDescriptorMixer);

        // Setup textures
        IDirect3DBaseTexture9*    e0    = _menu_pp?0:envdescren.sky_r_textures_env[0].second->surface_get();
        IDirect3DBaseTexture9*    e1    = _menu_pp?0:envdescren.sky_r_textures_env[1].second->surface_get();
        t_envmap_0->surface_set        (e0);    _RELEASE(e0);
        t_envmap_1->surface_set        (e1);    _RELEASE(e1);
    
        // Draw
        RCache.set_Element            (s_combine->E[0]    );
        RCache.set_Geometry            (g_combine_VP        );
        //RCache.set_Geometry            (g_combine        );

        RCache.set_c                ("m_v2w",            m_v2w    );
        RCache.set_c                ("L_ambient",        ambclr    );

        RCache.set_c                ("Ldynamic_color",    sunclr    );
        RCache.set_c                ("Ldynamic_dir",    sundir    );

        RCache.set_c                ("env_color",        envclr    );
        RCache.set_c                ("fog_color",        fogclr    );
        RCache.set_c                ("ssao_params",        fSSAONoise, fSSAOKernelSize, 0.0f, 0.0f);
        RCache.Render                (D3DPT_TRIANGLELIST,Offset,0,4,0,2);
    }

    // Forward rendering
    {
        u_setrt                            (rt_Generic_0,0,0,RDepth);        // LDR RT
        RCache.set_CullMode                (CULL_CCW);
        RCache.set_Stencil                (FALSE);
        RCache.set_ColorWriteEnable        ();
        //g_pGamePersistent->Environment().RenderClouds    ();
        RImplementation.render_forward    ();
        if (g_pGamePersistent)    g_pGamePersistent->OnRenderPPUI_main()    ;    // PP-UI
    }

    //    Igor: for volumetric lights
    //    combine light volume here
    if (m_bHasActiveVolumetric)
        phase_combine_volumetric();

    // Perform blooming filter and distortion if needed
    RCache.set_Stencil    (FALSE);
    phase_bloom            ( );                                                // HDR RT invalidated here

    if (ps_r2_aa_type == 1) {
        u_setrt(rt_Accumulator, 0, 0, 0);
        phase_fxaa(0);
        u_setrt(rt_Generic_0, 0, 0, 0);
        phase_fxaa(1);
        RCache.set_Stencil(FALSE);
    }
    else if (ps_r2_aa_type == 2)
    {
        phase_smaa();
        RCache.set_Stencil(FALSE);
    }
    
    // Distortion filter
    BOOL    bDistort    = RImplementation.o.distortion_enabled;                // This can be modified
    {
        if        ((0==RImplementation.mapDistort.size()) && !_menu_pp)        bDistort= FALSE;
        if (bDistort)        {
            u_setrt                        (rt_Generic_1,0,0,RDepth);        // Now RT is a distortion mask
            RCache.set_CullMode            (CULL_CCW);
            RCache.set_Stencil            (FALSE);
            RCache.set_ColorWriteEnable    ();
            CHK_DX(RDevice->Clear    ( 0L, nullptr, D3DCLEAR_TARGET, color_rgba(127,127,0,127), 1.0f, 0L));
            RImplementation.r_dsgraph_render_distort    ();
            if (g_pGamePersistent)    g_pGamePersistent->OnRenderPPUI_PP()    ;    // PP-UI
        }
    }

    // PP enabled ?
    //    Render to RT texture to be able to copy RT even in windowed mode.
    u_setrt(rt_Color, 0, 0, RDepth);

    RCache.set_CullMode(CULL_NONE);
    RCache.set_Stencil(FALSE);

    if (1)    
    {
        // 
        struct v_aa    {
            Fvector4    p;
            Fvector2    uv0;
            Fvector2    uv1;
            Fvector2    uv2;
            Fvector2    uv3;
            Fvector2    uv4;
            Fvector4    uv5;
            Fvector4    uv6;
        };

        float    _w                    = float(Device.TargetWidth);
        float    _h                    = float(Device.TargetHeight);
        float    ddw                    = 1.f/_w;
        float    ddh                    = 1.f/_h;
        p0.set                        (.5f/_w, .5f/_h);
        p1.set                        ((_w+.5f)/_w, (_h+.5f)/_h );

        // Fill vertex buffer
        v_aa* pv                    = (v_aa*) RCache.Vertex.Lock    (4,g_aa_AA->vb_stride,Offset);
        pv->p.set(EPS,            float(_h+EPS),    EPS,1.f); pv->uv0.set(p0.x, p1.y);pv->uv1.set(p0.x-ddw,p1.y-ddh);pv->uv2.set(p0.x+ddw,p1.y+ddh);pv->uv3.set(p0.x+ddw,p1.y-ddh);pv->uv4.set(p0.x-ddw,p1.y+ddh);pv->uv5.set(p0.x-ddw,p1.y,p1.y,p0.x+ddw);pv->uv6.set(p0.x,p1.y-ddh,p1.y+ddh,p0.x);pv++;
        pv->p.set(EPS,            EPS,            EPS,1.f); pv->uv0.set(p0.x, p0.y);pv->uv1.set(p0.x-ddw,p0.y-ddh);pv->uv2.set(p0.x+ddw,p0.y+ddh);pv->uv3.set(p0.x+ddw,p0.y-ddh);pv->uv4.set(p0.x-ddw,p0.y+ddh);pv->uv5.set(p0.x-ddw,p0.y,p0.y,p0.x+ddw);pv->uv6.set(p0.x,p0.y-ddh,p0.y+ddh,p0.x);pv++;
        pv->p.set(float(_w+EPS),float(_h+EPS),    EPS,1.f); pv->uv0.set(p1.x, p1.y);pv->uv1.set(p1.x-ddw,p1.y-ddh);pv->uv2.set(p1.x+ddw,p1.y+ddh);pv->uv3.set(p1.x+ddw,p1.y-ddh);pv->uv4.set(p1.x-ddw,p1.y+ddh);pv->uv5.set(p1.x-ddw,p1.y,p1.y,p1.x+ddw);pv->uv6.set(p1.x,p1.y-ddh,p1.y+ddh,p1.x);pv++;
        pv->p.set(float(_w+EPS),EPS,            EPS,1.f); pv->uv0.set(p1.x, p0.y);pv->uv1.set(p1.x-ddw,p0.y-ddh);pv->uv2.set(p1.x+ddw,p0.y+ddh);pv->uv3.set(p1.x+ddw,p0.y-ddh);pv->uv4.set(p1.x-ddw,p0.y+ddh);pv->uv5.set(p1.x-ddw,p0.y,p0.y,p1.x+ddw);pv->uv6.set(p1.x,p0.y-ddh,p0.y+ddh,p1.x);pv++;
        RCache.Vertex.Unlock        (4,g_aa_AA->vb_stride);

        //    Set up variable
        Fvector2    vDofKernel;
        vDofKernel.set(0.5f / Device.TargetWidth, 0.5f / Device.TargetHeight);
        vDofKernel.mul(ps_r2_dof_kernel_size);

        // Draw COLOR
        RCache.set_Element(s_combine->E[bDistort ? 1 : 2]);    // look at blender_combine.cpp
        Fvector3                    dof;
        g_pGamePersistent->GetCurrentDof(dof);
        RCache.set_c                ("dof_params",    dof.x, dof.y, dof.z, ps_r2_dof_sky);
//.        RCache.set_c                ("dof_params",    ps_r2_dof.x, ps_r2_dof.y, ps_r2_dof.z, ps_r2_dof_sky);
        RCache.set_c                ("dof_kernel",    vDofKernel.x, vDofKernel.y, ps_r2_dof_kernel_size, 0);
        
        RCache.set_Geometry            (g_aa_AA);
        RCache.Render                (D3DPT_TRIANGLELIST,Offset,0,4,0,2);
    }

    RCache.set_Stencil (FALSE);
    g_pGamePersistent->Environment().RenderFlares();    // lens-flares

    //    Igor: screenshot will not have postprocess applied.
    //    TODO: fox that later
    if (ps_r2_ls_flags_ext.test(R2FLAG_SPP_SATURATION))
    {
        PhaseSaturation();
        RCache.set_Stencil(FALSE);
    }
    if (ps_r2_ls_flags_ext.test(R2FLAG_SPP_VIGNETTE))
    {
        PhaseVignette();
        RCache.set_Stencil(FALSE);
    }
    if (ps_r2_ls_flags_ext.test(R2FLAG_SPP_ABERRATION))
    {
        PhaseAberration();
        RCache.set_Stencil(FALSE);
    }

    //    PP-if required
    phase_pp();

    //    Re-adapt luminance
    RCache.set_Stencil        (FALSE);


    //*** exposure-pipeline-clear
    {
        std::swap                    (rt_LUM_pool[gpu_id*2+0],rt_LUM_pool[gpu_id*2+1]);
        t_LUM_src->surface_set        (nullptr);
        t_LUM_dest->surface_set        (nullptr);
    }

#ifdef DEBUG
    RCache.set_CullMode    ( CULL_CCW );
    static    xr_vector<Fplane>        saved_dbg_planes;
    if (bDebug)        
        saved_dbg_planes= dbg_planes;
    else            
        dbg_planes        = saved_dbg_planes;

    if (1) for (u32 it=0; it<dbg_planes.size(); it++)
    {
        Fplane&        P    =    dbg_planes[it];
        Fvector        zero    ;
        zero.mul    (P.n,P.d);
        
        Fvector             L_dir,L_up=P.n,L_right;
        L_dir.set           (0,0,1);                if (_abs(L_up.dotproduct(L_dir))>.99f)  L_dir.set(1,0,0);
        L_right.crossproduct(L_up,L_dir);           L_right.normalize       ();
        L_dir.crossproduct  (L_right,L_up);         L_dir.normalize         ();

        Fvector                p0_,p1_,p2,p3;
        float                sz    = 100.f;
        p0_.mad                (zero,L_right,sz).mad    (L_dir,sz);
        p1_.mad                (zero,L_right,sz).mad    (L_dir,-sz);
        p2.mad                (zero,L_right,-sz).mad    (L_dir,-sz);
        p3.mad                (zero,L_right,-sz).mad    (L_dir,+sz);
        RCache.dbg_DrawTRI    (Fidentity,p0_,p1_,p2,0xffffffff);
        RCache.dbg_DrawTRI    (Fidentity,p2,p3,p0_,0xffffffff);
    }

    static    xr_vector<dbg_line_t>    saved_dbg_lines;
    if (bDebug)
        saved_dbg_lines    = dbg_lines;
    else
        dbg_lines        = saved_dbg_lines;

    RDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
    RDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
    RDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    if (1) for (u32 it=0; it<dbg_lines.size(); it++)
    {
        RCache.dbg_DrawLINE        (Fidentity,dbg_lines[it].P0,dbg_lines[it].P1,dbg_lines[it].color);
    }
#endif

#ifdef DEBUG
    dbg_spheres.clear    ();
    dbg_lines.clear        ();
    dbg_planes.clear    ();
#endif
}

void CRenderTarget::phase_wallmarks        ()
{
    // Targets
    RCache.set_RT(nullptr,2);
    RCache.set_RT(nullptr,1);
    u_setrt                                (rt_Color,nullptr,nullptr,RDepth);
    // Stencil    - draw only where stencil >= 0x1
    RCache.set_Stencil                    (TRUE,D3DCMP_LESSEQUAL,0x01,0xff,0x00);
    RCache.set_CullMode                    (CULL_CCW);
    RCache.set_ColorWriteEnable            (D3DCOLORWRITEENABLE_RED|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_BLUE);
}

void CRenderTarget::phase_combine_volumetric()
{
    u32            Offset                    = 0;
    Fvector2    p0,p1;

    u_setrt(rt_Generic_0, 0, 0, RDepth);
    //    Sets limits to both render targets
    RCache.set_ColorWriteEnable(D3DCOLORWRITEENABLE_RED|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_BLUE);
    {
        // Compute params
        Fmatrix        m_v2w;            m_v2w.invert                (Device.mView        );
        CEnvDescriptorMixer& envdesc= *g_pGamePersistent->Environment().CurrentEnv        ;
        const float minamb            = 0.001f;
        Fvector4    ambclr            = { _max(envdesc.ambient.x*2,minamb),    _max(envdesc.ambient.y*2,minamb),            _max(envdesc.ambient.z*2,minamb),    0    };
        ambclr.mul        (ps_r2_sun_lumscale_amb);

//.        Fvector4    envclr            = { envdesc.sky_color.x*2+EPS,    envdesc.sky_color.y*2+EPS,    envdesc.sky_color.z*2+EPS,    envdesc.weight                    };
        Fvector4    envclr            = { envdesc.hemi_color.x*2+EPS,    envdesc.hemi_color.y*2+EPS,    envdesc.hemi_color.z*2+EPS,    envdesc.weight                    };


        Fvector4    fogclr            = { envdesc.fog_color.x,    envdesc.fog_color.y,    envdesc.fog_color.z,        0    };
        envclr.x        *= 2*ps_r2_sun_lumscale_hemi; 
        envclr.y        *= 2*ps_r2_sun_lumscale_hemi; 
        envclr.z        *= 2*ps_r2_sun_lumscale_hemi;
        Fvector4    sunclr,sundir;

        // sun-params
        {
            light*        fuckingsun        = (light*)RImplementation.Lights.sun_adapted._get()    ;
            Fvector        L_dir,L_clr;    float L_spec;
            L_clr.set                    (fuckingsun->color.r,fuckingsun->color.g,fuckingsun->color.b);
            L_spec                        = u_diffuse2s    (L_clr);
            Device.mView.transform_dir    (L_dir,fuckingsun->direction);
            L_dir.normalize                ();

            sunclr.set                (L_clr.x,L_clr.y,L_clr.z,L_spec);
            sundir.set                (L_dir.x,L_dir.y,L_dir.z,0);
        }

        // Fill VB
        float    _w                    = float(RCache.get_width());
        float    _h                    = float(RCache.get_height());
        p0.set                        (.5f/_w, .5f/_h);
        p1.set                        ((_w+.5f)/_w, (_h+.5f)/_h );

        // Fill vertex buffer
        //Fvector4* pv                = (Fvector4*)    RCache.Vertex.Lock    (4,g_combine_VP->vb_stride,Offset);
        //pv->set                        (hclip(EPS,        _w),    hclip(_h+EPS,    _h),    p0.x, p1.y);    pv++;
        //pv->set                        (hclip(EPS,        _w),    hclip(EPS,        _h),    p0.x, p0.y);    pv++;
        //pv->set                        (hclip(_w+EPS,    _w),    hclip(_h+EPS,    _h),    p1.x, p1.y);    pv++;
        //pv->set                        (hclip(_w+EPS,    _w),    hclip(EPS,        _h),    p1.x, p0.y);    pv++;
        //RCache.Vertex.Unlock        (4,g_combine_VP->vb_stride);


        // Fill VB
        float    scale_X                = float(RCache.get_width())    / float(TEX_jitter);
        float    scale_Y                = float(RCache.get_height())/ float(TEX_jitter);

        // Fill vertex buffer
        FVF::TL* pv                    = (FVF::TL*)    RCache.Vertex.Lock    (4,g_combine_VP->vb_stride,Offset);
        pv->set                        (hclip(EPS,        _w),    hclip(_h+EPS,    _h),    p0.x, p1.y, 0, 0,            scale_Y    );    pv++;
        pv->set                        (hclip(EPS,        _w),    hclip(EPS,        _h),    p0.x, p0.y, 0, 0,            0        );    pv++;
        pv->set                        (hclip(_w+EPS,    _w),    hclip(_h+EPS,    _h),    p1.x, p1.y, 0, scale_X,    scale_Y    );    pv++;
        pv->set                        (hclip(_w+EPS,    _w),    hclip(EPS,        _h),    p1.x, p0.y, 0, scale_X,    0        );    pv++;
        RCache.Vertex.Unlock        (4,g_combine_VP->vb_stride);

        // Draw
        RCache.set_Element            (s_combine_volumetric->E[0]    );
        RCache.set_Geometry            (g_combine_VP        );

        RCache.set_c                ("m_v2w",            m_v2w    );
        RCache.set_c                ("L_ambient",        ambclr    );

        RCache.set_c                ("Ldynamic_color",    sunclr    );
        RCache.set_c                ("Ldynamic_dir",    sundir    );

        RCache.set_c                ("env_color",        envclr    );
        RCache.set_c                ("fog_color",        fogclr    );
        RCache.Render                (D3DPT_TRIANGLELIST,Offset,0,4,0,2);
    }
    RCache.set_ColorWriteEnable();
}
