#include "stdafx.h"

void    CRenderTarget::phase_occq    ()
{
    u_setrt                        ( RCache.get_width(),RCache.get_height(),RTarget,nullptr,nullptr,RDepth);
    RCache.set_CullMode            ( CULL_CCW    );
    RCache.set_Stencil            (TRUE,D3DCMP_LESSEQUAL,0x01,0xff,0x00);
    RCache.set_ColorWriteEnable    (FALSE        );
    RCache.set_Shader            ( s_occq    );
}
