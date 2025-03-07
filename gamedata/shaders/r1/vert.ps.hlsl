#include "common.hlsli"

struct v2p
{
    float2 tc0 : TEXCOORD0; // base
    float3 c0 : COLOR0;
    float fog : FOG;
};

// Pixel
float4 main(v2p I) : COLOR
{
    float4 t_base = tex2D(s_base, I.tc0);
    float3 final = t_base * I.c0 * 2.0f;
    final = lerp(fog_color.xyz, final, I.fog);

    // out
    return float4(final, t_base.a);
}
