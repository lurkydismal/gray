#include "common.hlsli"

struct v2p
{
    float2 tc0 : TEXCOORD0; // base0
    float2 tc1 : TEXCOORD1; // base1
    float2 tc2 : TEXCOORD2; // hemi0
    float2 tc3 : TEXCOORD3; // hemi1
    float4 c : COLOR0; // color
    float4 f : COLOR1; // color
    float fog : FOG;
};

// Pixel
uniform sampler2D s_base0;
uniform sampler2D s_base1;
uniform sampler2D s_hemi0;
uniform sampler2D s_hemi1;

float4 main(v2p I) : COLOR
{
    float4 base0 = tex2D(s_base0, I.tc0);
    float4 base1 = tex2D(s_base1, I.tc1);
    float4 hemi0 = tex2D(s_hemi0, I.tc2);
    float4 hemi1 = tex2D(s_hemi1, I.tc3);

    float4 base = lerp(base0, base1, I.f.w) * I.c;
    float hemi = lerp(hemi0, hemi1, I.f.w).w;

    float3 color = base * 2.0f * (0.5f + 0.5f * hemi);
    color = lerp(fog_color.xyz, color, I.fog);

    return float4(color, base.a);
}
