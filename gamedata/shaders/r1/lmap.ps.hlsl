#include "common.hlsli"

struct v2p
{
    float2 tc0 : TEXCOORD0; // base
    float2 tc1 : TEXCOORD1; // lmap
    float2 tc2 : TEXCOORD2; // lmap-hemi
    float3 c0 : COLOR0; // hemi
    float3 c1 : COLOR1; // sun
    float fog : FOG;
};

// Pixel
float4 main(v2p I) : COLOR
{
    float4 t_base = tex2D(s_base, I.tc0);
    float4 t_lmap = tex2D(s_lmap, I.tc1);

    // lighting
    float3 l_base = t_lmap.rgb; // base light-map
    float3 l_hemi = I.c0 * p_hemi(I.tc2); // hemi
    float3 l_sun = I.c1 * t_lmap.a; // sun color
    float3 light = L_ambient + l_base + l_sun + l_hemi;

    // final-color
    float3 final = light * t_base * 2.0f;
    final = lerp(fog_color.xyz, final, I.fog);

    // out
    return float4(final, t_base.a);
}
