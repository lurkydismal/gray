#include "common.hlsli"

struct v2p
{
    float2 tc0 : TEXCOORD0; // base
    float4 c0 : COLOR0; //    rgb.fog
};

// Pixel
//    Need PS 2.0 for the fog to be applied
float4 main(v2p I) : COLOR
{
    float4 t_base = tex2D(s_base, I.tc0);
    float3 final = t_base * I.c0 * 2.0f;

    // out
    return float4(final, t_base.a * I.c0.a * I.c0.a);
}
