#include "common.hlsli"

float4 main(float4 color : COLOR0, float fog : FOG) : COLOR
{
    float4 final = color;
    final.xyz = lerp(fog_color.xyz, final.xyz, fog);
    return final;
}
