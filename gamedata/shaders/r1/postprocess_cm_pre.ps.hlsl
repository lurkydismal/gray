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
uniform sampler2D s_distort;
uniform sampler2D s_grad0;
uniform sampler2D s_grad1;
uniform float4 c_colormap;

// Pixel
float4 main(v2p I) : COLOR
{
    float4 pre_pass = tex2D(s_base0, I.tc0);
    float grad_i = dot(pre_pass.rgb, (0.3333f).xxx);

    float3 image0 = tex2D(s_grad0, float2(grad_i, 0.5f));
    float3 image1 = tex2D(s_grad1, float2(grad_i, 0.5f));
    float3 image = lerp(image0, image1, c_colormap.y);
    image = lerp(pre_pass.rgb, image, c_colormap.x);

    return float4(image, 1.0f); // +mov
}
