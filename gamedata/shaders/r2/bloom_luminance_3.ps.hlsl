#include "common.hlsli"

struct v2p
{
    float4 tc0 : TEXCOORD0; // Central
    float4 tc1 : TEXCOORD1; // -1,+1
    float4 tc2 : TEXCOORD2; // -2,+2
    float4 tc3 : TEXCOORD3; // -3,+3
    float4 tc4 : TEXCOORD4; // -4,+4
    float4 tc5 : TEXCOORD5; // -5,+5
    float4 tc6 : TEXCOORD6; // -6,+6
    float4 tc7 : TEXCOORD7; // -7,+7
};

uniform float4 MiddleGray;

//    perform 4x4 bilinear, 8x8p, the step (C)
//    c):    8x8p    => 1x1p        with exp
#ifdef FP16_FILTER
// native bilinear
float sample(float2 tc)
{
    return dot(tex2D(s_image, tc), 1.0f / 4.0f); // sum components
}
#else
// emulate bilinear
float sample(float2 tc)
{
    float pfloat = 0.5f / 8.0f;
    float4 res;
    res.x = dot(tex2D(s_image, tc + float2(-pfloat, -pfloat)), 1.0f / 4.0f); // sum components
    res.y = dot(tex2D(s_image, tc + float2(+pfloat, -pfloat)), 1.0f / 4.0f); // sum components
    res.z = dot(tex2D(s_image, tc + float2(-pfloat, +pfloat)), 1.0f / 4.0f); // sum components
    res.w = dot(tex2D(s_image, tc + float2(+pfloat, +pfloat)), 1.0f / 4.0f); // sum components
    return dot(res, 1.0f / 4.0f); // sum components
}
#endif

float4 main(v2p I) : COLOR
{
    // sample
    float4 accum0;
    accum0.x = sample(I.tc0);
    accum0.y = sample(I.tc1);
    accum0.z = sample(I.tc2);
    accum0.w = sample(I.tc3);

    float4 accum1;
    accum1.x = sample(I.tc4);
    accum1.y = sample(I.tc5);
    accum1.z = sample(I.tc6);
    accum1.w = sample(I.tc7);

    float4 accum2;
    accum2.x = sample(I.tc0.wz);
    accum2.y = sample(I.tc1.wz);
    accum2.z = sample(I.tc2.wz);
    accum2.w = sample(I.tc3.wz);

    float4 accum3;
    accum3.x = sample(I.tc4.wz);
    accum3.y = sample(I.tc5.wz);
    accum3.z = sample(I.tc6.wz);
    accum3.w = sample(I.tc7.wz);

    // perform accumulation
    float4 final;
    final.x = dot(accum0, 1 / 4.h);
    final.y = dot(accum1, 1 / 4.h);
    final.z = dot(accum2, 1 / 4.h);
    final.w = dot(accum3, 1 / 4.h);
    float result = dot(final, 1 / 4.h);

    // OK
    float scale = MiddleGray.x / (result * MiddleGray.y + MiddleGray.z); // final
    float scale_prev = tex2D(s_tonemap, I.tc0).x;
    float rvalue = lerp(scale_prev, scale, MiddleGray.w);

    //            clamp        (rvalue, 1.f/8.f, 2.0f);

    clamp(rvalue, 1.f / 128.f, 20.0f);

    return rvalue;
}
