#include "common.hlsli"
#include "shared\waterconfig.hlsli"

struct v2p
{
    float2 tbase : TEXCOORD0; // base
    float2 tdist0 : TEXCOORD1; // d0
    float2 tdist1 : TEXCOORD2; // d1
};

uniform sampler2D s_distort0;
uniform sampler2D s_distort1;

// Pixel
float4 main(v2p I) : COLOR
{
    float4 t_base = tex2D(s_base, I.tbase);

    float2 t_d0 = tex2D(s_distort0, I.tdist0);
    float2 t_d1 = tex2D(s_distort1, I.tdist1);
    float2 distort = (t_d0 + t_d1) * 0.5f; // average
    float2 zero = float2(0.5f, 0.5f);
    float2 faded = lerp(distort, zero, t_base.a);

    float2 faded_bx2 = (faded * 2.0f - 1.0f) * W_DISTORT_POWER;
    float faded_dot = dot(float3(faded_bx2, 0.0f), 0.75f); // 0.75
    float alpha = 0.5f; // abs      (faded_dot);
    // out
    return float4(faded, 0.0f, alpha);
}
