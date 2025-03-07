#include "common.hlsli"

struct v2p
{
    float2 tc0 : TEXCOORD0; // base
    float2 tc1 : TEXCOORD1; // base
    float4 c0 : COLOR0;
    float4 c1 : COLOR1;
    float fog : FOG;
};

// Pixel
float4 main(v2p I) : COLOR
{
    float4 t_base = tex2D(s_base, I.tc0);
    float4 t_dt = tex2D(s_detail, I.tc1);

    float3 detail = t_dt * I.c0.a + I.c1.a;
    float3 final = (t_base * I.c0 * 2.0f) * detail * 2.0f;
    final = lerp(fog_color.xyz, final, I.fog);

    // out
    return float4(final.r, final.g, final.b, t_base.a);
}
