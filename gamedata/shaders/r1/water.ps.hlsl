#include "common.hlsli"

struct vf
{
    float4 hpos : POSITION;
    float2 tbase : TEXCOORD0; // base
    float2 tnorm0 : TEXCOORD1; // nm0
    float2 tnorm1 : TEXCOORD2; // nm1
    float3 M1 : TEXCOORD3;
    float3 M2 : TEXCOORD4;
    float3 M3 : TEXCOORD5;
    float3 v2point : TEXCOORD6;
    float4 c0 : COLOR0;
    float fog : FOG;
};

uniform sampler2D s_nmap;
uniform samplerCUBE s_env0;
uniform samplerCUBE s_env1;

float4 main(vf I) : COLOR
{
    float4 base = tex2D(s_base, I.tbase);
    float3 n0 = tex2D(s_nmap, I.tnorm0);
    float3 n1 = tex2D(s_nmap, I.tnorm1);
    float3 Navg = n0 + n1 - 1.0f;

    float3 Nw = mul(float3x3(I.M1, I.M2, I.M3), Navg);
    Nw = normalize(Nw);

    float3 v2point = normalize(I.v2point);
    float3 vreflect = reflect(v2point, Nw);
    float fresnel = saturate(dot(vreflect, v2point));
    float3 vreflectabs = abs(vreflect);
    float vreflectmax = max(vreflectabs.x, max(vreflectabs.y, vreflectabs.z));
    vreflect /= vreflectmax;
    vreflect.y = vreflect.y * 2.0f - 1.0f;

    float3 env0 = texCUBE(s_env0, vreflect);
    float3 env1 = texCUBE(s_env1, vreflect);
    float3 env = lerp(env0, env1, L_ambient.w);

    float power = pow(fresnel, 9);
    float amount = 0.55f + 0.25f * power; // 1=full env, 0=no env // 1=full env, 0=no env
    float alpha = 0.75f + 0.25f * power; // 1=full env, 0=no env
    float3 c_reflection = env * amount;
    float3 final = lerp(c_reflection, base.rgb, base.a);
    final *= I.c0 * 2;

    // Fogging
    float fog = 1.0f - I.c0.w;
    final = lerp(final, fog_color.xyz, fog);
    alpha *= 1.0f - fog * fog;

    return float4(final, alpha);
}
