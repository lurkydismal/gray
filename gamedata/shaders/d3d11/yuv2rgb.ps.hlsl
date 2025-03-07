#include "common.hlsli"

float4 main(float2 uv : TEXCOORD0) : SV_Target
{
    float3 YUV = s_base.Sample(smp_base, uv).xyz;

    float Y = YUV.z;
    float U = YUV.y;
    float V = YUV.x;

    float c = 1.16406f;
    float3 _Y = float3(c, c, c) * Y;
    float3 _U = float3(0, -0.390625f, +2.01562f) * U;
    float3 _V = float3(+1.59765f, -0.8125f, 0) * V;
    float3 _S = float3(-0.86961f, +0.53076f, -1.0786f);

    return float4(_Y + _U + _V + _S, 1);
}

