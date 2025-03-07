#include "common.hlsli"
#include "mblur.hlsli"
#include "dof.hlsli"

struct v2p
{
    float4 tc0 : TEXCOORD0; // Center
    float4 tc1 : TEXCOORD1; // LT
    float4 tc2 : TEXCOORD2; // RB
    float4 tc3 : TEXCOORD3; // RT
    float4 tc4 : TEXCOORD4; // LB
    float4 tc5 : TEXCOORD5; // Left / Right
    float4 tc6 : TEXCOORD6; // Top / Bottom
};

uniform sampler2D s_distort;

#define EPSDEPTH 0.001

float4 main(v2p I) : COLOR
{
#ifdef USE_DISTORT
    float depth = tex2D(s_position, I.tc0).z;
    float4 distort = tex2D(s_distort, I.tc0);
    float2 offset = (distort.xy - (127.0f / 255.0f)) * def_distort;
    float2 center = I.tc0 + offset;
    float depth_x = tex2D(s_position, center).z;
    if ((depth_x + EPSDEPTH) < depth)
    {
        center = I.tc0;
    }
#else
    float2 center = I.tc0;
#endif

    float3 img = dof(center);
    float4 bloom = tex2D(s_bloom, center);

    img = mblur(center, tex2D(s_position, I.tc0), img.rgb);
    img = tonemap(img, tex2Dlod(s_tonemap, float4(0.5f, 0.5f, 0.5f, 0.5f)).x);

#ifdef USE_DISTORT
    float3 blurred = bloom * def_hdr;
    img = lerp(img, blurred, distort.z);
#endif

    return combine_bloom(img, bloom);
}
