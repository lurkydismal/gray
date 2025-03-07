#include "common.hlsli"

uniform float4 weight[2];

float4 main(p_filter I) : SV_Target
{
    float4 accum = weight[1].w * s_bloom.Sample(smp_rtlinear, I.Tex0.xy);
    
    accum += weight[0].x * s_bloom.Sample(smp_rtlinear, I.Tex1.xy);
    accum += weight[0].x * s_bloom.Sample(smp_rtlinear, I.Tex1.wz);
    
    accum += weight[0].y * s_bloom.Sample(smp_rtlinear, I.Tex2.xy);
    accum += weight[0].y * s_bloom.Sample(smp_rtlinear, I.Tex2.wz);
    
    accum += weight[0].z * s_bloom.Sample(smp_rtlinear, I.Tex3.xy);
    accum += weight[0].z * s_bloom.Sample(smp_rtlinear, I.Tex3.wz);
    
    accum += weight[0].w * s_bloom.Sample(smp_rtlinear, I.Tex4.xy);
    accum += weight[0].w * s_bloom.Sample(smp_rtlinear, I.Tex4.wz);
    
    accum += weight[1].x * s_bloom.Sample(smp_rtlinear, I.Tex5.xy);
    accum += weight[1].x * s_bloom.Sample(smp_rtlinear, I.Tex5.wz);
    
    accum += weight[1].y * s_bloom.Sample(smp_rtlinear, I.Tex6.xy);
    accum += weight[1].y * s_bloom.Sample(smp_rtlinear, I.Tex6.wz);
    
    accum += weight[1].z * s_bloom.Sample(smp_rtlinear, I.Tex7.xy);
    accum += weight[1].z * s_bloom.Sample(smp_rtlinear, I.Tex7.wz);

    // OK
    return accum;
}

