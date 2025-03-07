#include "common.hlsli"

uniform float4 MiddleGray;

float sample(float2 tc)
{
    float4 data = s_image.Sample(smp_rtlinear, tc);
    return dot(data, 0.25f);
}

float4 main(p_filter I) : SV_Target
{
    // sample
    float4 accum0;
    accum0.x = sample(I.Tex0.xy);
    accum0.y = sample(I.Tex1.xy);
    accum0.z = sample(I.Tex2.xy);
    accum0.w = sample(I.Tex3.xy);

    float4 accum1;
    accum1.x = sample(I.Tex4.xy);
    accum1.y = sample(I.Tex5.xy);
    accum1.z = sample(I.Tex6.xy);
    accum1.w = sample(I.Tex7.xy);

    float4 accum2;
    accum2.x = sample(I.Tex0.wz);
    accum2.y = sample(I.Tex1.wz);
    accum2.z = sample(I.Tex2.wz);
    accum2.w = sample(I.Tex3.wz);

    float4 accum3;
    accum3.x = sample(I.Tex4.wz);
    accum3.y = sample(I.Tex5.wz);
    accum3.z = sample(I.Tex6.wz);
    accum3.w = sample(I.Tex7.wz);

    // perform accumulation
    float4 final;
    
    final.x = dot(accum0, 0.25f);
    final.y = dot(accum1, 0.25f);
    final.z = dot(accum2, 0.25f);
    final.w = dot(accum3, 0.25f);

    float result = dot(final, 0.25f);

    // OK
    float scale = MiddleGray.x / (result * MiddleGray.y + MiddleGray.z);
    float scale_prev = s_tonemap.Sample(smp_nofilter, I.Tex0.xy).x;
    float rvalue = lerp(scale_prev, scale, MiddleGray.w);

    clamp(rvalue, 1.f / 128.f, 20.0f);

    return rvalue;
}

