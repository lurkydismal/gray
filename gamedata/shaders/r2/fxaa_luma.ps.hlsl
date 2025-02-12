#include "common.hlsli"

struct v2p
{
    float2 tc0 : TEXCOORD0;
    float4 HPos : POSITION;
};

float4 main(v2p I) : COLOR
{
    float3 image = tex2D(s_image, I.tc0);
    return float4(image, dot(image, LUMINANCE_VECTOR));
}
