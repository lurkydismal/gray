#include "common.hlsli"

struct v2p
{
    float4 c0 : COLOR0;
};

// Pixel
float4 main(v2p I) : COLOR
{
    return I.c0;
}
