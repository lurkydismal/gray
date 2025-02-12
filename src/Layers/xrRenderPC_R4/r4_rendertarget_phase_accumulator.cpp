#include "stdafx.h"

void    CRenderTarget::phase_accumulator()
{
    // Targets
    if (dwAccumulatorClearMark==Device.dwFrame)    {
        // normal operation - setup
        u_setrt(rt_Accumulator, nullptr, nullptr, RDepth);
    } else {
        // initial setup
        dwAccumulatorClearMark                = Device.dwFrame;

        // clear
        u_setrt(rt_Accumulator, nullptr, nullptr, RDepth);
        //dwLightMarkerID                        = 5;                    // start from 5, increment in 2 units
        reset_light_marker();
        //    Igor: AMD bug workaround. Should be fixed in 8.7 catalyst
//        u32        clr4clear                    = color_rgba(0,0,0,0);    // 0x00
        //CHK_DX    (RDevice->Clear            ( 0L, nullptr, D3DCLEAR_TARGET, clr4clear, 1.0f, 0L));
        FLOAT ColorRGBA[4] = {0.0f, 0.0f, 0.0f, 0.0f};
        RContext->ClearRenderTargetView( rt_Accumulator->pRT, ColorRGBA);

        //    render this after sun to avoid troubles with sun
        /*
        // Render emissive geometry, stencil - write 0x0 at pixel pos
        RCache.set_xform_project                    (Device.mProject); 
        RCache.set_xform_view                        (Device.mView);
        // Stencil - write 0x1 at pixel pos - 
        RCache.set_Stencil                            ( TRUE,D3DCMP_ALWAYS,0x01,0xff,0xff,D3DSTENCILOP_KEEP,D3DSTENCILOP_REPLACE,D3DSTENCILOP_KEEP);
        //RCache.set_Stencil                        (TRUE,D3DCMP_ALWAYS,0x00,0xff,0xff,D3DSTENCILOP_KEEP,D3DSTENCILOP_REPLACE,D3DSTENCILOP_KEEP);
        RCache.set_CullMode                            (CULL_CCW);
        RCache.set_ColorWriteEnable                    ();
        RImplementation.r_dsgraph_render_emissive    ();
        */
        // Stencil    - draw only where stencil >= 0x1
        RCache.set_Stencil                    (TRUE,D3DCMP_LESSEQUAL,0x01,0xff,0x00);
        RCache.set_CullMode                    (CULL_NONE);
        RCache.set_ColorWriteEnable            ();
        
    }

    //    Restore viewport after shadow map rendering
    RImplementation.rmNormal();
}

void    CRenderTarget::phase_vol_accumulator()
{
    if (!m_bHasActiveVolumetric)
    {
        m_bHasActiveVolumetric = true;
        u_setrt(rt_Generic_2, nullptr, nullptr, RDepth);
        //u32        clr4clearVol                = color_rgba(0,0,0,0);    // 0x00
        //CHK_DX    (RDevice->Clear            ( 0L, nullptr, D3DCLEAR_TARGET, clr4clearVol, 1.0f, 0L));
        FLOAT ColorRGBA[4] = {0.0f, 0.0f, 0.0f, 0.0f};
        RContext->ClearRenderTargetView( rt_Generic_2->pRT, ColorRGBA);
    }
    else
    {
        u_setrt(rt_Generic_2, nullptr, nullptr, RDepth);
    }

    RCache.set_Stencil                            (FALSE);
    RCache.set_CullMode                            (CULL_NONE);
    RCache.set_ColorWriteEnable                    ();
}