#ifndef IRENDERABLE_H_INCLUDED
#define IRENDERABLE_H_INCLUDED

#include "Render.h"

//////////////////////////////////////////////////////////////////////////
// definition ("Renderable")
class    ENGINE_API    IRenderable            {
public:
    struct 
    {
        Fmatrix                            xform                        ;
        IRenderVisual*                    visual                        ;
        IRender_ObjectSpecific*            pROS                        ;
        BOOL                            pROS_Allowed                ;
    }    renderable;
public:
                                        IRenderable                    ();
    virtual                                ~IRenderable                ();
    IRender_ObjectSpecific*                renderable_ROS                ()    ;

    virtual    void                        renderable_Render            ()    = 0;
    virtual    BOOL                        renderable_ShadowGenerate    ()    { return FALSE; };
    virtual    BOOL                        renderable_ShadowReceive    ()    { return FALSE; };
};

#endif // IRENDERABLE_H_INCLUDED