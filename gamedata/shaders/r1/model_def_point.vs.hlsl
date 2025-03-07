#include "common.hlsli"
#include "skin.hlsli"

vf_point _main(v_model v)
{
    vf_point o;

    float4 pos = v.pos;
    float3 pos_w = mul(m_W, pos);
    float4 pos_w4 = float4(pos_w, 1);
    float3 norm_w = normalize(mul(m_W, v.norm));

    o.hpos = mul(m_WVP, pos); // xform, input in world coords
    o.tc0 = v.tc.xy; // copy tc
    o.color = calc_point(o.tc1, o.tc2, pos_w4, norm_w); // just hemisphere

    return o;
}

#define SKIN_VF vf_point
#include "skin_main.hlsli"
