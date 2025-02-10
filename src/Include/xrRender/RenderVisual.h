#ifndef    RenderVisual_included
#define    RenderVisual_included
#pragma once

class IKinematics;
class IKinematicsAnimated;
class IParticleCustom;
struct vis_data;

class IRenderVisual
{
public:
    IRenderVisual() { _ignore_optimization = false; }
    virtual ~IRenderVisual() {;}

    virtual vis_data&    _BCL    getVisData() = 0;
    virtual u32                    getType() = 0;

    bool _ignore_optimization;


    virtual shared_str    getDebugName() = 0;


    virtual    IKinematics*    _BCL    dcast_PKinematics            ()                { return 0;    }
    virtual    IKinematicsAnimated*    dcast_PKinematicsAnimated    ()                { return 0;    }
    virtual IParticleCustom*        dcast_ParticleCustom        ()                { return 0;    }
};

#endif    //    RenderVisual_included