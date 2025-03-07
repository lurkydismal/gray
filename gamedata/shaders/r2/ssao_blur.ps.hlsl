uniform float4 screen_res;
uniform sampler2D s_occ;

float ssao_blur_ps(float2 centerTC)
{
    // low weight center sample - will be used on edges
    float fSumWeight = 0.025f;
    float2 centerData = tex2D(s_occ, centerTC);

    float fOcclusion = centerData.r * fSumWeight;
    float centerDepth = centerData.g;

    float2 arrOffsets[4] =
    {
        float2(1, -1),
        float2(-1, -1),
        float2(1, 1),
        float2(-1, 1)
    };

    for (int i = 0; i < 4; i++)
    {
        float2 sampleTC = centerTC + screen_res.zw * arrOffsets[i];
        float2 sampleData = tex2D(s_occ, sampleTC);

        float fDepth = sampleData.g;
        float fDiff = 8 * abs(fDepth - centerDepth) / min(fDepth, centerDepth);
        float fWeight = saturate(1 - fDiff);

        fOcclusion += sampleData.r * fWeight;

        fSumWeight += fWeight;
    }

    fOcclusion /= fSumWeight;

    return fOcclusion;
}
