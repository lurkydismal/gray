#include "common.hlsli"

#define SMAA_HLSL_3
uniform float4 screen_res;
#define SMAA_RT_METRICS screen_res.zwxy

#define SMAA_PRESET_ULTRA

#include "smaa.hlsli"

SMAATexture2D(s_edgetex);
SMAATexture2D(s_areatex);
SMAATexture2D(s_searchtex);

// Struct
struct p_smaa
{
    float2 tc0 : TEXCOORD0; // Texture coordinates         (for sampling maps)
};

float4 main(p_smaa I) : COLOR
{
    float2 pixcoord;
    float4 offset[3];
    SMAABlendingWeightCalculationVS(I.tc0, pixcoord, offset);

    return SMAABlendingWeightCalculationPS(I.tc0, pixcoord, offset, s_edgetex, s_areatex, s_searchtex, 0.0f);
};
