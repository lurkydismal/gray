#include "stdafx.h"
#pragma hdrstop

#include "blender_light_mask.h"

CBlender_accum_direct_mask::CBlender_accum_direct_mask() {
    description.CLS = 0;
}
CBlender_accum_direct_mask::~CBlender_accum_direct_mask() {}

//    TODO: DX10:    implement CBlender_accum_direct_mask::Compile
void    CBlender_accum_direct_mask::Compile(CBlender_Compile& C) {
    IBlender::Compile(C);

    switch(C.iElement) {
        case SE_MASK_SPOT:        // spot or omni-part
        C.r_Pass("accum_mask", "dumb", false, TRUE, FALSE);
        C.r_ColorWriteEnable(false, false, false, false);
        C.r_End();
        break;
        case SE_MASK_POINT:        // point
        C.r_Pass("accum_mask", "dumb", false, TRUE, FALSE);
        C.r_ColorWriteEnable(false, false, false, false);
        C.r_End();
        break;
        case SE_MASK_DIRECT:    // stencil mask for directional light
        C.r_Pass("stub_notransform_t_scaled", "accum_sun_mask", false, FALSE, FALSE, TRUE, D3DBLEND_ZERO, D3DBLEND_ONE, TRUE, 1);
        C.r_dx10Texture("s_diffuse", r2_RT_albedo);
        C.r_dx10Texture("s_surface", r2_RT_S);
        C.r_dx10Texture("s_normal", r2_RT_N);
        C.r_dx10Texture("s_position", r2_RT_P);
        C.r_dx10Sampler("smp_nofilter");
        C.r_ColorWriteEnable(false, false, false, false);
        C.r_End();
        break;
        case SE_MASK_ACCUM_VOL:    // copy accumulator (temp -> real), volumetric (usually after blend)
        C.r_Pass("accum_volume", "copy_p", false, FALSE, FALSE);
        C.r_dx10Texture("s_generic", r2_RT_accum_temp);
        C.r_dx10Sampler("smp_nofilter");
        C.r_End();
        break;
        case SE_MASK_ACCUM_2D:    // copy accumulator (temp -> real), 2D (usually after sun-blend)
        C.r_Pass("stub_notransform_t_scaled", "copy", false, FALSE, FALSE);
        C.r_dx10Texture("s_generic", r2_RT_accum_temp);
        C.r_dx10Sampler("smp_nofilter");
        C.r_End();
        break;
        case SE_MASK_ALBEDO:    // copy accumulator, 2D (for accum->color, albedo_wo)
        C.r_Pass("stub_notransform_t_scaled", "copy", false, FALSE, FALSE);
        C.r_dx10Texture("s_generic", r2_RT_accum);
        C.r_dx10Sampler("smp_nofilter");
        C.r_End();
        break;
    }
}
