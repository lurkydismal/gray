// LightShadows.cpp: implementation of the CLightShadows class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LightShadows.h"
#include "../xrRender/LightTrack.h"
#include "../../xrEngine/xr_object.h"
#include "../xrRender/FBasicVisual.h"
#include "../../xrEngine/CustomHUD.h"

const    float        S_distance        = 48;
const    float        S_distance2        = S_distance*S_distance;
const    float        S_ideal_size    = 4.f;        // ideal size for the object
const    float        S_fade            = 4.5;
const    float        S_fade2            = S_fade*S_fade;

const    float        S_level            = .05f;        // clip by energy level
const    int            S_size            = 85;
const    int            S_rt_size        = 512;
const    int            batch_size        = 256;
const    float        S_tess            = .5f;
const    int         S_ambient        = 32;
const    int         S_clip            = 256-8;
const    D3DFORMAT    S_rtf            = D3DFMT_A8R8G8B8;
const    float        S_blur_kernel    = 0.75f;

const    u32            cache_old        = 30*1000;    // 30 secs

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLightShadows::CLightShadows()
{
    current    = 0;
    RT        = 0;

    LPCSTR    RTname            = "$user$shadow";
    LPCSTR    RTtemp            = "$user$temp";
    string128 RTname2;        xr_strconcat(RTname2,RTname,",",RTname);
    string128 RTtemp2;        xr_strconcat(RTtemp2,RTtemp,",",RTtemp);

    // 
    RT.create                (RTname,S_rt_size,S_rt_size,S_rtf);
    RT_temp.create            (RTtemp,S_rt_size,S_rt_size,S_rtf);
    sh_World.create            ("effects\\shadow_world",    RTname);
    geom_World.create        (FVF::F_LIT,    RCache.Vertex.Buffer(), nullptr);
    sh_BlurTR.create        ("blur4",        RTtemp2);
    sh_BlurRT.create        ("blur4",        RTname2);
    geom_Blur.create        (FVF::F_TL4uv,    RCache.Vertex.Buffer(), RCache.QuadIB);

    // Debug
    sh_Screen.create        ("effects\\screen_set",RTname);
    geom_Screen.create        (FVF::F_TL,        RCache.Vertex.Buffer(), RCache.QuadIB);
}

CLightShadows::~CLightShadows()
{
    // Debug
    sh_Screen.destroy        ();
    geom_Screen.destroy        ();

    geom_Blur.destroy        ();
    geom_World.destroy        ();

    sh_BlurRT.destroy        ();
    sh_BlurTR.destroy        ();
    sh_World.destroy        ();
    RT_temp.destroy            ();
    RT.destroy                ();

    // casters
    for (u32 it=0; it<casters_pool.size(); it++)
        xr_delete(casters_pool[it]);
    casters_pool.clear        ();

    // cache
    for (u32 it=0; it<cache.size(); it++)
        xr_free    (cache[it].tris);
    cache.clear                ();
}

void CLightShadows::set_object    (IRenderable* O)
{
    if (0==O)    current        = 0;
    else 
    {
        if (!O->renderable_ShadowGenerate()    || RImplementation.val_bHUD || ((CROS_impl*)O->renderable_ROS())->shadow_gen_frame==Device.dwFrame)
        {
            current        = 0;
            return;
        }

        const vis_data    &vis = O->renderable.visual->getVisData();
        Fvector        C;    O->renderable.xform.transform_tiny        (C,vis.sphere.P);
        float        R                = vis.sphere.R;
        float        D                = C.distance_to(Device.vCameraPosition)+R;
                    // D=0 -> P=0; 
                    // R<S_ideal_size -> P=max, R>S_ideal_size -> P=min
        float        _priority        = (D/S_distance)*(S_ideal_size/(R+EPS));
        if (_priority<1.f)        current    = O;
        else                    current = 0;
        
        if (current)
        {
            ((CROS_impl*)O->renderable_ROS())->shadow_gen_frame    =    Device.dwFrame;

            // alloc
            caster*    cs        = nullptr;
            if (casters_pool.empty())    cs    = new caster ();
            else {
                cs    = casters_pool.back    ();
                casters_pool.pop_back    ();
            }
                
            // 
            casters.push_back    (cs);
            cs->O                = current;
            cs->C                = C;
            cs->D                = D;
            cs->nodes.clear        ();
        }
    }
}

void CLightShadows::add_element    (NODE& N)
{
    if (0==current)                                        return;
    VERIFY2    (casters.back()->nodes.size()<24,"Object exceeds limit of 24 renderable parts/materials");
    if (0==N.pVisual->shader->E[SE_R1_LMODELS]._get())    return;
    casters.back()->nodes.push_back        (N);
}

//
void CLightShadows::calculate    ()
{
    if (casters.empty())        return;

    BOOL bRTS                    = FALSE;
    Device.Statistic->RenderDUMP_Scalc.Begin    ();
    RDevice->SetRenderState    (D3DRS_ZENABLE, D3DZB_FALSE);
    
    // iterate on objects
    int    slot_id        = 0;
    int slot_line    = S_rt_size/S_size;
    int slot_max    = slot_line*slot_line;
    const float    eps = 2*EPS_L;
    for (u32 o_it=0; o_it<casters.size(); o_it++)
    {
        caster&    C    = *casters    [o_it];
        if (C.nodes.empty())    continue;
        
        // Select lights and calc importance
        CROS_impl* LT            = (CROS_impl*)C.O->renderable_ROS();
        xr_vector<CROS_impl::Light>& lights = LT->lights;
        
        // iterate on lights
        for (u32 l_it=0; (l_it<lights.size()) && (slot_id<slot_max); l_it++)
        {
            CROS_impl::Light&    L            =    lights[l_it];
            if (L.energy<S_level)            continue;
            
            //Msg    ("~ light: %d",l_it);

            // setup rt+state(s) for first use
            if (!bRTS)    {
                bRTS                        = TRUE;
                RCache.set_RT                (RT_temp->pRT);
                RCache.set_ZB                (RImplementation.Target->pTempZB);
                RDevice->Clear(0, 0, D3DCLEAR_TARGET, color_xrgb(255, 255, 255), 1, 0);
            }

            // calculate light center
            Fvector        Lpos    = L.source->position;
            float        Lrange    = L.source->range;
            //Log    ("* l-pos:",Lpos);
            //Msg    ("* l-range: %f",Lrange);
            if (L.source->flags.type==IRender_Light::DIRECT)
            {
                // Msg        (" -direct- : %f",L.energy);
                Lpos.mul    (L.source->direction,-100);
                Lpos.add    (C.C);
                Lrange        = 120;
            } else {
                VERIFY        (_valid(Lpos));
                VERIFY        (_valid(C.C));
                float        _dist    ;
                while        (true)    {
                    _dist    =    C.C.distance_to    (Lpos);
                    //Msg        ("* o-dist: %f",    _dist);
                    if (_dist>EPS_L)        break;
                    Lpos.y                    +=    .01f;    //. hack to avoid light-in-the-center-of-object
                }
                float        _R        =    C.O->renderable.visual->getVisData().sphere.R+0.1f;
                //Msg    ("* o-r: %f",_R);
                if (_dist<_R)        {
                    Fvector            Ldir;
                    Ldir.sub        (C.C,Lpos);
                    Ldir.normalize    ();
                    Lpos.mad        (Lpos,Ldir,_dist-_R);
                    //Msg    ("* moving lpos");
                }
            }
            
            // calculate projection-matrix
            Fmatrix        mProject,mProjectR;
            float        p_dist    =    C.C.distance_to(Lpos);
            float        p_R        =    C.O->renderable.visual->getVisData().sphere.R;
            float        p_hat    =    p_R/p_dist;
            float        p_asp    =    1.f;
            float        p_near    =    p_dist-p_R-eps;                                    
            //float        p_nearR    =    C.C.distance_to(L.source->position) + p_R*0.85f + eps;
            //            p_nearR =    p_near;
            float        p_far    =    _min(Lrange,_max(p_dist+S_fade,p_dist+p_R));    
            if (p_near<eps)            continue;
            if (p_far<(p_near+eps))    continue;
            //    Igor: make check here instead of assertion in buil_projection_hat
            if (!(_abs(p_far-p_near) > eps)) continue;
            if (p_hat>0.9f)            continue;
            if (p_hat<0.01f)        continue;

            //Msg            ("* near(%f), near-x(%f)",p_near,p_nearR);
            
            mProject.build_projection_HAT    (p_hat,p_asp,p_near,    p_far);
            //    Igor: strange bug with building projection_hat
            //    building projection with the same parameters fails for the 
            //    second time
            //mProjectR.build_projection_HAT    (p_hat,p_asp,p_nearR,    p_far);
            mProjectR = mProject;
            RCache.set_xform_project        (mProject);
            
            // calculate view-matrix
            Fmatrix        mView;
            Fvector        v_D,v_N,v_R;
            v_D.sub                    (C.C,Lpos);
            v_D.normalize            ();
            if(1-_abs(v_D.y)<EPS)    v_N.set(1,0,0);
            else                    v_N.set(0,1,0);
            v_R.crossproduct        (v_N,v_D);
            v_N.crossproduct        (v_D,v_R);
            mView.build_camera        (Lpos,C.C,v_N);
            RCache.set_xform_view    (mView);
            
            // combine and build frustum
            Fmatrix                    mCombine,mCombineR;
            mCombine.mul            (mProject,mView);
            mCombineR.mul            (mProjectR,mView);
            
            // Select slot and set viewport
            int        s_x            =    slot_id%slot_line;
            int        s_y            =    slot_id/slot_line;
            D3DVIEWPORT9 VP        =    {(unsigned long) s_x*S_size, (unsigned long) s_y*S_size,S_size,S_size,0,1 };
            CHK_DX                    (RDevice->SetViewport(&VP));
            
            // Render object-parts
            for (u32 n_it=0; n_it<C.nodes.size(); n_it++)
            {
                NODE& N                    =    C.nodes[n_it];
                dxRender_Visual *V        =    N.pVisual;
                RCache.set_Element        (V->shader->E[SE_R1_LMODELS]);
                RCache.set_xform_world    (N.Matrix);
                V->Render                (-1.0f);
            }
            
            // register shadow and increment slot
            shadows.push_back        (shadow());
            shadows.back().O        =    C.O;
            shadows.back().slot        =    slot_id;
            shadows.back().C        =    C.C;
            shadows.back().M        =    mCombineR;
            shadows.back().L        =    L.source;
            shadows.back().E        =    L.energy;
#ifdef DEBUG
            shadows.back().dbg_HAT    =    p_hat;
#endif
            slot_id    ++;
        }
    }

    // clear casters
    for (u32 cs=0; cs<casters.size(); cs++)
        casters_pool.push_back(casters[cs]);
    casters.clear    ();
    
    // Blur
    if (bRTS)
    {
        // Fill VB
        u32                            Offset;
        FVF::TL4uv* pv                = (FVF::TL4uv*) RCache.Vertex.Lock    (4,geom_Blur.stride(),Offset);
        RImplementation.ApplyBlur4    (pv,S_rt_size,S_rt_size,S_blur_kernel);
        RCache.Vertex.Unlock        (4,geom_Blur.stride());
        
        // Actual rendering (pass0, temp2real)
        RCache.set_RT                (RT->pRT    );
        RCache.set_ZB                (RImplementation.Target->pTempZB    );
        RCache.set_Shader            (sh_BlurTR    );
        RCache.set_Geometry            (geom_Blur    );
        RCache.Render                (D3DPT_TRIANGLELIST,Offset,0,4,0,2);
    }
    
    // Finita la comedia
    RDevice->SetRenderState                (D3DRS_ZENABLE, D3DZB_TRUE);
    Device.Statistic->RenderDUMP_Scalc.End    ();
    
    RCache.set_xform_project    (Device.mProject);
    RCache.set_xform_view        (Device.mView);
}

#define CLS(a)    color_rgba    (a,a,a,a)

IC    bool        cache_search(const CLightShadows::cache_item& A, const CLightShadows::cache_item& B)
{
    if (A.O < B.O)    return true;
    if (A.O > B.O)    return false;
    if (A.L < B.L)    return true;
    if (A.L > B.L)    return false;
    return            false;    // eq
}

IC float PLC_energy    (Fvector& P, Fvector& N, light* L, float E)
{
    Fvector Ldir;
    if (L->flags.type==IRender_Light::DIRECT)
    {
        // Cos
        Ldir.invert    (L->direction);
        float D        = Ldir.dotproduct( N );
        if( D <=0 )                        return 0;
        return E;
    } else {
        // Distance
        float sqD    = P.distance_to_sqr(L->position);
        if (sqD > (L->range*L->range))    return 0;
        
        // Dir
        Ldir.sub    (L->position,P);
        Ldir.normalize_safe();
        float D        = Ldir.dotproduct( N );
        if( D <=0 )                        return 0;

        // Trace Light
        float R        = _sqrt        (sqD);
        float att    = 1-(1/(1+R));
        return (E * att);
    }
}

IC int PLC_calc    (Fvector& P, Fvector& N, light* L, float energy, Fvector& O)
{
    float    E        = PLC_energy(P,N,L,energy);
    float    C1        = clampr(Device.vCameraPosition.distance_to_sqr(P)/S_distance2,    0.f,1.f);
    float    C2        = clampr(O.distance_to_sqr(P)/S_fade2,                            0.f,1.f);
    float    A        = 1.f-1.5f*E*(1.f-C1)*(1.f-C2);
    return            iCeil(255.f*A);
}

__forceinline float PLC_energy_SSE(Fvector& P, Fvector& N, light* L, float E)
{
    Fvector Ldir;
    if (L->flags.type==IRender_Light::DIRECT)
    {
        // Cos
        Ldir.invert    (L->direction);
        float D        = Ldir.dotproduct( N );
        if( D <=0 )                        return 0;
        return E;
    } else {
        // Distance
        float sqD    = P.distance_to_sqr(L->position);
        if (sqD > (L->range*L->range))    return 0;
        
        // Dir
        Ldir.sub    (L->position,P);
        Ldir.normalize_safe();
        float D        = Ldir.dotproduct( N );
        if( D <=0 )                        return 0;

        // Trace Light
        float att;
        __m128 rcpr = _mm_rsqrt_ss( _mm_load_ss( &sqD ) );
        rcpr = _mm_rcp_ss( _mm_add_ss( rcpr , _mm_set_ss( 1.0f ) ) );
        _mm_store_ss( &att , rcpr );

        return (E * att);
    }
}

__forceinline int iCeil_SSE( float const x ) 
{
    return _mm_cvt_ss2si( _mm_set_ss( x ) );
}


void  PLC_calc3_SSE(int& c0, int& c1, int& c2, CRenderDevice& Device_, Fvector* P, Fvector& N, light* L, float energy, Fvector& O)
{
    float    E        = PLC_energy_SSE(P[0],N,L,energy);
    float    C1        = clampr(Device_.vCameraPosition.distance_to_sqr(P[0])/S_distance2,    0.f,1.f);
    float    C2        = clampr(O.distance_to_sqr(P[0])/S_fade2,                            0.f,1.f);
    float    A        = 1.f-1.5f*E*(1.f-C1)*(1.f-C2);
    c0 = iCeil_SSE(255.f*A);
    E        = PLC_energy_SSE(P[1],N,L,energy);
    C1        = clampr(Device_.vCameraPosition.distance_to_sqr(P[1])/S_distance2,    0.f,1.f);
    C2        = clampr(O.distance_to_sqr(P[1])/S_fade2,                            0.f,1.f);
    A        = 1.f-1.5f*E*(1.f-C1)*(1.f-C2);
    c1 = iCeil_SSE(255.f*A);
    E        = PLC_energy_SSE(P[2],N,L,energy);
    C1        = clampr(Device_.vCameraPosition.distance_to_sqr(P[2])/S_distance2,    0.f,1.f);
    C2        = clampr(O.distance_to_sqr(P[2])/S_fade2,                            0.f,1.f);
    A        = 1.f-1.5f*E*(1.f-C1)*(1.f-C2);
    c2 = iCeil_SSE(255.f*A);
}

void CLightShadows::render    ()
{
    // Gain access to collision-DB
    CDB::MODEL*        DB        = g_pGameLevel->ObjectSpace.GetStaticModel();
    CDB::TRI*        TRIS    = DB->get_tris();
    Fvector*        VERTS    = DB->get_verts();

    int            slot_line    = S_rt_size/S_size;
    
    // Projection and xform
    float _43                    =    Device.mProject._43;

    //    Handle biasing problem when near changes
    const float fMinNear = 0.1f;
    const float fMaxNear = 0.2f;
    const float fMinNearBias = 0.0002f;
    const float fMaxNearBias = 0.002f;
    float    fLerpCoeff    = (_43 - fMinNear) / (fMaxNear - fMinNear);
    clamp( fLerpCoeff, 0.0f, 1.0f );
    //    lerp
    Device.mProject._43            -=    fMinNearBias + (fMaxNearBias-fMinNearBias) * fLerpCoeff;
    //Device.mProject._43            -=    0.0002f; 
    Device.mProject._43            -=    0.002f; 
    //Device.mProject._43            -=    0.0008f; 
    RCache.set_xform_world        (Fidentity);
    RCache.set_xform_project    (Device.mProject);
    Fvector    View                = Device.vCameraPosition;
    
    // Render shadows
    RCache.set_Shader            (sh_World);
    RCache.set_Geometry            (geom_World);
    int batch                    = 0;
    u32 Offset                    = 0;
    FVF::LIT* pv                = (FVF::LIT*) RCache.Vertex.Lock    (batch_size*3,geom_World->vb_stride,Offset);
    for (u32 s_it=0; s_it<shadows.size(); s_it++)
    {
        Device.Statistic->RenderDUMP_Srender.Begin    ();
        shadow&        S            =    shadows[s_it];
        float        Le            =    S.L->color.intensity()*S.E;
        int            s_x            =    S.slot%slot_line;
        int            s_y            =    S.slot/slot_line;
        Fvector2    t_scale, t_offset;
        t_scale.set    (float(S_size)/float(S_rt_size),float(S_size)/float(S_rt_size));
        t_scale.mul (.5f);
        t_offset.set(float(s_x)/float(slot_line),float(s_y)/float(slot_line));
        t_offset.x    += .5f/S_rt_size;
        t_offset.y    += .5f/S_rt_size;
        
        // Search the cache
        cache_item*                        CI        = 0; BOOL    bValid = FALSE;
        cache_item                        CI_what; CI_what.O    = S.O; CI_what.L = S.L; CI_what.tris=0;
        xr_vector<cache_item>::iterator    CI_ptr    = std::lower_bound(cache.begin(),cache.end(),CI_what,cache_search);
        if (CI_ptr==cache.end())        
        {    // empty ?
            CI_ptr    = cache.insert        (CI_ptr,CI_what);
            CI        = &*CI_ptr;
            bValid    = FALSE;
        } else {
            if (CI_ptr->O != CI_what.O  || CI_ptr->L != CI_what.L)    
            {    // we found something different
                CI_ptr    = cache.insert        (CI_ptr,CI_what);
                CI        = &*CI_ptr;
                bValid    = FALSE;
            } else {
                // Everything, OK. Check if info is still relevant...
                CI        = &*CI_ptr;
                bValid    = TRUE;
                if (!CI->Op.similar(CI->O->renderable.xform.c))    bValid = FALSE;
                else if (!CI->Lp.similar(CI->L->position))        bValid = FALSE;
            }
        }
        CI->time                = Device.dwTimeGlobal;    // acess time

        if (!bValid)            {
            // Frustum
            CFrustum                F;
            F.CreateFromMatrix        (S.M,FRUSTUM_P_ALL);

            // Query
            xrc.frustum_options        (0);
            xrc.frustum_query        (DB,F);
            if (0==xrc.r_count())    continue;

            // Clip polys by frustum
            tess.clear                ();
            for (CDB::RESULT* p = xrc.r_begin(); p!=xrc.r_end(); p++)
            {
                VERIFY((p->id>=0)&&(p->id<DB->get_tris_count()));
                // 
                CDB::TRI&    t        = TRIS[p->id];
                if (t.suppress_shadows) continue;
                sPoly        A,B;
                A.push_back            (VERTS[t.verts[0]]);
                A.push_back            (VERTS[t.verts[1]]);
                A.push_back            (VERTS[t.verts[2]]);

                // Calc plane, throw away degenerate tris and invisible to light polygons
                Fplane                P;    float mag = 0;
                Fvector                t1,t2,n;
                t1.sub                (A[0],A[1]);
                t2.sub                (A[0],A[2]);
                n.crossproduct        (t1,t2);
                mag    = n.square_magnitude();
                if (mag<EPS_S)                        continue;
                n.mul                (1.f/_sqrt(mag));
                P.build_unit_normal    (A[0],n);
                float    DOT_Fade    = P.classify(S.L->position);
                if (DOT_Fade<0)        continue;

                // Clip polygon
                sPoly*        clip    = F.ClipPoly    (A,B);
                if (0==clip)        continue;

                // Triangulate poly 
                for (u32 v=2; v<clip->size(); v++)    {
                    tess.push_back    (tess_tri());
                    tess_tri& T        = tess.back();
                    T.v[0]            = (*clip)[0];
                    T.v[1]            = (*clip)[v-1];
                    T.v[2]            = (*clip)[v];
                    T.N                = P.n;
                }
            }

            // Remember params which builded cache item
            CI->O                    = S.O;
            CI->Op                    = CI->O->renderable.xform.c;
            CI->L                    = S.L;
            CI->Lp                    = CI->L->position;
            CI->tcnt                = (u32)tess.size();
            //Msg                        ("---free--- %x",u32(CI->tris));
            xr_free                    (CI->tris);    VERIFY(0==CI->tris);    
            if (tess.size())        {
                CI->tris            = xr_alloc<tess_tri>(CI->tcnt);
                //Msg                    ("---alloc--- %x",u32(CI->tris));
                CopyMemory        (CI->tris,&*tess.begin(),CI->tcnt * sizeof(tess_tri));
            }
        }

        // Fill VB
        for (u32 tid=0; tid<CI->tcnt; tid++)    {
            tess_tri&    TT        = CI->tris[tid];
            Fvector*     v        = TT.v;
            Fvector        T;
            Fplane        ttp;    ttp.build_unit_normal(v[0],TT.N);

            if (ttp.classify(View)<0)                        continue;
            /*
            int    c0        = PLC_calc(v[0],TT.N,S.L,Le,S.C);
            int    c1        = PLC_calc(v[1],TT.N,S.L,Le,S.C);
            int    c2        = PLC_calc(v[2],TT.N,S.L,Le,S.C);
            */
            int    c0,c1,c2;

            PLC_calc3_SSE(c0,c1,c2,Device,v,TT.N,S.L,Le,S.C);

            if (c0>S_clip && c1>S_clip && c2>S_clip)        continue;    
            clamp        (c0,S_ambient,255);
            clamp        (c1,S_ambient,255);
            clamp        (c2,S_ambient,255);

            S.M.transform(T,v[0]); pv->set(v[0],CLS(c0),(T.x+1)*t_scale.x+t_offset.x,(1-T.y)*t_scale.y+t_offset.y); pv++;
            S.M.transform(T,v[1]); pv->set(v[1],CLS(c1),(T.x+1)*t_scale.x+t_offset.x,(1-T.y)*t_scale.y+t_offset.y); pv++;
            S.M.transform(T,v[2]); pv->set(v[2],CLS(c2),(T.x+1)*t_scale.x+t_offset.x,(1-T.y)*t_scale.y+t_offset.y); pv++;

            batch++;
            if (batch==batch_size)    {
                // Flush
                RCache.Vertex.Unlock    (batch*3,geom_World->vb_stride);
                RCache.Render            (D3DPT_TRIANGLELIST,Offset,batch);

                pv                        = (FVF::LIT*) RCache.Vertex.Lock(batch_size*3,geom_World->vb_stride,Offset);
                batch                    = 0;
            }
        }
        Device.Statistic->RenderDUMP_Srender.End    ();
    }

    // Flush if nessesary
    RCache.Vertex.Unlock    (batch*3,geom_World->vb_stride);
    if (batch)                {
        RCache.Render            (D3DPT_TRIANGLELIST,Offset,batch);
    }
    
    // Clear all shadows and free old entries in the cache
    shadows.clear                ();
    for (int cit=0; cit<int(cache.size()); cit++)    {
        cache_item&        ci        = cache[cit];
        u32                time    = Device.dwTimeGlobal - ci.time;
        if                (time > cache_old)    {
            //Msg            ("---free--- %x",u32(ci.tris));
            xr_free        (ci.tris);    VERIFY(0==ci.tris);
            cache.erase (cache.begin()+cit);
            cit            --;
        }
    }

    // Projection
    Device.mProject._43            = _43;
    RCache.set_xform_project    (Device.mProject);
}
