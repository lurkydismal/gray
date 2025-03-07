#include "common.hlsli"

struct v2p
{
    float2 tc0 : TEXCOORD0; // base
    float3 tc1 : TEXCOORD1; // environment
    float3 c0 : COLOR0; // sun
    float fog : FOG;
};

// Pixel
float4 main(v2p I) : COLOR
{
    float4 t_base = tex2D(s_base, I.tc0);
    float4 t_env = texCUBE(s_env, I.tc1);

    float3 base = lerp(t_env, t_base, t_base.a);
    float3 light = I.c0;
    float3 final = light * base * 2.0f;
    final = lerp(fog_color.xyz, final, I.fog);

    // out
    return float4(final.r, final.g, final.b, t_base.a);
}
