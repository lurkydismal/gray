#include "common.hlsli"

// Pixel
float4 main(float4 depth : TEXCOORD1) : COLOR
{
    return depth.z / depth.w;
}
