#ifndef dxRenderFactory_included
#define dxRenderFactory_included
#pragma once

#include "../../Include/xrRender/RenderFactory.h"

#define RENDER_FACTORY_DECLARE(Class) \
    virtual I##Class* Create##Class(); \
    virtual void Destroy##Class(I##Class *pObject);

class dxRenderFactory : public IRenderFactory
{
    RENDER_FACTORY_DECLARE(UISequenceVideoItem)
    RENDER_FACTORY_DECLARE(UIShader)
    RENDER_FACTORY_DECLARE(StatGraphRender)
    RENDER_FACTORY_DECLARE(RenderDeviceRender)
#    ifdef DEBUG
        RENDER_FACTORY_DECLARE(ObjectSpaceRender)
#    endif // DEBUG
    RENDER_FACTORY_DECLARE(WallMarkArray)
    RENDER_FACTORY_DECLARE(StatsRender)

    RENDER_FACTORY_DECLARE(FlareRender)
    RENDER_FACTORY_DECLARE(ThunderboltRender)
    RENDER_FACTORY_DECLARE(ThunderboltDescRender)
    RENDER_FACTORY_DECLARE(RainRender)
    RENDER_FACTORY_DECLARE(LensFlareRender)
    RENDER_FACTORY_DECLARE(EnvironmentRender)
    RENDER_FACTORY_DECLARE(EnvDescriptorMixerRender)
    RENDER_FACTORY_DECLARE(EnvDescriptorRender)

    RENDER_FACTORY_DECLARE(FontRender)
};

extern    dxRenderFactory RenderFactoryImpl;

#endif    //    RenderFactory_included