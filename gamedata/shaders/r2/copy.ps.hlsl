#include "common.hlsli"

// Pixel
float4 main(float2 tc : TEXCOORD0) : COLOR
{
    return tex2D(s_base, tc);
}
