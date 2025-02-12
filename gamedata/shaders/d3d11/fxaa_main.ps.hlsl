#include "common.hlsli"
#include "fxaa.hlsli"

uniform float4 scaled_screen_res;

float4 main(v2p_aa_AA I) : SV_Target
{
    float2 rcpFrame = scaled_screen_res.zw;
    FxaaTex tex = {smp_rtlinear, s_image};

    return FxaaPixelShader(I.Tex0.xy,
        FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f), // FxaaFloat4 fxaaConsolePosPos,
        tex, // FxaaTex tex,
        tex, // FxaaTex fxaaConsole360TexExpBiasNegOne,
        tex, // FxaaTex fxaaConsole360TexExpBiasNegTwo,
        rcpFrame, // FxaaFloat2 fxaaQualityRcpFrame,
        FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f), // FxaaFloat4 fxaaConsoleRcpFrameOpt,
        FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f), // FxaaFloat4 fxaaConsoleRcpFrameOpt2,
        FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f), // FxaaFloat4 fxaaConsole360RcpFrameOpt2,
        0.35f, // FxaaFloat fxaaQualitySubpix,
        0.166f, // FxaaFloat fxaaQualityEdgeThreshold,
        0.0f, // FxaaFloat fxaaQualityEdgeThresholdMin,
        0.0f, // FxaaFloat fxaaConsoleEdgeSharpness,
        0.0f, // FxaaFloat fxaaConsoleEdgeThreshold,
        0.0f, // FxaaFloat fxaaConsoleEdgeThresholdMin,
        FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f) // FxaaFloat fxaaConsole360ConstDir,
    );
}
