#include "stdafx.h"
#include "FStaticRender_RenderTarget.h"
#include "../../xrEngine/IGame_Persistent.h"
#include "../xrRender/blender_fxaa.h"

static LPCSTR        RTname            = "$user$rendertarget";
static LPCSTR        RTTempName = "$user$rendertarget_temp";
static LPCSTR        RTname_color_map= "$user$rendertarget_color_map";
static LPCSTR        RTname_distort    = "$user$distort";

CRenderTarget::CRenderTarget()
{
    bAvailable            = FALSE;
    RT                    = 0;
    RT_color_map        = 0;
    pTempZB                = 0;
    ZB                    = 0;
    pFB                    = 0;

    param_blur            = 0.f;
    param_gray            = 0.f;
    param_noise            = 0.f;
    param_duality_h        = 0.f;
    param_duality_v        = 0.f;
    param_noise_fps        = 25.f;
    param_noise_scale    = 1.f;

    param_color_map_influence    =    0.0f;
    param_color_map_interpolate    =    0.0f;

    im_noise_time = 1.0f / 100.0f;
    im_noise_shift_w    = 0;
    im_noise_shift_h    = 0;

    param_color_base    = color_rgba(127,127,127,    0);
    param_color_gray    = color_rgba(85,85,85,        0);
    param_color_add.set( 0.0f, 0.0f, 0.0f );

    bAvailable            = Create    ();
    Msg                    ("* SSample: %s",bAvailable?"enabled":"disabled");
}

BOOL CRenderTarget::Create()
{
    b_fxaa = new CBlender_FXAA();
    curWidth            =  RCache.get_width();
    curHeight            = RCache.get_height();

    // Select mode to operate in
    rtWidth = RCache.get_width();
    rtHeight = RCache.get_height();

    while (rtWidth % 2)
        rtWidth--;

    while (rtHeight % 2)
        rtHeight--;
    Msg("* SSample: %dx%d", rtWidth, rtHeight);

    // Bufferts
    RT.create            (RTname,            rtWidth,rtHeight,D3DFMT_X8R8G8B8);
    RTTemp.create(RTTempName, rtWidth, rtHeight, D3DFMT_X8R8G8B8);

    RT_distort.create    (RTname_distort,    rtWidth,rtHeight,D3DFMT_X8R8G8B8);
    if (RImplementation.o.color_mapping)
    {
        //RT_color_map.create    (RTname_color_map,    rtWidth,rtHeight,D3DFMT_X8R8G8B8);
        RT_color_map.create    (RTname_color_map,    curWidth, curHeight, D3DFMT_X8R8G8B8);
    }
    //RImplementation.o.color_mapping = RT_color_map->valid();

    //FXAA
    s_fxaa.create(b_fxaa, "r1\\fxaa");
    g_fxaa.create(FVF::F_V, RCache.Vertex.Buffer(), RCache.QuadIB);

    if ((rtHeight!=RCache.get_height()) || (rtWidth!= RCache.get_width()))    {
        R_CHK        (RDevice->CreateDepthStencilSurface    (rtWidth,rtHeight,D3DFMT_D24S8,D3DMULTISAMPLE_NONE,0,TRUE,&ZB,nullptr));
    } else {
        ZB            = RDepth;
        ZB->AddRef    ();
    }

    // Temp ZB, used by some of the shadowing code
    R_CHK    (RDevice->CreateDepthStencilSurface    (512,512,D3DFMT_D24S8,D3DMULTISAMPLE_NONE,0,TRUE,&pTempZB,nullptr));

    //    Igor: TMP
    //    Create an RT for online screenshot makining
    //u32        w = Device.TargetWidth, h = Device.TargetHeight;
    //RDevice->CreateOffscreenPlainSurface(Device.TargetWidth,Device.TargetHeight,D3DFMT_A8R8G8B8,D3DPOOL_SYSTEMMEM,&pFB,nullptr);
    RDevice->CreateOffscreenPlainSurface(rtWidth,rtHeight,D3DFMT_X8R8G8B8,D3DPOOL_SYSTEMMEM,&pFB,nullptr);

    // Shaders and stream
    s_postprocess[0].create                ("postprocess");
    if (RImplementation.o.distortion)    s_postprocess_D[0].create("postprocess_d");
    if (RImplementation.o.color_mapping)
    {
        s_postprocess[1].create                ("postprocess_cm");
        if (RImplementation.o.distortion)    s_postprocess_D[1].create("postprocess_dcm");
    }
    g_postprocess.create                (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_SPECULAR|D3DFVF_TEX3,RCache.Vertex.Buffer(),RCache.QuadIB);
    return    RT->valid() && RT_distort->valid();
}

void CRenderTarget::phase_fxaa(u32 pass) {
    u32 Offset = 0;
    float _w = float(curWidth);
    float _h = float(curHeight);
    float ddw = 1.0f / _w;
    float ddh = 1.0f / _h;

    RCache.set_CullMode(CULL_NONE);
    RCache.set_Stencil(FALSE);

    FVF::V* pv = (FVF::V*)RCache.Vertex.Lock(4, g_fxaa->vb_stride, Offset);
    pv->set(ddw - 0.5f, ddh + _h - 0.5f, 0.0f, 0.0f, 1.0f);
    pv++;
    pv->set(ddw - 0.5f, ddh - 0.5f, 0.0f, 0.0f, 0.0f);
    pv++;
    pv->set(ddw + _w - 0.5f, ddh + _h - 0.5f, 0.0f, 1.0f, 1.0f);
    pv++;
    pv->set(ddw + _w - 0.5f, ddh - 0.5f, 0.0f, 1.0f, 0.0f);
    pv++;
    RCache.Vertex.Unlock(4, g_fxaa->vb_stride);
    
    RCache.set_Element(s_fxaa->E[pass]);
    RCache.set_Geometry(g_fxaa);
    RCache.set_c("screen_res", _w, _h, ddw, ddh);
    RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
}



CRenderTarget::~CRenderTarget    ()
{
    xr_delete(b_fxaa);
    _RELEASE                    (pFB);
    _RELEASE                    (pTempZB);
    _RELEASE                    (ZB);
    s_postprocess_D[1].destroy    ();
    s_postprocess[1].destroy    ();
    s_postprocess_D[0].destroy    ();
    s_postprocess[1].destroy    ();
    g_postprocess.destroy        ();
    RT_distort.destroy            ();
    RT_color_map.destroy        ();
    RT.destroy                    ();
    RTTemp.destroy();
}

void    CRenderTarget::calc_tc_noise        (Fvector2& p0, Fvector2& p1)
{
//.    CTexture*    T                    = RCache.get_ActiveTexture    (2);
//.    VERIFY2        (T, "Texture #3 in noise shader should be setted up");
//.    u32            tw                    = iCeil(float(T->get_Width    ())*param_noise_scale+EPS_S);
//.    u32            th                    = iCeil(float(T->get_Height ())*param_noise_scale+EPS_S);
    u32            tw                    = iCeil(256*param_noise_scale+EPS_S);
    u32            th                    = iCeil(256*param_noise_scale+EPS_S);
    VERIFY2        (tw && th, "Noise scale can't be zero in any way");
//.    if (bDebug)    Msg            ("%d,%d,%f",tw,th,param_noise_scale);

    // calculate shift from FPSes
    im_noise_time                    -= Device.fTimeDelta;
    if (im_noise_time<0)            {
        im_noise_shift_w            = ::Random.randI(tw?tw:1);
        im_noise_shift_h            = ::Random.randI(th?th:1);
        float    fps_time            = 1/param_noise_fps;
        while (im_noise_time<0)        im_noise_time += fps_time;
    }

    u32            shift_w                = im_noise_shift_w;
    u32            shift_h                = im_noise_shift_h;
    float        start_u                = (float(shift_w)+.5f)/(tw);
    float        start_v                = (float(shift_h)+.5f)/(th);
    u32            _w = (u32)RCache.get_target_width();
    u32            _h = (u32)RCache.get_target_height();
    u32            cnt_w                = _w / tw;
    u32            cnt_h                = _h / th;
    float        end_u                = start_u + float(cnt_w) + 1;
    float        end_v                = start_v + float(cnt_h) + 1;
 
    p0.set        (start_u,    start_v    );
    p1.set        (end_u,        end_v    );
}

void CRenderTarget::calc_tc_duality_ss    (Fvector2& r0, Fvector2& r1, Fvector2& l0, Fvector2& l1)
{
    // Calculate ordinaty TCs from blur and SS
    float    tw            = float(rtWidth);
    float    th            = float(rtHeight);
    if (rtHeight!=RCache.get_height())    param_blur = 1.f;
    Fvector2            shift,p0,p1;
    shift.set            (.5f/tw, .5f/th);
    shift.mul            (param_blur);
    p0.set                (.5f/tw, .5f/th).add            (shift);
    p1.set                ((tw+.5f)/tw, (th+.5f)/th ).add    (shift);

    // Calculate Duality TC
    float    shift_u        = param_duality_h*.5f;
    float    shift_v        = param_duality_v*.5f;

    r0.set(p0.x,p0.y);                    r1.set(p1.x-shift_u,p1.y-shift_v);
    l0.set(p0.x+shift_u,p0.y+shift_v);    l1.set(p1.x,p1.y);
}

bool CRenderTarget::NeedColorMapping()
{
    return RImplementation.o.color_mapping&&(param_color_map_influence>0.001f);
}

BOOL CRenderTarget::NeedPostProcess()
{
    bool    _blur    = (param_blur>0.001f);
    bool    _gray    = (param_gray>0.001f);
    bool    _noise    = (param_noise>0.001f);
    bool    _dual    = (param_duality_h>0.001f)||(param_duality_v>0.001f);
    
    bool    _menu_pp= g_pGamePersistent?g_pGamePersistent->OnRenderPPUI_query():false;

    bool    _cmap    = NeedColorMapping();

    bool    _cbase    = false;
    {
        int        _r    = color_get_R(param_color_base)    ; _r=_abs(_r-int(0x7f));
        int        _g    = color_get_G(param_color_base)    ; _g=_abs(_g-int(0x7f));
        int        _b    = color_get_B(param_color_base)    ; _b=_abs(_b-int(0x7f));
        if (_r>2 || _g>2 || _b>2)    _cbase    = true    ;
    }
    bool    _cadd    = false;
    {
        //int        _r    = color_get_R(param_color_add)    ;
        //int        _g    = color_get_G(param_color_add)    ;
        //int        _b    = color_get_B(param_color_add)    ;
        //if (_r>2 || _g>2 || _b>2)    _cadd    = true    ;
        int        _r    = _abs((int)(param_color_add.x*255));
        int        _g    = _abs((int)(param_color_add.y*255));
        int        _b    = _abs((int)(param_color_add.z*255));
        if (_r>2 || _g>2 || _b>2)    _cadd    = true    ;
    }
    return _blur || _gray || _noise || _dual || _cbase || _cadd || _cmap || _menu_pp; 
}

BOOL CRenderTarget::Perform        ()
{
    return Available() &&
        (ps_r2_aa_type > 0 || (RImplementation.m_bMakeAsyncSS) || NeedPostProcess() || (frame_distort==(Device.dwFrame-1)));
}

#define SHOW(a)        Log            (#a,a);
#define SHOWX(a)    Msg            ("%s %x",#a,a);
void CRenderTarget::Begin        ()
{
    if (!Perform())    
    {
        // Base RT
        RCache.set_RT            (RTarget);
        RCache.set_ZB            (RDepth);
        curWidth                =  RCache.get_width();
        curHeight                = RCache.get_height();
    } else {
        // Our 
        RCache.set_RT            (RT->pRT);
        RCache.set_ZB            (ZB);
        curWidth                = rtWidth;
        curHeight                = rtHeight;
    }
    Device.Clear                ();
}

struct TL_2c3uv {
    Fvector4    p;
    u32            color0;
    u32            color1;
    Fvector2    uv    [3];
    IC void    set    (float x, float y, u32 c0, u32 c1, float u0, float v0, float u1, float v1, float u2, float v2)    {    
        p.set    (x,y,EPS_S,1.f); 
        color0 = c0; 
        color1 = c1;
        uv[0].set(u0,v0); 
        uv[1].set(u1,v1); 
        uv[2].set(u2,v2); 
    }
};

void CRenderTarget::DoAsyncScreenshot    ()
{
    //    Igor: screenshot will not have postprocess applied.
    //    TODO: fox that later
    if (RImplementation.m_bMakeAsyncSS)
    {
        HRESULT hr;

        IDirect3DSurface9*    pFBSrc = RTarget;
        //    Don't addref, no need to release.
        //ID3DTexture2D *pTex = RT->pSurface;

        //hr = pTex->GetSurfaceLevel(0, &pFBSrc);

        //    SHould be async function
        hr = RDevice->GetRenderTargetData( pFBSrc, pFB );

        //pFBSrc->Release();

        RImplementation.m_bMakeAsyncSS = false;
    }
}

void CRenderTarget::End        ()
{
    if (g_pGamePersistent)    g_pGamePersistent->OnRenderPPUI_main    ()            ;    // PP-UI

    if (ps_r2_aa_type == 1) {
        RCache.set_RT(RTTemp->pRT);
        phase_fxaa(0);
        RCache.set_RT(RT->pRT);
        phase_fxaa(1);
        //RCache.set_Stencil(FALSE);
    }

    // find if distortion is needed at all
    BOOL    bPerform    = Perform                ()    ;
    BOOL    bDistort    = RImplementation.o.distortion;
    BOOL    bCMap        = NeedColorMapping();
    bool    _menu_pp    = g_pGamePersistent?g_pGamePersistent->OnRenderPPUI_query():false;
    if ((0==RImplementation.mapDistort.size()) && !_menu_pp)     bDistort    = FALSE;
    if (bDistort)        phase_distortion        ();

    // combination/postprocess
    RCache.set_RT        (RTarget);
    RCache.set_ZB        (RDepth);
    curWidth            =  RCache.get_width();
    curHeight            = RCache.get_height();
    
    if (!bPerform)        return;

    int        gblend        = clampr        (iFloor((1-param_gray)*255.f),0,255);
    int        nblend        = clampr        (iFloor((1-param_noise)*255.f),0,255);
    u32                    p_color            = subst_alpha        (param_color_base,nblend);
    u32                    p_gray            = subst_alpha        (param_color_gray,gblend);
    Fvector                p_brightness    = param_color_add    ;
    // Msg                ("param_gray:%f(%d),param_noise:%f(%d)",param_gray,gblend,param_noise,nblend);
    // Msg                ("base: %d,%d,%d",    color_get_R(p_color),        color_get_G(p_color),        color_get_B(p_color));
    // Msg                ("gray: %d,%d,%d",    color_get_R(p_gray),        color_get_G(p_gray),        color_get_B(p_gray));
    // Msg                ("add:  %d,%d,%d",    color_get_R(p_brightness),    color_get_G(p_brightness),    color_get_B(p_brightness));
    
    // Draw full-screen quad textured with our scene image
    u32        Offset;
    float    _w            = float( RCache.get_width());
    float    _h            = float(RCache.get_height());
    
    Fvector2            n0,n1,r0,r1,l0,l1;
    calc_tc_duality_ss    (r0,r1,l0,l1);
    calc_tc_noise        (n0,n1);

    // Fill vertex buffer
    float                du = ps_r1_pps_u, dv = ps_r1_pps_v;
    TL_2c3uv* pv        = (TL_2c3uv*) RCache.Vertex.Lock    (4,g_postprocess.stride(),Offset);
    pv->set(du+0,            dv+float(_h),    p_color, p_gray, r0.x, r1.y, l0.x, l1.y, n0.x, n1.y);    pv++;
    pv->set(du+0,            dv+0,            p_color, p_gray, r0.x, r0.y, l0.x, l0.y, n0.x, n0.y);    pv++;
    pv->set(du+float(_w),    dv+float(_h),    p_color, p_gray, r1.x, r1.y, l1.x, l1.y, n1.x, n1.y);    pv++;
    pv->set(du+float(_w),    dv+0,            p_color, p_gray, r1.x, r0.y, l1.x, l0.y, n1.x, n0.y);    pv++;
    RCache.Vertex.Unlock                                    (4,g_postprocess.stride());

    static    shared_str    s_colormap        = "c_colormap";
    if (bCMap)
    {
        RCache.set_RT        (RT_color_map->pRT);

        //    Prepare colormapped buffer
        RCache.set_Element    (bDistort ? s_postprocess_D[1]->E[4] : s_postprocess[1]->E[4]);
        RCache.set_Geometry    (g_postprocess);
        RCache.set_c        (s_colormap, param_color_map_influence,param_color_map_interpolate,0,0);
        RCache.Render        (D3DPT_TRIANGLELIST,Offset,0,4,0,2);

        RCache.set_RT        (RTarget);
        //return;
    }

    //    Element 0 for for normal post-process
    //    Element 4 for color map post-process
    //int iShaderElement    = bCMap ? 4 : 0;
    //RCache.set_Element    (bDistort ? s_postprocess_D->E[iShaderElement] : s_postprocess->E[iShaderElement]);

    RCache.set_Shader    (bDistort ? s_postprocess_D[bCMap] : s_postprocess[bCMap]);

    //RCache.set_Shader    (bDistort ? s_postprocess_D : s_postprocess);


    // Actual rendering
    static    shared_str    s_brightness    = "c_brightness";
    RCache.set_c        (s_brightness,p_brightness.x,p_brightness.y,p_brightness.z,0);
    RCache.set_c        (s_colormap, param_color_map_influence,param_color_map_interpolate,0,0);
    RCache.set_Geometry    (g_postprocess);
    RCache.Render        (D3DPT_TRIANGLELIST,Offset,0,4,0,2);
}

void    CRenderTarget::phase_distortion    ()
{
    frame_distort                                = Device.dwFrame;
    RCache.set_RT                                (RT_distort->pRT);
    RCache.set_ZB                                (ZB);
    RCache.set_CullMode                            (CULL_CCW);
    RCache.set_ColorWriteEnable                    ( );
    CHK_DX(RDevice->Clear                    ( 0L, nullptr, D3DCLEAR_TARGET, color_rgba(127,127,127,127), 1.0f, 0L));
    
    if(g_pGameLevel && g_pGamePersistent && !g_pGamePersistent->OnRenderPPUI_query() )
        RImplementation.r_dsgraph_render_distort    ( );
    else
        RImplementation.mapDistort.clear();

    if (g_pGamePersistent)    g_pGamePersistent->OnRenderPPUI_PP()    ;    // PP-UI
}
