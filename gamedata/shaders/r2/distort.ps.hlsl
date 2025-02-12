#include "common.hlsli"

struct v2p
{
    float2 tc : TEXCOORD0; // base & distort
};

// uniform sampler2D     s_base;
uniform sampler2D s_distort;

// Pixel
float4 main(v2p I) : COLOR
{
    float2 distort = tex2D(s_distort, I.tc);
    float2 offset = (distort.xy - 0.5f) * def_distort;
    float3 image = tex2D(s_base, I.tc + offset);

    // out
    return float4(image, 1.0f); // +mov
}
