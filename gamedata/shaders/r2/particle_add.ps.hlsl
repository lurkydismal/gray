#include "common.hlsli"

struct v2p
{
    float2 tc0 : TEXCOORD0; // base
    float4 c : COLOR0; // diffuse

#ifdef USE_SOFT_PARTICLES
    //    Igor: for additional depth dest
    float4 tctexgen : TEXCOORD1;
#endif //    USE_SOFT_PARTICLES
    float fog : FOG;
};

//    Must be less than view near
#define DEPTH_EPSILON 0.1h

float4 main(v2p I) : COLOR
{
    float4 result = I.c * tex2D(s_base, I.tc0);

    //    Igor: additional depth test
#ifdef USE_SOFT_PARTICLES
    float4 _P = tex2Dproj(s_position, I.tctexgen);
    float spaceDepth = _P.z - I.tctexgen.z - DEPTH_EPSILON;
    if (spaceDepth < -2 * DEPTH_EPSILON)
    {
        spaceDepth = 100000.0h; //  Skybox doesn't draw into position buffer
    }

    result.a *= Contrast(saturate(spaceDepth * 1.3h), 2);
    result.rgb *= Contrast(saturate(spaceDepth * 1.3h), 2);
#endif //    USE_SOFT_PARTICLES

    clip(result.a - (0.01f / 255.0f));
    result.w *= I.fog;
    result.xyz *= I.fog;

    return result;
}
