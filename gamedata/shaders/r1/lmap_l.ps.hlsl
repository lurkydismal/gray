#include "common.hlsli"

struct v2p
{
    float2 tc0 : TEXCOORD0; // lmap
    float2 tc1 : TEXCOORD1; // hemi
    float3 c0 : COLOR0;
    float fog : FOG;
};

// Pixel
float4 main(v2p I) : COLOR
{
    float4 t_lmap = tex2D(s_lmap, I.tc0);

    // lighting
    float3 l_base = t_lmap.rgb; // base light-map
    float3 l_hemi = I.c0 * p_hemi(I.tc1); // hemi
    float l_sun = t_lmap.a; // sun color
    float3 light = L_ambient + l_base + l_hemi;
    light = lerp(fog_color.xyz, light, I.fog);

    return float4(light, l_sun);
}
