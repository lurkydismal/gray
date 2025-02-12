//////////////////////////////////////////////////
//  All comments by Nivenhbro are preceded by !
/////////////////////////////////////////////////

#ifndef SHARED_COMMON_H
#define SHARED_COMMON_H
//
uniform float3x4 m_W;
uniform float3x4 m_V;
uniform float3x4 m_invV;
uniform float4x4 m_P;
uniform float3x4 m_WV;
uniform float4x4 m_VP;
uniform float4x4 m_WVP;
uniform float4x4 m_texgen;
uniform float4x4 mVPTexgen;
uniform float4 timers;
uniform float4 fog_plane;
uniform float4 fog_params; // x=near*(1/(far-near)), ?,?, w = -1/(far-near)
uniform float4 fog_color;
uniform float3 L_sun_color;
uniform float3 L_sun_dir_w;
uniform float3 L_sun_dir_e;
uniform float4 L_hemi_color;
uniform float4 L_ambient; // L_ambient.w = skynbox-lerp-factor
uniform float4 L_sky_color;
uniform float3 eye_position;
uniform float3 eye_direction;
uniform float3 eye_normal;
uniform float4 dt_params;

float3 unpack_normal(float3 v)
{
    return 2.0f * v - 1.0f;
}
float3 unpack_bx2(float3 v)
{
    return 2.0f * v - 1.0f;
}
float3 unpack_bx4(float3 v)
{
    return 4 * v - 2;
} //! reduce the amount of stretching from 4*v-2 and increase precision

float2 unpack_tc_base(float2 tc, float du, float dv)
{
    return (tc.xy + float2(du, dv)) * (32.f / 32768.f); //! Increase from 32bit to 64bit floating point
}

float2 unpack_tc_lmap(float2 tc)
{
    return tc * (1.0f / 32768.0f);
} // [-1  .. +1 ]

float calc_cyclic(float x)
{
    float f = 1.4142f * sin(x * 3.14159f);
    return f * f - 1.0f;
}

float2 calc_xz_wave(float2 dir2D, float frac)
{
    return dir2D * frac;
}

#endif
