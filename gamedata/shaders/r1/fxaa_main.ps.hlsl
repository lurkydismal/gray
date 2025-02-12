#include "common.hlsli"
#include "fxaa.hlsli"

uniform float4 screen_res;
uniform sampler2D s_image;

struct v2p
{
    float2 tc0 : TEXCOORD0;
    float4 HPos : POSITION;
};

float4 main(v2p I) : COLOR
{
    float2 rcpFrame = screen_res.zw;

    return FxaaPixelShader(I.tc0,
        FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f), // FxaaFloat4 fxaaConsolePosPos,
        s_image, // FxaaTex tex,
        s_image, // FxaaTex fxaaConsole360TexExpBiasNegOne,
        s_image, // FxaaTex fxaaConsole360TexExpBiasNegTwo,
        rcpFrame, // FxaaFloat2 fxaaQualityRcpFrame,
        FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f), // FxaaFloat4 fxaaConsoleRcpFrameOpt,
        FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f), // FxaaFloat4 fxaaConsoleRcpFrameOpt2,
        FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f), // FxaaFloat4 fxaaConsole360RcpFrameOpt2,
        0.35f, // FxaaFloat fxaaQualitySubpix,
        0.125f, // FxaaFloat fxaaQualityEdgeThreshold,
        0.0f, // 0.0625f,                                // FxaaFloat fxaaQualityEdgeThresholdMin,
        0.0f, // FxaaFloat fxaaConsoleEdgeSharpness,
        0.0f, // FxaaFloat fxaaConsoleEdgeThreshold,
        0.0f, // FxaaFloat fxaaConsoleEdgeThresholdMin,
        FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f) // FxaaFloat fxaaConsole360ConstDir,
    );
}
