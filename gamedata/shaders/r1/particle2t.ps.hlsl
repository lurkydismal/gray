#include "common.hlsli"

struct v2p
{
    float2 tc0 : TEXCOORD0; // base
    float2 tc1 : TEXCOORD1; // another
    float4 c : COLOR0; // diffuse
};

// Pixel
uniform sampler2D s_another;

float4 main(v2p I) : COLOR
{
    float3 base = I.c * tex2D(s_base, I.tc0);
    float4 grad = tex2D(s_another, I.tc1);
    float3 mult = base * grad * 2.0f;
    return float4(lerp(base, mult, grad.w), 1.0f);
}
