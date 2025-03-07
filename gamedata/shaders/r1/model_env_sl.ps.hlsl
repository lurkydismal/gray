#include "common.hlsli"

struct v2p
{
    float2 tc0 : TEXCOORD0; // base
    float3 tc1 : TEXCOORD1; // environment
    float2 tc2 : TEXCOORD2; // lmap
    float3 c0 : COLOR0; // sun
    float4 c1 : COLOR1; // lq-color + factor
};

// Pixel
float4 main(v2p I) : COLOR
{
    float4 t_base = tex2D(s_base, I.tc0);
    float4 t_env = texCUBE(s_env, I.tc1);

    // final-color
    float3 final = lerp(t_env, t_base, t_base.a);

    // out
    return float4(final.r, final.g, final.b, t_base.a);
}
