#include "common.hlsli"

struct v2p
{
    float4 color : COLOR0; // multiplier, color.w = noise_amount
    float4 gray : COLOR1; // (.3,.3,.3.,amount)
    float2 tc0 : TEXCOORD0; // base1 (duality)
    float2 tc1 : TEXCOORD1; // base2 (duality)
    float2 tc2 : TEXCOORD2; // base  (noise)
};

uniform sampler2D s_base0;
uniform sampler2D s_base1;
uniform sampler2D s_noise;
uniform float4 c_brightness;

// Pixel
float4 main(v2p I) : COLOR
{
    float3 t_0 = tex2D(s_base0, I.tc0);
    float3 t_1 = tex2D(s_base1, I.tc1);
    float3 image = (t_0 + t_1) * 0.5f; // add_d2
    float gray = dot(image, I.gray); // dp3
    image = lerp(gray, image, I.gray.w); // mul/mad

    float4 t_noise = tex2D(s_noise, I.tc2);
    float3 noised = image * t_noise * 2.0f; // mul_2x
    image = lerp(noised, image, I.color.w); // lrp ?
    image = (image * I.color + c_brightness) * 2.0f; // mad
    //            image    = (image + c_brightness) * I.color;        // mad ?

    // out
    // return  float4    (image.r,0,0,1);                    // +mov
    return float4(image, 1.0f); // +mov
}
