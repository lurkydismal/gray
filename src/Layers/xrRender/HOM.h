// HOM.h: interface for the CHOM class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "../../xrEngine/IGame_Persistent.h"

class occTri;

class CHOM  
#ifdef DEBUG_DRAW
    : public pureRender
#endif
{
private:
    xrXRC                    xrc;
    CDB::MODEL*                m_pModel;
    occTri*                    m_pTris;
    BOOL                    bEnabled;
    Fmatrix                    m_xform;
    Fmatrix                    m_xform_01;
#ifdef DEBUG
    u32                        tris_in_frame_visible    ;
    u32                        tris_in_frame            ;
#endif

    volatile u32            MT_frame_rendered;

    void                    Render_DB    (CFrustum&    base);
public:
    void                    Load        ();
    void                    Unload        ();
    void                    Render        (CFrustum&    base);
    void                    Render_ZB    ();
//    void                    Debug        ();

    void                    occlude        (Fbox2&        space) { }
    void                    Disable        ();
    void                    Enable        ();

    void                    MT_RENDER    ();

    BOOL                    visible        (vis_data&    vis);
    BOOL                    visible        (Fbox3&        B);
    BOOL                    visible        (Fsphere&    S);
    BOOL                    visible        (sPoly&        P);
    BOOL                    visible        (Fbox2&        B, float depth);    // viewport-space (0..1)

    CHOM    ();
    ~CHOM    ();

#ifdef DEBUG_DRAW
    virtual void            OnRender    ();
#endif
#ifdef DEBUG
            void            stats        ();
#endif
};
