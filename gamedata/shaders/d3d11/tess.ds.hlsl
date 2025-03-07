#include "common.hlsli"
#include "tess.hlsli"

[domain("tri")]
void main(in HS_CONSTANT_DATA_OUTPUT input,
    in float3 uvw : SV_DomainLocation, in const OutputPatch<p_bumped_new, 3> bp, out p_bumped_new output)
{
    float minc = min(uvw.x, min(uvw.y, uvw.z));

    [flatten]
    if (minc != 0.0f)
    {
        [flatten]
        if (((1.0f / 3.0f) - minc) > 0.01f)
        {
            float fK = (1.0f / 3.0f) / ((1.0f / 3.0f) - minc);
            fK = minc < 0.1 ? fK : 1.0f;
            uvw = lerp((1.0f / 3.0f).xxx, uvw, fK);
        }
    }

    float u = uvw.x;
    float v = uvw.y;
    float w = uvw.z;

    output.tcdh = bp[0].tcdh * w + bp[1].tcdh * v + bp[2].tcdh * u;
    output.position = bp[0].position * w + bp[1].position * v + bp[2].position * u;

    float3 M1 = bp[0].M1 * w + bp[1].M1 * v + bp[2].M1 * u;
    output.M1 = M1;
    float3 M2 = bp[0].M2 * w + bp[1].M2 * v + bp[2].M2 * u;
    output.M2 = M2;
    float3 M3 = bp[0].M3 * w + bp[1].M3 * v + bp[2].M3 * u;
    output.M3 = M3;
    float3 Normal = normalize(float3(M1.z, M2.z, M3.z));
    float3 triPos = output.position.xyz;

#if TESS_PN
    float3 N[3] =
    {
        float3(bp[0].M1.z, bp[0].M2.z, bp[0].M3.z),
        float3(bp[1].M1.z, bp[1].M2.z, bp[1].M3.z),
        float3(bp[2].M1.z, bp[2].M2.z, bp[2].M3.z)
    };

    float3 P[3] =
    {
        bp[0].position.xyz,
        bp[1].position.xyz,
        bp[2].position.xyz
    };

    ComputePatchVertex(P, N, uvw, input.patch, output.position.xyz, Normal);
#endif

#if TESS_HM
    ComputeDisplacedVertex(output.position.xyz, Normal, output.tcdh, output.tcdh.xy * dt_params.xy);
#endif

    [flatten]
    if (minc == 0)
    {
        output.position.xyz = triPos;
    }

    output.hpos = mul(m_P, float4(output.position.xyz, 1));

    output.hpos_curr = bp[0].hpos_curr * w + bp[1].hpos_curr * v + bp[2].hpos_curr * u; 
    output.hpos_old = bp[0].hpos_old * w + bp[1].hpos_old * v + bp[2].hpos_old * u;

#if 0
    output.hpos_curr = mul(m_P, float4(output.position.xyz, 1.0));
    output.hpos_old = mul(m_VP_old, float4(mul(m_invV, float4(output.position.xyz, 1.0)).xyz, 1.0));
#endif

    output.hpos.xy += m_taa_jitter.xy * output.hpos.w;
}

