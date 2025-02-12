#include "common.hlsli"

struct v2p
{
    float2 tc0 : TEXCOORD0; // base
    float4 c0 : COLOR0;
    float fog : FOG;
};

// Pixel
float4 main(v2p I) : COLOR
{
    float4 t_base = tex2D(s_base, I.tc0);
    float4 final = t_base * I.c0 * 2;
    final.w = t_base.w * I.c0.w;
    final.w *= I.fog;

    // out
    return final;
}
