#include "common.hlsli"

struct v2p
{
    float2 tc0 : TEXCOORD0; // base
};

// Pixel
float4 main(v2p I) : COLOR
{
    return /*(float4(1,1,1,1) - */ tex2D(s_base, I.tc0);
}
