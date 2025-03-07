#include "common.hlsli"
#include "shadow.hlsli"

struct v2p
{
    float3 lightToPos : TEXCOORD0; // light center to plane vector
    float3 vPos : TEXCOORD1; // position in camera space
    float fDensity : TEXCOORD2; // plane density alon Z axis
    //    float2    tNoise         : TEXCOORD3;    // projective noise
};

uniform float4 m_lmap[2];
uniform sampler2D s_noise;

#define USE_LMAP
#define USE_LMAPXFORM
#define USE_SHADOW

// Pixel
float4 main(v2p I) : COLOR
{
    // ----- shadow
    float4 P4 = float4(I.vPos, 1.0f);
    float4 PS = mul(m_shadow, P4);
    float s = 1.0f;
#ifdef USE_SHADOW
    s = shadow(PS);
#endif

    // ----- lightmap
    float4 lightmap = 1.0f;
#ifdef USE_LMAP
    #ifdef USE_LMAPXFORM
    PS.x = dot(P4, m_lmap[0]);
    PS.y = dot(P4, m_lmap[1]);
    #endif
    lightmap = tex2Dproj(s_lmap, PS); //
#endif

    // ----- attenuate
    float rsqr = dot(I.lightToPos, I.lightToPos); // distance 2 light (squared)
    float att = saturate(1 - rsqr * Ldynamic_pos.w); // q-linear attenuate
    // float  att     = saturate( 1 - (rsqr * Ldynamic_pos.w)*(rsqr * Ldynamic_pos.w) ); // q-linear attenuate
    // float  att     = 10*(1/(1+0.1*rsqr));
    // float  att     = 1.0h/rsqr;
    // float  att     = 1.0h/rsqr-Ldynamic_pos.w;
    // float  att     = 5*(sqrt(1.0h/rsqr)-sqrt(Ldynamic_pos.w));

    // ----- noise
    PS.xy /= PS.w;
    float time = timers.z * 0.1f;
    PS.xy /= 3;
    PS.x += time;
    float4 t_noise = tex2D(s_noise, PS);
    PS.x -= time;
    PS.y -= time * 0.70091f;
    t_noise *= tex2D(s_noise, PS);
    // t_noise *= 4;
    t_noise = t_noise * 0.5f + 0.5f;

    // out
    // float maxIntens = 1.0h/100.0h;
    // float maxIntens = 1.0h/40.0h;
    // float maxIntens = 1.0h/10.0h;
    float maxIntens = I.fDensity;
    float3 result = maxIntens * s * att;
    result *= lightmap;
    result *= Ldynamic_color * t_noise;

    //    result = maxIntens;
    //    result *= lightmap;

    //    result = 0.1h;
    //    result = 0.0h;
    return float4(result, 0.0f);
}
