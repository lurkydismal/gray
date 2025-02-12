#include "common.hlsli"

struct v2p
{
    float2 tc0 : TEXCOORD0; // base
    float2 tc1 : TEXCOORD1; // lmap
    float2 tc2 : TEXCOORD2; // detail
    float4 c0 : COLOR0; // hemi, c0.a *
    float4 c1 : COLOR1; // sun,  c1.a +
    float fog : FOG;
};

uniform sampler2D s_dt_r;
uniform sampler2D s_dt_g;
uniform sampler2D s_dt_b;
uniform sampler2D s_dt_a;

uniform sampler2D s_mask;

// Pixel
float4 main(v2p I) : COLOR
{
    float4 t_base = tex2D(s_base, I.tc0);
    float4 t_mask = tex2D(s_mask, I.tc0);

    t_mask = t_mask / dot(t_mask, 1.0);

    float4 t_lmap = tex2D(s_lmap, I.tc1);

    // lighting
    float3 l_base = t_lmap.rgb; // base light-map
    float3 l_hemi = I.c0 * t_base.a; // hemi is implicitly inside texture
    float3 l_sun = I.c1 * t_lmap.a; // sun color
    float3 light = L_ambient + l_base + l_sun + l_hemi;

    // calc D-texture
    float3 t_dt_r = tex2D(s_dt_r, I.tc2) * t_mask.r;
    float3 t_dt_g = tex2D(s_dt_g, I.tc2) * t_mask.g;
    float3 t_dt_b = tex2D(s_dt_b, I.tc2) * t_mask.b;
    float3 t_dt_a = tex2D(s_dt_a, I.tc2) * t_mask.a;
    float3 detail = t_dt_a + t_dt_b + t_dt_g + t_dt_r;

    // final-color
    float3 final = (light * t_base.rgb * 2.0f) * detail * 2.0f;
    final = lerp(fog_color.xyz, final, I.fog);
    // out
    return float4(final.rgb, 1.0f);
}
