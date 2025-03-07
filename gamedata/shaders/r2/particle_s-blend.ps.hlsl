#include "common.hlsli"

struct v2p
{
    float2 tc0 : TEXCOORD0; // base
    float4 c : COLOR0; // diffuse
};

// Pixel
float4 main(v2p I) : COLOR
{
    // alpha = 0     -> color=1
    // alpha = 1    -> color=c
    float4 c = I.c * tex2D(s_base, I.tc0);
    float3 r = lerp(float3(1.0f, 1.0f, 1.0f), c.xyz, c.w);
    return float4(r, 1.0f);
}
