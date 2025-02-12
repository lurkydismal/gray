#include "fluid_common.hlsli"

//////////////////////////////////////////////////////////////////////////////////////////
//    Pixel
float4 main(p_fluidsim input) : SV_Target
{
    if (IsNonEmptyCell(input.texcoords.xyz))
    {
        return 0;
    }

    float3 npos = GetAdvectedPosTexCoords(input);

    float4 r;
    float3 diff = abs(floatVolumeDim.xyz - input.cell0.xyz);

    // Must use regular semi-Lagrangian advection instead of BFECC at the volume boundaries
    if ((diff.x > (floatVolumeDim.x - 4)) || (diff.y > (floatVolumeDim.y - 4)) || (diff.z > (floatVolumeDim.z - 4)))
    {
        r = Texture_color.SampleLevel(samLinear, npos, 0);
    }
    else
    {
        // Texture_color contains \phi^n; Texture_tempscalar contains \bar{\phi}
        //  (i.e.: the result of 1 forward advection step, followed by a backwards advection step)
        r = 1.5f * Texture_color.SampleLevel(samLinear, npos, 0) -
            0.5f * Texture_tempscalar.SampleLevel(samLinear, npos, 0);
    }

    r = saturate(r);
    return r * modulate;
}
