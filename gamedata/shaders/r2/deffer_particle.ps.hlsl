#include "common.hlsli"
#include "sload.hlsli"

struct p_particle
{
    float4 color : COLOR0;
    p_flat base;
};

f_deffer main(p_particle II)
{
    f_deffer O;
    p_flat I;

    I = II.base;

    // 1. Base texture + kill pixels with low alpha
    float4 D = tex2D(s_base, I.tcdh);
    D *= II.color;
    clip(D.w - def_aref);

    // 2. Standart output
    float4 Ne = float4(normalize((float3)I.N.xyz), I.position.w);
    O = pack_gbuffer(Ne, float4(I.position.xyz, xmaterial), float4(D.xyz, def_gloss));

    return O;
}
